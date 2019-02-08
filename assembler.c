/*
	Name 1: John Koelling
	Name 2: Grayson Watkins
	UTEID 1: JKK887
	UTEID 2: gaw874
*/
/*
EE 460N, Lab 1
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h> /* String operations library */
#include <ctype.h> /* Library for useful character operations */
#include <limits.h> /* Library for definitions of common variable type characteristics */

/*
Parsing Assembly Language
Take a line of the input file and parse it into corresponding fields. Note that you need to write the isOpcode(char*) function which determines whether a string of characters is a valid opcode.
*/
#define MAX_LINE_LENGTH 255
enum
{
   DONE, OK, EMPTY_LINE
};

typedef struct {
	char name[5];
	short opcode;
	int renderInstructions[4];
	short isRegister[4];
	int orMask;
	int andMask;
} opcodeInstruction;

opcodeInstruction opcodeInstr[28] = {
	{ "add",0x01,{300,303,309,-1},{1,1,1,-1},0x0,0xFFC7 },
	{ "add",0x01,{300,303,507,-1},{1,1,0,-1},0x02,0xFFFF },
	{ "and",0x05,{300,303,309,-1},{1,1,1,-1},0x0,0xFFC7 },
	{ "and",0x05,{300,303,507,-1},{1,1,0,-1},0x02,0xFFFF },
	{ "br",0x00,{903,-1,-1,-1},{0,-1,-1,-1},0xE00,0xFFFF },
	{ "brn",0x00,{903,-1,-1,-1},{0,-1,-1,-1},0x800,0xF9FF },
	{ "brz",0x00,{903,-1,-1,-1},{0,-1,-1,-1},0x400,0xF5FF },
	{ "brp",0x00,{903,-1,-1,-1},{0,-1,-1,-1},0x200,0xF3FF },
	{ "brnz",0x00,{903,-1,-1,-1},{0,-1,-1,-1},0xB00,0xFDFF },
	{ "brnp",0x00,{903,-1,-1,-1},{0,-1,-1,-1},0xA00,0xFBFF },
	{ "brzp",0x00,{903,-1,-1,-1},{0,-1,-1,-1},0x600,0xF7FF },
	{ "brnzp",0x00,{903,-1,-1,-1},{0,-1,-1,-1},0xE00,0xFFFF }
};

// Global list of valid opcodes
/*
char opcodes[28][5] = {
    "add", "and", "br", "brn", "brz", "brp", "brnz", "brnp", "brzp", "brnzp", "halt", "jmp", "jsr", "jsrr", "ldb", "ldw",
            "lea", "nop", "not", "ret", "lshf", "rshfl", "rshfa", "rti", "stb", "stw", "trap", "xor"

};

int renderInstructions[28][4] = {
	//{Arg1,Arg2,...}  //100s place is how many bits to use. Remainder is offset from after opcode to place argument in
	{300,303,309,-1}, //ADD
	{300,303,309,-1}, // AND
	{300,303,309,-1}, // BRn
	{300,303,309,-1}, // BRn
	{300,303,309,-1}, // BRn
	{300,303,309,-1}, // BRn
	{300,303,309,-1}, // BRn
	{300,303,309,-1}, // BRn
	{300,303,309,-1}, // BRn
	{300,303,309,-1}, // HALT
	{303,-1,-1,-1}, // JMP
	{1101,-1,-1,-1}, // JSR
	{303,-1,-1,-1}, // JSRR
	{300,303,606,-1}, // LDB
	{300,303,606,-1}, // LDW
	{300,903,-1,-1} // LEA
};
*/


// Global .ORIG address
int16_t ORIG = 0;

// Global current address
int16_t currentAddress = 0;

// Global symbol table iterator
int symTabIt = 0;

// Global Symbol Table
#define MAX_LABEL_LEN 20
#define MAX_SYMBOLS 255
typedef struct {
    int address;
    char label[MAX_LABEL_LEN + 1];
} TableEntry;
TableEntry symbolTable[MAX_SYMBOLS];

// func decleration
int isOpcode(char * ptr);
int toNum( char * pStr );
void firstPass();
void secondPass();
void printSymbolTable();
int	readAndParse(FILE * pInfile, char * pLine, char ** pLabel, char ** pOpcode, char ** pArg1, char ** pArg2, char ** pArg3, char ** pArg4	) {
   char * lRet, * lPtr;
   int i;
   if( !fgets( pLine, MAX_LINE_LENGTH, pInfile ) )
	return( DONE );
   for( i = 0; i < strlen( pLine ); i++ )
	pLine[i] = tolower( pLine[i] );

   /* convert entire line to lowercase */
   *pLabel = *pOpcode = *pArg1 = *pArg2 = *pArg3 = *pArg4 = pLine + strlen(pLine);

   /* ignore the comments */
   lPtr = pLine;

   while( *lPtr != ';' && *lPtr != '\0' &&
   *lPtr != '\n' )
	lPtr++;

   *lPtr = '\0';
   if( !(lPtr = strtok( pLine, "\t\n ," ) ) )
	return( EMPTY_LINE );

   if( isOpcode( lPtr ) == -1 && lPtr[0] != '.' ) /* found a label */
   {
	*pLabel = lPtr;
	if( !( lPtr = strtok( NULL, "\t\n ," ) ) ) return( OK );
   }

   *pOpcode = lPtr;
   if( !( lPtr = strtok( NULL, "\t\n ," ) ) ) return( OK );

	*pArg1 = lPtr;

	if( !( lPtr = strtok( NULL, "\t\n ," ) ) ) return( OK );

	*pArg2 = lPtr;
	if( !( lPtr = strtok( NULL, "\t\n ," ) ) ) return( OK );

	*pArg3 = lPtr;
	if( !( lPtr = strtok( NULL, "\t\n ," ) ) ) return( OK );

	*pArg4 = lPtr;
	return( OK );
}

/**
 * Checks if a given string is a valid opcode
 * Returns the index of the instruction in the list, or -1 if not a valid opcode
 */
int isOpcode(char * ptr){
    // iterate through opcodes
    for(int i = 0; i < 28; i++) {
    	if(strcmp(ptr, opcodeInstr[i].name) == 0) {
            return i;
        }
    }
	return -1;
}


FILE* infile = NULL;
FILE* outfile = NULL;

int main(int argc, char* argv[]) {

     infile = fopen(argv[1], "r");
     outfile = fopen(argv[2], "w");

     if (!infile) {
       printf("Error: Cannot open file %s\n", argv[1]);
       exit(4);
		 }
     if (!outfile) {
       printf("Error: Cannot open file %s\n", argv[2]);
       exit(4);
     }

    firstPass();
    printSymbolTable();
    // Testing offsetCalc()
    //int offset = offsetCalc(0x1000, "y");
    //printf("computed offset is: 0x%04x\n", offset);
    secondPass();





     /* Close the files*/

     fclose(infile);
     fclose(outfile);
}

void printSymbolTable() {
    printf("symbol table:\n");
    for(int i = 0; i < symTabIt; i++){
        printf("%s   @address: 0x%04x\n", symbolTable[i].label, symbolTable[i].address);
    }
}


/*
 *  Parses code for the first time to create a symbol table
 */
void firstPass() {
    char lLine[MAX_LINE_LENGTH + 1],
            *lLabel, *lOpcode, *lArg1, *lArg2, *lArg3, *lArg4;
    int lRet = EMPTY_LINE; // may need to make sure this is ok

    // go to .ORIG
    while( lRet != DONE && ORIG == 0){
        lRet = readAndParse(infile, lLine, &lLabel,
                            &lOpcode, &lArg1, &lArg2, &lArg3, &lArg4 );

        if( lRet == OK){
            if( strcmp(lOpcode, ".orig") == 0){
                ORIG = toNum(lArg1);
                currentAddress = ORIG;
                printf("Starting address: 0x%04x\n", ORIG);
            }
        }
    }

    while( lRet != DONE ) {
        lRet = readAndParse(infile, lLine, &lLabel,
                            &lOpcode, &lArg1, &lArg2, &lArg3, &lArg4 );


        if( lRet == OK ) {
            // check for Label
            if(strcmp(lLabel, "") != 0) {
                // found a label
                strcpy(symbolTable[symTabIt].label, lLabel);
                symbolTable[symTabIt].address = currentAddress;
                symTabIt++;
                //printf("%s   @address: 0x%04x\n", lLabel, currentAddress);
            }
            currentAddress += 0x02;
        }
    }
}

/*
 * Parses code the second time and write compiled code to output file
 */
void secondPass() {
    // Begin second pass
    rewind(infile);

    char lLine[MAX_LINE_LENGTH + 1],
            *lLabel, *lOpcode, *lArg1, *lArg2, *lArg3, *lArg4;
    int currentPC = ORIG; // the current PC should be updated before the instruction is processed.
    int lRet = EMPTY_LINE;

    while( lRet != DONE ) {
        lRet = readAndParse(infile, lLine, &lLabel,
                            &lOpcode, &lArg1, &lArg2, &lArg3, &lArg4 );


        if( lRet == OK ) {
            currentPC += 0x02;
            if(strcmp(lOpcode, ".orig") == 0){
                currentPC += 2;
                lRet = readAndParse(infile, lLine, &lLabel,
                                    &lOpcode, &lArg1, &lArg2, &lArg3, &lArg4 );
            }
            int output = 0;
            int opcodeType = isOpcode(lOpcode);

            if (opcodeType == -1) // doesn't account for .orig or .end
                exit(2); // This is for bad opcode.

            //Change the opcode type to be one the correct sub-types (in terms of register vs. offset) for each instruction
            char* args[4] = {lArg1, lArg2, lArg3, lArg4};
            for (int i = 0; i < 8; i++) {
            	for (int x = 0; x < 4; x++) {
            		if ((opcodeInstr[opcodeType].isRegister[x] != -1) && (opcodeInstr[opcodeType].isRegister[x] != isRegister(args[x]))) { //TODO: change this if to deal with too many arguments given
            			x = -1;
            			i = -1;
            			opcodeType++;
            			if (strcmp(lOpcode, opcodeInstr[i].name) != 0)
            				continue; //TODO: change error handling. This is incorrect argument specification.
            		}
            	}
            }

            //Set first four bits of output to be the opcode.
            output |= opcodeInstr[opcodeType].opcode << 12;
            output |= opcodeInstr[opcodeType].orMask;

            //Put operands into the instruction
            // TODO: we must make sure that the PC offset is within bounds for the instruction
            for (int x = 0; x < 4; x++) {
                if (opcodeInstr[opcodeType].isRegister[x] != -1) {
                    output |= (assembleOperand(args[x], currentPC) << lShift(opcodeType, x)) && bitMask(opcodeType, x);
                } else break;
            }

            output &= opcodeInstr[opcodeType].andMask;
            fprintf( outfile, "0x%04X\n", output);
        }
    };
}

/*
 * Return the renderInstructions left shift value
 */
int lShift(int opc, int argN) {
	return 12 - opcodeInstr[opc].renderInstructions[argN] / 100 - opcodeInstr[opc].renderInstructions[argN] % 100;
}

int bitMask(int opc, int argN) {
	return 0xFFFF >> (16 - opcodeInstr[opc].renderInstructions[argN] / 100);
}

//Check if a given char string is a register "r."
int isRegister(char* str) {
    // TODO: string is empty. bad access exception thrown.
	if (str[0] == 'r') {
		if (str[1] >= '0')
			if (str[1] <= '7')
				return 1;
	} else
		return 0;
}

/*
 * Calculate PC Offset
 */
int offsetCalc(int currentPC, char* Arg){
    // valid Label formats: BR (9), JSR (11), LEA (9)
    if(Arg[0] == '#' | Arg[0] == 'x'){
        // it is PC offset
        // change to integer and return
        return toNum(Arg);

    }else{
        // it is Label
        // find label in symbol table
        for(int i = 0; i < symTabIt; i++){
            if(strcmp(symbolTable[i].label, Arg) == 0){
                // found the symbol
                int offset = symbolTable[i].address - currentPC;
                return offset; // may have to bit shift or something, I can't remember
            }
        }
        // symbol not found error
        exit(1);
    }
}

/*
 * Assemble a single operand
 */
int assembleOperand(char * arg, int PC) {
	if (isRegister(arg))
		return (int) arg[1];
	else
		return offsetCalc(PC, arg); //This works whether the offset is immediate or a label
}


/* Convert a String To a Number
Hex numbers must be in the form “x3000”, and decimal numbers must be in the form “#30”.
*/

int toNum( char * pStr ) {
    char * t_ptr;
    char * orig_pStr;
    int t_length,k;
    int lNum, lNeg = 0;
    long int lNumLong;

    orig_pStr = pStr;
    if( *pStr == '#' )				/* decimal */
    {
        pStr++;
        if( *pStr == '-' )				/* dec is negative */
        {
            lNeg = 1;
            pStr++;
        }
        t_ptr = pStr;
        t_length = strlen(t_ptr);
        for(k=0;k < t_length;k++)
        {
            if (!isdigit(*t_ptr))
            {
                printf("Error: invalid decimal operand, %s\n",orig_pStr);
                exit(4);
            }
            t_ptr++;
        }
        lNum = atoi(pStr);
        if (lNeg)
            lNum = -lNum;

        return lNum;
    }
    else if( *pStr == 'x' )	/* hex     */
    {
        pStr++;
        if( *pStr == '-' )				/* hex is negative */
        {
            lNeg = 1;
            pStr++;
        }
        t_ptr = pStr;
        t_length = strlen(t_ptr);
        for(k=0;k < t_length;k++)
        {
            if (!isxdigit(*t_ptr))
            {
                printf("Error: invalid hex operand, %s\n",orig_pStr);
                exit(4);
            }
            t_ptr++;
        }
        lNumLong = strtol(pStr, NULL, 16);    /* convert hex string into integer */
        lNum = (lNumLong > INT_MAX)? INT_MAX : lNumLong;
        if( lNeg )
            lNum = -lNum;
        return lNum;
    }
    else
    {
        printf( "Error: invalid operand, %s\n", orig_pStr);
        exit(4);  /* This has been changed from error code 3 to error code 4, see clarification 12 */
    }
}
