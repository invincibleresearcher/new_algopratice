#include <stdio.h>

#define MAXN 100

// c1
// c2 
// c3

int e[MAXN][MAXN];
int mp[MAXN][MAXN];
int tp[MAXN];

void init(void)
{
    int i, j;
    for (i = 0; i < MAXN; i++) {
        for (j = 0; j < MAXN; j++) {
            e[i][j] = -1;
            mp[i][j] = -1;
        }
    }
}

#define MAX_QSIZE MAXN

typedef struct {
    int buf[MAX_QSIZE];
    int wptr, size;
} stack;

static inline void sinit(stack *s)
{
    s->wptr = s->size = 0;
}

static inline void push(stack *s, int d)
{
    s->buf[s->wptr] = d;
    s->wptr++;
    s->size++;
}

static inline int pop(stack *s)
{
    int d = s->buf[s->wptr-1];
    s->wptr--;
    s->size--;
    return d;
}

int dist(int x, int y)
{
    int i, c, n;
    stack s;

    if (mp[x][y] > 0) return mp[x][y];

    for (i = 0; i < MAXN; i++) {
        tp[i] = -1;
    }

    tp[x] = 0;
    sinit(&s);
    push(&s, x);
    while(s.size > 0) {
        c = pop(&s);
        for (i = 0; i < MAXN; i++) {
            if (e[c][i] > 0) {
                if (mp[i][y] > 0) {
                    if (tp[y] == -1 || tp[c] + mp[i][y] < tp[y]) {
                        tp[y] = tp[c] + mp[i][y];
                    }
                }
                else if (tp[i] == -1 || e[c][i] + tp[c] < tp[i]) {
                    push(&s, c);
                    push(&s, i);
                    tp[i] = tp[c] + e[c][i];
                    break;
                }
            }
        }
    }

    for (i = 0; i < MAXN; i++) {
        if (tp[i] != -1) {
            mp[x][i] = tp[i];
            mp[i][x] = tp[i];
        }
    }

    return tp[y];
}

// input1.txt's graph is from https://en.wikipedia.org/wiki/Shortest_path_problem
int main(void)
{
    int i, x, y, d;
    int n;
    freopen("input1.txt", "r", stdin);

    init();
    scanf("%d", &n);
    for (i = 0; i < n; i++) {
        scanf("%d %d %d", &x, &y, &d);
        e[x][y] = d;
        e[y][x] = d;
    }

    scanf("%d", &n);
    
    for (i = 0; i < n; i++) {
        scanf("%d %d", &x, &y);
        printf("(%d %c,%d %c) : %d\n", x, x-1 + 'a', y, y-1 + 'a', dist(x, y));
    }

    setbuf(stdout, NULL);
    return 0;
}
