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

// Global .ORIG address
int ORIG = 0;

// Global current address
int currentAddress = 0;

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


int	readAndParse( FILE * pInfile, char * pLine, char ** pLabel, char ** pOpcode, char ** pArg1, char ** pArg2, char ** pArg3, char ** pArg4	) {
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
            return 1;
        }
    }
	return 0;
}


FILE* infile = NULL;
FILE* outfile = NULL;

int main(int argc, char* argv[]) {

    printf("hello world\n\n");
     /* open the source file */
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

     /* Do stuff with files */
	char lLine[MAX_LINE_LENGTH + 1],
		*lLabel, *lOpcode, *lArg1, *lArg2, *lArg3, *lArg4;
	int lRet;

    while( lRet != DONE ) {
		lRet = readAndParse(infile, lLine, &lLabel,
			&lOpcode, &lArg1, &lArg2, &lArg3, &lArg4 );


		if( lRet != DONE && lRet != EMPTY_LINE )
		{
			fprintf(infile, "%s %s %s %s %s %s\n", lLabel, lOpcode, lArg1, lArg2, lArg3, lArg4);
		}
	};


     /* Close the files*/

     fclose(infile);
     fclose(outfile);
}
