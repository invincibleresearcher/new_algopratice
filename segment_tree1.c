#include <stdio.h>

#define MAX_N 10000

#define L_CHILD(pi) (2*pi)
#define R_CHILD(pi) (2*pi+1)
#define PARENT(ci) (ci/2)

// https://www.geeksforgeeks.org/segment-tree-efficient-implementation/

struct sg_tree {
	int data[2*MAX_N];
	int size;
};

static inline int update_rule(int l_child, int r_child)
{
	return l_child + r_child;
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

	for (l += n, r += n; l < r; l >>= 1, r >>= 1)
	{
		printf("\t\tl: %d r: %d res: %d\n", l, r, res);
		if (l&1) res += t->data[l++];
		if (r&1) res += t->data[--r];
	}
	return res;
}

static inline int real_query(struct sg_tree *t, int l, int r)
{
	int i, res = 0;
	for (i = l; i < r; i++) {
		res = update_rule(res, t->data[t->size + i]);
	}
	return res;
}

#define QUERY_TEST(t, a,b) do {\
	printf("## [%d, %d) - query: %d - real answer: %d\n", a, b, query(t, a, b), real_query(t, a, b)); \
} while(0)

int main(void) {
	struct sg_tree t;
	int a[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12};

	build_sg(&t, a, sizeof(a)/sizeof(a[0]));

	QUERY_TEST(&t, 0, 12);
	QUERY_TEST(&t, 0, 3);
	QUERY_TEST(&t, 0, 2);
	QUERY_TEST(&t, 0, 1);

	QUERY_TEST(&t, 2, 12);
	QUERY_TEST(&t, 3, 8);
	QUERY_TEST(&t, 2, 7);
	QUERY_TEST(&t, 4, 10);

	return 0;
}
