# Matrix
This is my solution of 'Matrix' task. On the input we have matrix size and then elements. On the output we give the matrix determinant.

For example, 
```
        input: 2 1 0 0 1
        output: 1
```

## Build
To build program:
```
        git clone https://github.com/kefirRzevo/Vladimirov-CPP-course.git
        cd Vladimirov-CPP-course/Matrix
        mkdir build
        cd build
        cmake ..
        cmake --build .
```

## Matrix test
To check my tests:
```
        ./matrix_test
```

## Test Generator
There is script test/TestGen.py that can generate end2end tests. You can configure tests in file test/config.json. To generate tests, run:
```
        ../run.sh
```
