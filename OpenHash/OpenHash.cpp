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
    int     positions;//槽位数
    void    *vacated;//标志某个槽是否之前已经删除过元素

    int (*h1)(const void * key);//哈希函数，一般是k mod  m
    int (*h2)(const void * key);//哈希函数，一般是1 + （k mod m'），其中m'是m-1或者m-2
    int (*match)(const void * key1,const void * key2);//匹配函数
    void (*destroy)(void * data);//回收函数

    int size;//元素个数
    void **table;//数组--哈希表本质就是数组--数组里面的元素都是void*--二级指针内存模型

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
//线性探测---（k  mod  m  + i） mod m--很低效，冲突太多
static char vacated;//辅助标记位置，仅仅辅助作用，不做任何修改

int ohtbl_init(OHTbl *htbl,int positions,int (*h1)(const void * key),
               int (*h2)(const void * key),int (*match)(const void * key1,const void * key2),
               void (*destroy)(void * data))
{
    int i = 0;

    if((htbl->table = (void **)malloc(positions * sizeof(void *))) == NULL)//分配内存
        return -1;

    htbl->positions = positions;//重置元素个数

    for(i = 0;i < htbl->positions;i++)//初始化槽里面的每一个元素
    {
        htbl->table[i] = NULL;
    }

    htbl->vacated = &vacated;//指向标记位置

    //各成员函数初始化
    htbl->h1 = h1;
    htbl->h2 = h2;
    htbl->match = match;
    htbl->destroy = destroy;

    htbl->size = 0;//元素个数初始化

    return 0;
}

void ohtbl_destroy(OHTbl *htbl)
{
    int i = 0;

    if(htbl->destroy != NULL)//如果回收函数不为空
    {
        for(i = 0;i < htbl->positions;i++)//遍历每一个槽
        {
            if(htbl->table != NULL && htbl->table[i] != htbl->vacated)//如果该槽不为空（有元素），而且之前没有被删除过元素
                htbl->destroy(htbl->table[i]);//回收二级指针对应的内存
        }
    }

    free(htbl->table);//回收初始化函数里分配的内存--哈希表用的数组

    memset(htbl,0,sizeof(OHTbl));//清空哈希表结构体

    return ;
}

int ohtbl_insert(OHTbl * htbl,const void *data)
{
    void *temp;
    int position,
            i;

    if(htbl->size == htbl->positions)//表已经满了
        return -1;

    temp = (void *)data;//缓存要插入的元素

    if(ohtbl_lookup(htbl,&temp) == 0)//判断是否已存在
        return 1;

    for(i = 0; i < htbl->positions;i++)//探测槽
    {
        position = (htbl->h1(data) + (i*htbl->h2(data))) % htbl->positions;//双散列

        if(htbl->table[position] == NULL || htbl->table[position] == htbl->vacated)//确实是空槽或者该槽以前的元素已被删除
        {
            htbl->table[position] = (void *)data;//重置槽
            htbl->size++;//更新个数
            return 0;
        }
    }

    //探测失败
    return  -1;
}


int ohtbl_remove(OHTbl * htbl, void **data)
{
    int position,
            i;

    for(i = 0; i < htbl->positions;i++)//探测槽
    {
        position = (htbl->h1(*data) + (i*htbl->h2(*data))) % htbl->positions;//获取槽位置/索引

        if(htbl->table[position] == NULL )//空槽--探测结束
        {
            return -1;//失败
        }
        else if(htbl->table[position] == htbl->vacated)//之前已经移除过元素了
        {
            continue;//不用再删除该槽位置，探测下一个位置
        }
        else if(htbl->match(htbl->table[position],*data))//匹配上了
        {
            *data = htbl->table[position];//获取要删除元素的数据
            htbl->size--;//更新个数
            htbl->table[position] = htbl->vacated;//标记该槽位置已经被删除过元素--不能简单的设置为NULL
            return 0;
        }
    }
    //探测失败--没有找到匹配的数据
    return -1;
}

int ohtbl_lookup(const OHTbl * htbl, void **data)
{
    int position,
            i;

    //双散列探测
    for(i = 0; i < htbl->positions;i++)
    {
        position = (htbl->h1(*data) + (i*htbl->h2(*data))) % htbl->positions;//获取索引

        if(htbl->table[position] == NULL )
        {//空槽--找不到呗，不存在呗
            return -1;
        }
        else if(htbl->match(htbl->table[position],*data))//匹配上了
        {
            *data = htbl->table[position];//获取数据
            return 0;
        }
    }

    //没有找到
    return -1;
}

int main()
{

    return 0;
}
