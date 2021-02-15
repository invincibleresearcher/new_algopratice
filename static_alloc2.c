#include <stdio.h>
#include <malloc.h>

struct ln {
	int val;
	struct ln *next;
};


#define DEFINE_STATIC_ALLOC(n, s) \
struct n##_mem { \
	struct n pool[s]; \
	void *ptr[s]; \
	int wptr; \
}; \
\
struct n##_mem n##_mem_impl; \
\
static inline void n##_mem_init(void) \
{ \
	int i; \
	for (i = 0; i < s; i++) n##_mem_impl.ptr[i] = &n##_mem_impl.pool[i]; \
	n##_mem_impl.wptr = s; \
} \
\
static inline void *n##_alloc(void) \
{ \
	void *ptr = n##_mem_impl.ptr[n##_mem_impl.wptr-1]; \
	n##_mem_impl.wptr--; \
	return ptr; \
} \
\
static inline void n##_free(void *ptr) \
{ \
	n##_mem_impl.ptr[n##_mem_impl.wptr] = ptr; \
	n##_mem_impl.wptr++; \
}

DEFINE_STATIC_ALLOC(ln, 1000)


static void ln_add(struct ln *head, int v)
{
	struct ln *n = (struct ln*) ln_alloc();
	n->next = NULL;
	n->val = v;
	n->next = head->next;
	head->next = n;
}

static void ln_remove_front(struct ln *head)
{
	struct ln *tmp;
	if (!head->next) return;
	tmp = head->next;
	head->next = head->next->next;
	ln_free(tmp);
}

static void ln_print(struct ln *head)
{
	struct ln *it = head;
	printf("- - - - %s\n", __func__);
	while (it->next) {
		printf("%d, ", it->next->val);
		it = it->next;
	}
	printf("\n");
}

int main(void)
{
	int i, j;
	struct ln head;
	head.next = NULL;
	ln_mem_init();
	for (i = 0; i < 4; i++) {
		printf("%d-th iteration\n", i+1);
		for (j = 0; j < 10; j++) {
			ln_add(&head, j);
		}
		ln_print(&head);
		for (j = 0; j < 10; j++) {
			ln_remove_front(&head);
		}
	}
	return 0;
} 
