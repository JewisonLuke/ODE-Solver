#ifndef INTERPRET_H
#define INTERPRET_H
#endif

#define INPUT_SIZE 100
#define numValids 19
#define numParen 10

int strSize(char *input, int newLine);
int findIdx(char *input, int inputIdxNum, char op);
int checkValidity(char *input, int inputIdxNum);
int findChunks(char *input, int inputIdxNum, int *chunks);
int equationType(char *input, int inputIdxNum);
void addChunk(char *input, char *oneSideInput, int start, int end, int idx);
int findHighChunk(char *input, int inputIdxNum, int *chunks, int numChunks, int eqType);
int swapChunk(char *input, int inputIdxNum, int *chunks, int highestChunk);
int eqOneSide(char *input, char *oneSideInput, int inputIdxNum, int eqType);
int typeChar(char c);
int push(char c, char *stack, int *top);
char pop(char *stack, int *top);
char oppositeOp(char op);
void generatePostFix(char *input, int idxNum, char *postFix);
int interpret(char *input, int inputIdxNum, char *postFix);