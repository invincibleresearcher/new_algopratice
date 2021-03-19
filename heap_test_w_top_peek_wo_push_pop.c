#include <stdio.h>
#include <malloc.h>
// just for generating random number
#include <time.h>
#include <stdlib.h>

#define DATA_TOTAL_SIZE		30000
#define TEST1_CNT		50000
#define TEST2_CNT		70000

#define MAX_PRIORITY_SIZE	20

struct info {
	int val;
	int index; // index of data in heap
};

struct info_pool {
	struct info mem[DATA_TOTAL_SIZE];
	void *ptr[DATA_TOTAL_SIZE];
	int wptr;
};

struct info_pool info_pool_imp;

static inline void mem_init(void)
{
	int i;
	for (i = 0; i < DATA_TOTAL_SIZE; i++) {
		info_pool_imp.ptr[i] = &info_pool_imp.mem[i];
	}
	info_pool_imp.wptr = DATA_TOTAL_SIZE;
}

static inline void *mem_alloc(void)
{
	void *ptr = info_pool_imp.ptr[info_pool_imp.wptr-1];
	info_pool_imp.wptr--;
	return ptr;
}

static inline void mem_free(void *ptr)
{
	info_pool_imp.ptr[info_pool_imp.wptr] = ptr;
	info_pool_imp.wptr++;
}

#define L_CHILD(pi) (2*pi+1)
#define R_CHILD(pi) (2*pi+2)
#define PARENT(ci) ((ci-1)/2)

struct heap {
	struct info *data[DATA_TOTAL_SIZE];
	int size;
};

static inline int compare(struct info* a, struct info *b)
{
	return a->val - b->val;
}

static void heap_init(struct heap *h)
{
	h->size = 0;
}

static inline void do_swap(struct heap *h, int i, int j)
{
	struct info *tmp;

	tmp = h->data[i];
	h->data[i] = h->data[j];
	h->data[j] = tmp;

	h->data[i]->index = i;
	h->data[j]->index = j;
}

static inline void up_ward(struct heap *h, int ci)
{
	int pi;
	while (ci) {
		pi = PARENT(ci);
		if (compare(h->data[pi], h->data[ci]) >= 0) break;
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
			ci = (compare(h->data[li], h->data[ri]) >= 0) ? li : ri;
		}

		if (compare(h->data[pi], h->data[ci]) >= 0) {
			break;
		}
		do_swap(h, ci, pi);
		pi = ci;
	}
}

static inline void heap_add(struct heap *h, int val)
{
	struct info *i = (struct info *) mem_alloc();

	i->val = val;
	i->index = h->size;
	h->data[h->size] = i;
	h->size++;

	up_ward(h, h->size-1);
}

static inline void __heap_remove(struct heap *h, struct info *i)
{
	if (!h->size) return;

	if (!i) i = h->data[0];
	if (i->index < 0) return;

	h->data[i->index] = h->data[h->size-1];
	h->data[i->index]->index = i->index;
	h->size--;

	if (compare(i, h->data[i->index]) >= 0) {
		down_ward(h, i->index);
	}
	else {
		up_ward(h, i->index);
	}

	i->index = -1;
	mem_free(i);
}

static inline void heap_remove_top(struct heap *h)
{
	__heap_remove(h, NULL);
}

static inline void heap_remove_val(struct heap *h, struct info *i)
{
	__heap_remove(h, i);
}

struct top_heap {
	int data[MAX_PRIORITY_SIZE];
	int size;
};

static inline void top_heap_add(struct top_heap *th, struct heap *h, int i)
{
	int tmp, ci, pi;

	th->data[th->size] = i;
	ci = th->size;
	th->size++;
	while (ci) {
		pi = PARENT(ci);
		if (compare(h->data[th->data[pi]], h->data[th->data[ci]]) >= 0) break;
		tmp = th->data[ci];
		th->data[ci] = th->data[pi];
		th->data[pi] = tmp;
		ci = pi;
	}
}

static inline void top_heap_remove(struct top_heap *th, struct heap *h)
{
	int tmp, ci, pi, li, ri;
	th->data[0] = th->data[th->size-1];
	th->size--;
	pi = 0;
	while (1) {
		li = L_CHILD(pi);
		ri = R_CHILD(pi);

		if (li > th->size - 1) break;
		if (li == th->size -1) {
			ci = li;
		}
		else {
			ci = (compare(h->data[th->data[li]], h->data[th->data[ri]]) >= 0) ? li : ri;
		}

		if (compare(h->data[th->data[pi]], h->data[th->data[ci]]) >= 0) {
			break;
		}
		tmp = th->data[ci];
		th->data[ci] = th->data[pi];
		th->data[pi] = tmp;
		pi = ci;
	}
}

static int heap_get_highest(struct heap *h, int top_size, int ret[])
{
	int li, ri, ret_cnt = 0;
	struct top_heap th;
	th.size = 0;

	if (h->size) top_heap_add(&th, h, 0);
	while (ret_cnt < top_size && th.size) {
		ret[ret_cnt] = h->data[th.data[0]]->val;
		li = L_CHILD(th.data[0]);
		ri = R_CHILD(th.data[0]);

		top_heap_remove(&th, h);

		if (li < h->size) top_heap_add(&th, h, li);
		if (ri < h->size) top_heap_add(&th, h, ri);

		ret_cnt++;
	}

	return ret_cnt;
}

static int heap_get_highest_ref(struct heap *h, int top_size, int ret[])
{
	int i, ret_cnt = 0;
	while (ret_cnt < top_size && h->size) {
		ret[ret_cnt] = h->data[0]->val;
		heap_remove_top(h);
		ret_cnt++;
	}
	for (i = 0; i < ret_cnt; i++) {
		heap_add(h, ret[i]);
	}
	return ret_cnt;
}

static int heap_verify(struct heap *h)
{
	int i, li, ri;
	for (i = 0; i < h->size; i++) {
		if (h->data[i]->index == -1) {
			printf("\t# h->data[i]->index == -1\n");
			return -1;
		}
	}
	for (i = 0; i < h->size; i++) {
		li = L_CHILD(i);
		ri = R_CHILD(i);
		if (h->data[i]->index != i) {
			printf("\t# h->data[i]->index != i\n");
			return -1;
		}

		if (li > h->size - 1) {
			break;
		}
		else if (li == h->size - 1) {
			if (compare(h->data[i], h->data[li]) < 0) {
				printf("\t# compare(i, li) (%d, %d) < 0\n",
				       h->data[i]->val, h->data[li]->val);
				return -1;
			}
		}
		else {
			if (compare(h->data[i], h->data[li]) < 0) {
				printf("\t# compare(i, li) (%d, %d) < 0\n",
				       h->data[i]->val, h->data[li]->val);
				return -1;
			}
			if (compare(h->data[i], h->data[ri]) < 0) {
				printf("\t# compare(i, ri) (%d, %d) < 0\n",
				       h->data[i]->val, h->data[ri]->val);
				return -1;
			}
		}
	}
	return 0;
}

#define TEST_BASIC1(tot_cnt, h, func,...) do {\
	func(__VA_ARGS__); \
	if (heap_verify(h) == -1) { \
		printf("\t%s fail at tot_cnt: %d\n", #func, tot_cnt); \
		exit(EXIT_FAILURE); \
	} \
} while(0)

static void test1(void)
{
	int i, rv = 0;
	int tot_cnt = 0;
	struct heap h;

	printf("## %s start\n", __func__);

	mem_init();
	heap_init(&h);
	while (tot_cnt < TEST1_CNT) {
		rv = rand() % 100;
		if ((rv < 60 || h.size == 0) && h.size < DATA_TOTAL_SIZE) {
			TEST_BASIC1(tot_cnt, &h, heap_add, &h, rand());
			tot_cnt++;
		}
		else if (rv < 80) {
			rv = rand() % h.size;
			TEST_BASIC1(tot_cnt, &h, heap_remove_val, &h, h.data[rv]);
			tot_cnt++;
		}
		else {
			TEST_BASIC1(tot_cnt, &h, heap_remove_top, &h);
			tot_cnt++;
		}
	}
	printf("## %s end tot_tc: %d\n", __func__, tot_cnt);
}

static void test2(void)
{
	int i, rv = 0;
	int tot_cnt = 0, tot_prio_cnt = 0;
	struct heap h;

	int given_size, ret_cnt1, ret_cnt2;
	int top_prio[MAX_PRIORITY_SIZE];
	int top_prio_ref[MAX_PRIORITY_SIZE];

	printf("## %s start\n", __func__);

	mem_init();
	heap_init(&h);
	while (tot_cnt < TEST2_CNT) {
		rv = rand() % 100;
		if ((rv < 60 || h.size == 0) && h.size < DATA_TOTAL_SIZE) {
			TEST_BASIC1(tot_cnt, &h, heap_add, &h, rand());
			tot_cnt++;
		}
		else if (rv < 80) {
			rv = rand() % h.size;
			TEST_BASIC1(tot_cnt, &h, heap_remove_val, &h, h.data[rv]);
			tot_cnt++;
		}
		else {
			TEST_BASIC1(tot_cnt, &h, heap_remove_top, &h);
			tot_cnt++;
		}
		//
		rv = rand() % 3;
		if (!rv) {
			given_size = rand() % MAX_PRIORITY_SIZE;
			if (!given_size) given_size = 3;
			ret_cnt1 = heap_get_highest(&h, given_size, top_prio);
			ret_cnt2 = heap_get_highest_ref(&h, given_size, top_prio_ref);

			if (ret_cnt1 != ret_cnt2) {
				printf("\t%s fail at top priority ret_cnt1: %d != ret_cnt2: %d\n",
				       __func__, ret_cnt1, ret_cnt2);
				exit(EXIT_FAILURE); \
			}
			for (i = 0; i < ret_cnt1; i++) {
				if (top_prio[i] != top_prio_ref[i]) {
					printf("\t%s fail at top_prio[%d]: %d != top_prio_ref[%d]: %d\n",
					       __func__, i, top_prio[i], i, top_prio_ref[i]);
					exit(EXIT_FAILURE); \
				}
			}
			tot_cnt++;
			tot_prio_cnt++;
		}
	}
	printf("## %s end tot_tc: %d tot_prio_cnt: %d\n", __func__, tot_cnt, tot_prio_cnt);
}

int main(void)
{
	int it = 0;
	srand((unsigned int)time(NULL));

	printf("## TEST START\n");
	test2();
	printf("## TEST DONE\n");
	return 0;
}
