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

/************************************************************************/

typedef struct _CHTbl
{

    int buckets;//链表数量/桶数

    int (*h)(const void *key);//哈希函数
    int (*match)(const void * key1,const void * key2);//匹配函数
    void (*destroy)(void * data);//回收内存

    int size;//元素个数
    List * table;//哈希表

}CHTbl;

#define chtbl_size(htbl)    ((htbl)->size)

int chtbl_init(CHTbl * htbl,int buckets,int (*h)(const void *key),
               int (*match)(const void * key1,const void * key2),
               void (*destroy)(void * data));

void chtbl_destroy(CHTbl *htbl);

int chtbl_insert(CHTbl * htbl,const void * data);

int chtbl_remove(CHTbl * htbl,void ** data);

int chtbl_lookup(const CHTbl * htbl,void ** data);



//O(n)
int chtbl_init(CHTbl * htbl,int buckets,int (*h)(const void *key),
               int (*match)(const void * key1,const void * key2),
               void (*destroy)(void * data))
{
    int i = 0;

    //分配内存
    if((htbl->table = (List *)malloc(buckets * sizeof(List))) == NULL)
        return -1;

    //重置桶数
    htbl->buckets = buckets;

    //初始化每一个桶/链表
    for(i = 0;i < htbl->buckets;i++)
    {
        list_init(&htbl->table[i],destroy);
    }

    //重置相关回调函数
    htbl->h = h;
    htbl->match = match;
    htbl->destroy = destroy;

    //重置元素个数
    htbl->size = 0;
    return 0;
}


//O(n)
void chtbl_destroy(CHTbl *htbl)
{
    int i = 0;
    //销毁每一个桶
    for(i = 0;i < htbl->buckets;i++)
    {
        list_destroy(&htbl->table[i]);
    }

    //释放为总的桶分配的内存
    free(htbl->table);

    //清空哈希表结构体
    memset(htbl,0,sizeof(CHTbl));

    return ;
}

//O(1)
int chtbl_insert(CHTbl * htbl,const void * data)
{
    void *temp;
    int  bucket,retval;

    temp = (void *)data;//缓存数据

    if(chtbl_lookup(htbl,&temp) == 0)//查询是否已存在
        return 1;

    bucket = htbl->h(data) % htbl->buckets;//k%m得到索引\哈希表中的位置\槽位--获取槽位

    if((retval = list_ins_next(&htbl->table[bucket],NULL,data)) == 0)//插入元素到具体桶中
        htbl->size++;

    return retval;//返回插入结果
}

//O(1)
int chtbl_remove(CHTbl * htbl,void ** data)
{
    ListElmt *element,//遍历某个桶时用于缓存桶中元素
            *prev;//记录要删除元素前面的元素
    int bucket;//索引--根据键得到

    bucket = htbl->h(*data) % htbl->buckets;//根据哈希函数获取键，再根据键获取索引--用于确定哪一个桶
    prev = NULL;

    for(element = list_head(&htbl->table[bucket]);element != NULL;element = list_next(element))//遍历具体的桶
    {
        if(htbl->match(*data,list_data(element)))//是否匹配目标元素
        {
            if(list_rem_next(&htbl->table[bucket],prev,data)== 0)//删除元素
            {
                htbl->size--;
                return 0;
            }
            else
            {
                return -1;
            }
        }
        prev = element;//重置前驱元素
    }
    return -1;
}

//O(1)
int chtbl_lookup(const CHTbl * htbl,void ** data)
{
    ListElmt *element;
    int bucket;

    bucket = htbl->h(*data) % htbl->buckets;

    for(element = list_head(&htbl->table[bucket]);element != NULL;element = list_next(element))//遍历桶
    {
        if(htbl->match(*data,list_data(element)))//匹配目标元素
        {
            *data = list_data(element);//取出数据
            return 0;
        }
    }
    return -1;
}
/************************************************************************************/
//简单模拟词法分析器

typedef enum Token_ {lexit, error, digit, other} Token;//标记类型

typedef struct Symbol_ {

char               *lexeme;//语义
Token              token;//类型

} Symbol;//符号结构体


//模拟从输入流istream获取下一个空格分割的字符串
static char *next_token(const char *istream)
{
    char * tmp = (char *)istream;
    tmp++;
    tmp--;
    return NULL;

}

//将有意义的字符串进行标记并插入符号表，最后返回标记类型给解释器（句法分析）
Token lex(const char *istream, CHTbl *symtbl)
{
    Token token;
    Symbol *symbol;
    int     length,
            retval,
            i;

    //分配内存
    if((symbol = (Symbol*)malloc(sizeof(Symbol))) == NULL)
    {
        return error;
    }

    //处理输入流
    if((symbol->lexeme = next_token(istream)) == NULL)//没有可用字符串
    {
        free(symbol);
        return lexit;
    }
    else
    {
        symbol->token = digit;//默认是数字类型的标识符

        length = strlen(symbol->lexeme);//获取字符串长度

        for(i = 0;i < length;i++)
        {
            if(!isdigit(symbol->lexeme[i]))//检测是否真的是数字
            {
                symbol->token = other;//不是数字
                break;
            }
        }

        memcpy(&token,&symbol->token,sizeof(Token));//拷贝类型

        if((retval = chtbl_insert(symtbl,symbol)) < 0)//插入符号表
        {
            free(symbol);
            return error;
        }
        else if(retval == 1)
        {
            free(symbol);
        }
    }

    return token;//返回类型给解释器

}




int main()
{

    return 0;
}
