gcc opto.c -o slow
gcc -O3 -funroll-loops opto.c -o fast