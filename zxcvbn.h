#ifndef ZXCVBN_H
#define ZXCVBN_H

#include <stddef.h>
#include <sys/queue.h>

LIST_HEAD(zxcvbn_dict_head, zxcvbn_dict);

struct zxcvbn_node;

struct zxcvbn_spatial_graph {
    const char *data[256][9];
    double degree;
    unsigned int n_chars;
    unsigned int token_size;
    unsigned int n_coords;
};

struct zxcvbn_dict {
    struct zxcvbn *zxcvbn;
    LIST_ENTRY(zxcvbn_dict) list;
    int allocated;
    char name[PATH_MAX];
    struct zxcvbn_node *root;
};

struct zxcvbn {
    int allocated;
    void *(*zxcvbn_malloc)(size_t size);
    void *(*zxcvbn_realloc)(void *ptr, size_t size);
    void (*zxcvbn_free)(void *ptr);
    unsigned int n_symbols;
    char pack_table[256];
    unsigned int pack_table_size;
    struct zxcvbn_dict_head dict_head;
    struct zxcvbn_spatial_graph spatial_graph_qwerty;
    struct zxcvbn_spatial_graph spatial_graph_dvorak;
    struct zxcvbn_spatial_graph spatial_graph_keypad;
    struct zxcvbn_spatial_graph spatial_graph_macpad;
    struct zxcvbn_spatial_graph spatial_graph_alphabet;
};

enum zxcvbn_match_type {
    ZXCVBN_MATCH_TYPE_DICT,
    ZXCVBN_MATCH_TYPE_SPATIAL,
    ZXCVBN_MATCH_TYPE_DIGITS,
    ZXCVBN_MATCH_TYPE_DATE,
    ZXCVBN_MATCH_TYPE_BRUTEFORCE,
};

struct zxcvbn_match {
    enum zxcvbn_match_type type;
    CIRCLEQ_ENTRY(zxcvbn_match) list;
    struct zxcvbn_spatial_graph *spatial_graph;
    int i, j;
    unsigned int turns;
    unsigned int shifted;
    unsigned int separator;
    unsigned int long_year;
    unsigned int only_year;
    unsigned int rank;
    double entropy;
};

CIRCLEQ_HEAD(zxcvbn_match_head, zxcvbn_match);

struct zxcvbn_res {
    struct zxcvbn *zxcvbn;
    struct zxcvbn_match_head match_head;
    struct zxcvbn_match match_buf[32];
    struct zxcvbn_match *matches;
    unsigned int n_matches;
    unsigned int n_matches_reserved;
    double entropy;
};

struct zxcvbn *
zxcvbn_init(struct zxcvbn *zxcvbn_buf,
            void *(*zxcvbn_malloc)(size_t size),
            void *(*zxcvbn_realloc)(void *ptr, size_t size),
            void (*zxcvbn_free)(void *ptr),
            const char *symbols);

struct zxcvbn_dict *
zxcvbn_dict_init(struct zxcvbn *zxcvbn, struct zxcvbn_dict *dict_buf, const char *name);

int
zxcvbn_dict_add_word(struct zxcvbn_dict *dict, const char *word, unsigned int word_len, unsigned int rank);

void
zxcvbn_res_init(struct zxcvbn_res *res, struct zxcvbn *zxcvbn);

void
zxcvbn_res_release(struct zxcvbn_res *res);

int
zxcvbn_match(struct zxcvbn_res *res, const char *password, unsigned int password_len,
             char **dict_words, unsigned int n_dict_words);

const char *
zxcvbn_match_type_string(enum zxcvbn_match_type type);

void
zxcvbn_release(struct zxcvbn *zxcvbn);

#endif