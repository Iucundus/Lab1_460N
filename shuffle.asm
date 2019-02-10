    .ORIG x3000
        LEA R1, Arr
        LDW R1, R1, #0 ; R1 now contains address of array

START   LDW R2, R1, #0 ; R2 contains data from Arr[i]
        LDW R3, R1, #0 ; R3 also contains data from Arr[i]
        RSHFL R3, R3, #8 ; leftmost bits shifted to right
	RSHFL R3, R3, #8
        LSHF R2, R2, #8 ; Rightmost bits shifted to left
	LSHF R2, R2, #8 
        ADD R2, R2, R3
        STW R2, R1, #0 ; store result back in Arr[i]
        ADD R1, R1, #2 ; increment address
        ADD R0, R0, #-1 ; decrement array counter (i)
        BRp START ; loop until array is empty

        HALT


Arr     .FILL x3050

    .END
