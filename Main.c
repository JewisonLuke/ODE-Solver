#include <stdio.h>
#include <stdlib.h>
#include "Interpret.h"
#include "Solve.h"
#include "Graph.h"

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
    int postFixIdx;
    if (postFix == NULL) {
        printf("Memory Allocation Failed\n");
        return 3;
    }

    int eqType = interpret(input, inputIdxNum, postFix, &postFixIdx);
    if (eqType < 0) {
        return 2;
    }
    
    float xi = -5.0f;
    float xf = 5.0f;
    float dx = 0.01f;
    float* x = (float*)malloc(sizeof(float)*(int)(((xf - xi) / dx)));
    float* y = (float*)malloc(sizeof(float)*(int)(((xf - xi) / dx)));
    float* zeros = (float*) malloc(sizeof(float)*(int)(((xf - xi) / dx)));
    int size = 0;
    if (x == NULL || y == NULL || zeros == NULL) {
        printf("Malloc Failed\n");
        return 4;
    }
    solveInputEq(postFix, postFixIdx, eqType, xi, xf, dx, x, y, zeros, &size);

    printf("%d Zeroes\n", size);
    if (eqType == 1 && zeros != NULL) {
        for (int i = 0; i < size; i++) {
            printf("Zero %d: %f\n", i + 1, zeros[i]);
        }
    }

    graph();

    //printf("%s\n", input);

    return 0;
}