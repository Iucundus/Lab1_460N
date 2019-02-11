.ORIG x3000
LEA R0, TEN
LDW R1, R0, #0		;This instruction whatever
START	ADD R1, R1, #1
BRZ done
BR START
		;blank line
DONE TRAP x25		;Last executable insruction
TEN  .FILL x000A 	;This is 10 in 2's comp, hexadecimal
	.END		;The pseudo-op
