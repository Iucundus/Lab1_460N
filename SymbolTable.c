//
// Created by Grayson Watkins on 2/1/19.
//

#define MAX_LABEL_LEN 20
#define MAX_SYMBOLS 255
typedef struct {
    int address;
    char label[MAX_LABEL_LEN + 1];    /* Question for the reader: Why do we need to add 1? */
} TableEntry;
TableEntry symbolTable[MAX_SYMBOLS];

