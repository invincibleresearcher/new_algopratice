#include <stdio.h>
#include <stdlib.h>

// @c1
// @c2
// @c3

struct tn {
    int v;
    struct tn *l, *r;
};

void __print_inorder(struct tn *n)
{
    if (!n) return;
    if (n->l) __print_inorder(n->l);
    printf("%d, ", n->v);
    if (n->r) __print_inorder(n->r);
}

void print_inorder(struct tn *n)
{
    __print_inorder(n);
    printf("\n- - - - - - - - - \n");
}

void tree_clean(struct tn *n)
{
    if (!n) return;
    if (n->l) {
        tree_clean(n->l);
        n->l = NULL;
    }
    if (n->r) {
        tree_clean(n->r);
        n->r = NULL;
    }
    free(n);
}

static inline struct tn *create_node(void)
{
    struct tn *n = malloc(sizeof(*n));
    n->l = n->r = NULL;
    return n;
}

struct tn *add(struct tn* n, int v)
{
    if (!n) {
        n = create_node();
        n->v = v;
        return n;
    }

    if (v < n->v) {
        n->l = add(n->l, v);
    }
    else {
        n->r = add(n->r, v);
    }
    return n;
}

int search(struct tn *n, int v)
{
    while (n) {
        if (n->v == v) {
            return 0;
        }
        else if (v < n->v) {
            n = n->l;
        }
        else {
            n = n->r;
        }
    }
    return -1;
}

int min_find(struct tn *n)
{
    while (n->l) {
        n = n->l;
    }
    return n->v;
}

struct tn *delete(struct tn *n, int v)
{
    int minv;
    struct tn *tmp;
    if (v < n->v) {
        n->l = delete(n->l, v);
        return n;
    }
    else if (v > n->v) {
        n->r = delete(n->r, v);
        return n;
    }

    if (n->v != v) return n;

    if (!n->l && !n->r) {
        free(n);
        return NULL;
    }
    else if (!n->l && n->r) {
        tmp = n->r;
        free(n);
        return tmp;
    }
    else if (n->l && !n->r) {
        tmp = n->l;
        free(n);
        return tmp;
    }
    else {
        minv = min_find(n->r);
        n->r = delete(n->r, minv);
        n->v = minv;
    }
    return n;
}

void __print_highest(struct tn *n, int *c, int ll)
{
    if (!n) return;

    if (n->r && *c < ll) __print_highest(n->r, c, ll);

    if (*c < ll) {
        printf("%d, ", n->v);
        (*c)++;
    }

    if (n->l && *c < ll) __print_highest(n->l, c, ll);
}

void print_highest(struct tn *n, int ll)
{
    int c = 0;
    printf("highest(%d) : ", ll);
    __print_highest(n, &c, ll);
    printf("\n- - - - - - - - - - - -\n");
}

void __print_lowest(struct tn *n, int *c, int ll)
{
    if (!n) return;

    if (n->l && *c < ll) __print_lowest(n->l, c, ll);

    if (*c < ll) {
        printf("%d, ", n->v);
        (*c)++;
    }

    if (n->r && *c < ll) __print_lowest(n->r, c, ll);
}

void print_lowest(struct tn *n, int ll)
{
    int c = 0;
    printf("lowest(%d) : ", ll);
    __print_lowest(n, &c, ll);
    printf("\n- - - - - - - - - - - -\n");
}

struct tn *root = NULL;

void _add(int v)
{
    printf("%d added\n", v);
    root = add(root, v);
}

void test1(void)
{
    printf("%s> start\n", __func__);
    _add(20);
    _add(15);
    _add(28);
    _add(10);
    _add(17);
    _add(4);
    _add(12);

    print_inorder(root);
    print_highest(root, 1);
    print_highest(root, 3);
    print_highest(root, 5);
    print_lowest(root, 1);
    print_lowest(root, 3);
    print_lowest(root, 5);

    _add(19);
    _add(26);
    _add(33);
    _add(30);
    _add(35);
    print_inorder(root);
    print_highest(root, 1);
    print_highest(root, 3);
    print_highest(root, 5);
    print_lowest(root, 1);
    print_lowest(root, 3);
    print_lowest(root, 5);
}

void search_ut(int v)
{
    int ret = search(root, v);
    if (!ret) {
        printf("(O) %d found\n", v);
    }
    else {
        printf("(X) %d not found\n", v);
    }
}

void test2(void)
{
    printf("%s> start\n", __func__);
    search_ut(1);
    search_ut(5);
    search_ut(20);
    search_ut(10);
    search_ut(50);
    search_ut(19);
    search_ut(30);
    search_ut(35);
}

void _del(int v)
{
    printf("%d deleted\n", v);
    root = delete(root, v);
}

void test3(void)
{
    int i;
    printf("%s> start\n", __func__);
    for (i = 0; i < 10; i++) {
        _del(4);
        print_inorder(root);
        _del(17);
        print_inorder(root);
        _del(33);
        print_inorder(root);
        _del(26);
        print_inorder(root);
        _del(20);
        print_inorder(root);
        print_highest(root, 3);
        print_lowest(root, 3);

        _add(4);
        print_inorder(root);
        _add(17);
        print_inorder(root);
        _add(33);
        print_inorder(root);
        _add(26);
        print_inorder(root);
        _add(20);
        print_inorder(root);
        print_highest(root, 3);
        print_lowest(root, 3);
    }
}

int main(void)
{
    test1();
    test2();
    test3();

    tree_clean(root);
    return 0;
}
