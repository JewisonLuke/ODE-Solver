#define _CRT_NONSTDC_NO_DEPRECATE
#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include "Graph.h"

void shiftArrLeft(char* arr) {
    int idx = 0;
    while (arr[idx] != '\0') {
        arr[idx] = arr[idx+1];
        idx++;
    }
}

void shiftArrRight(char* arr, char c) {
    int idx = 0;
    char temp1;
    char temp2;

    temp1 = arr[0];
    temp2 = arr[1];
    arr[0] = c;
    idx++;
    while (temp1 != '\0') {
        arr[idx] = temp1;
        temp1 = temp2;
        temp2 = arr[idx + 1];
        idx++;
    }
    arr[idx] = '\0';
}

void cleanEquation(char* equation) {
    int size = 0;
    int parenNum = 0;
    int first = 1;
    int invert = 0;
    int idx = 0;
    while (equation[size] != '\0') {
        size++;
    }
    if (equation[size - 1] == '=') {
        equation[size - 1] = '\0';
        invert = 1;
        size--;
    }
    if (invert) {
        for (int i = 0; i < size; i++) {
            if (equation[i] == ' ') {
                shiftArrLeft(equation + i);
                i--;
                continue;
            }
            if (first) {
                if (equation[i] == '-') {
                    shiftArrLeft(equation + i);
                } else if (equation[i] != ' ') {
                    shiftArrRight(equation + i, '-');
                }
                first = 0;
                continue;
            }
            if (equation[i] == '(') {
                parenNum++;
            } else if (equation[i] == ')') {
                parenNum--;
            }
            else if (parenNum == 0) {
                if (equation[i] == '+') {
                    equation[i] = '-';
                }
                else if (equation[i] == '-') {
                    equation[i] = '+';
                }
            }
            if (equation[i] != ' ') {
                first = 0;
            }
        }
    }
    while (idx < size) {
        if (equation[idx] == ' ' && equation[idx + 1] == ' ') {
            shiftArrLeft(equation + idx);
            size--;
            continue;
        }
        idx++;
    }
}

void graph(float* x, float* y, float xi, float xf, int numPoints, char* equation)
{
    FILE *fp = fopen("graph.txt", "w");

    float min = y[0], max = y[0];

    for (int i = 0; i < numPoints; i++)
    {
        fprintf(fp, "%f %f\n", x[i], y[i]);
        if (y[i] > max) {
            max = y[i];
        }
        else if (y[i] < min) {
            min = y[i];
        }
    }

    fclose(fp);

    FILE *gp = _popen("\"C:\\Program Files\\gnuplot\\bin\\gnuplot.exe\" -persistent", "w");

    cleanEquation(equation);

    fprintf(gp, "set title '%s'\n", equation);
    fprintf(gp, "set xlabel 'x'\n");
    fprintf(gp, "set ylabel 'y'\n");
    fprintf(gp, "set grid\n");

    fprintf(gp, "set key off\n");      // removes "data.txt" legend
    fprintf(gp, "set border lw 1.5\n");
    fprintf(gp, "set xzeroaxis\n");
    fprintf(gp, "set yzeroaxis\n");
    fprintf(gp, "set xrange [%f:%f]\n", xi, xf);
    fprintf(gp, "set yrange [%f:%f]\n", min, max);

    fprintf(gp, "plot 'graph.txt' with lines lw 2 lc rgb 'blue'\n");

    _pclose(gp);
}