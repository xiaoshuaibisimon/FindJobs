/*
 * bitree.h
 *
 *  Created on: 2017年5月18日
 *      Author: Administrator
 */

#ifndef BITREE_H_
#define BITREE_H_

#include "list.h"


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

int postorder(BiTreeNode *node,List *list);
int inorder(BiTreeNode *node,List *list);
int preorder(const BiTreeNode * node,List *list);


#endif /* BITREE_H_ */
