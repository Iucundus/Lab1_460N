    .ORIG x3000
        ADD R0, R0, R0      
        ADD R0, R0, #1
        AND R0, R0, R0
        AND R0, R0, #2
        BR B1
        BRn B1
        BRz B1
        BRp B1
        BRnz B1
        BRnp B1
        BRpz B1
        BRnzp B1
B1      JMP R0
        JSR SR
        JSRR R0
SR      LDB R0, R0, #1
        LDW R0, R0, #1
        LEA R0, B1
        NOT R0, R0
        RET
        RTI
        LSHF R0, R0, #1
        RSHFL R0, R0, #1
        RSHFA R0, R0, #1
        STB R0, R0,  #1
        STW R0, R0, #1
        TRAP x25
        HALT
        .FILL x0001
        XOR R0, R0, R0
        XOR R0, R0, #1
        .END