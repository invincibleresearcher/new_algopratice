#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MAX_STRLEN 200
#define HASH_T_SIZE 22

// c1

typedef struct {
    char key[MAX_STRLEN];
    int val;
} pair;

struct node {
    pair p;
    struct node *next;
};

typedef struct {
    struct node head[HASH_T_SIZE];
} hash;

static void hash_init(hash *h)
{
    int i = 0;
    for (i = 0; i < HASH_T_SIZE; i++) {
        h->head[i].next = NULL;
    }
}

static inline unsigned int hash_code(char *s)
{
    int i, len;
    unsigned int c = 0;

    len = strlen(s);
    for (i = 0; i < len; i++) {
        c = 31*c + s[i];
    }
    return c;
}

static inline void __hash_add(struct node *h, char *key, int val)
{
    struct node *n;
    n = calloc(1, sizeof(*n));

    strcpy(n->p.key, key);
    n->p.val = val;

    n->next = h->next;
    h->next = n;
}

static void hash_add(hash *h, char *key, int val)
{
    unsigned int idx = hash_code(key) % HASH_T_SIZE;

    __hash_add(&h->head[idx], key, val);
}

static inline int __get_val(struct node *h, char *key, int *val)
{
    struct node *it = h;
    while (it->next) {
        if (!strcmp(key, it->next->p.key)) {
            *val = it->next->p.val;
            return 1;
        }
        it = it->next;
    }
    return 0;
}

static int get_val(hash *h, char *key, int *val)
{
    unsigned int idx = hash_code(key) % HASH_T_SIZE;
    return __get_val(&h->head[idx], key, val);
}

static inline void __hash_remove(struct node *h, char *key)
{
    struct node *it = h, *tmp;
    while (it->next) {
        if (!strcmp(key, it->next->p.key)) {
            tmp = it->next;
            it->next = it->next->next;
            free(tmp);
            break;
        }
        it = it->next;
    }
}

static void hash_remove(hash *h, char *key)
{
    unsigned int idx = hash_code(key) % HASH_T_SIZE;
    __hash_remove(&h->head[idx], key);
}

void print_hash(hash *h, char *key)
{
    int r, val;
    r = get_val(h, key, &val);
    printf("%s: find:%d val:%d\n", key, r, val);
}

int main(void) {
    hash h;

    hash_init(&h);

    hash_add(&h, "car", 1);
    hash_add(&h, "air-plane", 200);
    hash_add(&h, "dream", 300);
    hash_add(&h, "baby", 44);
    hash_add(&h, "computer", 14);
    hash_add(&h, "aaa", 143);
    hash_add(&h, "monkey", 20);
    hash_add(&h, "deeplearning", 1432);

    print_hash(&h, "haha");
    print_hash(&h, "monkkk");
    print_hash(&h, "air-plane");
    print_hash(&h, "car");

    hash_remove(&h, "car");
    print_hash(&h, "car");
    print_hash(&h, "deeplearning");
    return 1;
}
