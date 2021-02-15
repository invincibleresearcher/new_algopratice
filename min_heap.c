#include <stdio.h>
#include <malloc.h>

#define MAX_ARRAY (100 + 1)

// when parent is at i-th, its children are at 2i+1, 2i+2
struct heap {
	int size;
	int array[MAX_ARRAY];
};

static void heap_init(struct heap *h)
{
	h->size = 0;
}

// https://en.wikipedia.org/wiki/Heap_(data_structure)
static inline int get_l_child(int pi)
{
	return 2*pi+1;
}

static inline int get_r_child(int pi)
{
	return 2*pi+2;
}

static inline int get_parent(int ci)
{
	return (ci-1)/2;
}

static inline void do_swap(struct heap *h, int i, int j)
{
	int tmp;
	tmp = h->array[i];
	h->array[i] = h->array[j];
	h->array[j] = tmp;
}

static inline int __heaptify(struct heap *h, int pi)
{
	int li, ri, mi;
	li = get_l_child(pi);
	ri = get_r_child(pi);
	if (li >= h->size && ri >= h->size) return -1;
	if (li >= h->size) {
		mi = ri;
	} else if (ri >= h->size) {
		mi = li;
	} else {
		mi = h->array[li] <= h->array[ri] ? li : ri;
	}

	if (h->array[pi] <= h->array[mi]) return -1;

	do_swap(h, mi, pi);
	return mi;
}

static void heap_add(struct heap *h, int v)
{
	int ci = h->size, pi;
	h->array[ci] = v;
	h->size++;
	while (1) {
		pi = get_parent(ci);
		if (h->array[pi] <= h->array[ci]) break;
		do_swap(h, ci, pi);
		ci = pi;
	}
}

// not correct
static void heaptify(struct heap *h, int array[], int size)
{
	int i;
	heap_init(h);
	for (i = 0; i < size; i++) {
		heap_add(h, array[i]);
	}
}

static inline int heap_min(struct heap *h)
{
	return h->array[0];
}

static int heap_remove_min(struct heap *h)
{
	int ret = h->array[0], pi = 0;
	h->array[0] = h->array[h->size-1];
	h->size--;
	pi = 0;
	while ((pi = __heaptify(h, pi)) != -1);
	return ret;
}

static void print_heap(struct heap *h)
{
	int i, li, ri;
	printf("- - - - array - - - -\n");
	for (i = 0; i < h->size; i++) {
		li = get_l_child(i);
		ri = get_r_child(i);
		printf("%d", h->array[i]);
		if (li <= h->size-1) {
			printf("(%d,", h->array[li]);
		} else {
			printf("(n,");
		}

		if (ri <= h->size-1) {
			printf("%d) ", h->array[ri]);
		} else {
			printf("n) ");
		}
	}
	printf("\n");
}

void test1(void)
{
	struct heap h;

	printf("test 1>\n");
	heap_init(&h);
	heap_add(&h, 3);
	heap_add(&h, 2);
	heap_add(&h, 1);
	print_heap(&h);

	heap_add(&h, 10);
	heap_add(&h, 20);
	heap_add(&h, 30);
	heap_add(&h, 40);
	print_heap(&h);

	printf("remove min: %d\n", heap_remove_min(&h));
	print_heap(&h);

	printf("remove min: %d\n", heap_remove_min(&h));
	print_heap(&h);

	printf("remove min: %d\n", heap_remove_min(&h));
	print_heap(&h);

	heap_add(&h, 4);
	heap_add(&h, 8);
	heap_add(&h, 100);
	print_heap(&h);

	printf("remove min: %d\n", heap_remove_min(&h));
	print_heap(&h);

	printf("remove min: %d\n", heap_remove_min(&h));
	print_heap(&h);
}

void test2(void)
{
	int array[] = {
		31, 30, 29, 28, 27, 26, 25, 24, 23, 22, 21, 20, 19, 17, 16,
		15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1};

	struct heap h;

	printf("test 2>\n");
	heap_init(&h);
	heaptify(&h, array, sizeof(array)/sizeof(int));
	print_heap(&h);
}

void test3(void)
{
	int array[] = {
		31, 30, 29, 28, 27, 26, 25, 24, 23, 22, 21, 20, 19, 17, 17, 16,
		15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1};

	struct heap h;

	printf("test 3>\n");
	heap_init(&h);
	heaptify(&h, array, sizeof(array)/sizeof(int));

	printf(" after all add done\n");
	print_heap(&h);
	while (h.size > 0) {
		printf("remove min : %d\n", heap_remove_min(&h));
	}
}

int main(void)
{
	test1();
	test2();
	test3();
	return 0;
}
