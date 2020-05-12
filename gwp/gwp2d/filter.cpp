#include<stdio.h>
#include<stdlib.h>
#include<mkl.h>
#include"header.h"

void createfilter(char* fname, int N, int Nangles, int K, float os, float mu, char* cfile, int* L1, int* L2, float* lambda1,float* lambda2, float* xi_cent)
{
    //create filter
	cplx* fh=(cplx*)malloc(N*N*sizeof(cplx));
    for(int i=0;i<N*N;i++){fh[i].x=0.0;fh[i].y=0.0;} fh[N/2*N+N/2].x=1.0;
    
	//fwd filter
	gwpfwd(fh,N,Nangles,K,os,mu,cfile,L1,L2,lambda1,lambda2,xi_cent);
	delete[] fh;

	//adj filter
	cplx* fhrec=new cplx[N*N];
	for(int i=0;i<N*N;i++) {fhrec[i].x=0;fhrec[i].y=0;}
	gwpadj(fhrec,N,Nangles,K,os, cfile,L1,L2,lambda1,lambda2,xi_cent);
	
	FILE* filefres=fopen(fname,"wb");
	float* fhrrec=new float[N*N];
	for(int i=0;i<N*N;i++) {fhrrec[i]=fhrec[i].x;}
	fwrite(fhrrec,sizeof(float),N*N,filefres);

	fclose(filefres);
	delete[] fhrec;delete[] fhrrec;

}

void applyfilter(char* fname, cplx* f, int N, int Nangles, int K, float os, float mu, char* cfile, int* L1, int* L2, float* lambda1,float* lambda2, float* xi_cent)
{
	cplx* fh= new cplx[N*N];
	float* fhr= new float[N*N];
	FILE* file=fopen(fname,"rb");
    fread(fhr,sizeof(float),N*N,file);
    fclose(file);

	for(int i=0;i<N*N;i++) {fh[i].x=fhr[i]; fh[i].y=0;}
	//fft filter
    int g=0;

    for(int i2=0;i2<N;i2++)for(int i1=0;i1<N;i1++)
    {
        g=(1-2*((i1+1)%2))*(1-2*((i2+1)%2));
        fh[i2*N+i1].x*=g;
        fh[i2*N+i1].y*=g;
    }
	int Nt[2];
	Nt[0]=N; Nt[1]=N;
	DFTI_DESCRIPTOR_HANDLE Desc_Handle;

	DftiCreateDescriptor( &Desc_Handle, DFTI_SINGLE,DFTI_COMPLEX, 2, Nt);
	DftiSetValue(Desc_Handle,DFTI_FORWARD_SCALE,1.0);
	DftiCommitDescriptor( Desc_Handle );
	DftiComputeForward( Desc_Handle, fh);

    for(int i2=0;i2<N;i2++)for(int i1=0;i1<N;i1++)
    {
        g=(1-2*((i1+1)%2))*(1-2*((i2+1)%2));
        fh[i2*N+i1].x*=g;
        fh[i2*N+i1].y*=g;
    }
	//abs
	float* fh_abs=new float[N*N];
    for(int k=0;k<N*N;k++)fh_abs[k]=sqrt(fh[k].x*fh[k].x+fh[k].y*fh[k].y);

	//fft image

    for(int i2=0;i2<N;i2++)for(int i1=0;i1<N;i1++)
    {
        g=(1-2*((i1+1)%2))*(1-2*((i2+1)%2));
        f[i2*N+i1].x*=g;
        f[i2*N+i1].y*=g;
    }
	
	DftiCreateDescriptor( &Desc_Handle, DFTI_SINGLE,DFTI_COMPLEX, 2, Nt);
	DftiSetValue(Desc_Handle,DFTI_FORWARD_SCALE,1.0);
	DftiCommitDescriptor( Desc_Handle );
	DftiComputeForward( Desc_Handle, f);

//fftshift
	for(int i2=0;i2<N;i2++)for(int i1=0;i1<N;i1++)
    {
        g=(1-2*((i1+1)%2))*(1-2*((i2+1)%2));
        f[i2*N+i1].x*=g;
        f[i2*N+i1].y*=g;
    }

    
    //f=f*fh_abs/fh_abs^2 in spectrum
	for(int i2=0;i2<N;i2++)for(int i1=0;i1<N;i1++)
    {
        //float norm=fh_abs[i2*N+i1]>0.001?fh_abs[i2*N+i1]:0.001;
        f[i2*N+i1].x*=fh_abs[i2*N+i1];
        f[i2*N+i1].y*=fh_abs[i2*N+i1];
    }
    
    for(int i2=0;i2<N;i2++)for(int i1=0;i1<N;i1++)
    {
		fh_abs[i2*N+i1]=fh_abs[i2*N+i1]*fh_abs[i2*N+i1];
        float max=fh_abs[i2*N+i1]>0.00001?fh_abs[i2*N+i1]:0.00001;
        f[i2*N+i1].x/=max;
        f[i2*N+i1].y/=max;
    }

//fft image

    for(int i2=0;i2<N;i2++)for(int i1=0;i1<N;i1++)
    {
        g=(1-2*((i1+1)%2))*(1-2*((i2+1)%2));
        f[i2*N+i1].x*=g;
        f[i2*N+i1].y*=g;
    }

    
	DftiCreateDescriptor( &Desc_Handle, DFTI_SINGLE,DFTI_COMPLEX, 2, Nt);
	DftiSetValue(Desc_Handle,DFTI_BACKWARD_SCALE,1.0/(float)(N*N));
	DftiCommitDescriptor( Desc_Handle );
	DftiComputeBackward( Desc_Handle, f);

//fftshift
	for(int i2=0;i2<N;i2++)for(int i1=0;i1<N;i1++)
    {
        g=(1-2*((i1+1)%2))*(1-2*((i2+1)%2));
        f[i2*N+i1].x*=g;
        f[i2*N+i1].y*=g;
    }
	delete[] fh; delete[] fhr;
}
	