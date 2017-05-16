#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int
bisearch(void *sorted,const void *target,int size,int esize,
         int (*compare)(const void * key1,const void *key2))
{
    int     left,
            middle,
            right;

    left = 0;
    right = size -1;

    while(left <= right)
    {
        middle = (left + right) / 2;
        switch(compare(((char *)sorted + (esize * middle)),target))
        {
            case -1:
                left = middle + 1;
                break;

            case 1:
                right = middle - 1;
                break;

            case 0:
                return middle;

            default:
                break;

        }
    }

    return -1;
}
/**********************************************************************/
#define  SPELL_SIZE         31

static int
compare_str(const void * str1,const void *str2)
{
    int     retval;
    if((retval = strcmp((const char *)str1,(const char *)str2)) > 0)
        return 1;
    else if(retval  < 0)
        return -1;
    else
        return 0;
}

int
spell(char (*dictionary)[SPELL_SIZE],int size,const char *word)
{
    if(bisearch(dictionary,word,size,SPELL_SIZE,compare_str) >= 0)
        return 1;
    else
        return 0;
}

int main()
{

   return 0;
}
