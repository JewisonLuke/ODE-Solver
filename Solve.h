#ifndef SOLVE_H
#define SOLVE_H

#define ln2 0.6931471806f
#define e 2.7182818285f
#define tolerance 0.000001f
#define h 0.0001f

float ePower(float val);
float ln(float val);
float absoluteVal(float val);
float power(float op1, float op2);
int pushF(float input, float* eqStack, int* eqStackTop);
float popF(float* eqStack, int* eqStackTop);
float equation(char* postFix, int postFixIdx, float xval);
float derivative(char* postFix, int postFixIdx, float xVal);
float solveNewton(char* postFix, int postFixIdx, float startVal);
void newton(char* postFix, int postFixIdx, float xi, float xf, float dx, float* x, float* y, float* zeros, int* size);
int solveInputEq(char* postFix, int postFixIdx, int eqType, float xi, float xf, float dx, float* x, float* y, float* zeros, int* size);

#endif