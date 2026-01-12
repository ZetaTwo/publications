#include <optional>
#include <string>

#include "llvm/IR/PassManager.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/IR/IRBuilder.h"

using namespace llvm;

namespace
{
    std::optional<StringRef> get_annotation_name(Value *&annotationName)
    {
        if (auto *GV = dyn_cast<GlobalVariable>(annotationName))
        {
            if (GV->hasInitializer())
            {
                if (auto *CDA = dyn_cast<ConstantDataArray>(GV->getInitializer()))
                {
                    StringRef annotationNameString = CDA->getAsCString();
                    return std::make_optional(annotationNameString);
                }
            }
        }
        return std::nullopt;
    }

    std::optional<uint64_t> get_obfuscation_parameter(Value *&annotation)
    {
        if (auto *GV = dyn_cast<GlobalVariable>(annotation))
        {
            if (GV->hasInitializer())
            {
                if (auto *CS = dyn_cast<ConstantStruct>(GV->getInitializer()))
                {
                    if (CS->getNumOperands() > 0)
                    {
                        if (auto *CInt = dyn_cast<ConstantInt>(CS->getOperand(0)))
                        {
                            return std::make_optional(CInt->getZExtValue());
                        }
                    }
                }
            }
        }
        return std::nullopt;
    }

    std::optional<uint64_t> get_obfuscation_metadata(Instruction &I)
    {
        if (MDNode *N = I.getMetadata("obfuscate"))
        {
            Metadata *MD = N->getOperand(0);

            if (auto *ConstMD = dyn_cast<ConstantAsMetadata>(MD))
            {
                if (auto *CI = dyn_cast<ConstantInt>(ConstMD->getValue()))
                {
                    int64_t val = CI->getSExtValue();
                    return std::make_optional(val);
                }
            }
        }

        return std::nullopt;
    }

    bool create_obfuscation_metadata(LLVMContext &Ctx, Value *&annotatedPtr, uint64_t obfuscation_param)
    {
        bool changed = false;
        for (User *U : annotatedPtr->users())
        {
            if (LoadInst *Load = dyn_cast<LoadInst>(U))
            {
                ConstantInt *CInt = ConstantInt::get(Type::getInt64Ty(Ctx), obfuscation_param);
                ConstantAsMetadata *IntMD = ConstantAsMetadata::get(CInt);
                MDNode *Node = MDNode::get(Ctx, IntMD);
                Load->setMetadata("obfuscate", Node);
                changed = true;
                //break;
            }
        }

        return changed;
    }

    bool process_annotation(CallInst *&CI)
    {
        bool changed = false;
        if (CI->arg_size() > 4)
        {
            Value *annotatedPtr = CI->getOperand(0)->stripPointerCasts();
            Value *annotationName = CI->getArgOperand(1)->stripPointerCasts();
            Value *annotation = CI->getArgOperand(4)->stripPointerCasts();
            LLVMContext &Ctx = CI->getContext();

            if (auto annotationNameString = get_annotation_name(annotationName))
            {
                if (annotationNameString == "obfuscate")
                {
                    if (auto obfuscationData = get_obfuscation_parameter(annotation))
                    {
                        auto changed_instr = create_obfuscation_metadata(Ctx, annotatedPtr, *obfuscationData);
                        changed = changed || changed_instr;
                    }
                }
            }
        }
        return changed;
    }

    bool process_instruction(Instruction &I)
    {
        bool changed = false;
        if (auto *CI = dyn_cast<CallInst>(&I))
        {
            Function *calledFunction = CI->getCalledFunction();
            if (calledFunction && calledFunction->getName().starts_with("llvm.var.annotation"))
            {
                auto changed_instr = process_annotation(CI);
                changed = changed || changed_instr;
            }
        }

        if (auto obfuscate = get_obfuscation_metadata(I))
        {
            IRBuilder<> Builder(I.getNextNode());
            StringRef OriginalName = I.getName();
            I.setName(OriginalName.str() + ".raw");

            Value *LoadVal = &I;
            Type *Ty = I.getType();
            Value *Mask = ConstantInt::get(Ty, (*obfuscate)-1);

            // x = x ^ (x&(c-1))
            Value *Mask2 = Builder.CreateAnd(LoadVal, Mask, "and_step");
            //Value *Mask3 = Builder.CreateShl(Mask2, ConstantInt::get(Ty, 8), "shift_step");
            Value *NewVal = Builder.CreateAdd(LoadVal, Mask2, "xor_step");

            I.replaceUsesWithIf(NewVal, [&](Use &U) { 
                if(U.getUser() == Mask2) { return false; }
                //if(U.getUser() == Mask3) { return false; }
                if(U.getUser() == NewVal) { return false; }
                return true;
            });
            changed = true;
        }

        return changed;
    }

    bool visitor(Function &F)
    {
        bool changed = false;
        for (auto &BB : F)
        {
            for (auto &I : BB)
            {
                bool changed_instr = process_instruction(I);
                changed = changed || changed_instr;
            }
        }
        return changed;
    }

    struct HelloWorldPass : PassInfoMixin<HelloWorldPass>
    {
        PreservedAnalyses run(Function &F, FunctionAnalysisManager &)
        {
            bool changed = visitor(F);
            if (changed)
            {
                return PreservedAnalyses::none();
            }
            else
            {
                return PreservedAnalyses::all();
            }
        }

        // static bool isRequired() { return true; }
        static bool isRequired() { return false; }
    };
} // namespace

llvm::PassPluginLibraryInfo getHelloWorldPluginInfo()
{
    return {LLVM_PLUGIN_API_VERSION, "HelloWorld", LLVM_VERSION_STRING,
            [](PassBuilder &PB)
            {
                PB.registerPipelineParsingCallback(
                    [](StringRef Name, PassManager<Function> &FPM,
                       ArrayRef<PassBuilder::PipelineElement>)
                    {
                        if (Name == "hello-world")
                        {
                            FPM.addPass(HelloWorldPass());
                            return true;
                        }
                        return false;
                    });

                PB.registerPipelineStartEPCallback(
                    [](ModulePassManager &MPM, OptimizationLevel Level)
                    {
                        MPM.addPass(createModuleToFunctionPassAdaptor(HelloWorldPass()));
                    });
            }};
}

extern "C" LLVM_ATTRIBUTE_WEAK ::llvm::PassPluginLibraryInfo
llvmGetPassPluginInfo()
{
    return getHelloWorldPluginInfo();
}
