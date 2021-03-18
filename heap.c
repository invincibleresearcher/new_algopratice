#include <stdio.h>
#include <malloc.h>
// just for generating random number
#include <time.h>
#include <stdlib.h>

//#define DEBUG_ON

#define DATA_TOTAL_SIZE 30000

struct info {
	int val;
	int index; // index of data in heap
};

int mem_index = 0;
struct info mem[DATA_TOTAL_SIZE];

#define L_CHILD(pi) (2*pi+1)
#define R_CHILD(pi) (2*pi+2)
#define PARENT(ci) ((ci-1)/2)

struct heap {
	// contain the indices of mem 
	int data[DATA_TOTAL_SIZE];
	int size;
};

static inline int __compare(struct info* a, struct info *b)
{
	if (a->val > b->val) {
		return 1;
	}
	else if (a->val == b->val) {
		return 0;;
	}
	else {
		return -1;
	}
}

static inline int compare(struct heap *h, int i, int j)
{
	return __compare(&mem[h->data[i]], &mem[h->data[j]]);
}

static void heap_init(struct heap *h)
{
	h->size = 0;
}

static inline void do_swap(struct heap *h, int i, int j)
{
	int tmp;
	tmp = h->data[i];
	h->data[i] = h->data[j];
	h->data[j] = tmp;
	mem[h->data[i]].index = i;
	mem[h->data[j]].index = j;
}

static inline void up_ward(struct heap *h, int ci)
{
	int pi;
	while (ci) {
		pi = PARENT(ci);
		if (compare(h, pi, ci) >= 0) {
			break;
		}
		do_swap(h, ci, pi);
		ci = pi;
	}
}

static inline void down_ward(struct heap *h, int pi)
{
	int ci, li, ri;
	while (1) {
		li = L_CHILD(pi);
		ri = R_CHILD(pi);
		if (li > h->size - 1) break;
		if (li == h->size -1) {
			ci = li;
		}
		else {
			ci = (compare(h, li, ri) >= 0) ? li : ri;
		}

		if (compare(h, pi, ci) >= 0) {
			break;
		}
		do_swap(h, ci, pi);
		pi = ci;
	}
}

static inline void heap_add(struct heap *h, int val)
{
	mem[mem_index].val = val;
	mem[mem_index].index = h->size;

	h->data[h->size] = mem_index;

	mem_index++;
	h->size++;

	up_ward(h, h->size-1);
}

static inline void __heap_remove(struct heap *h, int i)
{
	struct info rr;

	if (mem[h->data[i]].index == -1) return;

	rr.val = mem[h->data[i]].val;
	mem[h->data[i]].index = -1;

	h->data[i] = h->data[h->size-1];
	mem[h->data[i]].index = i;
	h->size--;

	if (__compare(&rr, &mem[h->data[i]]) >= 0) {
#ifdef DEBUG_ON
		printf("@@ heap_remove - down_ward\n");
#endif
		down_ward(h, i);
	}
	else {
#ifdef DEBUG_ON
		printf("@@ heap_remove - up_ward\n");
#endif
		up_ward(h, i);
	}
}

static inline void heap_remove_top(struct heap *h)
{
	__heap_remove(h, 0);
}

static inline void heap_remove_val(struct heap *h, int mem_idx)
{
	__heap_remove(h, mem[mem_idx].index);
}

static int heap_verify(struct heap *h)
{
	int i, li, ri;
	for (i = 0; i < h->size; i++) {
		if (mem[h->data[i]].index == -1) {
			printf("\t# mem[h->data[i]].index == -1\n");
			return -1;
		}
	}
	for (i = 0; i < h->size; i++) {
		li = L_CHILD(i);
		ri = R_CHILD(i);
		if (mem[h->data[i]].index != i) {
			printf("\t# mem[h->data[i]].index != i\n");
			return -1;
		}

		if (li > h->size - 1) {
			break;
		}
		else if (li == h->size - 1) {
			if (compare(h, i, li) < 0) {
				printf("\t# compare(h, i, li) (%d, %d) < 0\n",
				       mem[h->data[i]].val, mem[h->data[li]].val);
				return -1;
			}
		}
		else {
			if (compare(h, i, li) < 0) {
				printf("\t# compare(h, i, li) (%d, %d) < 0\n",
				       mem[h->data[i]].val, mem[h->data[li]].val);
				return -1;
			}
			if (compare(h, i, ri) < 0) {
				printf("\t# compare(h, i, ri) (%d, %d) < 0\n",
				       mem[h->data[i]].val, mem[h->data[ri]].val);
				return -1;
			}
		}
	}
	return 0;
}

static void heap_add_test(struct heap *h, int val, int tot_cnt)
{
#ifdef DEBUG_ON
	printf("@@ heap add - val: %d\n", val);
#endif
	heap_add(h, val);
	if (heap_verify(h) == -1) {
		printf("\tadd fail w/ val: %d tot_cnt: %d\n", val, tot_cnt);
		exit(EXIT_FAILURE);
	}
}

static void heap_remove_top_test(struct heap *h, int tot_cnt)
{
#ifdef DEBUG_ON
	printf("@@ heap remove top\n");
#endif
	heap_remove_top(h);
	if (heap_verify(h) == -1) {
		printf("\tremove fail tot_cnt: %d\n", tot_cnt);
		exit(EXIT_FAILURE);
	}
}

static inline void heap_remove_val_test(struct heap *h, int mem_idx, int tot_cnt)
{
	struct info rr;

#ifdef DEBUG_ON
	printf("@@ heap remove val - mem_idx: %d\n", mem_idx);
#endif

	if (mem[mem_idx].index == -1) return;
	rr.val = mem[mem_idx].val;
	rr.index = mem[mem_idx].index;

#ifdef DEBUG_ON
	printf("@@ heap remove val - val: %d heap index: %d\n", rr.val, rr.index);
#endif

	heap_remove_val(h, mem_idx);

	if (heap_verify(h) == -1) {
		printf("\tremove_val fail w/ mem_idx: %d val: %d index: %d tot_cnt: %d\n",
		       mem_idx, rr.val, rr.index, tot_cnt);
		exit(EXIT_FAILURE);
	}
}

static void test1(void)
{
	int i, rv = 0;
	int tot_cnt = 0;
	struct heap h;

	printf("## %s start\n", __func__);

	for (i = 0; i < DATA_TOTAL_SIZE; i++) {
		mem[i].index = -1;
	}

	mem_index = 0;
	heap_init(&h);
	while (mem_index < DATA_TOTAL_SIZE) {
		rv = rand() % 100;
		if (rv < 60 || mem_index == 0) {
			heap_add_test(&h, rand(), tot_cnt);
			tot_cnt++;
		}
		else if (rv < 80) {
			rv = rand() % mem_index;
			if (mem[rv].index > 0) {
				heap_remove_val_test(&h, rv, tot_cnt);
				tot_cnt++;
			}
		}
		else {
			if (h.size > 0) {
				heap_remove_top_test(&h, tot_cnt);
				tot_cnt++;
			}
		}
	}
	printf("## %s end tot_tc: %d\n", __func__, tot_cnt);
}


int main(void)
{
	int it = 0;
	srand((unsigned int)time(NULL));

	printf("## TEST START\n");
	test1();
	printf("## TEST DONE\n");
	return 0;
}
