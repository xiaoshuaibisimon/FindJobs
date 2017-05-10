#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

typedef struct _ListElmt
{
    void * data;//数据域
    struct _ListElmt * next;//指针域
}ListElmt;

typedef struct _List{
    int size;//节点个数
    int (*match)(const void * key1,const void * key2);//扩展用
    void (*destroy)(void *data);//自定义回收内存函数
    ListElmt * head;//头结点指针
    ListElmt * tail;//尾节点指针
}List;

//int list_size(List * list);
#define list_size(list)     ((list)->size)

//ListElmt* list_head(const List *list);
#define list_head(list)     ((list)->head)

//ListElmt* list_tail(const List *list);
#define list_tail(list)     ((list)->tail)

//int list_is_head(const ListElmt *element);
#define list_is_head(list,element)      ((element) == (list)->head ? 1 :0)

//int list_is_tail(const ListElmt *element);
#define list_is_tail(element)      ((element)->next == NULL ? 1 :0)

//void * list_data(const ListElmt *element);
#define list_data(element)      ((element)->data)

//ListElmt* list_next(const ListElmt *element);
#define list_next(element)      ((element)->next)


void list_init(List * list,void (*destroy)(void *data));
void list_destroy(List *list);
int list_ins_next(List* list,ListElmt *element,const void * data);
int list_rem_next(List* list,ListElmt *element,void ** data);


/*链表初始化--O（1）*/
void list_init(List * list,void (*destroy)(void *data))
{
    list->size = 0;
    list->head = NULL;
    list->tail = NULL;
    list->match = NULL;
    list->destroy = destroy;
}

/*链表销毁--O（n）*/
void list_destroy(List *list)
{
    void * data;
    while(list_size(list) > 0)
    {
        /*移除每一个元素*/
        if(list_rem_next(list,NULL,(void**)&data) == 0 && list->destroy != NULL)
        {
            /*调用用户自定义的内存回收函数*/
            list->destroy(data);
        }
    }
    /*不允许进行其他链表操作，清空链表结构*/
    memset(list,0,sizeof(List));
    return ;
}

/*插入一个元素--O（1）*/
int list_ins_next(List* list,ListElmt *element,const void * data)
{
    /*缓存新元素--毕竟主调函数只给出数据域的指针，并不给出完整的元素结构体变量*/
    ListElmt * new_element;

    /*分配内存*/
    if((new_element = (ListElmt*)malloc(sizeof(ListElmt))) == NULL)//每插入一个元素就调用一次malloc--只是节点元素大小
    {
        return -1;
    }

    /*缓存数据*/
    new_element->data = (void*)data;//由程序员维护该内存空间

    /*插入元素*/
    if(element == NULL)//在头部插入
    {
        if(list_size(list) == 0)//空链表
        list->tail = new_element;//新元素就是尾部元素

        new_element->next = list->head;//新元素指向之前的头结点
        list->head = new_element;//头结点变成新元素
    }else{//在非头部插入
        if(element->next == NULL)//尾部插入
            list->tail = new_element;//尾节点更新

        /*最容易想到的插入情况*/
        new_element->next = element->next;
        element->next = new_element;
    }
    list->size++;

    return 0;
}
/*移除一个元素--O（1）*/
int list_rem_next(List* list,ListElmt *element, void ** data)
{
    /*缓存旧元素*/
    ListElmt * old_element;

    /*不允许删除控链表的元素*/
    if(list_size(list) == 0)
        return -1;

    /*移除元素*/
    if(element == NULL)
    {
        //缓存旧元素/待删除元素
        old_element = list->head;
        *data = (void *)(old_element->data);//缓存数据指针
        list->head = old_element->next;//重置头结点

        if(list_size(list) == 1)//如果只有一个节点，更新尾节点
            list->tail = NULL;
    }else{

        //缓存旧元素/待删除元素
        old_element = element->next;
        *data = (void*)(old_element->data);//缓存数据指针
        element->next = old_element->next;//更新元素的指针域

        if(element->next == NULL)//如果删除的是尾节点，更新尾节点
            list->tail = element;
    }

    /*释放节点元素所占内存--注意：此时并没有释放元素里面数据域指向的内存*/
    free(old_element);

    /*更新节点个数*/
    list->size--;

    return 0;
}
/******************************************************/

typedef struct _OHTbl
{
    int     positions;
    void    *vacated;

    int (*h1)(const void * key);
    int (*h2)(const void * key);
    int (*match)(const void * key1,const void * key2);
    void (*destroy)(void * data);

    int size;
    void **table;

}OHTbl;

#define ohtbl_size(htbl)  ((htbl)->size)


//O(n)
int ohtbl_init(OHTbl *htbl,int positions,int (*h1)(const void * key),
               int (*h2)(const void * key),int (*match)(const void * key1,const void * key2),
               void (*destroy)(void * data));

//O(n)
void ohtbl_destroy(OHTbl *htbl);

//O(1)
int ohtbl_insert(OHTbl * htbl,const void *data);

//O(1)
int ohtbl_remove(OHTbl * htbl, void **data);

//O(1)
int ohtbl_lookup(const OHTbl * htbl, void **data);

static char vacated;

int ohtbl_init(OHTbl *htbl,int positions,int (*h1)(const void * key),
               int (*h2)(const void * key),int (*match)(const void * key1,const void * key2),
               void (*destroy)(void * data))
{
    int i = 0;

    if((htbl->table = (void **)malloc(positions * sizeof(void *))) == NULL)
        return -1;

    htbl->positions = positions;

    for(i = 0;i < htbl->positions;i++)
    {
        htbl->table[i] = NULL;
    }

    htbl->vacated = &vacated;

    htbl->h1 = h1;
    htbl->h2 = h2;
    htbl->match = match;
    htbl->destroy = destroy;

    htbl->size = 0;

    return 0;
}

void ohtbl_destroy(OHTbl *htbl)
{
    int i = 0;

    if(htbl->destroy != NULL)
    {
        for(i = 0;i < htbl->positions;i++)
        {
            if(htbl->table != NULL && htbl->table[i] != htbl->vacated)
                htbl->destroy(htbl->table[i]);
        }
    }

    free(htbl->table);

    memset(htbl,0,sizeof(OHTbl));

    return ;
}

int ohtbl_insert(OHTbl * htbl,const void *data)
{
    void *temp;
    int position,
            i;

    if(htbl->size == htbl->positions)
        return -1;

    temp = (void *)data;

    if(ohtbl_lookup(htbl,&temp) == 0)
        return 1;

    for(i = 0; i < htbl->positions;i++)
    {
        position = (htbl->h1(data) + (i*htbl->h2(data))) % htbl->positions;

        if(htbl->table[position] == NULL || htbl->table[position] == htbl->vacated)
        {
            htbl->table[position] = (void *)data;
            htbl->size++;
            return 0;
        }
    }

    return  -1;
}


int ohtbl_remove(OHTbl * htbl, void **data)
{
    int position,
            i;

    for(i = 0; i < htbl->positions;i++)
    {
        position = (htbl->h1(*data) + (i*htbl->h2(*data))) % htbl->positions;

        if(htbl->table[position] == NULL )
        {
            return -1;
        }
        else if(htbl->table[position] == htbl->vacated)
        {
            continue;
        }
        else if(htbl->match(htbl->table[position],*data))
        {
            *data = htbl->table[position];
            htbl->size--;
            htbl->table[position] = htbl->vacated;
            return 0;
        }
    }
    return -1;
}
//
int ohtbl_lookup(const OHTbl * htbl, void **data)
{
    int position,
            i;

    for(i = 0; i < htbl->positions;i++)
    {
        position = (htbl->h1(*data) + (i*htbl->h2(*data))) % htbl->positions;

        if(htbl->table[position] == NULL )
        {
            return -1;
        }
        else if(htbl->match(htbl->table[position],*data))
        {
            *data = htbl->table[position];
            return 0;
        }
    }
    return -1;
}

int main()
{

    return 0;
}
