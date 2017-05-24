/*
 * root.c
 *
 *  Created on: 2017年5月18日
 *      Author: Administrator
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

int
root(double (*f)(double x),double (*g)(double x),double *x,int n,double precision)
{
    double x1,x0;
    int i = 0;
    x0 = *x;
    for(i = 0;i < n;i++)
    {
        if(g(x0) == 0)
        {//导函数为0
            return -1;
        }

        x1 = (x0 - f(x0)/g(x0));//牛顿迭代

        if(fabs(x1 - x0) < precision || fabs(f(x1))<precision)
        {//满足精度
            *x= x1;
            return 0;
        }
        else
        {//准备下一次迭代
            x0 = x1;
        }
    }

    //解方程失败
    return -1;
}


int main()
{

    return 0;
}

