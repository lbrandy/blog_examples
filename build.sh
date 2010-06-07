#gcc opto.c -o opto_slow
#gcc -O3 -funroll-loops opto.c -o fast_opto

gcc signal_blur.c -o signal_blur_slow
gcc -O3 signal_blur.c -o signal_blur_opt