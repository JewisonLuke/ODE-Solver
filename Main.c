#include <stdio.h>
#include <stdlib.h>
#include "Interpret.h"

#define INPUT_SIZE 100

int main() {
    char* input = (char*)malloc(INPUT_SIZE);
    if (input == NULL) {
        printf("Memory Allocation Failed\n");
        return 3;
    }
    
    
    printf("Enter Equation: ");
    fgets(input, INPUT_SIZE, stdin);

    int inputIdxNum = strSize(input, 1);
    if (inputIdxNum == INPUT_SIZE) {
        printf("Input too large\n");
        return 1;
    }

    char* postFix = (char*)malloc(INPUT_SIZE);
    if (postFix == NULL) {
        printf("Memory Allocation Failed\n");
        return 3;
    }

    int eqType = interpret(input, inputIdxNum, postFix);
    if (eqType < 0) {
        return 2;
    }

    //printf("%s\n", input);

    return 0;
}