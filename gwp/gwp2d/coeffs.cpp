#include"header.h"
#include<stdio.h>

void savecoeffs(int k, int ang,cplx* coeffs,int* icoeffs,int ncoeffs,char* fileprefix,int N,int flg)
{
	char str[256];
	char* sw="ab";
	if(flg==0) sw="wb";
	sprintf(str,"%s_c_%d",fileprefix,N);
	FILE *f1=fopen(str,sw);
	sprintf(str,"%s_i_%d",fileprefix,N);
	FILE *f2=fopen(str,sw);
	sprintf(str,"%s_n_%d",fileprefix,N);
	FILE *f3=fopen(str,sw);

	fseek (f2, 0, SEEK_END);
	unsigned int cur=ftell(f2); 
	printf("cur==%d\n",cur);

	fwrite(&cur,sizeof(int),1,f3);
	fwrite(&ncoeffs,sizeof(int),1,f3);
	if(ncoeffs!=0)
	{
		fwrite(coeffs,sizeof(cplx),ncoeffs,f1);
		fwrite(icoeffs,sizeof(int),ncoeffs,f2);
	}
	fclose(f1);fclose(f2);fclose(f3);
}

int loadcoeffs(int k,int ang,int K,cplx* coeffs,int* icoeffs,int N,char* fileprefix)
{
    char str[256];
    sprintf(str,"%s_c_%d",fileprefix,N);
    FILE *f1=fopen(str,"rb");
    sprintf(str,"%s_i_%d",fileprefix,N);
    FILE *f2=fopen(str,"rb");
    sprintf(str,"%s_n_%d",fileprefix,N);
    FILE *f3=fopen(str,"rb");
    fseek(f3,(ang*K+k)*2*sizeof(int),SEEK_SET);

    int ncoeffs=0;unsigned int cur=0;
    fread(&cur,sizeof(int),1,f3);
    fread(&ncoeffs,sizeof(int),1,f3);
    if(ncoeffs!=0)
    {
        fseek(f2,cur,SEEK_SET); //int
        fseek(f1,2*cur,SEEK_SET); //cplx
        fread(coeffs,sizeof(cplx),ncoeffs,f1);
        int aa=fread(icoeffs,sizeof(int),ncoeffs,f2);
        if(aa!=ncoeffs) fprintf(stderr,"aa=%d %d\n",aa,ncoeffs);

    }
    fclose(f1);fclose(f2);fclose(f3);
    return ncoeffs;
}
