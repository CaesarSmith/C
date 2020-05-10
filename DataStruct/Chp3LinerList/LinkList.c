#include <stdio.h>
#include <stdlib.h>

#define OK 1
#define ERROR 0
#define TRUE 1
#define FALSE 0

typedef int Status;
typedef int ElemType;

typedef struct Node
{
    ElemType data;
    struct Node *next;
} Node;
typedef struct Node *LinkList; //实际上是一个指向头结点的指针，即头指针

Status Visit(ElemType c)
{
    printf("%d ", c);
    return OK;
}

Status InitList(LinkList *L)
{
    *L = (LinkList)malloc(sizeof(Node));
    if (!L)
    {
        return ERROR;
    }

    (*L)->next = NULL;
    return OK;
}

Status ListEmpty(LinkList L)
{
    if (L->next)
    {
        return FALSE;
    }
    else
    {
        return TRUE;
    }
}

Status ClearList(LinkList *L)
{
    LinkList p, q;
    p = (*L)->next;
    while (p)
    {
        q = p->next;
        free(p);
        p = q;
    }
    (*L)->next = NULL;
    return OK;
}

Status GetElem(LinkList L, int i, ElemType *e)
{
    int j = 0;
    LinkList p = L->next;
    for (j = 1; j <= i; j++)
    {
        if (!(p->next))
        {
            return ERROR;
        }
        p = p->next;
    }
    *e = p->data;
    return OK;
}

int LocateElem(LinkList L, ElemType e)
{
    int i = 1;
    LinkList p = L->next;
    while (p)
    {
        if (p->data == e)
        {
            return i;
        }
        else
        {
            p = p->next;
            i++;
        }
    }
    return 0;
}

Status ListInsert(LinkList *L, int i, ElemType e)
{
    int j = 1;
    LinkList p = (*L)->next, q;
    if (i <= 0)
    {
        return ERROR;
    }
    while (j <= i)
    {
        if (!p)
            return ERROR;
        j++;
        p = p->next;
    }

    q = (LinkList)malloc(sizeof(Node));
    q->data = e;
    q->next = p->next;
    p->next = q;

    return OK;
}

Status ListDelete(LinkList *L, int i, ElemType *e)
{
    int j;
    if (i >= 1 && i <= L->length)
    {
        *e = L->data[i - 1];
        for (j = i; j < L->length; j++)
        {
            L->data[j - 1] = L->data[j];
        }
        L->length--;
        return OK;
    }
    else
    {
        return ERROR;
    }
}

int ListLength(LinkList L)
{
    return L.length;
}

Status ListTraverse(LinkList L)
{
    int i = 0;
    for (i = 0; i < L.length; i++)
    {
        Visit(L.data[i]);
    }
    printf("\n");
    return OK;
}

void Union(LinkList *La, LinkList Lb)
{
    int la_len = 0, lb_len = 0, i = 0;
    ElemType e;
    la_len = ListLength(*La);
    lb_len = ListLength(Lb);
    for (i = 1; i <= lb_len; i++)
    {
        GetElem(Lb, i, &e);
        if (!LocateElem(*La, e))
        {
            ListInsert(La, ++la_len, e);
        }
    }
}

int main()
{

    LinkList L;
    LinkList Lb;

    ElemType e;
    Status i;
    int j, k;
    i = InitList(&L);
    printf("初始化L后：L.length=%d\n", L.length);
    for (j = 1; j <= 5; j++)
        i = ListInsert(&L, 1, j);
    printf("在L的表头依次插入1～5后：L.data=");
    ListTraverse(L);

    printf("L.length=%d \n", L.length);
    i = ListEmpty(L);
    printf("L是否空：i=%d(1:是 0:否)\n", i);

    i = ClearList(&L);
    printf("清空L后：L.length=%d\n", L.length);
    i = ListEmpty(L);
    printf("L是否空：i=%d(1:是 0:否)\n", i);

    for (j = 1; j <= 10; j++)
        ListInsert(&L, j, j);
    printf("在L的表尾依次插入1～10后：L.data=");
    ListTraverse(L);

    printf("L.length=%d \n", L.length);

    ListInsert(&L, 1, 0);
    printf("在L的表头插入0后：L.data=");
    ListTraverse(L);
    printf("L.length=%d \n", L.length);

    GetElem(L, 5, &e);
    printf("第5个元素的值为：%d\n", e);
    for (j = 3; j <= 4; j++)
    {
        k = LocateElem(L, j);
        if (k)
            printf("第%d个元素的值为%d\n", k, j);
        else
            printf("没有值为%d的元素\n", j);
    }

    k = ListLength(L); /* k为表长 */
    for (j = k + 1; j >= k; j--)
    {
        i = ListDelete(&L, j, &e); /* 删除第j个数据 */
        if (i == ERROR)
            printf("删除第%d个数据失败\n", j);
        else
            printf("删除第%d个的元素值为：%d\n", j, e);
    }
    printf("依次输出L的元素：");
    ListTraverse(L);

    j = 5;
    ListDelete(&L, j, &e); /* 删除第5个数据 */
    printf("删除第%d个的元素值为：%d\n", j, e);

    printf("依次输出L的元素：");
    ListTraverse(L);

    //构造一个有10个数的Lb
    i = InitList(&Lb);
    for (j = 6; j <= 15; j++)
        i = ListInsert(&Lb, 1, j);

    Union(&L, Lb);

    printf("依次输出合并了Lb的L的元素：");
    ListTraverse(L);

    return 0;
}
