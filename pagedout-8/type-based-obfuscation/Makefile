
cmake -B build -DLT_LLVM_INSTALL_DIR=/usr/lib/llvm-21 -DCMAKE_C_COMPILER=clang-21 -DCMAKE_CXX_COMPILER=clang++-21 .
cmake --build build

# Generate an LLVM test file
clang-21 -O1 -S -emit-llvm test1.c -o test1.ll
opt-21 -load-pass-plugin ./hello-world/build/libHelloWorld.so -passes=hello-world -disable-output test1.ll

opt-21 -S -load-pass-plugin ./hello-world/build/libHelloWorld.so -passes=hello-world test1.ll > test2.ll


clang-21 -O0 -Xclang -disable-O0-optnone -S -emit-llvm test1.cpp -o test1.ll
opt-21 -S -load-pass-plugin ./hello-world/build/libHelloWorld.so -passes='function(hello-world)' test1.ll -o test2.ll
opt-21 -S -passes='default<O2>' test2.ll -o test3.ll
clang-21 -o test test3.ll
clang-21 -O2 -o test test1.cpp

clang++-21 -O2 -fpass-plugin=./hello-world/build/libHelloWorld.so test1.cpp -o test
