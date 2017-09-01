rm -rf bin; mkdir -p bin
gcc -g -o bin/svm *.c svm/*.c parser/*.c