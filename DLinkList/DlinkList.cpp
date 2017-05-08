#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct _DListElmt
{
    void * data;//数据域
    struct _DListElmt * prev;//指针域
    struct _DListElmt * next;//指针域
}DListElmt;

typedef struct _DList{
    int size;//节点个数
    int (*match)(const void * key1,const void * key2);//扩展用
    void (*destroy)(void *data);//自定义回收内存函数
    DListElmt * head;//头结点指针
    DListElmt * tail;//尾节点指针
}DList;

//int dlist_size(DList * list);
#define dlist_size(list)     ((list)->size)

//DListElmt* dlist_head(const DList *list);
#define dlist_head(list)     ((list)->head)

//DListElmt* dlist_tail(const DList *list);
#define dlist_tail(list)     ((list)->tail)

//int dlist_is_head(const DListElmt *element);
#define dlist_is_head(element)      ((element)->prev == NULL ? 1 :0)

//int dlist_is_tail(const DListElmt *element);
#define dlist_is_tail(element)      ((element)->next == NULL ? 1 :0)

//void * dlist_data(const DListElmt *element);
#define dlist_data(element)      ((element)->data)

//DListElmt* dlist_next(const DListElmt *element);
#define dlist_next(element)      ((element)->next)

//DListElmt* dlist_prev(const DListElmt *element);
#define dlist_prev(element)      ((element)->prev)


void dlist_init(DList * list,void (*destroy)(void *data));
void dlist_destroy(DList *list);
int dlist_ins_next(DList* list,DListElmt *element,const void * data);
int dlist_ins_prev(DList* list,DListElmt *element,const void * data);
int dlist_remove(DList* list,DListElmt *element,void ** data);



/*链表初始化--O(1)*/
void dlist_init(DList * list,void (*destroy)(void *data))
{
    list->head = NULL;
    list->tail = NULL;
    list->size = 0;
    list->destroy = destroy;
    list->match = NULL;
}

/*销毁链表--O（n）*/
void dlist_destroy(DList *list)
{
    void * data;
    while(dlist_size(list) > 0)
    {
        //每次都从尾部删除
        if(dlist_remove(list,dlist_tail(list),(void **)&data) == 0 && list->destroy != NULL)
        {
            list->destroy(data);//执行用户自定义资源回收函数
        }
    }

    //情况链表结构体
    memset(list,0,sizeof(DList));
    return ;
}


/*在指定元素后面插入元素--O（1）
后插法重点考虑非空插入的时候，在尾部插入*/
int dlist_ins_next(DList* list,DListElmt *element,const void * data)
{
    DListElmt * new_element;

    //只有当链表为空时，形参element才可以为空
    if(element == NULL && dlist_size(list) != 0)
        return -1;

    //分配内存
    if((new_element = (DListElmt*)malloc(sizeof(DListElmt))) == NULL)
        return -1;

    //缓存主调函数的数据（指针）
    new_element->data = (void *)data;//新元素的数据指针指向主调函数的实参数据域

    //插入新元素
    if(dlist_size(list) == 0)//考虑插入的时候，如果为控链表
    {
        list->head = new_element;//重置头结点
        list->head->prev = NULL;//前后指针都为空
        list->head->next = NULL;
        list->tail = new_element;//重置尾节点
    }else{//插入的时候不是空链表

        //设置新元素的前后指针
        new_element->next = element->next;
        new_element->prev = element;

        if(element->next == NULL)//如果在尾部插入
            list->tail = new_element;
        else//非尾部插入新元素
            element->next->prev = new_element;

        element->next = new_element;//不管在什么地方，element都是新元素的前驱元素
    }

    //更新元素个数
    list->size++;
    return 0;
}

/*在指定元素前面插入元素--O（1）
前插法重点考虑非空插入的时候，在头部插入*/
int dlist_ins_prev(DList* list,DListElmt *element,const void * data)
{
    DListElmt * new_element;

    //只有当链表为空时，形参element才可以为空
    if(element == NULL && dlist_size(list) != 0)
        return -1;

    //分配内存
    if((new_element = (DListElmt*)malloc(sizeof(DListElmt))) == NULL)
        return -1;

    //缓存主调函数的数据（指针）
    new_element->data = (void *)data;

    if(dlist_size(list) == 0)//考虑插入的时候，如果为控链表
    {
        list->head = new_element;//重置头结点
        list->head->prev = NULL;//前后指针都为空
        list->head->next = NULL;
        list->tail = new_element;//重置尾节点
    }
    else//插入的时候不是空链表
    {
        //设置新元素的前后指针
        new_element->next = element;
        new_element->prev = element->prev;

        if(element->prev == NULL)//如果在头部插入
            list->head = new_element;//重置头结点
        else//非头部插入
            element->prev->next = new_element;

        element->prev = new_element;//不管如何element都是新元素的后继元素
    }

    //更新元素个数
    list->size++;
    return 0;
}

/*删除指定元素--O（1）*/
int dlist_remove(DList* list,DListElmt *element,void ** data)
{
    //不能删除空元素或者空链表
    if(element == NULL || dlist_size(list) == 0)//链表有0个元素
        return -1;

    //缓存数据对应的指针
    *data = element->data;

    //如果是删除头部
    if(element == list->head)
    {
        //重置头结点
        list->head = element->next;

        if(element->next == NULL)//删除的是尾节点（表面该链表只有1个元素）
            list->tail = NULL;//重置尾节点
        else//虽然删除的是头结点，但是该链表还有其他元素
            element->next->prev = NULL;//更新新的头结点的前驱指针
    }
    else//删除的不是头结点，且链表至少有两个元素
    {
        element->prev->next = element->next;//更新前驱节点的后继指针

        if(element->next == NULL)//如果删除的是尾节点
            list->tail = element->prev;//重置尾节点
        else//不是尾节点
            element->next->prev = element->prev;//更新后继节点的前驱指针
    }

    //释放元素所占内存（并非元素所指的内存块）
    free(element);

    //更新元素个数
    list->size--;

    return 0;
}

int main()
{
    return 0;
}
