#include <stdio.h>
#include <stdlib.h>

#define CHILD_LEN 26

typedef struct trie {
    int is_terminal;
    int val;
    struct trie *c[CHILD_LEN];
} Trie;

static inline int char_to_inx(char a)
{
    return a - 'a';
}

void trie_init(Trie *t)
{
    int i = 0;
    t->is_terminal = 0;
    for (i = 0; i < CHILD_LEN; i++) {
        t->c[i] = NULL;
    }
}

void trie_add(Trie *t, char *str, int val)
{
    int i;
    while (str[0] != '\0') {
        i = char_to_inx(*str);
        if (!t->c[i]) {
            t->c[i] = calloc(1, sizeof(*t));
        }
        t = t->c[i];
        str++;
    }
    
    t->is_terminal = 1;
    t->val = val;
}

static inline int mstr_len(char *str)
{
    int cnt = 0;
    while (*str != '\0') {
        cnt++;
        str++;
    }
    return cnt;
}

int trie_find(Trie *t, char *str, int *val)
{
    int i = 0;

    while (str[0] != '\0') {
        i = char_to_inx(*str);
        if (!t->c[i]) return 0;
        t = t->c[i];
        str++;
    }

    if (t->is_terminal) {
        *val = t->val;
        return 1;
    }
    else {
        return 0;
    }
}

static inline int is_empty(Trie *t)
{
    int i;
    for (i = 0; i < CHILD_LEN; i++) {
        if (t->c[i]) return 0;
    }
    return 1;
}

Trie *__trie_remove(Trie *t, char *str)
{
    int i;
    if (*str != '\0') {
        i = char_to_inx(*str);
        t->c[i] = __trie_remove(t->c[i], ++str);
    }

    if (is_empty(t)) {
        free(t);
        return NULL;
    }
    else {
        if (t->is_terminal && *str == '\0') {
            t->is_terminal = 0;
        }
        return t;
    }
}

void trie_remove(Trie *t, char *str)
{
    int i;
    i = char_to_inx(*str);
    t->c[i] = __trie_remove(t->c[i], ++str);
}

static void __trie_free(Trie *t)
{
    int i = 0;
    for (i = 0; i < CHILD_LEN; i++) {
        if (t->c[i]) {
            __trie_free(t->c[i]);
        }
    }
    free(t);
}

void trie_free(Trie *t)
{
    int i;
    for (i = 0; i < CHILD_LEN; i++) {
        if (t->c[i]) __trie_free(t->c[i]);
    }
}

static void print_trie(Trie *t, char *s)
{
    int v, r;
    r = trie_find(t, s, &v);
    printf("%s: ret:%d val:%d\n", s, r, v);
}

int main(void)
{
    int i = 0;
    Trie t;

    trie_init(&t);

    for (i = 0; i < 5; i++) {
        printf("!! %d iter\n", i+1);

        trie_add(&t, "yon", 0);
        trie_add(&t, "buying", 2);
        trie_add(&t, "bu", 3);
        trie_add(&t, "y", 4);
        trie_add(&t, "yx", 5);

        print_trie(&t, "a");
        print_trie(&t, "b");
        print_trie(&t, "bu");

        printf("---\n");

        print_trie(&t, "bua");
        print_trie(&t, "yon");
        print_trie(&t, "buying");
        print_trie(&t, "yz");
        print_trie(&t, "yd");
        print_trie(&t, "y");

        printf("===\n");

        trie_remove(&t, "y");
        print_trie(&t, "y");

        print_trie(&t, "yx");
        trie_remove(&t, "yx");
        print_trie(&t, "yx");

        print_trie(&t, "yon");
        trie_remove(&t, "yon");
        print_trie(&t, "yon");

        print_trie(&t, "bu");
        trie_remove(&t, "bu");
        print_trie(&t, "bu");

        print_trie(&t, "buying");
    }

    trie_remove(&t, "buying");

    printf("trie empty:%d\n", is_empty(&t));

    trie_free(&t);
    return 0;
}
