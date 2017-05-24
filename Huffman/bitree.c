/*
 * bitree.c
 *
 *  Created on: 2017年5月18日
 *      Author: Administrator
 */

#include "bitree.h"


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
