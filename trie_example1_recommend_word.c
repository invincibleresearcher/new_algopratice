#include <stdio.h>
// the below are just for tcs & debug
#include <string.h>
#include <stdlib.h>
#include <time.h>

#define MAX_WORDS	20000
#define MAX_WORD_LEN	20
#define MAX_NUM_OF_TYPE	30000
#define MAX_NUM_OF_BAN  20000
#define MAX_TRIE_NODE	((MAX_NUM_OF_TYPE + MAX_NUM_OF_BAN) * MAX_WORD_LEN)

/**
 * practice for suggesting most typed words for the given word prefix
 * 1. a word can be typed w/ # of typing
 * 2. a word can be given as a forbidden word for suggestion
 * 3. extra note> for code opt. purpose, it's guaranteed that word removal is
 * not included in this scenario
 */

enum option {
	TYPED,
	BANNED,
};

char words[MAX_WORDS][MAX_WORD_LEN + 1];
int words_size = 0;

static inline int mstrlen(const char src[]) {
	int c = 0;
	while (src[c] != '\0') ++c;
	return c;
}

static inline int mstrcpy(char dst[], const char src[]) {
	int c = 0;
	while ((dst[c] = src[c]) != '\0') ++c;
	return c;
}

static inline int mstrcmp(const char str1[], const char str2[]) {
	int c = 0;
	while (str1[c] != '\0' && str1[c] == str2[c]) ++c;
	return str1[c] - str2[c];
}

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

struct wordi {
	int typed_cnt;
	char is_banned;
};

#define TYPED_CNT(idx) (wordi_db[idx].typed_cnt)
#define IS_BANNED(idx) (wordi_db[idx].is_banned)
struct wordi wordi_db[MAX_WORDS];

struct trie {
	int word_idx;
	int most_typed_word_from_prefix;
	//
	struct trie *p;
	struct trie *c[26]; // by alphabet order
	struct trie *c_used[26]; // by adding order
	int child_cnt;
};

struct trie trie_head;
struct trie trie_pool[MAX_TRIE_NODE];
int trie_pool_idx = 0;

static inline void trie_node_init(struct trie *t)
{
	int i;
	for (i = 0; i < 26; i++) t->c[i] = NULL;
	t->child_cnt = 0;
	t->word_idx = t->most_typed_word_from_prefix= -1;
}

static inline struct trie *alloc_trie(struct trie *p)
{
	struct trie *n = &trie_pool[trie_pool_idx];
	trie_pool_idx++;
	trie_node_init(n);
	n->p = p;
	return n;
}

static inline void
update_most_typed_word_at_current_node(struct trie *t)
{
	int i, ci, cnt = t->child_cnt, selected = -1;
	if (t->word_idx != -1 && !IS_BANNED(t->word_idx)) {
		selected = t->word_idx;
	}
	for (i = 0; i < cnt; i++) {
		ci = t->c_used[i]->most_typed_word_from_prefix;
		if (ci == -1) continue;
		if (selected == -1 || TYPED_CNT(ci) > TYPED_CNT(selected)) {
			selected = ci;
		}

	}
	t->most_typed_word_from_prefix = selected;
}

static inline void
update_most_typed_word_backward(struct trie *t)
{
	while (t != &trie_head) {
		update_most_typed_word_at_current_node(t);
		t = t->p;
	}
}

static void trie_add(char *str, enum option opt, int typed_cnt)
{
	int cidx = 0;
	struct trie *t = &trie_head;
	struct wordi *wi;
	char *ori_str = str;

	while (*str != '\0') {
		cidx = *str - 'a';
		if (!t->c[cidx]) {
			t->c[cidx] = alloc_trie(t);
			t->c_used[t->child_cnt] = t->c[cidx];
			t->child_cnt++;
		}
		t = t->c[cidx];
		str++;
	}

	if (t->word_idx == -1) {
		wordi_db[words_size].typed_cnt = 0;
		wordi_db[words_size].is_banned = 0;
		mstrcpy(words[words_size], ori_str);
		t->word_idx = words_size;
		words_size++;
	}

	wi = &wordi_db[t->word_idx];
	if (opt == TYPED) {
		wi->typed_cnt += typed_cnt;
	}
	else {
		wi->is_banned = 1;
	}
	update_most_typed_word_backward(t);
}


// storing recommend word in dst & return its len.
// if not found, just copy prefix in dst
static inline int trie_get_recommended_word(char *prefix, char *dst)
{
	int cidx;
	struct trie *t = &trie_head;
	char *str = prefix;
	while (*str != '\0') {
		cidx = *str - 'a';
		if (!t->c[cidx]) {
			return mstrcpy(dst, prefix);
		}
		t = t->c[cidx];
		str++;
	}

	if (t->most_typed_word_from_prefix == -1) {
		return mstrcpy(dst, prefix);
	}
	return mstrcpy(dst, words[t->most_typed_word_from_prefix]);
}

static inline int has_prefix(char *prefix, char *str)
{
	int i, l1 = mstrlen(prefix), l2 = mstrlen(str);
	if (l2 < l1) return 0;
	for (i = 0; i < l1; i++) {
		if (prefix[i] != str[i]) return 0;
	}
	return 1;
}

static inline int trie_get_recommended_word_ref(char *prefix, char *dst)
{
	int i, tmp, sel = -1;
	for (i = 0; i < words_size; i++) {
		if (!has_prefix(prefix, words[i])) continue;
		if (IS_BANNED(i)) continue;
		if (TYPED_CNT(i) == 0) continue;
		if (sel == -1 || TYPED_CNT(i) > TYPED_CNT(sel)) {
			sel = i;
		}
	}
	if (sel == -1) {
		return mstrcpy(dst, prefix);
	}
	return mstrcpy(dst, words[sel]);
}

#define DO_TEST1(prefix) do {\
	int tmp1, tmp2; \
	tmp1 = trie_get_recommended_word(prefix, ret[tc]); \
	tmp2 = trie_get_recommended_word_ref(prefix, ans[tc]); \
	if (tmp1 != tmp2 || mstrcmp(ret[tc], ans[tc])) {\
		printf("@@ERR1 prefix: %s ans: %s != ret: %s\n", \
		       prefix, ans[tc], ret[tc]); \
		exit(-1); \
	} \
	tc++; \
} while(0)

static void test1(void)
{
#define TEST1_ANS_SIZE 50
	int tc = 0;
	char ans[TEST1_ANS_SIZE][MAX_WORD_LEN + 1];
	char ret[TEST1_ANS_SIZE][MAX_WORD_LEN + 1];
	//
	printf("## %s start..\n", __func__);
	words_size = 0;
	trie_node_init(&trie_head);
	trie_pool_idx = 0;
	//
	trie_add("string", TYPED, 1);
	trie_add("strike", TYPED, 3);
	trie_add("stress", TYPED, 5);
	trie_add("str", TYPED, 4);

	DO_TEST1("st");
	trie_add("strike", TYPED, 3);
	DO_TEST1("st");
	trie_add("string", TYPED, 4);
	DO_TEST1("st");
	trie_add("string", BANNED, 0);
	DO_TEST1("st");

	trie_add("s", TYPED, 10);
	DO_TEST1("s");
	trie_add("stress", TYPED, 12);
	DO_TEST1("s");
	trie_add("s", BANNED, 0);
	DO_TEST1("s");
	trie_add("stressful", TYPED, 14);
	DO_TEST1("s");
	trie_add("stressful", TYPED, 24);
	DO_TEST1("s");
	DO_TEST1("str");

	DO_TEST1("still");
	DO_TEST1("stressful");
	DO_TEST1("stl");

	trie_add("great", BANNED, 0);
	DO_TEST1("great");
	trie_add("g", BANNED, 0);
	DO_TEST1("g");
	trie_add("grrr", TYPED,  1);
	DO_TEST1("g");
	DO_TEST1("gr");

	printf("## %s end..\n", __func__);
}

int main(void)
{
	test1();
	return 0;
}
