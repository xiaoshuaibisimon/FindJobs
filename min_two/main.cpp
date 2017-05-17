#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <math.h>


void
lsqe(const double *x,const double *y,int n,double *b1,double *b0)
{
    double sumx,sumy,sumx2,sumxy;

    int i;

    sumx = 0.0;
    sumy = 0.0;

    sumxy = 0.0;
    sumx2 = 0.0;

    for(i = 0;i < n;i++)
    {//遍历x和y，求出累加和
        sumx += x[i];
        sumy += y[i];

        sumx2 += pow(x[i],2.0);
        sumxy += (x[i]*y[i]);
    }

    *b1 = (sumxy - (sumx*sumy)/((double)n))/(sumx2-(sumx*sumx)/((double)n));//重要公式
    *b0 = (sumy - (*b1)*sumx)/((double)n);//重要公式

    return ;
}


int main()
{
    return 0;
}



