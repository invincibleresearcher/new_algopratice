#include <stdio.h>

#define MAX_LEN 100

// c1
// c2

typedef struct {
    int buf[MAX_LEN];
    int wptr, rptr;
    int size;
} queue;

void queue_init(queue *q)
{
    q->size = q->wptr = q->rptr = 0;
}

void enq(queue *q, int d)
{
    q->buf[q->wptr] = d;
    q->wptr++;
    q->size++;
    if (q->wptr >= MAX_LEN) q->wptr -= MAX_LEN;
}

int deq(queue *q)
{
    int d = q->buf[q->rptr];
    q->rptr++;
    q->size--;
    if (q->rptr >= MAX_LEN) q->rptr -= MAX_LEN;
    return d;
}

int queue_size(queue *q)
{
    return q->size;
}

int main(void)
{
    queue s;

    queue_init(&s);
    enq(&s, 1);
    enq(&s, 2);
    enq(&s, 3);
    enq(&s, 4);
    printf("size:%d\n", queue_size(&s));

    printf("%d\n", deq(&s));
    printf("%d\n", deq(&s));
    printf("%d\n", deq(&s));

    enq(&s, 10);
    enq(&s, 20);

    printf("%d\n", deq(&s));
    printf("%d\n", deq(&s));
    printf("%d\n", deq(&s));

    printf("size:%d\n", queue_size(&s));
    
    return 0;
}
