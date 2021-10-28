# Adda

Single file hardcoded compiler to llvm IR, built for the dumbest language with the simplest syntax ever
## Syntax examples
```
add 1 1
add 3 5
add 40 20
```
## Running Adda
```
clang++ -g -O3 main.cpp `llvm-config --cxxflags --ldflags --system-libs --libs core` -o main
./main
```
