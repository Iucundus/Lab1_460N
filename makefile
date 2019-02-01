build: assembler.c
	gcc -std=c99 -o assemble assembler.c

run: assemble AsmExample.asm
	./assemble AsmExample.asm AsmOutput.obj

clean: assemble AsmOutput.obj
	rm assemble
	rm AsmOutput.obj


