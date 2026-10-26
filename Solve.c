#include "Solve.h"
#include "Interpret.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

int factorial(int val) {
    if (val == 0) {
        return 1;
    }
    int product = val;
    for (int i = val - 1; i > 0; i--) {
        product *= i;
    }
    return product;
}

float powerI(float val, int power) {
    float result = 1;
    for (int i = 0; i < power; i++) {
        result *= val;
    }
    return result;
}

float ePower(float val) {
    int eCount = 0;
    float upperBound = 1;
    float lowerBound = -1;
    while (val > upperBound) {
        eCount++;
        val -= 1;
    }
    while (val < lowerBound) {
        eCount--;
        val += 1;
    }
    int order = 8;
    float sum = 0;
    for (int i = 0; i <= order; i++) {
        sum += powerI(val, i) / ((float)factorial(i));
    }
    if (eCount >= 0) {
        return sum * powerI(e, eCount);
    } else {
        return sum / powerI(e,eCount);
    }
}

float ln(float val) {
    int ln2Count = 0;
    float upperBound = 1.25;
    float lowerBound = 0.6;
    while (val > upperBound) {
        val = val / 2;
        ln2Count++;
    }
    while (val < lowerBound) {
        val = val * 2;
        ln2Count--;
    }
    //printf("%d\n", ln2Count);
    int order = 10;
    float sum = 0;
    for (int i = 1; i <= order; i++) {
        if (i % 2 == 1) {
            sum += powerI(val - 1, i) / ((float)i);
        } else {
            sum -= powerI(val - 1, i) / ((float)i);
        }
    }
    return sum += ((float)ln2Count)*ln2;
}

float absoluteVal(float val) {
    if (val < 0) {
        return -val;
    }
    return val;
}

float power(float op1, float op2) {
    float tolerance = 0.000001;
    if (absoluteVal(op2-round(op2)) <= tolerance) {
        return powerI(op1, (int)op2);
    }

    return ePower(op2*ln(op1));
}

int pushF(float input, float* eqStack, int* eqStackTop) {
    if (*eqStackTop == INPUT_SIZE) {
        return 0;
    }
    eqStack[*eqStackTop] = input;
    (*eqStackTop)++;
    return 1;
}

float popF(float* eqStack, int* eqStackTop) {
    if (*eqStackTop == 0) {
        return NAN;
    }
    (*eqStackTop)--;
    return eqStack[*eqStackTop];
}

float equation(char* postFix, int postFixIdx, float xval) {
    int idx = 0;
    float* eqStack = (float*)malloc(INPUT_SIZE*sizeof(float));
    int eqStackTop = 0;
    int typeC;
    int number;
    float value;
    float op1, op2, result;
    while(idx < postFixIdx) {
        //printf("character: %c\n", postFix[idx]);
        typeC = typeChar(postFix[idx]);
        if (typeC == 1) {
            number = 0;
            while (postFix[idx] == '9') {
                number+=9;
                idx++;
            }
            number += postFix[idx] - '0';
            idx++;

            value = 0;
            for (int i = 0; i < number; i++) {
                value *= 10;
                value += (float)(postFix[idx] - '0');
                idx++;
            }
            //printf("pushed %f\n", value);
            if (!pushF(value, eqStack, &eqStackTop)) {
                return NAN;
            }
            continue;
        }
        if (typeC == 3) {
            if (postFix[idx] == 'x') {
                //printf("pushed %f\n", xval);
                if (!pushF(xval, eqStack, &eqStackTop)) {
                    return NAN;
                }
            }
            idx++;
            continue;
        }
        if (typeC == 2) {
            op2 = popF(eqStack, &eqStackTop);
            op1 = popF(eqStack, &eqStackTop);
            //printf("popped %f and %f\n", op1, op2);
            switch (postFix[idx]) {
                case '+':
                    result = op1 + op2;
                    break;
                case '-':
                    result = op1 - op2;
                    break;
                case '*':
                    result = op1 * op2;
                    break;
                case '/':
                    result = op1 / op2;
                    break;
                case '^':
                    result = power(op1,op2);
                    break;
                case 'r':
                    result = power(op1, 1/op2);
                    break;
            }
            //printf("pushed %f\n", result);
            if (result == NAN || !pushF(result, eqStack, &eqStackTop)) {
                return NAN;
            }
            idx++;
            continue;
        }
        idx++;
    }

    //printf("result: %f\n", eqStack[eqStackTop - 1]);
    return popF(eqStack, &eqStackTop);
}

int solve(char* postFix, int postFixIdx, int eqType, float xi, float xf, float dx, float* x, float* y) {
    float xval = 0.5;
    float result = equation(postFix, postFixIdx, xval);
    printf("%f\n", result);

    return 1;
}