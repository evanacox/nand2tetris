// Runs an infinite loop that listens to the keyboard input.
// When a key is pressed (any key), the program blackens the screen,
// i.e. writes "black" in every pixel;
// the screen should remain fully black as long as the key is pressed. 
// When no key is pressed, the program clears the screen, i.e. writes
// "white" in every pixel;
// the screen should remain fully clear as long as no key is pressed.

// Put your code here.
    @SCREEN     // load SCREEN
    D=A         // set D to &screen

    @bcurr_row  // init variable curr_row
    M=D         // set it to the first row of SCREEN

    @wcurr_row  // current whitening row
    M=D

(LOOP)
    @KBD        // load key being pressed
    D=M         // set D to the key being pressed

    @BLACKEN    // load BLACKEN
    D;JNE       // if the value of the key being pressed isnt 0, jump to BLACKEN

(WHITEN)
    @SCREEN     // load SCREEN
    D=A         // set D to &screen
    
    @bcurr_row  // init variable curr_row
    M=D         // set it to the first row of SCREEN

    @24575      // max row addr
    D=A         // set D to that constant

    @wcurr_row  // load curr_row
    D=D-M       // set D to constant - curr_row

    @LOOP       // load LOOP
    D;JLT       // jump to LOOP if constant - curr_row is less than 0

    @wcurr_row  // load the current row
    A=M         // set A to the address `current_row` is at
    M=0         // blacken the entire row (32677 doesn't fill the last bit)

    @wcurr_row  // load curr_row
    M=M+1       // increment it

    @LOOP       // load LOOP
    0;JMP       // jump to LOOP

(BLACKEN)
    @SCREEN     // load SCREEN
    D=A         // set D to &screen
    
    @wcurr_row  // init variable curr_row
    M=D         // set it to the first row of SCREEN

    @24575      // max row addr
    D=A         // set D to that constant

    @bcurr_row  // load curr_row
    D=D-M       // set D to constant - curr_row

    @LOOP       // load LOOP
    D;JLT       // jump to LOOP if constant - curr_row is less than 0

    @bcurr_row  // load the current row
    A=M         // set A to the address `current_row` is at
    M=-1        // blacken the entire row (32677 doesn't fill the last bit)

    @bcurr_row  // load curr_row
    M=M+1       // increment it

    @LOOP       // load LOOP
    0;JMP       // jump to LOOP
