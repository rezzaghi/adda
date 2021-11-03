# Adda

Single file hardcoded compiler to llvm IR, built for the dumbest language with the simplest syntax ever
## Syntax examples
```
add 1 1
add 3 5
add 40 20
```
## Output
```
; ModuleID = 'my cool jit'
source_filename = "my cool jit"

define double @main() {
entry:
  ret double add (double 1.000000e+00, double 1.000000e+00)
}

; ModuleID = 'my cool jit'
source_filename = "my cool jit"

define double @main() {
entry:
  ret double add (double 3.000000e+00, double 5.000000e+00)
}


```

## Running Adda
```
clang++ -g -O3 main.cpp `llvm-config --cxxflags --ldflags --system-libs --libs core` -o main
./main
```
