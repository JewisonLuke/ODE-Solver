#include <stdio.h>
#include "Interpret.c"

#define INPUT_SIZE 100

int main() {
    char input[INPUT_SIZE];

    int inputIdxNum = INPUT_SIZE;
    
    printf("Enter ODE: ");
    scanf("%s", input);

    for (int i = 0; i < INPUT_SIZE; i++) {
        if (input[i] == '\0') {
            inputIdxNum = i;
            break;
        }
    } //finds which value is the last in the array, then I don't have to search through all indexes that aren't filled

    if (inputIdxNum == INPUT_SIZE) {
        printf("Input too large\n");
        return 1;
    }

    interpret(input);

    printf("%s\n", input);

    return 0;
}