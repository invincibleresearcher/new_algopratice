#include <stdio.h>
// the below are just for tcs & debug
#include <string.h>
#include <stdlib.h>
#include <time.h>

static inline long timer_check(void)
{
	static struct timespec prev_ts;
	struct timespec ts;
	long tdiff = 0;
	int ret = clock_gettime(CLOCK_MONOTONIC, &ts);
	tdiff = (ts.tv_sec - prev_ts.tv_sec) * 1000 +
		(ts.tv_nsec - prev_ts.tv_nsec) / 1000000;
	memcpy(&prev_ts, &ts, sizeof(ts));
	return tdiff;
}

// overlap definition : overlap the boundary of two boxes or inclusion of
// another box, etc
// 0 : only check the existence of overlapped box for the given bounding box
// 1 : return total # of overlapped boxes for the given bounding box
int option = 0;

#define MAX_C 10000
#define MAX_R 10000

#define GRID_R 1000
#define GRID_C 1000

#define MAX_RECT 10000

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
	return n##_mem_impl.ptr[--n##_mem_impl.wptr]; \
} \
\
static inline void n##_free(void *ptr) \
{ \
	n##_mem_impl.ptr[n##_mem_impl.wptr++] = ptr; \
}

#define L_MAX_R_SIZE	(MAX_R/GRID_R)
#define L_MAX_C_SIZE	(MAX_C/GRID_C)
#define MAX_HEAD_SIZE	((L_MAX_R_SIZE*L_MAX_C_SIZE)+1)
#define MAX_LN_SIZE	(MAX_HEAD_SIZE * MAX_RECT)

#define INDEX(x,y,w) (y*w+x)

struct rect {
	int x, y, w, h;
	char is_visited;
	void *lnptr[MAX_HEAD_SIZE];
};
DEFINE_STATIC_ALLOC(rect, MAX_RECT)

struct rect_ln {
	struct rect *r;
	struct rect_ln *prev, *next;
};
DEFINE_STATIC_ALLOC(rect_ln, MAX_LN_SIZE)

struct rect_ln head[MAX_HEAD_SIZE];

static inline void list_init(void)
{
	int i;
	for (i = 0; i < MAX_HEAD_SIZE; i++) head[i].next = NULL;
}

static inline void __add_list(struct rect_ln *h, struct rect_ln *n)
{
	n->prev = h;
	n->next = h->next;
	if (h->next) h->next->prev = n;
	h->next = n;
}

static inline void add_list(int i, struct rect *r)
{
	struct rect_ln *ln;

	ln = (struct rect_ln *) rect_ln_alloc();
	ln->r = r;
	r->lnptr[i] = ln;
	__add_list(&head[i], ln);
}

static inline struct rect *new_rect(int x, int y, int w, int h)
{
	int i;
	struct rect *r = (struct rect *) rect_alloc();
	r->is_visited = 0;
	r->x = x; r->y = y; r->w = w; r->h = h;
	for (i = 0; i < MAX_HEAD_SIZE; i++) r->lnptr[i] = NULL;
	return r;
}

static inline int add_rect(int x, int y, int w, int h)
{
	int i, j, box_sx, box_sy, box_ex, box_ey;
	struct rect *r;

	if (x < 0 || y < 0) return -1;
	if (x+w >= MAX_R || y+h > MAX_C) return -1;

	r = new_rect(x, y, w, h);
	add_list(0, r);

	box_sx = x / GRID_C; box_ex = (int)(((float)(x + w)) / GRID_C);
	box_sy = y / GRID_R; box_ey = (int)(((float)(y + h)) / GRID_R);

	for (i = box_sy; i <= box_ey; i++) {
		for (j = box_sx; j <= box_ex; j++) {
			add_list(INDEX(j, i, L_MAX_C_SIZE)+1, r);
		}
	}
	return 0;
}

static inline void __remove_list(struct rect_ln *n)
{
	n->prev->next = n->next;
	if (n->next) n->next->prev = n->prev;
	rect_ln_free(n);
}

static inline void remove_rect(struct rect *r)
{
	int i;
	for (i = 0; i < MAX_HEAD_SIZE; i++) {
		if (r->lnptr[i]) {
			__remove_list((struct rect_ln *)r->lnptr[i]);
		}
	}
	rect_free(r);
}

static inline int is_overlap(int x1, int y1, int w1, int h1,
			     int x2, int y2, int w2, int h2)
{
	if (x1+w1 < x2 || x2+w2 < x1) return 0;
	if (y1+h1 < y2 || y2+h2 < y1) return 0;
	return 1;
}

static struct rect *visited[MAX_RECT];
static int visited_cnt = 0;

static inline int __query_rect(struct rect_ln *head, int x, int y, int w, int h)
{
	int i, j, cnt = 0, tmp_cnt;
	struct rect *r;
	struct rect_ln *it = head;

	while (it->next) {
		r = it->next->r;
		if (!r->is_visited) {
			tmp_cnt = is_overlap(r->x, r->y, r->w, r->h, x, y, w, h);
			cnt += tmp_cnt;
			if (option == 0 && tmp_cnt) {
				return 1;
			}
			if (tmp_cnt) {
				r->is_visited = 1;
				visited[visited_cnt++] = r;
			}
		}
		it = it->next;
	}
	return cnt;
}

static void visited_clear(void)
{
	int i;
	for (i = 0; i < visited_cnt; i++) {
		visited[i]->is_visited = 0;
	}
	visited_cnt = 0;
}

static inline int query_rect(int x, int y, int w, int h)
{
	int cnt = 0;
	int idx, i, j, box_sx, box_sy, box_ex, box_ey;

	box_sx = x / GRID_C; box_ex = (int)(((float)(x + w)) / GRID_C);
	box_sy = y / GRID_R; box_ey = (int)(((float)(y + h)) / GRID_R);
	for (i = box_sy; i <= box_ey; i++) {
		for (j = box_sx; j <= box_ex; j++) {
			idx = INDEX(j, i, L_MAX_C_SIZE) + 1;
			cnt += __query_rect(&head[idx], x, y, w, h);
			if (option == 0 && cnt) return 1;
		}
	}
	return cnt;
}

static inline int query_rect_ref(int x, int y, int w, int h)
{
	int cnt = 0;
	cnt += __query_rect(&head[0], x, y, w, h);
	return cnt;
}

#define ADD_TEST(x,y,w,h) do {\
	int ret = add_rect(x, y, w, h);\
	if (ret) printf("@@@ [%d,%d,%d,%d] add fail\n", x, y, w, h); \
} while(0)

#define QUERY_TEST(x,y,w,h) do {\
	int ret1, ret2; \
	ret1 = query_rect(x,y,w,h); \
	ret2 = query_rect_ref(x,y,w,h); \
	if (ret1 != ret2) printf("@@@@ [%d,%d,%d,%d] query fail ret: %d ans: %d\n",\
				 x, y, w, h, ret1, ret2);\
} while(0)

static void test1(void)
{
	int i, tmp, cnt = 0;
	struct rect r;
#define TEST1_QUERY_SIZE 3000
	struct rect qr[TEST1_QUERY_SIZE];
	int ret[TEST1_QUERY_SIZE], ans[TEST1_QUERY_SIZE];

	printf("## %s start\n", __func__);
	rect_mem_init();
	rect_ln_mem_init();
	list_init();

	srand((unsigned int)time(NULL));

	while (cnt < 6000) {
		r.x = rand() % MAX_C;
		if (r.x == MAX_C -1) r.x = MAX_C - 300;
		tmp = rand() % GRID_C;
		if (!tmp) tmp = GRID_C / 4;
		r.w = tmp;
		if (r.x + r.w >= MAX_C) r.w = MAX_C -1 - r.x;

		r.y = rand() % MAX_R;
		if (r.y == MAX_R -1) r.y = MAX_R - 300;
		tmp = rand() % GRID_R;
		if (!tmp) tmp = GRID_R / 4;
		r.h = tmp;
		if (r.y + r.h >= MAX_R) r.h = MAX_R -1 - r.y;

		ADD_TEST(r.x, r.y, r.w, r.h);
		cnt++;
	}

	cnt = 0;
	while (cnt < TEST1_QUERY_SIZE) {
		tmp = rand() % MAX_C;
		if (!tmp) tmp = MAX_C / 4;
		qr[cnt].x = rand() % tmp;
		qr[cnt].w = tmp - qr[cnt].x;
		tmp = rand() % MAX_R;
		if (!tmp) tmp = MAX_C / 4;
		qr[cnt].y = rand() % tmp;
		qr[cnt].h = tmp - qr[cnt].y;
		cnt++;
	}

	timer_check();
	cnt = 0;
	while (cnt < TEST1_QUERY_SIZE) {
		ans[cnt] = query_rect_ref(qr[cnt].x, qr[cnt].y, qr[cnt].w, qr[cnt].h);
		visited_clear();
		cnt++;
	}
	printf("cal time: %ld ms\n", timer_check());

	timer_check();
	cnt = 0;
	while (cnt < TEST1_QUERY_SIZE) {
		ret[cnt] = query_rect(qr[cnt].x, qr[cnt].y, qr[cnt].w, qr[cnt].h);
		visited_clear();
		cnt++;
	}
	printf("cal time: %ld ms\n", timer_check());

	cnt = 0;
	while (cnt < TEST1_QUERY_SIZE) {
		if (ans[cnt] != ret[cnt]) {
			printf("[%d test] ans:%d != ret:%d [%d,%d,%d,%d]\n",
			       cnt, ans[cnt], ret[cnt],
			       qr[cnt].x, qr[cnt].y, qr[cnt].w, qr[cnt].h);

			exit(-1);
		}
		cnt++;
	}

	printf("## %s end\n", __func__);
}

int main(void)
{
	int i;
	for (i = 0; i < 10; i++) {
		printf("\n\n## %d-th test1 test\n", i+1);
		test1();
	}
	return 0;
} 
