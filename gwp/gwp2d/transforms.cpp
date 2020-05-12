#include<stdio.h>
#include<stdlib.h>
#include<mkl.h>
#include"header.h"

void gwpfwd(cplx* f, int N, int Nangles, int K, float os, float mu, char* cfile, int* L1, int* L2, float* lambda1,float* lambda2, float* xi_cent)
{
	float *x,*y,*xr,*yr;
	float g,gwpf,a,b,c,phi;
	int num;
	float R[4];

	//box
	x=new float[L1[K-1]]; for(int i=0;i<L1[K-1];i++) {x[i]=(-L1[K-1]/2+i+(int)(xi_cent[K-1]*os))/os;}
	y=new float[L2[K-1]]; for(int i=0;i<L2[K-1];i++) {y[i]=(-L2[K-1]/2+i)/os;}
	//rotated box
	xr=new float[L1[K-1]*L2[K-1]];
	yr=new float[L1[K-1]*L2[K-1]];

	//coeffs of a large box
	cplx* coefftmpK=new cplx[L1[K-1]*L2[K-1]];

	int J=L1[K-1]*L2[K-1];

	cplx* xt = new cplx[J];
	for(int k=0;k<N*N;k++)
	{
		f[k].x/=sqrt((float)(N*N));
		f[k].y/=sqrt((float)(N*N));
	}
	for(int ang=0;ang<Nangles;ang++)//points on the circle
	{
		fprintf(stderr,"angle %d) ",ang);
		float theta=ang*PI/Nangles;
		R[0]=cos(theta); R[1]=sin(theta);
		R[2]=-sin(theta); R[3]=cos(theta);
		for(int i2=0;i2<L2[K-1];i2++)for(int i1=0;i1<L1[K-1];i1++)
		{
			xr[i2*L1[K-1]+i1]=(R[0]*x[i1]+R[1]*y[i2])/(float)N;
			yr[i2*L1[K-1]+i1]=(R[2]*x[i1]+R[3]*y[i2])/(float)N;
		}
		for(int i=0;i<L1[K-1]*L2[K-1];i++){coefftmpK[i].x=0.0f; coefftmpK[i].y=0.0f;}

//eq2us from F space to the box!!!!!!!!start

		for (int k1=0;k1<N;k1++) 
			for (int k2=0;k2<N;k2++)
			{
				for(int j=0;j<J;j++)
				{
					xt[j].x=0;
					xt[j].y=-2*PI*((k1-N/2)*xr[j]+(k2-N/2)*yr[j]); 
				}
				vcExp(J,(MKL_Complex8*)xt,(MKL_Complex8*)xt);
				cblas_caxpy(J,&f[k2*N+k1].x,xt,1,coefftmpK,1);
			}
//eq2us from F space to the box!!!!!!!!sfinish

			for(int k=0;k<K;k++)
			{
				cplx* coefftmp=new cplx[L1[k]*L2[k]];
				int xi_centk=(int)(xi_cent[k]*os);
				int xi_centK=(int)(xi_cent[K-1]*os);
				int xstK=xi_centk-L1[k]/2+L1[K-1]/2-xi_centK;
				int ystK=-L2[k]/2+L2[K-1]/2;
				for(int i2=0;i2<L2[k];i2++)for(int i1=0;i1<L1[k];i1++)
				{
					float x11=(-L1[k]/2+i1+(int)(os*xi_cent[k]))/os-xi_cent[k];
					float y11=(-L2[k]/2+i2)/os;
					gwpf=exp(-lambda1[k]*x11*x11-lambda2[k]*y11*y11);
					coefftmp[i2*L1[k]+i1].x=gwpf*coefftmpK[(ystK+i2)*L1[K-1]+(xstK+i1)].x;
					coefftmp[i2*L1[k]+i1].y=gwpf*coefftmpK[(ystK+i2)*L1[K-1]+(xstK+i1)].y;
				}

				int xstk=xi_centk-L1[k]/2;
				int ystk=-L2[k]/2;
				for(int i2=0;i2<L2[k];i2++)for(int i1=0;i1<L1[k];i1++)
				{g=(1-2*((i1+1)%2))*(1-2*((i2+1)%2));coefftmp[i2*L1[k]+i1].x*=g;coefftmp[i2*L1[k]+i1].y*=g;}


				DFTI_DESCRIPTOR_HANDLE Desc_Handle;
				int Nt[2];
				Nt[0]=L2[k]; Nt[1]=L1[k];
				DftiCreateDescriptor( &Desc_Handle, DFTI_SINGLE,DFTI_COMPLEX, 2, Nt);
				DftiSetValue(Desc_Handle,DFTI_BACKWARD_SCALE,1.0/sqrt((float)L1[k]*L2[k]));
				DftiCommitDescriptor( Desc_Handle );
				DftiComputeBackward( Desc_Handle, coefftmp);
				DftiFreeDescriptor(&Desc_Handle);


				for(int i2=0;i2<L2[k];i2++)for(int i1=0;i1<L1[k];i1++)
				{g=(1-2*((i1+1)%2))*(1-2*((i2+1)%2));coefftmp[i2*L1[k]+i1].x*=g;coefftmp[i2*L1[k]+i1].y*=g;}

				float phase,qqr,qqi,qr,qi;
				for (int j1=0;j1<L1[k];j1++)
				{
					phase=6.283185307179586*(j1/(1.0*L1[k])-0.5)*(L1[k]/2+xstk);qqr=cos(phase);qqi=sin(phase);
					for (int j2=0;j2<L2[k];j2++)
					{
						qr=coefftmp[j1+j2*L1[k]].x;
						qi=coefftmp[j1+j2*L1[k]].y;
						coefftmp[j1+j2*L1[k]].x=qr*qqr-qi*qqi;
						coefftmp[j1+j2*L1[k]].y=qr*qqi+qi*qqr;
					}
				}
				num=0;

				cplx* coeffs=new cplx[L1[k]*L2[k]];
				int* icoeffs=new int[L1[k]*L2[k]];

				for(int i2=0;i2<L2[k];i2++)for(int i1=0;i1<L1[k];i1++)
				{
					float cx=coefftmp[i2*L1[k]+i1].x;float cy=coefftmp[i2*L1[k]+i1].y;
					if(cx*cx+cy*cy>=mu*mu)
					{
						coeffs[num].x=cx;coeffs[num].y=cy;
						icoeffs[num]=i2*L1[k]+i1;
						num++;
					}
				}
				int ncoeffs=num;
				fprintf(stderr,"ncoeff=%d\n",ncoeffs);
				savecoeffs(k,ang,coeffs,icoeffs,ncoeffs,cfile,N,k+ang);
				delete[] coefftmp;delete[] coeffs;delete[] icoeffs;
			}
	}
	delete[] x;delete[] y;delete[] xr;delete[] yr;delete[] coefftmpK;delete[] xt;
}

void gwpadj(cplx* f, int N, int Nangles, int K, float os, char* cfile, int* L1, int* L2, float* lambda1,float* lambda2, float* xi_cent)
{
	float *x,*y,*xr,*yr;
	float g,gwpf,a,b,c,phi;
	int num;
	float R[4];

	//box
	x=new float[L1[K-1]]; for(int i=0;i<L1[K-1];i++) {x[i]=(-L1[K-1]/2+i+(int)(xi_cent[K-1]*os))/os;}
	y=new float[L2[K-1]]; for(int i=0;i<L2[K-1];i++) {y[i]=(-L2[K-1]/2+i)/os;}
	//rotated box
	xr=new float[L1[K-1]*L2[K-1]];
	yr=new float[L1[K-1]*L2[K-1]];

	int J=L1[K-1]*L2[K-1];
	cplx* xt = new cplx[N*N];
	
	//coeffs of a large box
	cplx* coefftmpK=new cplx[L1[K-1]*L2[K-1]];
	for(int ang=0;ang<Nangles;ang++)//points on the sphere
	{
		fprintf(stderr,"angle %d) ",ang);
		float theta=ang*PI/Nangles;
		for(int i=0;i<L1[K-1]*L2[K-1];i++){coefftmpK[i].x=0.0f; coefftmpK[i].y=0.0f;}
		int i22=L2[K-1]/2-1;
		int i11=L1[K-1]/2-1;

		for(int k=0;k<K;k++)
		{
			cplx* coefftmp=new cplx[L1[k]*L2[k]];
			cplx* coeffs=new cplx[L1[k]*L2[k]];
			int* icoeffs=new int[L1[k]*L2[k]];
			int ncoeffs=loadcoeffs(k,ang,K,coeffs,icoeffs,N,cfile);
			fprintf(stderr,"ncoeff=%d\n",ncoeffs);
			for(int i=0;i<L1[k]*L2[k];i++){coefftmp[i].x=0.0f;coefftmp[i].y=0.0f;}
			for(int i=0;i<ncoeffs;i++)
			{
				coefftmp[icoeffs[i]].x=coeffs[i].x;
				coefftmp[icoeffs[i]].y=coeffs[i].y;
			}

			int xi_centk=(int)(xi_cent[k]*os);
			int xi_centK=(int)(xi_cent[K-1]*os);

			int xstK=xi_centk-L1[k]/2+L1[K-1]/2-xi_centK;
			int ystK=-L2[k]/2+L2[K-1]/2;

			int xstk=xi_centk-L1[k]/2;
			int ystk=-L2[k]/2;

			float phase,qqr,qqi,qr,qi;
			for (int j1=0;j1<L1[k];j1++)
			{
				phase=-6.283185307179586*(j1/(1.0*L1[k])-0.5)*(L1[k]/2+xstk);qqr=cos(phase);qqi=sin(phase);
				for (int j2=0;j2<L2[k];j2++)
				{
					qr=coefftmp[j1+j2*L1[k]].x;
					qi=coefftmp[j1+j2*L1[k]].y;
					coefftmp[j1+j2*L1[k]].x=qr*qqr-qi*qqi;
					coefftmp[j1+j2*L1[k]].y=qr*qqi+qi*qqr;
				}
			}

			for(int i2=0;i2<L2[k];i2++)for(int i1=0;i1<L1[k];i1++)
			{
				g=(1-2*((i1+1)%2))*(1-2*((i2+1)%2));
				coefftmp[i2*L1[k]+i1].x*=g;
				coefftmp[i2*L1[k]+i1].y*=g;
			}
			int Nt[2];
			Nt[0]=L2[k]; Nt[1]=L1[k];
			DFTI_DESCRIPTOR_HANDLE Desc_Handle;

			DftiCreateDescriptor( &Desc_Handle, DFTI_SINGLE,DFTI_COMPLEX, 2, Nt);
			DftiSetValue(Desc_Handle,DFTI_FORWARD_SCALE,1.0/sqrt((float)L1[k]*L2[k]));
			DftiCommitDescriptor( Desc_Handle );
			DftiComputeForward( Desc_Handle, coefftmp);

			for(int i2=0;i2<L2[k];i2++)for(int i1=0;i1<L1[k];i1++)
			{
				g=(1-2*((i1+1)%2))*(1-2*((i2+1)%2));
				coefftmp[i2*L1[k]+i1].x*=g;
				coefftmp[i2*L1[k]+i1].y*=g;
			}

			for(int i2=0;i2<L2[k];i2++)for(int i1=0;i1<L1[k];i1++)
			{

				float x11=(-L1[k]/2+i1+(int)(os*xi_cent[k]))/os-xi_cent[k];
				float y11=(-L2[k]/2+i2)/os;
				gwpf=exp(-lambda1[k]*x11*x11-lambda2[k]*y11*y11);

				coefftmpK[(ystK+i2)*L1[K-1]+(xstK+i1)].x+=gwpf*coefftmp[i2*L1[k]+i1].x;
				coefftmpK[(ystK+i2)*L1[K-1]+(xstK+i1)].y+=gwpf*coefftmp[i2*L1[k]+i1].y;

			}
			free(coeffs);
			free(icoeffs);
			free(coefftmp);
		}

		R[0]=cos(theta); R[1]=sin(theta);
		R[2]=-sin(theta); R[3]=cos(theta);

		for(int i2=0;i2<L2[K-1];i2++)for(int i1=0;i1<L1[K-1];i1++)
		{
			xr[i2*L1[K-1]+i1]=(R[0]*x[i1]+R[1]*y[i2])/(float)N;
			yr[i2*L1[K-1]+i1]=(R[2]*x[i1]+R[3]*y[i2])/(float)N;
		}
//us2eq from box to the f space!!!!!!!!start
		int J = L1[K-1]*L2[K-1];
		for (int j=0;j<J;j++) 
		{
			for(int k1=0;k1<N;k1++)
				for(int k2=0;k2<N;k2++)
				{
					
					xt[k2*N+k1].x=0;
					xt[k2*N+k1].y=2*PI*((k1-N/2)*xr[j]+(k2-N/2)*yr[j]);
				}
				cplx ac;
				ac.x=coefftmpK[j].x/sqrt((float)(N*N));
				ac.y=coefftmpK[j].y/sqrt((float)(N*N));
				vcExp(N*N,(MKL_Complex8*)xt,(MKL_Complex8*)xt);
				cblas_caxpy(N*N,&ac,xt,1,f,1);
		}
//us2eq from spectrum to the box!!!!!!!!finish		
	}

	delete[] x;delete[] y;delete[] xr;delete[] yr;delete[] coefftmpK;delete[] xt;
}
