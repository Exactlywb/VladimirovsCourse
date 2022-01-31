#include <stdio.h>
#include <stdlib.h>
typedef struct node
{
    int data;
    struct node *l_node;
    struct node *r_node;
    struct node *p_node;
} node;
typedef struct sPlayTree_DS
{
    struct node *root_node;
} sPlayTree_DS;
node *n_node(int data)
{
    node *n = malloc(sizeof(node));
    n->data = data;
    n->p_node = NULL;
    n->r_node = NULL;
    n->l_node = NULL;
    return n;
}
sPlayTree_DS *new_sPlayTree_DS()
{
    sPlayTree_DS *t = malloc(sizeof(sPlayTree_DS));
    t->root_node = NULL;
    return t;
}
node *maximum(sPlayTree_DS *t, node *x)
{
    while (x->r_node != NULL)
        x = x->r_node;
    return x;
}
void l_node_rotate(sPlayTree_DS *t, node *x)
{
    node *y = x->r_node;
    x->r_node = y->l_node;
    if (y->l_node != NULL)
    {
        y->l_node->p_node = x;
    }
    y->p_node = x->p_node;
    if (x->p_node == NULL)
    { // x is  root_node
        t->root_node = y;
    }
    else if (x == x->p_node->l_node)
    { // x is  l_node child
        x->p_node->l_node = y;
    }
    else
    { // x is  r_node child
        x->p_node->r_node = y;
    }
    y->l_node = x;
    x->p_node = y;
}
void r_node_rotate(sPlayTree_DS *t, node *x)
{
    node *y = x->l_node;
    x->l_node = y->r_node;
    if (y->r_node != NULL)
    {
        y->r_node->p_node = x;
    }
    y->p_node = x->p_node;
    if (x->p_node == NULL)
    { // x is  root_node
        t->root_node = y;
    }
    else if (x == x->p_node->r_node)
    { // x is  l_node child
        x->p_node->r_node = y;
    }
    else
    { // x is  r_node child
        x->p_node->l_node = y;
    }
    y->r_node = x;
    x->p_node = y;
}
void splay(sPlayTree_DS *t, node *n)
{
    while (n->p_node != NULL)
    { // node is not  root_node
        if (n->p_node == t->root_node)
        { // node is child of  root_node, one
            // rotation
            if (n == n->p_node->l_node)
            {
                r_node_rotate(t, n->p_node);
            }
            else
            {
                l_node_rotate(t, n->p_node);
            }
        }
        else
        {
            node *p = n->p_node;
            node *g = p->p_node; // grand p_node
            if (n->p_node->l_node == n &&
                p->p_node->l_node == p)
            { // both are  l_node children
                r_node_rotate(t, g);
                r_node_rotate(t, p);
            }
            else if (n->p_node->r_node == n &&
                     p->p_node->r_node == p)
            { // both are  r_node children
                l_node_rotate(t, g);
                l_node_rotate(t, p);
            }
            else if (n->p_node->r_node == n && p->p_node->l_node == p)
            {
                l_node_rotate(t, p);
                r_node_rotate(t, g);
            }
            else if (n->p_node->l_node == n && p->p_node->r_node == p)
            {
                r_node_rotate(t, p);
                l_node_rotate(t, g);
            }
        }
    }
}
void insert(sPlayTree_DS *t, node *n)
{
    node *y = NULL;
    node *temp = t->root_node;
    while (temp != NULL)
    {
        y = temp;
        if (n->data < temp->data)
            temp = temp->l_node;
        else
            temp = temp->r_node;
    }
    n->p_node = y;
    if (y == NULL) // newly added node is  root_node
        t->root_node = n;
    else if (n->data < y->data)
        y->l_node = n;
    else
        y->r_node = n;
    splay(t, n);
}
node *search(sPlayTree_DS *t, node *n, int x)
{
    if (x == n->data)
    {
        splay(t, n);
        return n;
    }
    else if (x < n->data)
        return search(t, n->l_node, x);
    else if (x > n->data)
        return search(t, n->r_node, x);
    else
        return NULL;
}
void delete (sPlayTree_DS *t, node *n)
{
    splay(t, n);
    sPlayTree_DS *l_node_subtree = new_sPlayTree_DS();
    l_node_subtree->root_node = t->root_node->l_node;
    if (l_node_subtree->root_node != NULL)
        l_node_subtree->root_node->p_node = NULL;
    sPlayTree_DS *r_node_subtree = new_sPlayTree_DS();
    r_node_subtree->root_node = t->root_node->r_node;
    if (r_node_subtree->root_node != NULL)
        r_node_subtree->root_node->p_node = NULL;
    free(n);
    if (l_node_subtree->root_node != NULL)
    {
        node *m = maximum(l_node_subtree, l_node_subtree->root_node);
        splay(l_node_subtree, m);
        l_node_subtree->root_node->r_node = r_node_subtree->root_node;
        t->root_node = l_node_subtree->root_node;
    }
    else
    {
        t->root_node = r_node_subtree->root_node;
    }
}
void inorder(sPlayTree_DS *t, node *n)
{
    if (n != NULL)
    {
        inorder(t, n->l_node);
        printf("%d\n", n->data);
        inorder(t, n->r_node);
    }
}
int main()
{
    sPlayTree_DS *t = new_sPlayTree_DS();
    node *p, *q, *r, *s, *t, *u, *v, *w, *x, *y, *z, *a, *b;
    p = n_node(10);
    q = n_node(20);
    r = n_node(30);
    s = n_node(100);
    t = n_node(90);
    u = n_node(40);
    v = n_node(50);
    w = n_node(60);
    x = n_node(70);
    y = n_node(80);
    z = n_node(150);
    a = n_node(110);
    b = n_node(120);
    insert(t, p);
    insert(t, q);
    insert(t, r);
    insert(t, s);
    insert(t, t);
    insert(t, u);
    insert(t, v);
    insert(t, w);
    insert(t, x);
    insert(t, y);
    insert(t, z);
    insert(t, a);
    insert(t, b);
    delete (t, p);
    delete (t, b);
    inorder(t, t->root_node);
    return 0;
}