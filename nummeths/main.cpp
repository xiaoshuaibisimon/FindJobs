#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

int
interpol(const double *x,const double *fx,int n,
         double *z,double *pz,int m)
{
    double term,*table,*coeff;
    int     i,j,k;

    //分配最大行差商表需要的内存--其实就是第一行（从第0行开始计数--x【i】）
    if((table = (double *)malloc(sizeof(double)*n)) == NULL)//fx【i】
        return -1;

    //分配差值多项式系数需要的内存--如果已知n个点的函数值，则差值多项式一共有n个系数--第0个是fx【0】
    if((coeff = (double *)malloc(sizeof(double)*n)) == NULL)
    {
        free(table);
        return -1;
    }

    //初始化差商表（第一行）
    memcpy(table,fx,sizeof(double)*n);

    //最开始的系数就是fx【0】
    coeff[0] = table[0];

    //如果已知n个点的值--则一共有（n+1）行差商表（序号是0--n）
    //由于第0行是x【i】，第一行是fx【i】，所以要计算第二行的时候从第一行开始即可
    //一共需要计算的行数是(n- 1),最后一行（第n行）基于第（n-1）计算
    for(k = 1;k < n;k++)
    {//外层循环记录以第几行为基础
        for(i = 0;i < n - k;i++)
        {//计算当前行（第k+1行）每一个条目
            j = i + k;//计算出差商的最大下标--j和i之间相差的刚好是上一行的序号k（序号从0开始计算）
            table[i] = (table[i+1] - table[i])/(x[j] - x[i]);//根据差商表和差商公式计算出当前行的条目值（差商）
        }

        //每一行的所有条目计算完毕以后--最开始的差商就是第k个系数
        coeff[k] = table[0];
    }

    //释放差商表的内存空间
    free(table);

    //针对待求点进行遍历--每一个待求点都需要进行一次差值多项式的求值
    for(k = 0;k < m;k++)
    {
        //首项是系数coeff[0]--常数项
        pz[k] = coeff[0];

        for(j = 1;j <n;j++)
        {//累加多项式里面的每一项--已知n个点一共有n个项，其中有一个常数项coeff[0]

            term = coeff[j];//缓存每一项系数
            for(i = 0;i < j;i++)
            {//累乘计算每一项的值--第j项对应的阶数是j（x【0】---x【j-1】）
                term = term * (z[k]-x[i]);
            }

            //至此计算出了每一项的值--将其累加
            pz[k] += term;
        }//至此计算完毕一个待求点的多项式结果--开始计算下一个点的插值多项式值

    }

    //回收系数的内存
    free(coeff);

    return 0;
}

int main()
{
    return 0;
}
