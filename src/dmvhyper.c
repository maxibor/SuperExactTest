#include <R.h>
#include <Rmath.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include "mvhyper.h"
void C_dmvhyper(int *x, int *nL, int *L, int *n, double *p, int *logp){
/*
x:     number of elements overlap between all subsets
nL:    number of subsets
L:     subset sizes
n:     background size
p:     output probability
logp:  return log probability
*/
	int i, j, k, l;
	int i0=0;
	int aSize=max(L,*nL) - *x + 1;
	double f1[aSize], f0[aSize];
	double temp;
	int minL=min(L,*nL);
	if(*nL == 2){
		*p=C_dhyper(*x,L[0],*n-L[0],L[1],*logp);
		return;
	}
	for(i=0; i < aSize; i++) f1[i]=(double) 0;
	*p=0;
	//from inner-most to outer-most
	for(i=1; i <= *nL - 1; i++){
		if(i==1){
			l = *x;
			f1[0]=C_dhyper(*x,l,*n - l,L[*nL - 1],i0);
			for(l = *x +1; l <= min2(minL,*n + *x - L[*nL - 1]); l++){
				f1[l - *x] = f1[l - *x -1] * ((double)(*n - l+1-L[*nL - 1] + *x)/(double)(l - *x))  * ((double)l/(double)(*n -l+1));
			}
			continue;
		}
		memcpy ( f0, f1, aSize * sizeof((double) 0) );
		if(*nL - i>=2){
			for(k = *x; k <= minL;k++){ //calculate f_l(k)
				f1[k - *x]=0;
				l=max2(*x,k+L[*nL - i] - *n);
				temp = C_dhyper(l,L[*nL - i],*n - L[*nL - i],k,i0);
				f1[k - *x] += temp * f0[l - *x];
				for(l=l+1;l <= k; l++){ //sum over l for each k
					temp = temp * ((double)(L[*nL - i]-l+1)/(double)l) * ((double)(k-l+1) /(double)(*n - L[*nL - i]-k+l));
					f1[k - *x] += temp * f0[l - *x];
				}
			}
			continue;
		}
		//final integration
		j=max2(*x,L[1]+L[0] - *n);
		temp=C_dhyper(j,L[1],*n - L[1],L[0],i0);
		*p += temp * f1[j - *x];
		for(j=j+1;j <= minL;j++){
			temp=temp * ((double)(L[1]-j+1)/(double)j) * ((double)(L[0]-j+1) /(double)(*n - L[1]-L[0]+j));
			*p += temp * f1[j - *x];
		}
	}
	if(*logp>0) *p = log(*p);
	return;
}
double C_dhyper(int x, int w, int b, int n, int logp){
//probability of getting x white balls out of n draws from an urn with w white balls and b black balls
	double result;
	if(x>w || x>n || b+x<n){
		result=0;
		if(logp==1) result=log(result);
	}else{
		result=C_logChoose(w,x)+C_logChoose(b,n-x);
		result=result-C_logChoose(w+b,n);
		if(logp==0) result=exp(result);
	}
	return(result);
}
