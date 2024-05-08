Dynamic_Lib.so: hmm_pic.o
	gcc -shared -o libfoo.so hmm_pic.o
	gcc -o main main.c  
        LD_LIBRARY_PATH=./mylib ./main

Static_Lib.a: hmm.o
	ar -rs libfoo.a hmm.o
	gcc -c main.c 
	gcc -o main main.o 


hmm.o: hmm.c
	gcc -o hmm.o -c hmm.c


hmm_pic.o: hmm.c
	gcc -c -FPIC  hmm.c