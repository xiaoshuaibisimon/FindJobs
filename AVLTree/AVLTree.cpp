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

/************************************************************************/

//平衡因子

#define            AVL_LFT_HEAVY         1
#define            AVL_BALANCED          0
#define            AVL_RGT_HEAVY        -1

//AVL树节点结构体
typedef struct AvlNode_ {

void               *data;//数据域
int                hidden;//是否隐藏
int                factor;//平衡因子--维持稳态

} AvlNode;

//AVL树（优秀的二叉搜索树）基于普通二叉树实现
typedef BiTree BisTree;

#define bistree_size(tree) ((tree)->size)

//O（1）
void bistree_init(BisTree *tree, int (*compare)(const void *key1, const void
   *key2), void (*destroy)(void *data));

//O（n）
void bistree_destroy(BisTree *tree);

//O（lgn）
int bistree_insert(BisTree *tree, const void *data);

//O（lgn）
int bistree_remove(BisTree *tree, const void *data);

//O（lgn）
int bistree_lookup(BisTree *tree, void **data);


static void destroy_right(BisTree *tree, BiTreeNode *node);

//LL或者LR
static void rotate_left(BiTreeNode **node) {

    BiTreeNode         *left,
                       *grandchild;//LR会用到

    left = bitree_left(*node);//获取平衡因子为+-2的起始节点（从该节点开始需要进行平衡调整）的左孩子

    if (((AvlNode *)bitree_data(left))->factor == AVL_LFT_HEAVY) {//左倾斜--属于LL的情况

       //进行LL旋转

       //1. A的左指针指向left的右节点
       bitree_left(*node) = bitree_right(left);

       //2. left的右指针指向节点A
       bitree_right(left) = *node;

       //3.更新A和left的平衡因子都为0
       ((AvlNode *)bitree_data(*node))->factor = AVL_BALANCED;
       ((AvlNode *)bitree_data(left))->factor = AVL_BALANCED;

       //4. 将原来指向节点A的指针指向节点left--node是一个指向指针的指针
       //在树中，用节点类型的二级指针（node ××）表示指向节点指针（node×）的指针
       *node = left;

       }

    else {//left节点起始的树是右倾斜

       //进行LR旋转

       //获取grandchild
       grandchild = bitree_right(left);

       //1. left的右指针指向grandchild的左孩子
       bitree_right(left) = bitree_left(grandchild);

       //2. grandchild的左指针指向left
       bitree_left(grandchild) = left;

       //3. A的左指针指向grandchild的右孩子
       bitree_left(*node) = bitree_right(grandchild);

       //4. grandchild的右指针指向A
       bitree_right(grandchild) = *node;

       //5. 更新平衡因子
       switch (((AvlNode *)bitree_data(grandchild))->factor) {//根据grandchild之前的平衡因子更新平衡因子

          case AVL_LFT_HEAVY://+1

              ((AvlNode *)bitree_data(*node))->factor = AVL_RGT_HEAVY;//A为-1
              ((AvlNode *)bitree_data(left))->factor = AVL_BALANCED;//left为0
              break;

          case AVL_BALANCED://0

              ((AvlNode *)bitree_data(*node))->factor = AVL_BALANCED;//A为0
              ((AvlNode *)bitree_data(left))->factor = AVL_BALANCED;//left为0
              break;

          case AVL_RGT_HEAVY://-1

              ((AvlNode *)bitree_data(*node))->factor = AVL_BALANCED;//A为0
              ((AvlNode *)bitree_data(left))->factor = AVL_LFT_HEAVY;//left为+1
              break;

       }

       ((AvlNode *)bitree_data(grandchild))->factor = AVL_BALANCED;//不管如何grandchild的平衡因子最后总是0

       //6. 将原来指向A的指针指向grandchild
       *node = grandchild;

    }

    return;

}

//RR或者RL

static void rotate_right(BiTreeNode **node) {

    BiTreeNode         *right,
                       *grandchild;//RL会用到

    right = bitree_right(*node);//获取平衡因子为+-2的起始节点/*node指向的节点（从该节点开始需要进行平衡调整）的右孩子

    if (((AvlNode *)bitree_data(right))->factor == AVL_RGT_HEAVY)
    {//右倾斜--属于RR情况

       //进行RR旋转

       //1. A的右指针指向right的左孩子
       bitree_right(*node) = bitree_left(right);

       //2. right的左指针指向A
       bitree_left(right) = *node;

       //3. 更新平衡因子--都为0
       ((AvlNode *)bitree_data(*node))->factor = AVL_BALANCED;
       ((AvlNode *)bitree_data(right))->factor = AVL_BALANCED;

       //4. 将原来指向A的指针指向right
       *node = right;

    }
    else//right起始节点的树是左倾斜
    {

       //进行RL旋转

       //获取grandchild
       grandchild = bitree_left(right);

       //1. right的左指针指向grandchild的右孩子
       bitree_left(right) = bitree_right(grandchild);

       //2. grandchild的右指针指向right
       bitree_right(grandchild) = right;

       //3. A的右指针指向grandchild的左孩子
       bitree_right(*node) = bitree_left(grandchild);

       //4. grandchild的左指针指向A
       bitree_left(grandchild) = *node;


       //5. 更新平衡因子
       switch (((AvlNode *)bitree_data(grandchild))->factor) {//根据grandchild之前的平衡因子更新平衡因子

          case AVL_LFT_HEAVY://+1

              ((AvlNode *)bitree_data(*node))->factor = AVL_BALANCED;//A为0
              ((AvlNode *)bitree_data(right))->factor = AVL_RGT_HEAVY;//right为-1
              break;

          case AVL_BALANCED://0

              ((AvlNode *)bitree_data(*node))->factor = AVL_BALANCED;//A为0
              ((AvlNode *)bitree_data(right))->factor = AVL_BALANCED;//right为0
              break;

          case AVL_RGT_HEAVY://-1

              ((AvlNode *)bitree_data(*node))->factor = AVL_LFT_HEAVY;//A为+1
              ((AvlNode *)bitree_data(right))->factor = AVL_BALANCED;//right为0
              break;

       }

       ((AvlNode *)bitree_data(grandchild))->factor = AVL_BALANCED;//不管何时，grandchild的平衡因子都是0

       //6. 将原来指向A的指针指向grandchild
       *node = grandchild;

    }

    return;

}

//销毁左子树
static void destroy_left(BisTree *tree, BiTreeNode *node) {

    BiTreeNode         **position;


    //不允许操作空树
    if (bitree_size(tree) == 0)
       return;

    //决定开始销毁的起始节点
    if (node == NULL)
       position = &tree->root;//从root节点开始--销毁整个AVL树
    else
       position = &node->left;//node的左孩子开始的子树---node的左子数

    //销毁节点
    if (*position != NULL) {

        //后序遍历
       destroy_left(tree, *position);
       destroy_right(tree, *position);

       if (tree->destroy != NULL) {

          //释放AvlNode结构体中（void ×data）对应的内存
          //AVL树里面的节点总的来说是二叉树节点，只不过节点里面的数据指针void*指向的是AvlNode类型的数据
          //AvlNode类型的数据里面又指向其他真实有效的数据类型
          tree->destroy(((AvlNode *)(*position)->data)->data);

       }

       //释放节点里面的（void×data对应的类型）--仅仅是一个AvlNode结构体占据的内存
       free((*position)->data);

       //仅仅是树里面的节点内存--一个BiTreeNode结构体占据的内存
       free(*position);
       *position = NULL;

       //更新节点个数

       tree->size--;

    }

    return;

}

//销毁右子树
static void destroy_right(BisTree *tree, BiTreeNode *node) {

    BiTreeNode         **position;



    if (bitree_size(tree) == 0)
       return;


    if (node == NULL)
       position = &tree->root;
    else
       position = &node->right;



    if (*position != NULL) {

       destroy_left(tree, *position);
       destroy_right(tree, *position);

       if (tree->destroy != NULL) {

          tree->destroy(((AvlNode *)(*position)->data)->data);

       }

       free((*position)->data);
       free(*position);
       *position = NULL;

       tree->size--;

    }

    return;

}

//向AVL树插入节点
static int insert(BisTree *tree, BiTreeNode **node, const void *data, int
   *balanced) {

    AvlNode            *avl_data;

    int                cmpval,
                       retval;


    if (bitree_is_eob(*node)) {//是否将节点插入空树


       //分配内存
       if ((avl_data = (AvlNode *)malloc(sizeof(AvlNode))) == NULL)
          return -1;

       avl_data->factor = AVL_BALANCED;//平衡因子
       avl_data->hidden = 0;//不隐藏
       avl_data->data = (void *)data;//AvlNode里面的data指向真是的有效数据

       //调用二叉树的插入函数插入根节点
       return bitree_ins_left(tree, *node, avl_data);

       }

    else {//不是插入空树

       cmpval = tree->compare(data, ((AvlNode *)bitree_data(*node))->data);//将要插入的数据与当前节点的数据相比

       if (cmpval < 0)
       {//小于则插入左子树--保持二叉搜索树的特性

            if (bitree_is_eob(bitree_left(*node)))
            {//当前节点没有左孩子--此时才是真的找到插入位置

                if ((avl_data = (AvlNode *)malloc(sizeof(AvlNode))) == NULL)
                return -1;

                avl_data->factor = AVL_BALANCED;//设置平衡因子为0
                avl_data->hidden = 0;//不隐藏
                avl_data->data = (void *)data;//设置有效数据

                if (bitree_ins_left(tree, *node, avl_data) != 0)//插入新节点
                return -1;

                *balanced = 0;//破坏了平衡性--新节点的父节点（*node）的平衡因子变化了--需要更新

            }
            else
            {

                //递归调用--往左移动新节点
                if ((retval = insert(tree, &bitree_left(*node), data, balanced))
                != 0)
                {

                return retval;

                }

            }

            //保证AVL树的平衡性
            if (!(*balanced))//还未平衡--遍历树当前这一层的节点，并更新新节点的平衡因子
            {

                switch (((AvlNode *)bitree_data(*node))->factor) {//根据顶点的平衡因子决定如何旋转

                case AVL_LFT_HEAVY://之前就是左倾斜

                    rotate_left(node);//左旋转
                    *balanced = 1;//没有节点需要更新了--操作完成--保持了平衡
                    break;

                case AVL_BALANCED://之前平衡

                    ((AvlNode *)bitree_data(*node))->factor = AVL_LFT_HEAVY;//插入左节点以后肯定是左倾斜了--更新新节点平衡因子
                    break;

                case AVL_RGT_HEAVY://之前右倾斜

                    ((AvlNode *)bitree_data(*node))->factor = AVL_BALANCED;//插入新节点以后肯定是平衡了
                    *balanced = 1;//没有节点需要更新了

                }

            }

          } /* if (cmpval < 0) */

       else if (cmpval > 0) {//大于则插入右子树--保持二叉搜索树的特性


          if (bitree_is_eob(bitree_right(*node))) {//没有右孩子--找到合适位置插入

             if ((avl_data = (AvlNode *)malloc(sizeof(AvlNode))) == NULL)
                return -1;

             avl_data->factor = AVL_BALANCED;//新节点平衡因子是平衡的--0
             avl_data->hidden = 0;
             avl_data->data = (void *)data;

             if (bitree_ins_right(tree, *node, avl_data) != 0)
                return -1;

             *balanced = 0;

             }

          else {

              //递归调用--往右移动新节点
             if ((retval = insert(tree, &bitree_right(*node), data, balanced))
                != 0) {

                return retval;

             }

          }


          //确保AVL树的平衡
          if (!(*balanced)) {//有节点的平衡因子需要更新--（*node）

             switch (((AvlNode *)bitree_data(*node))->factor) {//之前的平衡因子

                case AVL_LFT_HEAVY://之前左倾斜

                    ((AvlNode *)bitree_data(*node))->factor = AVL_BALANCED;//插入新节点到右子树以后变平衡了
                    *balanced = 1;//没有节点的因子需要更新
                    break;

                case AVL_BALANCED://之前平衡

                    ((AvlNode *)bitree_data(*node))->factor = AVL_RGT_HEAVY;//变成右倾斜了
                    break;

                case AVL_RGT_HEAVY://之前右倾斜

                    rotate_right(node);//，插入以后更是右倾斜了，变成了+-2需要进行旋转了
                    *balanced = 1;//没有节点的因子需要更新了

             }

          }

          } /* if (cmpval > 0) */

       else {//要插入的数据和当前节点的数据一样

            if (!((AvlNode *)bitree_data(*node))->hidden)//如果匹配的节点没有被隐藏--什么也不做--表示树里面已经存在相应的节点

                return 1;
            else {

                //插入新节点，并取消隐藏
                if (tree->destroy != NULL)
                {//回收内存函数有效
                   //回收已被隐藏的数据
                    tree->destroy(((AvlNode *)bitree_data(*node))->data);
                 }

                //更新数据
                ((AvlNode *)bitree_data(*node))->data = (void *)data;
                ((AvlNode *)bitree_data(*node))->hidden = 0;//取消隐藏

               //不用旋转进行平衡调整--结构没有变化
                *balanced = 1;

            }

       }

    }

    return 0;

}


//定位隐藏的节点--在惰性移除的时候会用到
static int hide(BisTree *tree, BiTreeNode *node, const void *data) {

    int                cmpval,
                       retval;

    if (bitree_is_eob(node)) {//起始节点是边缘--给的参数是空指针--直接返回
       return -1;
    }

    cmpval = tree->compare(data, ((AvlNode *)bitree_data(node))->data);//比较要寻找数据和当前节点的数据

    if (cmpval < 0) {//比当前节点小--递归往左子树寻找
       retval = hide(tree, bitree_left(node), data);
    }
    else if (cmpval > 0) {//比当前节点大--递归往右子树寻找
       retval = hide(tree, bitree_right(node), data);
    }
    else {//匹配上了--bingo
       ((AvlNode *)bitree_data(node))->hidden = 1;//隐藏起来吧，少年--所谓的惰性移除
       retval = 0;
    }

    return retval;

}

//查询给定数据的节点
static int lookup(BisTree *tree, BiTreeNode *node, void **data) {

    int                cmpval,
                       retval;

    if (bitree_is_eob(node)) {//起始节点是边缘--给的参数是空指针--直接返回
       return -1;
    }

    //比较
    cmpval = tree->compare(*data, ((AvlNode *)bitree_data(node))->data);

    if (cmpval < 0) {//左子树
       retval = lookup(tree, bitree_left(node), data);
    }
    else if (cmpval > 0) {//右子树
       retval = lookup(tree, bitree_right(node), data);
    }
    else {
       if (!((AvlNode *)bitree_data(node))->hidden) {//没有被隐藏
          //取出数据
          *data = ((AvlNode *)bitree_data(node))->data;
          retval = 0;
        }
        else {//已经被隐藏了，表示没找到
          return -1;
        }

    }

    return retval;
}

//AVL树初始化
void bistree_init(BisTree *tree, int (*compare)(const void *key1, const void
   *key2), void (*destroy)(void *data))
{

    bitree_init(tree, destroy);//普通二叉树初始化
    tree->compare = compare;//重置匹配函数

    return;
}

//销毁AVL树
void bistree_destroy(BisTree *tree)
{

    //销毁树里面的所有节点
    destroy_left(tree, NULL);

    //清空AVL树结构体，不允许任何操作

    memset(tree, 0, sizeof(BisTree));

    return;

}

//插入节点
int bistree_insert(BisTree *tree, const void *data)
{

    int                balanced = 0;//默认是没有节点需要更新因子的

    return insert(tree, &bitree_root(tree), data, &balanced);//调用insert实现插入

}

//移除节点
int bistree_remove(BisTree *tree, const void *data)
{
    return hide(tree, bitree_root(tree), data);//使用惰性移除
}

//查询某个节点
int bistree_lookup(BisTree *tree, void **data)
{

    return lookup(tree, bitree_root(tree), data);//返回以后，data指向AvlNode结构体里面的data成员指向的内存--实际有效的数据的起始地址

}

int main()
{
    return 0;
}
