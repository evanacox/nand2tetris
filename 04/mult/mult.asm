// Multiplies R0 and R1 and stores the result in R2.
// (R0, R1, R2 refer to RAM[0], RAM[1], and RAM[2], respectively.)

    @R2           // load R2
    M = 0         // set R2 to 0 initially

(MULTIPLY)
    @R1           // load R1
    D = M         // set D to the value at R1

    @END          // load END
    D;JEQ         // goto END if R1 is 0

    @R0           // load R0 
    D = M         // set D to value at R0
    
    @R2           // load R2
    M = M + D     // add R0 to R2

    @R1           // load R1 (Operand #2)
    M = M - 1     // subtract 1 from R1

    @MULTIPLY     // load MULTIPLY
    0;JMP         // goto MULTIPLY

(END)
    @END          // the program expects a constant loop as the "end"
    0;JMP

