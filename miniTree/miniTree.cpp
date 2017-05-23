#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <float.h>

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

/****************************************************************************************/

typedef struct _AdjList
{
        void *vertex;
        Set adjacent;
}AdjList;


typedef struct _Graph
{
    int     vcount;
    int     ecount;

    int(*match)(const void *key1,const void * key2);
    void (*destroy)(void *data);

    List    adjlists;
}Graph;

typedef enum _VertexColor{white,gray,black}VertexColor;


#define graph_adjlists(graph)       ((graph)->adjlists)

#define graph_vcount(graph)         ((graph)->vcount)

#define graph_ecount(graph)         ((graph)->ecount)


//O(1)
void graph_init(Graph *graph,int(*match)(const void *key1,const void * key2),
                void (*destroy)(void *data));

//O(V+E)
void graph_destroy(Graph *graph);

//O(V)--主要是搜索有无重复顶点用了O（V），插入只是O（1）
int graph_ins_vertex(Graph *graph,const void *data);

//O(V)--搜索和对set_insert的调用都是O（V）
int graph_ins_edge(Graph *graph,const void *data1,const void *data2);

//O（V+E）
int graph_rem_vertex(Graph *graph,void **data);


//O（V）
int graph_rem_edge(Graph *graph, void *data1, void **data2);

//O（V）
int graph_adjlist(const Graph *graph,const void *data,AdjList **adjlist);

//O（V）
int graph_is_adjacent(const Graph *graph,const void *data1,const void *data2);


void graph_init(Graph *graph, int (*match)(const void *key1, const void
   *key2), void (*destroy)(void *data))
{
    graph->vcount = 0;
    graph->ecount = 0;
    graph->match = match;
    graph->destroy = destroy;

    //初始化邻接表链表
    list_init(&graph->adjlists, NULL);

    return;

}

void graph_destroy(Graph *graph)
{

    AdjList            *adjlist;

    //移除每一个邻接表结构体（顶点），并回收每一个邻接表结构体包含的邻接表（某个顶点的邻接顶点的集合）
    //每个顶点就是一个邻接表结构体，里面包含实际的顶点数据以及预支邻接的顶点组成的集合
    while (list_size(&graph->adjlists) > 0)
    {//链表还有元素

       if (list_rem_next(&graph->adjlists, NULL, (void **)&adjlist) == 0)
       {//移除链表里的节点

          set_destroy(&adjlist->adjacent);//回收每一个节点/顶点对应的邻接表--销毁集合

          if (graph->destroy != NULL)
             graph->destroy(adjlist->vertex);//回收顶点
          free(adjlist);//回收邻接表结构体所占用的内存
       }

    }

    //销毁整个邻接表结构链表
    list_destroy(&graph->adjlists);

    //清空图结构体--之后不能进行其他操作
    memset(graph, 0, sizeof(Graph));

    return;
}

int graph_ins_vertex(Graph *graph, const void *data)
{

    ListElmt           *element;//用于辅助遍历链表--确保是否重复

    AdjList            *adjlist;//缓存新的顶点

    int                retval;

    //遍历邻接表链表--确保没有插入重复顶点
    for (element = list_head(&graph->adjlists); element != NULL; element =
       list_next(element))
    {

       if (graph->match(data, ((AdjList *)list_data(element))->vertex))
          return 1;//有重复顶点返回1

    }

    //正式插入顶点到图里面
    if ((adjlist = (AdjList *)malloc(sizeof(AdjList))) == NULL)//新分配一个顶点/邻接表结构体
       return -1;

    adjlist->vertex = (void *)data;//新的顶点的数据指针指向调用者传入的数据--填充新顶点的数据

    //初始化新顶点的邻接点集合/邻接表
    set_init(&adjlist->adjacent, graph->match, graph->destroy);

    //在链表尾部插入新顶点/邻接表结构体
    if ((retval = list_ins_next(&graph->adjlists, list_tail(&graph->adjlists),
       adjlist)) != 0)
    {
       return retval;
    }

    //更新顶点容量
    graph->vcount++;

    return 0;

}

int graph_ins_edge(Graph *graph, const void *data1, const void *data2)
{

    ListElmt           *element;

    int                retval;



    //以下两个for循环的目的是确保两个顶点都已经存在于图中了
    for (element = list_head(&graph->adjlists); element != NULL; element =
       list_next(element))
    {//判断第二个顶点

       if (graph->match(data2, ((AdjList *)list_data(element))->vertex))
          break;

    }

    if (element == NULL)
       return -1;

    for (element = list_head(&graph->adjlists); element != NULL; element =
       list_next(element))
    {//判断第一个顶点

       if (graph->match(data1, ((AdjList *)list_data(element))->vertex))
          break;

    }

    if (element == NULL)
       return -1;

    //将第二个顶点插入第一个顶点对应的邻接表中（第一个顶点自己的邻接顶点的集合）
    //言外之意，对于有向图来说，某个顶点（起的）边的数量就是子集邻接表的容量
    //无向图则需要再将第一个顶点插入第二个顶点的邻接表，同时边的数量是递增2
    if ((retval = set_insert(&((AdjList *)list_data(element))->adjacent, data2))
       != 0)
    {

       return retval;

    }

    //更新边的数量
    graph->ecount++;

    return 0;

}

int graph_rem_vertex(Graph *graph, void **data)
{

    ListElmt           *element,//用于遍历链表
                       *temp,//缓存即将删除的顶点
                       *prev;//指向即将删除的元素的前驱元素（顶点）

    AdjList            *adjlist;//顶点的邻接表

    int                found;//找到顶点的标志

    //遍历链表以及每个顶点包含的邻接表

    prev = NULL;
    found = 0;

    for (element = list_head(&graph->adjlists); element != NULL; element =
       list_next(element))
    {//遍历整个链表O（V）

       //如果要删除的顶点位于某个顶点的邻接表--不允许删除--直接返回--O（E）
       if (set_is_member(&((AdjList *)list_data(element))->adjacent, *data))
          return -1;

       //匹配到目标顶点--缓存目标顶点
       if (graph->match(*data, ((AdjList *)list_data(element))->vertex))
       {
          temp = element;
          found = 1;//置位找到标志
       }

       //如果没找到，记录前驱顶点
       if (!found)
          prev = element;

    }

    //遍历完整个链表都没有匹配上--找不到呗--直接返回吧
    if (!found)
       return -1;

    //如果找到的顶点的邻接表不为空--不允许删除--直接返回
    if (set_size(&((AdjList *)list_data(temp))->adjacent) > 0)
       return -1;

    //从链表中移除一个顶点
    if (list_rem_next(&graph->adjlists, prev, (void **)&adjlist) != 0)
       return -1;

    //缓存要删除顶点的数据（顶点值）--由调用者维护
    *data = adjlist->vertex;
    //回收顶点所占内存
    free(adjlist);

    //更新顶点容量
    graph->vcount--;

    return 0;

}

int graph_rem_edge(Graph *graph, void *data1, void **data2)
{
    ListElmt           *element;

    //遍历整个链表
    for (element = list_head(&graph->adjlists); element != NULL; element =
       list_next(element))
    {
       if (graph->match(data1, ((AdjList *)list_data(element))->vertex))//找到第一个顶点（起始顶点）
          break;
    }

    if (element == NULL)//没有找到第一个顶点--不存在该边--直接返回呗
       return -1;

    //将第二个顶点从第一个顶点的邻接表中删除
    if (set_remove(&((AdjList *)list_data(element))->adjacent, data2) != 0)
       return -1;

    //更新容量
    graph->ecount--;

    return 0;
}


int graph_adjlist(const Graph *graph, const void *data, AdjList **adjlist)
{

    ListElmt           *element,
                       *prev;

    prev = NULL;//遍历链表用

    //遍历整个链表--为的是找到指定顶点
    for (element = list_head(&graph->adjlists); element != NULL; element =
       list_next(element))
    {

       if (graph->match(data, ((AdjList *)list_data(element))->vertex))//找到指定的顶点
          break;
       prev = element;//记录前驱元素

    }

    //如果没找到指定的顶点--就没有邻接表咯--直接返回
    if (element == NULL)
       return -1;

    //返回指定顶点的邻接表
    *adjlist = (AdjList *)list_data(element);

    return 0;

}


int graph_is_adjacent(const Graph *graph, const void *data1, const void
   *data2)
{

    ListElmt           *element,
                       *prev;


    prev = NULL;

    //遍历整个链表--为的是找到指定顶点--从而找到第一个顶点的邻接表
    for (element = list_head(&graph->adjlists); element != NULL; element =
       list_next(element)) {

       if (graph->match(data1, ((AdjList *)list_data(element))->vertex))//找到第一个顶点
          break;
       prev = element;//记录前驱元素

    }

    //如果没找到第一个顶点--就没有邻接表咯--直接返回
    if (element == NULL)
       return 0;

    //判断第二个顶点是否是第一个顶点的邻接点（是否在其邻接表中）
    //邻接表就是个集合
    return set_is_member(&((AdjList *)list_data(element))->adjacent, data2);

}

/***********************************************************************************************/

//顶点结构体
typedef struct _MstVertex
{
    void                *data;//数据域
    double              weight;//权重--该顶点是某条边终点

    VertexColor         color;//色值
    double              key;//键值

    struct _MstVertex   *parent;//父节点

}MstVertex;

int mst(Graph *graph,const MstVertex *start,List *span,int(*match)(const void *key1,const void * key2));

int mst(Graph *graph,const MstVertex *start,List *span,
        int(*match)(const void *key1,const void * key2))
{

    AdjList         *adjlist;//邻接表结构
    MstVertex       *mst_vertex,//邻接表结构体链表中的顶点
                    *adj_vertex;//某个顶点的邻接表中的邻接顶点

    ListElmt        *element,//遍历链表
                    *member;//遍历邻接表（集合）

    double          minimum;//缓存最小键值
    int             found,i;

    found = 0;//是否找到起始顶点

    //初始化邻接表结构体链表中的所有顶点
    for(element = list_head(&graph_adjlists(graph));element != NULL;element = list_next(element))
    {//O（v）
        mst_vertex = (MstVertex*)((AdjList *)list_data(element))->vertex;

        if(match(mst_vertex,start))
        {//找到起始顶点
            mst_vertex->color = white;//一开始还未选择的时候也初始化为白色
            mst_vertex->key = 0;//初始键值为0--最小
            mst_vertex->parent = NULL;//没有父节点
            found = 1;//找到起始顶点标志
        }
        else
        {//初始化所有的非起始顶点
            mst_vertex->color = white;
            mst_vertex->key = DBL_MAX;
            mst_vertex->parent = NULL;
        }
    }

    //没有找到起始顶点
    if(!found)
        return -1;

    //利用prim算法计算最小生成树
    i = 0;

    while(i < graph_vcount(graph))//遍历每一个链表顶点（v次运算）
    {
        //选择所有白色顶点中键值最小的顶点
        minimum = DBL_MAX;//保证每次都可以从白色顶点中选取最小键值的顶点

        for(element = list_head(&graph_adjlists(graph));element != NULL;element = list_next(element))
        {//针对每一个顶点，都要遍历一次链表中的所有顶点--寻找当前最小键值的顶点/邻接表结构体--O（v）
            mst_vertex = (MstVertex*)((AdjList *)list_data(element))->vertex;

            if(mst_vertex->color == white && mst_vertex->key < minimum)
            {//更新白色顶点中的最小键值--黑色顶点是上一次决定的最小键值的顶点，不再关注
                minimum = mst_vertex->key;

                adjlist = (AdjList *)list_data(element);//当前最小键值对应的邻接表/顶点
            }
        }

        //找到了最小键值的顶点对应的邻接表结构体--设置颜色为黑色--选中了该顶点
        ((MstVertex*)adjlist->vertex)->color = black;

        //遍历所选择的顶点的邻接表
        for(member = list_head(&adjlist->adjacent);member != NULL;member = list_next(member))
        {//O（E）
            adj_vertex = (MstVertex*)list_data(member);//获取邻接顶点
            for(element = list_head(&graph_adjlists(graph));element != NULL;element = list_next(element))
            {//针对每一个邻接顶点/每一条边，都要遍历一次链表--寻找他在链表中匹配的顶点的信息--O（V）
                mst_vertex = (MstVertex*)((AdjList *)list_data(element))->vertex;

                if(match(mst_vertex,adj_vertex))
                {//链表中的顶点和邻接表中的顶点匹配上了
                    if(mst_vertex->color == white && adj_vertex->weight < mst_vertex->weight)
                    {//修改邻接顶点匹配的链表顶点的键值和父节点
                        mst_vertex->key = adj_vertex->weight;
                        mst_vertex->parent = (MstVertex*)adjlist->vertex;
                    }
                    //一旦找到邻接顶点匹配的链表顶点--开始匹配下一个邻接顶点
                    break;
                }
            }
        }

        //准备选择下一个顶点
        i++;
    }

    //将最小生成树装载到span链表--只要span有效，需要保证graph里面的链表节点都有效
    list_init(span,NULL);

    for(element = list_head(&graph_adjlists(graph));element != NULL;element = list_next(element))
    {//遍历邻接表结构体链表
        mst_vertex = (MstVertex*)((AdjList *)list_data(element))->vertex;

        if(mst_vertex->color == black)
        {//按照链表里面的节点顺序，将所有选择了的顶点加入新的链表中--span尾部插入
            if(list_ins_next(span,list_tail(span),mst_vertex) != 0)
            {
                list_destroy(span);
                return -1;
            }
        }
    }

    return 0;
}

int main()
{
    return 0;
}
