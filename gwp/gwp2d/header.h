#include<math.h>
#define DBG 1
#define PI (atan(1.0f)*4)
struct cplx{float x, y;};

//work with coeffs
void savecoeffs(int k, int ang,cplx* coeffs,int* icoeffs,int ncoeffs,char* fileprefix,int N,int flg);
int loadcoeffs(int k,int ang,int K,cplx* coeffs,int* icoeffs,int N,char* fileprefix);

//transforms
void gwpfwd(cplx* f, int N, int Nangles, int K, float os,  float mu,char* cfile, int* L1, int* L2, float* lambda1,float* lambda2, float* xi_cent);
void gwpadj(cplx* frec, int N, int Nangles, int K, float os, char* cfile, int* L1, int* L2, float* lambda1,float* lambda2, float* xi_cent);

//filter
void createfilter(char* fname, int N, int Nangles, int K, float os, float mu, char* cfile, int* L1, int* L2, float* lambda1,float* lambda2, float* xi_cent);
void applyfilter(char* fname, cplx* f, int N, int Nangles, int K, float os, float mu, char* cfile, int* L1, int* L2, float* lambda1,float* lambda2, float* xi_cent);
