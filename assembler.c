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

// Global list of valid opcodes
char opcodes[28][5] = {
    "add", "and", "br", "brn", "brz", "brp", "brnz", "brnp", "brzp", "brnzp", "halt", "jmp", "jsr", "jsrr", "ldb", "ldw",
            "lea", "nop", "not", "ret", "lshf", "rshfl", "rshfa", "rti", "stb", "stw", "trap", "xor"

};

// Hex values that the opcodes correspond to
int opcodesobj[28] = {0x01, 0x02, 0x03, 0x04, 0x01, 0x02, 0x03, 0x04, 0x01, 0x02, 0x03, 0x04, 0x01, 0x02, 0x03, 0x04, 0x01, 0x02, 0x03, 0x04, 0x01, 0x02, 0x03, 0x04, 0x01, 0x02, 0x03, 0x04};

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

int orMasks[28] = { // All are undone
	0x0, //ADD
	0x0, // AND
	0x0, // BRn
	0x0, // BRn
	0x0, // BRn
	0x0, // BRn
	0x0, // BRn
	0x0, // BRn
	0x0, // BRn
	0x0, // HALT
	0x0, // JMP
	0x0, // JSR
	0x0, // JSRR
	0x0, // LDB
	0x0, // LDW
	0x0 // LEA
};

int andMasks[28] = { // All are undone
	0xFFFF, //ADD
	0xFFFF, // AND
	0xFFFF, // BRn
	0xFFFF, // BRn
	0xFFFF, // BRn
	0xFFFF, // BRn
	0xFFFF, // BRn
	0xFFFF, // BRn
	0xFFFF, // BRn
	0xFFFF, // HALT
	0xFFFF, // JMP
	0xFFFF, // JSR
	0xFFFF, // JSRR
	0xFFFF, // LDB
	0xFFFF, // LDW
	0xFFFF // LEA
};

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
int isOpcode(char * ptr){
    // iterate through opcodes
    for(int i = 0; i < 28; i++){
        if(strcmp(ptr, opcodes[i]) == 0){
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
            // TODO: make sure to implement a count to count the address of the current instruction
            // TODO: to calculate PC Offset, use offsetCalc( currentPC, Arg)
            int output = 0;
            //output = output + *lOpcode; //TODO: rewrite this. Set first four bits of output to be the opcode.
            output = output << 12;
            int opcodeType = isOpcode(lOpcode);
            //TODO: Change the opcode type to be one of the sub-types for each instruction

            output |= orMasks[opcodeType];
            //TODO: make the bits to put in the actual object file, whether by PC offset or labels or whatever

            if (renderInstructions[opcodeType][0] != -1) {
                output |= (assembleOperand(lArg1) << lShift(opcodeType, 0)) && bitMask(opcodeType, 0);
            }
            if (renderInstructions[opcodeType][1] != -1) {
                output |= (assembleOperand(lArg1) << lShift(opcodeType, 1)) && bitMask(opcodeType, 1);
            }
            if (renderInstructions[opcodeType][2] != -1) {
                output |= (assembleOperand(lArg1) << lShift(opcodeType, 2)) && bitMask(opcodeType, 2);
            }
            if (renderInstructions[opcodeType][3] != -1) {
                output |= (assembleOperand(lArg1) << lShift(opcodeType, 3)) && bitMask(opcodeType, 3);
            }

            output &= andMasks[opcodeType];
            fprintf( outfile, "0x%04X\n", output);

            //currentAddress += 0x02;
        }
    };
}

/*
 * Return the renderInstructions left shift value
 */
int lShift(int opc, int argN) {
	return 12 - renderInstructions[opc][argN] / 100 - renderInstructions[opc][argN] % 100;
}

int bitMask(int opc, int argN) {
	return 0xFFFF >> (16 - renderInstructions[opc][argN] / 100);
}

/*
 * Calculate PC Offset
 */
int offsetCalc(int currentPC, char* Arg){
    // ToDo: if PC Offset: return offset, else if Label: calculate and return offset
    // valid Label formats: BR (9), JSR (11), LEA (9)
    if(Arg[0] == '#'){
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
        exit(1);
    }
}

/*
 * Assemble a single operand
 */
int assembleOperand(char * arg) {
	if (arg[0] == 'r')
		return (int) arg[1];
	return toNum(arg);
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
