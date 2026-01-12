// for each instruction:
if (auto *CI = dyn_cast<CallInst>(&I)) {
Function *f = CI->getCalledFunction();
if (f && f->getName().starts_with("llvm.var.annotation")) {
Value *annotatedPtr = CI->getOperand(0)->stripPointerCasts(),
*annotationName = CI->getArgOperand(1)->stripPointerCasts(),
*annotation = CI->getArgOperand(4)->stripPointerCasts();
LLVMContext &Ctx = CI->getContext();
auto *GV = dyn_cast<GlobalVariable>(annotationName);
auto *CDA = dyn_cast<ConstantDataArray>(GV->getInitializer());
StringRef annotationNameString = CDA->getAsCString();
// if annotationNameString == "obf"
auto *GV = dyn_cast<GlobalVariable>(annotation);
auto *CS = dyn_cast<ConstantStruct>(GV->getInitializer());
auto *CInt = dyn_cast<ConstantInt>(CS->getOperand(0));
int obfuscation_param = CInt->getZExtValue();
for (User *U : annotatedPtr->users()) {
if (LoadInst *Load = dyn_cast<LoadInst>(U)) {
ConstantInt *CInt = ConstantInt::get(Type::getInt64Ty(Ctx), obfuscation_param);
ConstantAsMetadata *IntMD = ConstantAsMetadata::get(CInt);
MDNode *Node = MDNode::get(Ctx, IntMD);
Load->setMetadata("obfuscate", Node);
}}}}

// for each instruction
if (MDNode *N = I.getMetadata("obfuscate")) {
Metadata *MD = N->getOperand(0);
auto *ConstMD = dyn_cast<ConstantAsMetadata>(MD)
auto *CI = dyn_cast<ConstantInt>(ConstMD->getValue())
int64_t val = CI->getSExtValue();
// Use IRBuilder to build obfuscation
// Use I.replaceUsesWithIf(...) to replace expression
}