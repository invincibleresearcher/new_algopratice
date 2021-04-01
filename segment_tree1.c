#include <stdio.h>
// the below headers are just fo tcs
#include <stdint.h>
#include <stdlib.h>

#define MAX_N 10000

#define L_CHILD(pi) (2*pi)
#define R_CHILD(pi) (2*pi+1)
#define PARENT(ci) (ci/2)

enum opt {
	OPT_FIND_SUM,
	OPT_FIND_MAX,
	OPT_FIND_MIN,
};
// https://www.geeksforgeeks.org/segment-tree-efficient-implementation/

struct sg_tree {
	int data[2*MAX_N];
	int size;
};

enum opt option = OPT_FIND_SUM;

static inline int update_rule(int l_child, int r_child)
{
	if (option == OPT_FIND_SUM) {
		return l_child + r_child;
	}
	else if (option == OPT_FIND_MAX) {
		return l_child > r_child ? l_child : r_child;
	}
	else {
		return l_child > r_child ? r_child : l_child;
	}
}

static inline void sg_update(struct sg_tree *t, int pi)
{
	t->data[pi] = update_rule(t->data[L_CHILD(pi)], t->data[R_CHILD(pi)]);
}

static inline void build_sg(struct sg_tree *t, int a[], int size)
{
	int i;
	t->size = size;
	for (i = 0; i < size; i++) {
		t->data[size+i] = a[i];
	}
	for (i = size-1; i > 0; i--) {
		sg_update(t, i);
	}
}

static inline void update_node(struct sg_tree *t, int idx, int new_val)
{
	int pi, ci = t->size + idx;

	t->data[ci] = new_val;
	while (1) {
		pi = PARENT(ci);
		if (pi <= 1) break;
		sg_update(t, pi);
		ci = pi;
	}
}

static inline int query(struct sg_tree *t, int l, int r)
{
	int n = t->size, res = 0;

	if (option == OPT_FIND_SUM) {
		res = 0;
	}
	else if (option == OPT_FIND_MAX) {
		res = INT32_MIN;
	}
	else {
		res = INT32_MAX;
	}

	for (l += n, r += n; l < r; l /= 2, r /= 2)
	{
		if (l&1) res = update_rule(res, t->data[l++]);
		if (r&1) res = update_rule(res, t->data[--r]);
	}
	return res;
}

static inline int real_query(struct sg_tree *t, int l, int r)
{
	int i, res = 0;
	if (option == OPT_FIND_SUM) {
		for (i = l; i < r; i++) {
			res += t->data[t->size + i];
		}
	}
	else if (option == OPT_FIND_MAX) {
		res = t->data[t->size + l];
		for (i = l+1; i < r; i++) {
			if (t->data[t->size + i] > res) {
				res = t->data[t->size + i];
			}
		}
	}
	else {
		res = t->data[t->size + l];
		for (i = l+1; i < r; i++) {
			if (t->data[t->size + i] < res) {
				res = t->data[t->size + i];
			}
		}
	}
	return res;
}

#define QUERY_TEST(t, a, b) do {\
	int r1, r2; \
	r1 = query(t, a, b); \
	r2 = real_query(t, a, b); \
	if (r1 == r2) { \
		printf("## PASSED [%d, %d) res: %d\n", a, b, r1); \
	} \
	else { \
		printf("!! WRONG !! [%d, %d) - query: %d - real answer: %d\n", a, b, r1, r2); \
		exit(-1); \
	} \
} while(0)

static void test1(void)
{
	struct sg_tree t;
	int a[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12};
	build_sg(&t, a, sizeof(a)/sizeof(a[0]));

	printf("## %s start\n", __func__);

	QUERY_TEST(&t, 0, 12);
	QUERY_TEST(&t, 0, 3);
	QUERY_TEST(&t, 0, 2);
	QUERY_TEST(&t, 0, 1);

	QUERY_TEST(&t, 2, 12);
	QUERY_TEST(&t, 3, 8);
	QUERY_TEST(&t, 2, 7);
	QUERY_TEST(&t, 4, 10);
}

static void test2(void)
{
	struct sg_tree t;
	int a[] = {1, 2, 3, 4, 5};
	build_sg(&t, a, sizeof(a)/sizeof(a[0]));

	printf("## %s start\n", __func__);

	QUERY_TEST(&t, 0, 2);
	QUERY_TEST(&t, 2, 4);
	QUERY_TEST(&t, 0, 4);
	QUERY_TEST(&t, 0, 5);

	QUERY_TEST(&t, 3, 4);
	QUERY_TEST(&t, 3, 5);
	QUERY_TEST(&t, 1, 4);
}

static void test3(void)
{
	struct sg_tree t;
	int a[] = {-1, 2, -3, 4, -5, 6, -7, 8, -9, 10, -11, 12};
	build_sg(&t, a, sizeof(a)/sizeof(a[0]));

	printf("## %s start\n", __func__);

	QUERY_TEST(&t, 0, 12);
	update_node(&t, 0, 2);
	QUERY_TEST(&t, 0, 12);

	QUERY_TEST(&t, 0, 3);
	update_node(&t, 1, 12);
	QUERY_TEST(&t, 0, 3);

	QUERY_TEST(&t, 0, 2);
	QUERY_TEST(&t, 0, 1);

	QUERY_TEST(&t, 2, 12);
	update_node(&t, 5, 88);
	QUERY_TEST(&t, 2, 12);
	QUERY_TEST(&t, 3, 8);
	update_node(&t, 5, 21);
	QUERY_TEST(&t, 3, 8);
	update_node(&t, 5, 1);
	QUERY_TEST(&t, 3, 8);
	QUERY_TEST(&t, 2, 7);
	QUERY_TEST(&t, 4, 10);
}


int main(void) {

	option = OPT_FIND_SUM;
	printf("## find sum start..\n");
	test1();
	test2();
	test3();

	option = OPT_FIND_MAX;
	printf("## find max start..\n");
	test1();
	test2();
	test3();

	option = OPT_FIND_MIN;
	printf("## find min start..\n");
	test1();
	test2();
	test3();
	return 0;
}
