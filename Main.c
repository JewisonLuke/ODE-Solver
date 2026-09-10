#include <stdio.h>
#include "Interpret.c"

#define INPUT_SIZE 100

int main() {
    char input[INPUT_SIZE];
    
    printf("Enter Equation: ");
    fgets(input, INPUT_SIZE, stdin);

    int inputIdxNum = strSize(input, 1);
    if (inputIdxNum == INPUT_SIZE) {
        printf("Input too large\n");
        return 1;
    }

    char postFix[INPUT_SIZE];

    if (!interpret(input, inputIdxNum, postFix)) {
        return 2;
    }

    //printf("%s\n", input);

    return 0;
}