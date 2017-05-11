#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
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
/*****************************************************************************/

typedef struct _BiTreeNode
{

    void * data;//数据
    struct _BiTreeNode *left;//左指针
    struct _BiTreeNode *right;//右指针

}BiTreeNode;

typedef struct _BiTree
{
    int size;//节点个数
    int (*compare)(const void * key1,const void *key2);//匹配节点用--留待以后扩展
    void (*destroy)(void * data);//回收节点对应的数据
    BiTreeNode *root;//根节点
}BiTree;

#define bitree_size(tree)       ((tree)->size)
#define bitree_root(tree)       ((tree)->root)
//某一层是否达到了边缘
#define bitree_is_eob(node)     ((node) == NULL)
#define bitree_is_leaf(node)    ((node)->left == NULL && (node)->right == NULL)
#define bitree_data(node)       ((node)->data)
#define bitree_left(node)       ((node)->left)
#define bitree_right(node)       ((node)->right)


void bitree_init(BiTree * tree,void (*destroy)(void * data));
void bitree_destroy(BiTree * tree);
int bitree_ins_left(BiTree *tree,BiTreeNode *node,const void *data);
int bitree_ins_right(BiTree *tree,BiTreeNode *node,const void *data);

void bitree_rem_left(BiTree *tree,BiTreeNode *node);
void bitree_rem_right(BiTree *tree,BiTreeNode *node);

int bitree_merge(BiTree *merge,BiTree *left,BiTree *right,const void *data);


//O（1）
void bitree_init(BiTree * tree,void (*destroy)(void * data))
{
    tree->size = 0;
    tree->destroy = destroy;//用于回收内存
    tree->root = NULL;
    tree->compare = NULL;

    return ;
}

//O（n）
void bitree_destroy(BiTree * tree)
{
    bitree_rem_left(tree,NULL);//简单调用移除左

    memset(tree,0,sizeof(BiTree));//清空二叉树结构体

    return ;
}

//O（1）
int bitree_ins_left(BiTree *tree,BiTreeNode *node,const void *data)
{
    BiTreeNode *new_node,
                ** position;//获取待插入位置

    if(node == NULL)//是否插入空的二叉树
    {
        if(bitree_size(tree) > 0)//二叉树不为空--node也不能为空
            return -1;
        position = &tree->root;//插入的是root节点
    }
    else
    {
        if(bitree_left(node) != NULL)//是否已经有了左孩子
            return -1;

        position = &node->left;//没有左孩子--合理插入
    }

    if((new_node = (BiTreeNode *)malloc(sizeof(BiTreeNode))) == NULL)//分配内存
        return -1;

    new_node->data = (void *)data;//缓存数据
    new_node->left = NULL;
    new_node->right = NULL;

    *position = new_node;//更新节点

    tree->size++;//更新个数

    return 0;
}


//O（1）
int bitree_ins_right(BiTree *tree,BiTreeNode *node,const void *data)
{
    BiTreeNode *new_node,
                ** position;

    if(node == NULL)
    {
        if(bitree_size(tree) > 0)
            return -1;
        position = &tree->root;
    }
    else
    {
        if(bitree_right(node) != NULL)//是否存在右孩子
            return -1;

        position = &node->right;
    }

    if((new_node = (BiTreeNode *)malloc(sizeof(BiTreeNode))) == NULL)
        return -1;

    new_node->data = (void *)data;
    new_node->left = NULL;
    new_node->right = NULL;

    *position = new_node;

    tree->size++;

    return 0;
}

//O（n）
void bitree_rem_left(BiTree *tree,BiTreeNode *node)//移除node的左子树
{
    BiTreeNode **position;//指向节点指针的指针--二级指针
    if(bitree_size(tree) == 0)//不能对空二叉树操作
        return ;

    if(node == NULL)//直接从根节点开始删除
        position = &tree->root;
    else
        position = &node->left;//从node的左孩子开始删除

    if(*position != NULL)//位置有效--节点指针有效
    {
        //后序遍历
        bitree_rem_left(tree,*position);//移除左子树
        bitree_rem_right(tree,*position);//移除右子树
        if(tree->destroy != NULL)//回收函数有效
        {
            tree->destroy((*position)->data);//回收对应节点的数据--另一个二级指针（void ×data）
        }
        free(*position);//回收该节点的内存
        *position = NULL;//避免野指针

        tree->size--;//更新个数
    }

    return ;

}

//O（n）
void bitree_rem_right(BiTree *tree,BiTreeNode *node)
{
    BiTreeNode **position;
    if(bitree_size(tree) == 0)
        return ;

    if(node == NULL)
        position = &tree->root;
    else
        position = &node->right;//从node的右孩子开始删除

    if(*position != NULL)
    {
        bitree_rem_left(tree,*position);
        bitree_rem_right(tree,*position);
        if(tree->destroy != NULL)
        {
            tree->destroy((*position)->data);
        }
        free(*position);
        *position = NULL;

        tree->size--;
    }

    return ;
}

//合并两棵树，data为root
int bitree_merge(BiTree *merge,BiTree *left,BiTree *right,const void *data)
{
    bitree_init(merge,left->destroy);//初始化目标树

    if(bitree_ins_left(merge,NULL,data) != 0)//插入根节点
    {
        bitree_destroy(merge);
        return -1;
    }

    bitree_root(merge)->left = bitree_root(left);//设置root的左孩子
    bitree_root(merge)->right = bitree_root(right);//设置root的右孩子

    merge->size = merge->size + bitree_size(left) + bitree_size(right);//更新节点个数

    //撤销掉原来树的关系--避免通过原来的left或者right还能访问熄灯呢merge
    left->root = NULL;
    left->size = 0;
    right->root = NULL;
    right->size = 0;

    return 0;
}

//先序遍历
int preorder(const BiTreeNode * node,List *list)
{
    if(!bitree_is_eob(node))//起始节点不是该层的结束节点--否则不进行遍历
    {
        if(list_ins_next(list,list_tail(list),bitree_data(node)) != 0)//起始节点
            return -1;
        if(!bitree_is_eob(bitree_left(node)))//左子树
            if(preorder(bitree_left(node),list) != 0)
                return -1;

        if(!bitree_is_eob(bitree_right(node)))//右子树
            if(preorder(bitree_right(node),list) != 0)
                return -1;

    }

    return 0;
}


//中序遍历
int inorder(BiTreeNode *node,List *list)
{
    if(!bitree_is_eob(node))
    {
        if(!bitree_is_eob(bitree_left(node)))
            if(inorder(bitree_left(node),list) != 0)
                return -1;

        if(list_ins_next(list,list_tail(list),bitree_data(node)) != 0)
            return -1;

        if(!bitree_is_eob(bitree_right(node)))
            if(inorder(bitree_right(node),list) != 0)
                return -1;
    }

    return 0;
}

//后序遍历
int postorder(BiTreeNode *node,List *list)
{
    if(!bitree_is_eob(node))
    {
        if(!bitree_is_eob(bitree_left(node)))
            if(postorder(bitree_left(node),list) != 0)
                return -1;

        if(!bitree_is_eob(bitree_right(node)))
            if(postorder(bitree_right(node),list) != 0)
                return -1;

        if(list_ins_next(list,list_tail(list),bitree_data(node)) != 0)
            return -1;
    }

    return 0;
}

int main()
{
    return 0;
}
