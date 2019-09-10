## Useful Commands

* `clang -emit-llvm hello.c -S -o hello.ll` (To generate IR in readable form)
* `clang -emit-llvm hello.c -c -o hello.bc` 
* `llvm-dis hello.bc` (Will generate `hello.ll` file like done previously)
* `opt -mem2reg hello.bc -o hello.ssa.bc` (To produce in SSA form, note: `-reg2mem` can be used to get back from SSA form)
* `opt -load ./lib/LLVMMyOpt.so -myopt ./hello.bc -o hello.new.bc` (To get output using your optimisations)

## When writing pass in src folder

* Write the pass like done in various other folders. Note that we usually write passes in `lib/Transforms/yourPass`.
* Then go to build directory, and type `make`.

