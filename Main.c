#include <stdio.h>
#include "Interpret.c"

#define INPUT_SIZE 100

int main() {
    char input[INPUT_SIZE];

    int inputIdxNum = INPUT_SIZE;
    
    printf("Enter ODE: ");
    fgets(input, INPUT_SIZE, stdin);

    for (int i = 0; i < INPUT_SIZE; i++) {
        if (input[i] == '\n') {
            inputIdxNum = i;
            break;
        }
    } //finds which value is the last in the array, then I don't have to search through all indexes that aren't filled

    if (inputIdxNum == INPUT_SIZE) {
        printf("Input too large\n");
        return 1;
    }

    if (!interpret(input, inputIdxNum)) {
        return 2;
    }

    //printf("%s\n", input);

    return 0;
}