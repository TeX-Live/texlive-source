#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#ifndef UTFPATGEN_VERSION
#define UTFPATGEN_VERSION "1.0"
#endif

struct trie {
    size_t capacity;
    size_t occupied;
    size_t node_max;
    size_t base_max;
    size_t pattern_count;
    char *nodes;  // _c
    size_t *links;  // _l
    size_t *aux;  // _r
    char *taken;
};

struct trie *init_trie(size_t capacity);
struct trie *resize_trie(struct trie *t, size_t new_capacity);
void relink_trie(struct trie *t);
void destroy_trie(struct trie *t);

bool put_first_level(struct trie *t);

bool copy_node(struct trie *from, size_t from_index, struct trie *to, size_t to_index);

bool get_base_used(struct trie *t, size_t index);
bool set_base_used(struct trie *t, size_t index, bool used);

void set_links(struct trie *t, size_t from, size_t to);

bool is_node_occupied(struct trie *t, size_t index);

bool find_base_for_first_fit(struct trie *t, struct trie *q, size_t *out_base);
bool first_fit(struct trie *t, struct trie *q, size_t *out_base);
bool unpack(struct trie *from, size_t base, struct trie *to);
size_t traverse_trie(struct trie *t, const char *pattern);

struct output {
    size_t value;
    size_t position;
    size_t next_op_index;
};

struct outputs {
    size_t capacity;
    size_t count;
    struct output *data;
    size_t lookup_cap;
    size_t lookup_cnt;
    size_t *lookup;
};

struct outputs *init_outputs(size_t capacity);
struct outputs *resize_outputs(struct outputs *ops, size_t capacity);
void destroy_outputs(struct outputs *ops);
bool resize_lookup(struct outputs *ops, size_t new_cap, struct trie *t);

struct pattern_trie {
    struct trie *t;
    struct outputs *ops;
};

struct pattern_trie *init_pattern_trie(size_t trie_capacity, size_t outputs_capacity);
void destroy_pattern_trie(struct pattern_trie *pt);

bool new_trie_output(struct pattern_trie *pt, size_t value, size_t position, size_t next, size_t *op_index);
size_t hash_trie_output(struct outputs *ops, size_t value, size_t position, size_t next);

bool insert_pattern(struct trie *t, const char *pattern, size_t *out_op_index, struct trie *helper_trie);
bool insert_substring(struct trie *t, const char *pattern, size_t end, size_t length, size_t *out_op_index, struct trie *helper_trie);
bool repack(struct trie *t, struct trie *q, size_t *node, size_t *link, char value);
bool set_output(struct pattern_trie *pt, size_t node, size_t value, size_t position);

struct pattern_counts {
    size_t capacity;
    size_t size;
    size_t *good;
    size_t *bad;
};

struct pattern_counts *init_pattern_counts(size_t capacity);
struct pattern_counts *resize_pattern_counts(struct pattern_counts *pc, size_t new_capacity);
void destroy_pattern_counts(struct pattern_counts *pc);

struct count_trie {
    struct trie *t;
    struct pattern_counts *cnts;
};

struct count_trie *init_count_trie(size_t trie_capacity, size_t counts_capacity);
void destroy_count_trie(struct count_trie *ct);

enum hyphen_type { // 2-bit logic: upper = hyphen was found, lower = hyphen is present
    NO_HYF = 0,
    MISS_HYF = 1,
    BAD_HYF = 2,
    GOOD_HYF = 3,
};

struct params {
    // global
    uint8_t left_hyphen_min;
    uint8_t right_hyphen_min;
    char bad_hyphen;
    char missed_hyphen;
    char good_hyphen;
    uint8_t hyph_start;
    uint8_t hyph_finish;
    uint8_t word_weight;
    FILE *dictionary_file;
    FILE *pattern_file;
    FILE *output_file;
    FILE *translate_file;
    // level specific
    uint8_t hyph_level;
    uint8_t pat_start;
    uint8_t pat_finish;
    uint8_t good_wt;
    uint8_t bad_wt;
    uint8_t thresh;
    enum hyphen_type good_dot;
    enum hyphen_type bad_dot;
    // pass specific
    uint8_t pat_len;
    uint8_t pat_dot;
};

struct params *init_params();
void reset_params(struct params *params);
void destroy_params(struct params *params);

struct pass_stats {
    size_t good_pat_cnt;
    size_t bad_pat_cnt;
    size_t good_cnt;
    size_t bad_cnt;
    size_t miss_cnt;
    size_t level_pattern_cnt;
    uint8_t max_level;
    bool more_to_come;
};

struct string_buffer {
    size_t capacity;
    size_t size;
    char *data;
    bool eof;
};

struct string_buffer *init_buffer(size_t capacity);
struct string_buffer *resize_buffer(struct string_buffer *buf, size_t new_capacity);
void reset_buffer(struct string_buffer *buf);
void destroy_buffer(struct string_buffer *buf);

struct stack {
    size_t capacity;
    size_t top;
    size_t *data;
};

struct stack *init_stack(size_t capacity);
struct stack *resize_stack(struct stack *s, size_t new_capacity);
void destroy_stack(struct stack *s);
bool put_on_stack(struct stack *s, size_t value);
size_t get_top_value(struct stack *s);
void set_top_value(struct stack *s, size_t value);

struct word {
    size_t capacity;
    size_t size;
    size_t length;
    char *translated;
    size_t *true_hyphens;
    uint8_t *found_hyphens;
    bool *no_more;
};

struct word *init_word(size_t capacity);
struct word *resize_word(struct word *word, size_t new_capacity);
void reset_word(struct word *word);
void destroy_word(struct word *word);

bool append_char_to_word(struct word *word, char c);

size_t get_true_hyphen(struct word *word, size_t index);
bool set_true_hyphen(struct word *word, size_t index, size_t value);

uint8_t get_found_hyphen(struct word *word, size_t index);
bool set_found_hyphen(struct word *word, size_t index, uint8_t value);

bool get_no_more(struct word *word, size_t index);
bool set_no_more(struct word *word, size_t index, bool value);

#ifndef BAD_OP_VALUE
#define BAD_OP_VALUE (size_t) 255
#endif

#ifndef EMPTY_OP_VALUE
#define EMPTY_OP_VALUE (size_t) 0
#endif
bool is_utf_start_byte(uint8_t byte);

bool collect_count_trie(struct count_trie *ct, struct pattern_trie *pt, struct params *params, struct pass_stats *ps);
bool traverse_count_trie(struct count_trie *ct, struct pattern_trie *pt, struct params *params, struct pass_stats *ps);

bool delete_bad_patterns(struct pattern_trie *pt);
void deallocate_node(struct trie *t, size_t t_index);
bool link_around_bad_outputs(struct pattern_trie *pt, size_t t_index);
bool delete_patterns(struct pattern_trie *pt);

struct translate_table {
    struct trie *mapping;
    struct string_buffer *alphabet;
    size_t *index_to_alphabet;
    size_t letter_count;
    size_t letter_capacity;
};

struct translate_table *init_tr_table(size_t mapping_capacity, size_t alphabet_capacity);
void destroy_tr_table(struct translate_table *tt);

bool read_translate(struct params *params, struct translate_table *tt);
bool parse_header(struct string_buffer *buf, struct params *params);
bool parse_letters(struct string_buffer *buf, struct translate_table *tt, struct trie *helper_trie);
bool default_ascii_mapping(struct translate_table *tt, struct trie *helper_trie);
char *get_lower(struct translate_table *tt, const char *letter);
size_t get_letter_index(struct translate_table *tt, char *letter);
bool convert_index(size_t index, struct word *word);
size_t convert_byte_sequence(char **sequence);

bool output_patterns(struct pattern_trie *pt, struct translate_table *tt, FILE *pattern_file);
void output_pattern(struct string_buffer *pattern, struct translate_table *tt, struct outputs *ops, size_t op_index, FILE *pattern_file);
size_t get_highest_level(struct outputs *ops, size_t start_index, size_t position);

bool read_line(FILE *stream, struct string_buffer *buf);
bool append_char(struct string_buffer *buf, char c);
bool append_string(struct string_buffer *buf, const char *str);

#ifndef EDGE_OF_WORD
// not used in UTF-8
#define EDGE_OF_WORD (char) 0xff
#endif

bool parse_word(struct string_buffer *buf, struct translate_table *tt, struct params *params, struct word *out_word);

bool hyphenate_word(struct word *word, struct pattern_trie *pt, struct params *params);
void count_dots(struct word *word, struct params *params, struct pass_stats *ps);
void output_hyphenated_word(FILE *pattmp, struct word *word, struct translate_table *tt, struct params *params);

bool process_word(struct word *word, struct count_trie *ct, struct params *params, struct trie *helper_trie);

bool process_dictionary(struct params *params, struct translate_table *tt, struct pattern_trie *pt, struct pass_stats *ps);
bool process_all_words(struct params *params, struct translate_table *tt, struct pattern_trie *pt, struct pass_stats *ps, struct count_trie *ct);
bool hyphenate_dictionary(struct params *params, struct translate_table *tt, struct pattern_trie *pt, bool output, struct pass_stats *ps);
bool hyphenate_all_words(struct params *params, struct translate_table *tt, struct pattern_trie *pt, FILE *pattmp, struct pass_stats *ps);

struct pattern {
    size_t capacity;
    size_t size;
    size_t length;
    char *text;
    uint8_t *hyphens;
};

#ifndef HYPHEN_FLAG
// not used in UTF-8
#define HYPHEN_FLAG (char) 0xfe
#endif

struct pattern *init_pattern(size_t capacity);
struct pattern *resize_pattern(struct pattern *pat, size_t new_capacity);
void reset_pattern(struct pattern *pat);
void destroy_pattern(struct pattern *pat);

bool convert_index_to_pattern(size_t index, struct pattern *pat);

uint8_t get_hyphen(struct pattern *pat, size_t index);
bool set_hyphen(struct pattern *pat, size_t index, uint8_t value);

bool read_patterns(struct params *params, struct pattern_trie *pt, struct translate_table *tt, struct pass_stats *ps);
bool parse_pattern(struct string_buffer *buf, struct pattern *out_pattern, struct translate_table *tt);
bool insert_new_pattern(struct pattern *pat, struct pattern_trie *pt, struct pass_stats *ps, struct trie *helper_trie);

bool parse_input(char *argv[], int argc, struct params *params);
void print_help();
void print_version();