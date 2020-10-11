#include <math.h>
#include "pav_analysis.h"

//para simplificar los cálculos, asumo sgn(0) = 1
int sgn(float x)
{
    return (x < 0) ? -1 : 1;
}

float compute_power(const float *x, unsigned int N) {
 float sum=0;
 for (int n = 0; n < N-1; n++)
 {
    sum = sum + pow(x[n],2);
    
 }
 return 10*log10(sum/N);
}

float compute_am(const float *x, unsigned int N) {
float sum2=0;
for (int n = 0; n < N-1; n++)
 {
    sum2 = sum2 + fabs(x[n]);
 }
return sum2/N;
}

float compute_zcr(const float *x, unsigned int N, float fm) {
 float ret = 0;
    for (int i = 1; i < N; i++)
        ret += ((int) sgn(x[i]) != sgn(x[i-1]));
    return 0.5 * fm * ret / (N - 1);
}

