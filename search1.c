#include <stdio.h>
#include <stdlib.h>

#define M 399
#define N 934

// c1

int map[N][M];

void map_clear(void)
{
    int i, j;
    for (i = 0; i < N; i++) {
        for (j = 0; j < M; j++) {
            map[i][j] = 0;
        }
    }
}

void map_visit_check(void)
{
    int i, j, cnt = 0;
    printf("visit check start..\n");
    for (i = 0; i < N; i++) {
        for (j = 0; j < M; j++) {
            if (map[i][j] != 1) {
                printf("\t %d,%d = %d\n", j, i, map[i][j]);
                cnt++;
            }
        }
    }
    if (cnt == 0) {
        printf("\t correct!\n");
    }
    else {
        printf("\t %d fail\n", cnt);
    }
}

typedef struct {
    int x, y;
} pos;

#define MAX_SIZE (M*N)
typedef struct {
    pos p[MAX_SIZE];
    int wptr;
    int size;
} stack;

void sinit(stack *s)
{
    s->wptr = s->size = 0;
}

void push(stack *s, pos p)
{
    s->p[s->wptr] = p;
    s->wptr++;
    s->size++;
}

pos pop(stack *s)
{
    pos p = s->p[s->wptr-1];
    s->size--;
    s->wptr--;
    return p;
}

typedef struct {
    pos p[MAX_SIZE];
    int wptr, rptr, size;
} que;

void qinit(que *q)
{
    q->wptr = q->rptr = q->size = 0;
}

void enq(que *q, pos p)
{
    q->p[q->wptr] = p;
    q->wptr++;
    q->size++;
    if (q->wptr >= MAX_SIZE) q->wptr -= MAX_SIZE;
}

pos deq(que *q)
{
    pos p = q->p[q->rptr];
    q->rptr++;
    q->size--;
    if (q->rptr >= MAX_SIZE) q->rptr -= MAX_SIZE;
    return p;
}

void visit_by_dfs(int x, int y)
{
    int loopcnt = 0;
    pos cp, np;
    stack s;

    sinit(&s);
    cp.x = x; cp.y = y;
    map[y][x] = 1;
    push(&s, cp);

    while (s.size > 0) {
        cp = pop(&s);
        if (cp.x - 1 >= 0 && map[cp.y][cp.x-1] == 0) {
            map[cp.y][cp.x-1]++;
            push(&s, cp);
            np.x = cp.x - 1; np.y = cp.y;
            push(&s, np);
        }
        else if (cp.x + 1 < M && map[cp.y][cp.x+1] == 0) {
            map[cp.y][cp.x+1]++;
            push(&s, cp);
            np.x = cp.x + 1; np.y = cp.y;
            push(&s, np);
        }
        else if (cp.y - 1 >=0 && map[cp.y-1][cp.x] == 0) {
            map[cp.y-1][cp.x]++;
            push(&s, cp);
            np.x = cp.x; np.y = cp.y - 1;
            push(&s, np);
        }
        else if (cp.y + 1 < N && map[cp.y+1][cp.x] == 0) {
            map[cp.y+1][cp.x]++;
            push(&s, cp);
            np.x = cp.x; np.y = cp.y + 1;
            push(&s, np);
        }
        loopcnt++;
    }

    printf("%s loop cnt: %d\n", __func__, loopcnt);
}

void visit_by_bfs(int x, int y)
{
    int loopcnt = 0;
    pos cp, np;
    que q;

    qinit(&q);
    cp.x = x; cp.y = y;
    map[y][x] = 1;
    enq(&q, cp);
    while (q.size > 0) {
        cp = deq(&q);
        if (cp.x - 1 >= 0 && map[cp.y][cp.x-1] == 0) {
            map[cp.y][cp.x-1]++;
            np.x = cp.x - 1; np.y = cp.y;
            enq(&q, np);
        }

        if (cp.x + 1 < M && map[cp.y][cp.x+1] == 0) {
            map[cp.y][cp.x+1]++;
            np.x = cp.x + 1; np.y = cp.y;
            enq(&q, np);
        }

        if (cp.y - 1 >=0 && map[cp.y-1][cp.x] == 0) {
            map[cp.y-1][cp.x]++;
            np.x = cp.x; np.y = cp.y - 1;
            enq(&q, np);
        }

        if (cp.y + 1 < N && map[cp.y+1][cp.x] == 0) {
            map[cp.y+1][cp.x]++;
            np.x = cp.x; np.y = cp.y + 1;
            enq(&q, np);
        }

        loopcnt++;
    }
    printf("%s loop cnt: %d\n", __func__, loopcnt);
}

int main(int argc, char **argv) {
    int x = 0, y = 0;

    if (argc == 3) {
        x = atoi(argv[1]);
        y = atoi(argv[2]);
        if (x < 0 || x >= M) x = 0;
        if (y < 0 || y >= N) y = 0;
    }

    printf("start pos = (%d, %d)\n", x, y);

    map_clear();
    visit_by_dfs(x, y);
    map_visit_check();

    map_clear();
    visit_by_bfs(x, y);
    map_visit_check();
    return 0;
}
