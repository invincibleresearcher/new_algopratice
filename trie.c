#include <stdio.h>
#include <stdlib.h>

#define CHILD_LEN 26

struct trie {
    int is_terminal;
    int val;
    struct trie *c[CHILD_LEN];
};

static inline int char_to_inx(char a)
{
    return a - 'a';
}

void trie_init(struct trie *t)
{
    int i = 0;
    t->is_terminal = 0;
    for (i = 0; i < CHILD_LEN; i++) {
        t->c[i] = NULL;
    }
}

/*
void trie_add(struct trie *t, char *str, int val)
{
    int i;

    if (str[0] == '\0') {
        t->is_terminal = 1;
        t->val = val;
        return;
    }

    i = char_to_inx(str[0]);

    if (!t->c[i]) {
        t->c[i] = calloc(1, sizeof(*t));
    }
    
    trie_add(t->c[i], str+1, val);
}
*/

void trie_add(struct trie *t, char *str, int val)
{
    int i;
    while (str[0] != '\0') {
        i = char_to_inx(str[0]);
        if (!t->c[i]) {
            t->c[i] = calloc(1, sizeof(*t));
        }
        t = t->c[i];
        str = str+1;
    }
    
    t->is_terminal = 1;
    t->val = val;
}

/*
int trie_find(struct trie *t, char *str, int *val)
{
    int i;

    if (str[0] == '\0') {
        if (t->is_terminal) {
            *val = t->val;
            return 1;
        }
        else {
            return 0;
        }
    }

    i = char_to_inx(str[0]);
    if (!t->c[i]) return 0;
    return trie_find(t->c[i], str+1, val);
}
*/

int trie_find(struct trie *t, char *str, int *val)
{
    int i = 0;

    while (str[0] != '\0') {
        i = char_to_inx(str[0]);
        if (!t->c[i]) return 0;
        t = t->c[i];
        str = str+1;
    }

    if (t->is_terminal) {
        *val = t->val;
        return 1;
    }
    else {
        return 0;
    }
}

void trie_free(struct trie *t)
{
    int i;
    for (i = 0; i < CHILD_LEN; i++) {
        if (t->c[i]) trie_free(t->c[i]);
    }

    free(t);
}

static void print_trie(struct trie *t, char *s)
{
    int v, r;
    r = trie_find(t, s, &v);
    printf("%s: ret:%d val:%d\n", s, r, v);
}

int main(void)
{
    struct trie *t;
    t = malloc(sizeof(*t));

    trie_init(t);

    trie_add(t, "yon", 0);
    trie_add(t, "buying", 2);
    trie_add(t, "bu1", 3);
    trie_add(t, "y", 4);
    trie_add(t, "y1", 5);

    print_trie(t, "a");
    print_trie(t, "b");
    print_trie(t, "bu");

    printf("---\n");

    print_trie(t, "bu1");
    print_trie(t, "yon");
    print_trie(t, "buying");
    print_trie(t, "y2");
    print_trie(t, "y1");
    print_trie(t, "y");

    trie_free(t);
    return 0;
}
