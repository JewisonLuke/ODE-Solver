#define _CRT_NONSTDC_NO_DEPRECATE
#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include "Graph.h"

void graph(void)
{
    FILE *fp = fopen("data.txt", "w");

    for (double x = -10; x <= 10; x += 0.1)
    {
        fprintf(fp, "%lf %lf\n", x, x * x);
    }

    fclose(fp);

    FILE *gp = _popen("\"C:\\Program Files\\gnuplot\\bin\\gnuplot.exe\" -persistent", "w");

    fprintf(gp, "set title 'y = x^2'\n");
    fprintf(gp, "set xlabel 'x'\n");
    fprintf(gp, "set ylabel 'y'\n");
    fprintf(gp, "set grid\n");

    fprintf(gp, "set key off\n");      // removes "data.txt" legend
    fprintf(gp, "set border lw 1.5\n");
    fprintf(gp, "set xzeroaxis\n");
    fprintf(gp, "set yzeroaxis\n");
    fprintf(gp, "set xrange [-10:10]\n");
    fprintf(gp, "set yrange [0:100]\n");

    fprintf(gp, "plot 'data.txt' with lines lw 2 lc rgb 'blue'\n");

    _pclose(gp);
}