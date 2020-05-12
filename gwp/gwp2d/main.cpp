#include<stdio.h>
#include<mkl.h>
#include"header.h"
int roundf(float x){return (int)(x >= 0.0f ? floorf(x + 0.5f) : ceilf(x - 0.5f));}
float log2f(float n){return log(n) / log(2.0f);}

int main()
{
	//init parameters
	int N=128;
	float ep=1e-6;
	int alpha=1;
	int beta=3;
	float os=2.0f;
	float mu=1e-6;
	int Nangles=32;

	//precomputing
	int K= 3*roundf(log2f((float)N/64.0f));
	int nf_start=roundf(log2f(N/64.0f));
	
	float* nf= new float[K];
	float* xi_cent=new float[K];
	float* lambda1=new float[K];
	float* lambda2=new float[K];
	int* L1=new int[K];
	int* L2=new int[K];

	float step=(nf_start+1)/((float)K-1);
	float lam1,lam2;
	for(int i=0;i<K;i++) nf[i]=pow(2,nf_start-i*step);
	if(K==0) {nf[0]=0.5;K=1;}//N=64

	int sizecoeffs=0;
	int K1,K2;
	for(int i=0;i<K;i++)
	{
		xi_cent[i]=N/nf[i]/(2*PI);
		lam1=4*log(2.0f)*(xi_cent[i]*alpha)*(xi_cent[i]*alpha);
		lam2=lam1/(beta*beta);
		lambda1[i]=(PI*PI)/lam1;lambda2[i]=(PI*PI)/lam2;//?

		K1=roundf(sqrt(-log(ep)/lambda1[i]));
		K2=roundf(sqrt(-log(ep)/lambda2[i]));
		L1[i]=4*(int)floorf(os/2.0f*K1);L2[i]=4*(int)floorf(os/2.0f*K2);

		sizecoeffs+=(L1[i]*L2[i]);
		//recompute xi_cent lambda
		xi_cent[i]=((int)(xi_cent[i]*os))/os;
		lam1=4*log(2.0f)*(xi_cent[i]*alpha)*(xi_cent[i]*alpha);
		lambda1[i]=(PI*PI)/lam1;lambda2[i]=(PI*PI)/lam2;
	}

	createfilter("filter128", N, Nangles, K, os, mu, "filter_coeffs128", L1, L2, lambda1,lambda2, xi_cent);
	
	//////read image

	float* fr=new float[N*N];
	cplx* f=new cplx[N*N];

	FILE* file=fopen("sigsbee_128-128.bin","rb");
	fread(fr,sizeof(float),N*N,file);//has to be normalized
	for(int i=0;i<N*N;i++) {f[i].x=fr[i]; f[i].y=0.0f;}

	applyfilter("filter128",f, N, Nangles, K, os, mu, "filter_coeffs128", L1, L2, lambda1,lambda2, xi_cent);
	
	float* frf=new float[N*N];
	FILE* filef=fopen("sigsbee_128-128_smoothexxpf.bin","wb");
	for(int i=0;i<N*N;i++) {frf[i]=f[i].x;}
	fwrite(frf,sizeof(float),N*N,filef);
	fclose(filef);

	//fwd
	gwpfwd(f,N,Nangles,K,os,mu, "coeffs128",L1,L2,lambda1,lambda2,xi_cent);
	delete[] f;delete[] fr;

	//adj
	float* frrec=new float[N*N];
	cplx* frec=new cplx[N*N];
	for(int i=0;i<N*N;i++) {frec[i].x=0;frec[i].y=0;}
	gwpadj(frec,N,Nangles,K,os, "coeffs128",L1,L2,lambda1,lambda2,xi_cent);

	FILE* fileres=fopen("sigsbee_128-128rec.bin”,”wb");
	for(int i=0;i<N*N;i++) {frrec[i]=frec[i].x;}
	fwrite(frrec,sizeof(float),N*N,fileres);

	delete[] frec;delete[] frrec;
	return 0;
}