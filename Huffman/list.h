/*
 * list.h
 *
 *  Created on: 2017年5月18日
 *      Author: Administrator
 */

#ifndef LIST_H_
#define LIST_H_

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


#endif /* LIST_H_ */
