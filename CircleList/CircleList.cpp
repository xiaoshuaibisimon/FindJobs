#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//haha
typedef struct _CListElmt
{
    void *data;
    struct _CListElmt * next;
}CListElmt;


typedef struct _CList
{
    int size;
    int (*match)(const void * key1,const void * key2);
    void (*destroy)(void * data);
    CListElmt * head;
}CList;


void clist_init(CList *list,void (*destroy)(void * data));
void clist_destroy(CList *list);
int clist_ins_next(CList *list,CListElmt * element,const void * data);
int clist_rem_next(CList *list,CListElmt * element,void ** data);

#define clist_size(list)    ((list)->size)

#define clist_head(list)    ((list)->head)

#define clist_is_head(list,element) ((element) == (list)->head ? 1 :0)

#define clist_data(element) ((element)->data)

#define clist_next(element) ((element)->next)

void clist_init(CList *list,void (*destroy)(void * data))
{
    list->size = 0;
    list->head = NULL;
    list->match = NULL;
    list->destroy = destroy;
}

void clist_destroy(CList *list)
{
    void * data;

    while(clist_size(list) > 0)
    {
        if((clist_rem_next(list,NULL,(void **)&data) == 0)
                && (list->destroy != NULL))
        {
            list->destroy(data);
        }
    }

    memset(list,0,sizeof(CList));
    return ;
}

int clist_ins_next(CList *list,CListElmt * element,const void * data)
{
    CListElmt * new_element;

    if((new_element = (CListElmt*)malloc(sizeof(CListElmt))) == NULL)
        return -1;

    new_element->data = (void *)data;

    //插入元素的时候不用考虑插入尾节点的特殊情况--已经没有尾节点的概念，只需关注空链表和头结点即可
    if(clist_size(list) == 0)//空链表
    {
        new_element->next = new_element;//自己指向自己
        list->head = new_element;//重置头结点
    }
    else
    {
        //常见操作
        new_element->next = element->next;
        element->next = new_element;
    }

    list->size++;
    return 0;
}

int clist_rem_next(CList *list,CListElmt * element,void ** data)
{
    CListElmt * old_element;

    if(clist_size(list) == 0)
        return -1;

    //取出数据
    *data = element->next->data;
    //缓存元素
    old_element = element->next;

    //删除的时候，考虑只有一个节点--重置头结点
    if(element->next == element)
    {
        list->head = NULL;
    }
    else
    {
        //常见情况
        element->next = old_element->next;
        if(old_element == clist_head(list))//考虑删除的是头结点
            list->head = old_element->next;//重置头结点
    }

    //释放节点元素内存
    free(old_element);

    list->size--;

    return 0;
}

int main()
{
    return 0;
}
