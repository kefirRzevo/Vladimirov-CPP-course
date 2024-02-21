# ParaCL
This is my solution of 'ParaCL' task. On the input we have a `program.cl`. On the output we execute it with the interpreter.

For example, 
```

        input: `program.cl`:

            fst = 0; //type is not needed, all types - int
            snd = 1;
            iters = ?; // read from stdin number

            while (iters > 0) { // for `if` the same
                tmp = fst;
                fst = snd;
                snd = snd + tmp;
                iters = iters - 1;
            }

            print snd;

        output: Fibonacci sequence
```
Enable options of `paracl`:
`--dot`    - to dump AST in dot format
`--cl`     - to decode AST to the same program
`--disasm` - to disasm program to my assembler
`--cpp`    - to generate cpp code


## Build
To build program:
```
        git clone https://github.com/kefirRzevo/ParaCL.git
        cd ParaCL
        mkdir build
        cd build
        cmake ..
        cmake --build .
```

## ParaCL test
To check my tests:
```
        ./paracl_test
        ./vm_test
```
