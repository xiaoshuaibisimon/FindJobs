#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

/*************************************************/

typedef List Set;

#define set_size(set)    ((set)->size)

//O(1)
void set_init(Set *set,int (*match)(const void * key1,const void *key2),
              void (*destroy)(void * data));

//O(n)
//void set_destroy(Set *set);
#define set_destroy  list_destroy

//O(n)
//0:-1:1
int set_insert(Set* set,const void * data);

//O(n)
//0:-1
int set_remove(Set *set,void **data);

//O(nm)
//0:-1
int set_union(Set * setu,const Set * set1,const Set *set2);

//O(nm)
//0:-1
int set_intersection(Set * seti,const Set * set1,const Set *set2);

//O(nm)
//0:-1
int set_difference(Set * setd,const Set * set1,const Set *set2);


//O(n)
//0:1
int set_is_member(const Set * set,const void * data);


//O(nm)
//0:1
int set_is_subset(const Set * set1,const Set * set2);

//O(nm)
//0:1
int set_is_equal(const Set * set1,const Set * set2);


void set_init(Set *set,int (*match)(const void * key1,const void *key2),
              void (*destroy)(void * data))
{
    list_init(set,destroy);//普通的链表初始化
    set->match = match;//针对集合指定match回调函数--用来判断元素是否匹配

    return ;
}

int set_insert(Set* set,const void * data)
{
    if(set_is_member(set,data))//先判断该元素是否已经存在集合中
        return 1;

    return list_ins_next(set,list_tail(set),data);//在链表尾部插入新元素
}

int set_remove(Set *set,void **data)
{
    ListElmt *member,*prev;

    prev = NULL;

    for(member = list_head(set);member != NULL;member = list_next(member))//遍历整个链表/集合---寻找匹配元素
    {
        if(set->match(*data,list_data(member)))//找到匹配元素
            break;
        prev = member;//记录将要删除元素的前驱元素
    }

    if(member == NULL)//遍历完集合没有找到匹配的元素
        return -1;
    return list_rem_next(set,prev,data);//将prev后面的元素从集合删除
}

int set_union(Set * setu,const Set * set1,const Set *set2)
{
    ListElmt * member;
    void *data;

    set_init(setu,set1->match,NULL);//初始化并集

    for(member = list_head(set1);member != NULL;member = list_next(member))//遍历set1
    {
        data = list_data(member);//取出set1里面的每一个元素
        if(list_ins_next(setu,list_tail(setu),data) != 0)//将set1的每一个元素都插入并集
        {
            set_destroy(setu);//插入出错--销毁集合
            return -1;
        }
    }

    for(member = list_head(set2);member != NULL;member = list_next(member))//遍历set2
    {
        if(set_is_member(setu,list_data(member)))//先判断set2中元素是否已经存在于并集
            continue;
        else
        {
            //将set2中的合法元素插入并集
            data = list_data(member);
            if(list_ins_next(setu,list_tail(setu),data) != 0)
            {
                set_destroy(setu);
                return -1;
            }
        }
    }

    return 0;
}

int set_intersection(Set * seti,const Set * set1,const Set *set2)
{
    ListElmt * member;
    void *data;

    set_init(seti,set1->match,NULL);//初始化交集

    for(member = list_head(set1);member != NULL;member = list_next(member))//遍历set1
    {
        if(set_is_member(set2,list_data(member)))//如果set1中的元素在set2中出现
        {
            //将该元素插入交集
            data = list_data(member);
            if(list_ins_next(seti,list_tail(seti),data) != 0)
            {
                set_destroy(seti);
                return -1;
            }
        }
    }
    return 0;
}

int set_difference(Set * setd,const Set * set1,const Set *set2)
{
    ListElmt * member;
    void *data;

    set_init(setd,set1->match,NULL);//初始化差集

    for(member = list_head(set1);member != NULL;member = list_next(member))//遍历set1
    {
        if(!set_is_member(set2,list_data(member)))//如果set1中的元素在set2中没有出现
        {
            //将该元素插入差集
            data = list_data(member);
            if(list_ins_next(setd,list_tail(setd),data) != 0)
            {
                set_destroy(setd);
                return -1;
            }
        }
    }

    return 0;

}

int set_is_member(const Set * set,const void * data)
{
    ListElmt * member;//链表/集合元素

    for(member = list_head(set);member != NULL;member = list_next(member))//遍历集合
    {
        if(set->match(data,list_data(member)))//匹配上了返回1
            return 1;
    }

    return 0;//不是该集合成员返回0
}

int set_is_subset(const Set * set1,const Set * set2)
{
    ListElmt * member;

    if(set_size(set1) > set_size(set2))//如果set1的元素多余set2--set1肯定不是set2的子集
        return 0;

    for(member = list_head(set1);member != NULL;member = list_next(member))//遍历set1
    {
        if(!set_is_member(set2,member))//在还没有遍历完set1的情况下，出现非set2的元素，表明set1不是set2的子集
            return 0;
    }

    //遍历set1结束--所有元素都是set2的成员--set1是set2的子集
    return 1;
}

int set_is_equal(const Set * set1,const Set * set2)
{
    if(set_size(set1) != set_size(set2))//先判断元素个数是否相等--个数不相等必然两个集合不相等
        return 0;

    return set_is_subset(set1,set2);//个数相等的前提下，如果set1是set2的子集---则两个集合相等
}

/**********************************************/


//定义一个结构体--通过key区分subsets中的各个元素（也就是各个子集）
//因为集合/链表的元素的数据指针是void*，所以集合的元素可以是任意类型的数据结构+next指针
//这里的subsets集合的元素数据类型（void* data）还是集合，只不过该集合用KSet*表示，以便可以用key进行区分
typedef struct _KSet
{
    void * key;
    Set set;
}KSet;

int covering(Set * members,Set *subsets,Set *cover)
{
    Set intersection;//缓存每一个subsets成员与members的交集
    KSet *subset = NULL;//在贪心算法中缓存每一次得到的近似最优子集（通过max_member获取）
    ListElmt *member = NULL,//遍历集合用
             *max_member = NULL;//缓存subsets中最大的成员（覆盖members最多的subsets成员）
    void * data = NULL;//缓存集合中元素的数据指针
    int max_size = 0;//最大交集的元素个数

    set_init(cover,subsets->match,NULL);//初始化目标集合--覆盖集合

    while(set_size(members) > 0 && set_size(subsets))//只要members和subsets元素个数都大于0--就迭代寻找近似最优解--全部覆盖members
    {
        max_size = 0;//members剩余成员和subsets剩余成员最大子集的个数
        for(member = list_head(subsets);member != NULL;member = list_next(member))//遍历subsets
        {
            //每一个子集和members求交集
            if(set_intersection(&intersection,&(((KSet *)list_data(member))->set),members) != 0)
                return -1;

            if(set_size(&intersection) > max_size)//更新最大交集个数和最大子集--最优子集
            {
                max_member = member;
                max_size = set_size(&intersection);
            }

            set_destroy(&intersection);//销毁交集
        }

        if(max_size == 0)//遍历完subsets没有找到members的交集--不可能全部覆盖--返回1
            return 1;

        subset = (KSet*)list_data(max_member);//每次找到的最大的子集--从subsets中得到的元素对应的data是KSet类型

        if(set_insert(cover,subset) != 0)//将该子集插入覆盖集合
            return -1;

        //将mmebers中已经被覆盖的成员删除
        for(member = list_head(&(((KSet*)list_data(max_member))->set));member!=NULL;member = list_next(member))//遍历之前刚刚得到的子集
        {
            data = list_data(member);
            if(set_remove(members,(void **)&data) == 0
                    && members->destroy != NULL)//将自己中成员从members中删除
            {
                members->destroy(data);//回收内存
            }
        }

        //将已经被选中的子集从subsets中删除
        if(set_remove(subsets,(void **)&subset) != 0)
            return -1;

    }

    //当遍历完subsets以后，members中还有剩余元素--不可能完全覆盖--返回1
    if(set_size(members) > 0)
        return 1;

    //members中没有剩余元素--找到完全覆盖集合--cover
    return 0;
}


int main()
{
    return 0;
}