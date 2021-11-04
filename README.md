# Adda

Single file hardcoded compiler to llvm IR, built for the dumbest language with the simplest syntax ever
## Syntax examples
```
add 1 1
add 3 5
```
## Output
```
; ModuleID = 'my cool jit'
source_filename = "my cool jit"

define double @main() {
entry:
  ret double 2.000000e+00
}

; ModuleID = 'my cool jit'
source_filename = "my cool jit"

define double @main() {
entry:
  ret double 8.000000e+00
}

```

## Running Adda
```
clang++ -g -O3 main.cpp `llvm-config --cxxflags --ldflags --system-libs --libs core` -o main
./main
```
