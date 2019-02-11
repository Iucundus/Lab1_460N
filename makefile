build: assembler.c
	gcc -std=c99 -o assemble assembler.c

run: assemble AsmExample.asm
	./assemble AsmExample.asm AsmOutput.obj
	./assemble Ammount4Overflow.asm	Ammount4OverflowOut.obj
	./assemble Offset6OutofBounds.asm Offset6OutofBoundsOut.obj
	./assemble incorrectOpcode.asm	incorrectOpcodeOut.obj
	./assemble tooManyOperands.asm tooManyOperandsOut.obj
	./assemble AsmTestAllInstructions.asm AsmTestAllInstructionsOut.obj
	./assemble Offset9OutofBounds.asm Offset9OutofBoundsOut.obj	
	./assemble incorrectOperand.asm incorrectOperandOut.obj
	./assemble Imm5Overflow.asm Imm5OverflowOut.obj
	./assemble Trapvect8OutofBounds.asm Trapvect8OutofBoundsOut.obj
	./assemble e1.asm e1Out.obj
	./assemble Offset11OutofBounds.asm Offset11OutofBoundsOut.obj
	./assemble tooFewOperands.asm tooFewOperandsOut.obj

clean: assemble AsmOutput.obj
	rm assemble
	rm AsmOutput.obj
