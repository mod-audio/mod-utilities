#include <cmath>

void LPF1(float *u, float *y, int N, double f_before, double f_now, double T, double *Y_1, double *U_1, double *Y_2, double *U_2 );
void HPF1(float *u, float *y, int N, double f_before, double f_now, double T, double *Y_1, double *U_1, double *Y_2, double *U_2 );
void LPF2(float *u, float *y, int N, double f_before, double f_now, double T, double *Y_1, double *U_1, double *Y_2, double *U_2 );
void HPF2(float *u, float *y, int N, double f_before, double f_now, double T, double *Y_1, double *U_1, double *Y_2, double *U_2 );
void BPF1(double *u, double *y, int N, double f_before, double f_now, double BW_before, double BW_now, double T, double *Y_1, double *U_1, double *Y_2, double *U_2, double *Y_3, double *U_3, double *Y_4, double *U_4 );
void BPF2(double *u, double *y, int N, double f_before, double f_now, double BW_before, double BW_now, double T, double *Y_1, double *U_1, double *Y_2, double *U_2, double *Y_3, double *U_3, double *Y_4, double *U_4 );
