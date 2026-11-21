#include "Solve.h"
#include "Interpret.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

float cosine(float x) {
    while (x > PI) {
        x -= 2*PI;
    }
    while (x < -PI) {
        x += 2*PI;
    }
    float sum = 0;
    float temp;
    for (int i = 0; i <= 6; i++) {
        temp = 1;
        if (i % 2 == 1) {
            temp *= -1;
        }
        temp *= power(x, 2*i);
        temp /= (float)factorial(2*i);
        sum += temp;
    }
    return sum;
}

float sine(float x) {
    while (x > PI) {
        x -= 2*PI;
    }
    while (x < -PI) {
        x += 2*PI;
    }
    float sum = 0;
    float temp;
    for (int i = 0; i <= 6; i++) {
        temp = 1;
        if (i % 2 == 1) {
            temp *= -1;
        }
        temp *= power(x, 2*i + 1);
        temp /= (float)factorial(2*i + 1);
        sum += temp;
    }
    return sum;
}

float tangent(float x) {
    float cosResult = cosine(x);
    if (absoluteVal(cosResult) <= tolerance) {
        return NAN;
    }
    return sine(x) / cosResult;
}

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
    int numFunctions = 5;
    int currFunc = -1;
    float** funcStack = (float**)malloc(numFunctions*sizeof(float*));
    int* funcStackTop = (int*)malloc(numFunctions*sizeof(int));
    if (eqStack == NULL || funcStack == NULL || funcStackTop == NULL) {
        return NAN;
    }
    float* stack = eqStack;
    int* stackTop = &eqStackTop;
    int typeC;
    int number;
    float value;
    float op1, op2, result;
    int decimalNum;
    float temp;
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
            decimalNum = 0;
            for (int i = 0; i < number; i++) {
                if (postFix[idx] == '.') {
                    decimalNum = 1;
                    idx++;
                    continue;
                }
                if (decimalNum == 0) {
                    value *= 10;
                    value += (float)(postFix[idx] - '0');
                    idx++;
                } else {
                    temp = (float)(postFix[idx] - '0');
                    for (int j = 0; j < decimalNum; j++) {
                        temp /= 10;
                    }
                    value += temp;
                    idx++;
                }
            }
            //printf("pushed %f\n", value);
            if (!pushF(value, stack, stackTop)) {
                return NAN;
            }
            continue;
        }
        else if (typeC == 3) {
            if (postFix[idx] == 'x') {
                //printf("pushed %f\n", xval);
                if (!pushF(xval, stack, stackTop)) {
                    return NAN;
                }
            }
            idx++;
            continue;
        }
        else if (typeC == 2) {
            op2 = popF(stack, stackTop);
            op1 = popF(stack, stackTop);
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
            if (result == NAN || !pushF(result, stack, stackTop)) {
                return NAN;
            }
            idx++;
            continue;
        } if (postFix[idx] == 'f') {
            currFunc++;
            if (currFunc < numFunctions) {
                funcStack[currFunc] = (float*)malloc(INPUT_SIZE*sizeof(float));
                if (funcStack[currFunc] == NULL) {
                    return NAN;
                }
            } else {
                funcStack = (float**)realloc(funcStack, 2*numFunctions*sizeof(float*));
                funcStackTop = (int*)realloc(funcStackTop, 2*numFunctions*sizeof(int));
                numFunctions *= 2;
                if (funcStack == NULL || funcStackTop == NULL) {
                    return NAN;
                }
                funcStack[currFunc] = (float*)malloc(INPUT_SIZE*sizeof(float));
                if (funcStack[currFunc] == NULL) {
                    return NAN;
                }
            }
            funcStackTop[currFunc] = 0;
            stack = funcStack[currFunc];
            stackTop = &funcStackTop[currFunc];
        } else if (typeC == 4) {
            op1 = popF(stack, stackTop);
            free(funcStack[currFunc]);
            currFunc--;
            if (currFunc < 0) {
                stack = eqStack;
                stackTop = &eqStackTop;
            } else {
                stack = funcStack[currFunc];
                stackTop = &funcStackTop[currFunc];
            }
            switch(postFix[idx]) {
                case 'c':
                    result = cosine(op1);
                    break;
                case 's':
                    result = sine(op1);
                    break;
                case 't':
                    result = tangent(op1);
                    break;
            }
            if (result == NAN || !pushF(result, stack, stackTop)) {
                return NAN;
            }
            idx++;
            continue;
        } else if (typeC == 5) {
            //printf("pushed %f\n", value);
            if (!pushF(e, stack, stackTop)) {
                return NAN;
            }
            idx++;
            continue;
        }
        idx++;
    }

    //printf("result: %f\n", eqStack[eqStackTop - 1]);
    float answer = popF(eqStack, &eqStackTop);
    free(eqStack);
    free(funcStack);
    free(funcStackTop);
    return answer;
}

float derivative(char* postFix, int postFixIdx, float xVal) {
    float value = (equation(postFix, postFixIdx, xVal + h) - equation(postFix, postFixIdx, xVal)) / h;
    return value;
}

float solveNewton(char* postFix, int postFixIdx, float startVal) {
    int iterations = 15;
    float xVal = startVal;
    for (int i = 0; i < iterations; i++) {
        xVal = startVal - (equation(postFix, postFixIdx, xVal) / derivative(postFix, postFixIdx, xVal));
    }
    return xVal;
}

void newton(char* postFix, int postFixIdx, float xi, float xf, float dx, float* x, float* y, float* zeros, int* size) {
    int numVals = 0;
    float prevVal = -1;
    int init = 0;

    for (int i = 0; i <= (int)((xf-xi)/dx); i++) {
        x[i] = (float)(i)*dx + xi;
        y[i] = equation(postFix, postFixIdx, x[i]);
        if (!init) {
            prevVal = y[i];
            init = 1;
            continue;
        }
        if (absoluteVal(y[i]) < tolerance) {
            zeros[numVals] = x[i];
            numVals++;
        }
        else if ((y[i] > 0 && prevVal < 0) || (y[i] < 0 && prevVal > 0)) {
            zeros[numVals] = solveNewton(postFix, postFixIdx, x[i]);
            numVals++;
        }
        prevVal = y[i];
    }

    if (numVals == 0) {
        (*size) = 0;
        free(zeros);
        zeros = NULL;
        return;
    }
    float* newZeros = (float*) realloc(zeros, sizeof(float)*numVals);
    if (newZeros != NULL) {
        zeros = newZeros;
    }
    else {
        printf("realloc Failed\n");
    }
    
    (*size) = numVals;

    return;
}

int solveInputEq(char* postFix, int postFixIdx, int eqType, float xi, float xf, float dx, float* x, float* y, float* zeros, int* size) {
    if (eqType == 1) {
        newton(postFix, postFixIdx, xi, xf, dx, x, y, zeros, size);
        return 1;
    } else if (eqType == 2) {
        for (int i = 0; i <= (int)((xf-xi)/dx); i++) {
            x[i] = (float)(i)*dx + xi;
            y[i] = equation(postFix, postFixIdx, x[i]);
        }
        return 1;
    } else if (eqType == 3){

    }

    return 1;
}