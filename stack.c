#include <stdio.h>

#define MAX_LEN 100

// c1 add
// c2 add

typedef struct {
    int buf[MAX_LEN];
    int wptr;
} stack;

void stack_init(stack *s)
{
    s->wptr = 0;
}

void push(stack *s, int d)
{
    s->buf[s->wptr] = d;
    s->wptr++;
}

int pop(stack *s)
{
    int v = s->buf[s->wptr-1];
    s->wptr--;
    return v;
}

int stack_size(stack *s)
{
    return s->wptr;
}

int main(void)
{
    stack s;

    stack_init(&s);
    push(&s, 1);
    push(&s, 2);
    push(&s, 3);
    push(&s, 4);
    printf("size:%d\n", stack_size(&s));

    printf("%d\n", pop(&s));
    printf("%d\n", pop(&s));
    printf("%d\n", pop(&s));

    push(&s, 10);
    push(&s, 20);

    printf("%d\n", pop(&s));
    printf("%d\n", pop(&s));
    printf("%d\n", pop(&s));

    printf("size:%d\n", stack_size(&s));
    return 1;
}
