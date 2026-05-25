#include <stdio.h>
#include <string.h>
#include "../utfpatgen.h"

/* Test context structure holding commonly used test fixtures */
struct test_context {
    struct trie *helper_trie;
    struct pattern_trie *pt;
    struct translate_table *tt;
    struct params *params;
    struct string_buffer *buf;
    struct pattern *pat;
    struct word *word;
    struct count_trie *ct;
};

/* Initialize test context with all structures pre-allocated */
struct test_context *setup_test_context() {
    struct test_context *ctx = malloc(sizeof(struct test_context));
    if (ctx == NULL) {
        return NULL;
    }

    /* Initialize all structures */
    ctx->helper_trie = init_trie(256);
    ctx->pt = init_pattern_trie(256, 256);
    ctx->tt = init_tr_table(256, 256);
    ctx->params = init_params();
    ctx->buf = init_buffer(256);
    ctx->pat = init_pattern(256);
    ctx->word = init_word(256);
    ctx->ct = init_count_trie(256, 256);

    /* Check if any allocation failed */
    if (ctx->helper_trie == NULL || ctx->pt == NULL || ctx->tt == NULL ||
        ctx->params == NULL || ctx->buf == NULL || ctx->pat == NULL ||
        ctx->word == NULL || ctx->ct == NULL) {
        /* Clean up any successful allocations */
        if (ctx->helper_trie != NULL) destroy_trie(ctx->helper_trie);
        if (ctx->pt != NULL) destroy_pattern_trie(ctx->pt);
        if (ctx->tt != NULL) destroy_tr_table(ctx->tt);
        if (ctx->params != NULL) destroy_params(ctx->params);
        if (ctx->buf != NULL) destroy_buffer(ctx->buf);
        if (ctx->pat != NULL) destroy_pattern(ctx->pat);
        if (ctx->word != NULL) destroy_word(ctx->word);
        if (ctx->ct != NULL) destroy_count_trie(ctx->ct);
        free(ctx);
        return NULL;
    }

    return ctx;
}

/* Clean up test context and all allocated resources */
void teardown_test_context(struct test_context *ctx) {
    if (ctx == NULL) {
        return;
    }
    if (ctx->helper_trie != NULL) {
        destroy_trie(ctx->helper_trie);
    }
    if (ctx->pt != NULL) {
        destroy_pattern_trie(ctx->pt);
    }
    if (ctx->tt != NULL) {
        destroy_tr_table(ctx->tt);
    }
    if (ctx->params != NULL) {
        destroy_params(ctx->params);
    }
    if (ctx->buf != NULL) {
        destroy_buffer(ctx->buf);
    }
    if (ctx->pat != NULL) {
        destroy_pattern(ctx->pat);
    }
    if (ctx->word != NULL) {
        destroy_word(ctx->word);
    }
    if (ctx->ct != NULL) {
        destroy_count_trie(ctx->ct);
    }
    free(ctx);
}

/* Run a test function with automatic setup and teardown */
void run_test(void (*test_func)(struct test_context *)) {
    struct test_context *ctx = setup_test_context();
    if (ctx != NULL) {
        test_func(ctx);
        teardown_test_context(ctx);
    } else {
        printf("ERROR: Failed to setup test context\n");
    }
}

/* Helper function to get pattern output */
struct output get_pattern_output(struct pattern_trie *pt, const char *pattern){
    size_t trie_index = traverse_trie(pt->t, pattern);
    struct output empty = {.value = EMPTY_OP_VALUE};
    if (trie_index == 0) {
        return empty;
    }
    size_t op_index = pt->t->aux[trie_index];
    if (op_index == 0) {
        return empty;
    }
    return pt->ops->data[pt->ops->lookup[op_index]];
}

/* Helper function to create mock buffer */
struct string_buffer *mock_buffer(const char *str) {
    struct string_buffer *buf = init_buffer(strlen(str) + 1);
    if (buf != NULL) {
        strcpy(buf->data, str);
        buf->size = strlen(str);
    }
    return buf;
}

/* Helper function to print buffer contents */
void print_buffer(struct string_buffer *buf) {
    printf("Buffer(size=%zu, capacity=%zu, eof=%d):\n", buf->size, buf->capacity, buf->eof);
    for (size_t i = 0; i < buf->size; i++) {
        printf(" buf[%zu] = '%c' (0x%02x)\n", i, buf->data[i], (uint8_t)buf->data[i]);
    }
}

/* Helper function to print outputs */
void print_outputs(struct outputs *ops) {
    for (size_t i = 1; i < ops->capacity+1; i++) {
        struct output op = ops->data[i];
        if (op.value != EMPTY_OP_VALUE) {
            printf("Output %zu: value=%zu, position=%zu\n", i, op.value, op.position);
        }
    }
    printf("Count: %zu, Capacity: %zu\n", ops->count, ops->capacity);
}

/* ===== TEST FUNCTIONS ===== */

void test_read_line(struct test_context *ctx) {
    printf("---- Read Line Test ----\n");
    FILE *file = fopen("test/read_line_test.txt", "r");
    if (file == NULL) {
        fputs("Could not open read_line_test.txt\n", stderr);
        return;
    }

    while (read_line(file, ctx->buf)) {
        if (ctx->buf->eof) {
            break;
        }
        printf("Read line: '%s'\n", ctx->buf->data);
        reset_buffer(ctx->buf);
    }

    fclose(file);
}

void test_parse_header(struct test_context *ctx){
    printf("\n---- Parse Header Test ----\n");

    const char *full_header = " 510 xyz";
    const char *no_header = " a A  ";
    const char *incomplete_header = " 1   x";
    const char *bad_header = "baadf00d";

    const char *test_headers[4] = {full_header, no_header, incomplete_header, bad_header};

    struct string_buffer *buf_mock;
    for (size_t i = 0; i < 4; i++){
        buf_mock = mock_buffer(test_headers[i]);
        reset_params(ctx->params);
        bool parsed = parse_header(buf_mock, ctx->params);
        printf("Header '%s'", test_headers[i]);
        if (parsed) {
            printf(": lefthyphenmin %d, righthyphenmin %d, bad '%c', missed '%c', good '%c'\n",
                   ctx->params->left_hyphen_min, ctx->params->right_hyphen_min,
                   ctx->params->bad_hyphen, ctx->params->missed_hyphen, ctx->params->good_hyphen);
        } else {
            printf(" was not parsed\n");
        }
        destroy_buffer(buf_mock);
    }
}

void test_trie(struct test_context *ctx) {
    printf("\n---- Trie Test ----\n");

    const char *patterns[] = {"test", "tea", "text"};
    size_t op_index;
    for (size_t i = 0; i < 2; i++){
        if (insert_pattern(ctx->pt->t, patterns[i], &op_index, ctx->helper_trie) &&
            set_output(ctx->pt, op_index, (uint8_t)(i+1), i + 1)) {
            printf("Pattern '%s' inserted successfully.\n", patterns[i]);
        } else {
            printf("Failed to insert pattern '%s'.\n", patterns[i]);
        }
    }

    struct output retrieved_op;
    for (size_t i = 0; i < 3; i++){
        retrieved_op = get_pattern_output(ctx->pt, patterns[i]);
        if (retrieved_op.value != EMPTY_OP_VALUE) {
            printf("Retrieved output for pattern '%s': value=%zu, position=%zu\n",
                   patterns[i], retrieved_op.value, retrieved_op.position);
        } else {
            printf("No output found for pattern '%s'.\n", patterns[i]);
        }
    }
}

void test_read_letters(struct test_context *ctx) {
    printf("\n---- Read Letters Test ----\n");

    strcpy(ctx->buf->data, " a A Á ˇA  ");
    ctx->buf->size = strlen(ctx->buf->data);

    if (!default_ascii_mapping(ctx->tt, ctx->helper_trie)) {
        return;
    }
    printf("Default mapping loaded successfully.\n");

    char *lower;
    char *letters[] = {"F", "ˇA", "ř"};
    for (size_t i = 0; i < 3; i++) {
        if ((lower = get_lower(ctx->tt, letters[i])) != 0) {
            printf("Letter '%s' found in trie, lower-case letter is '%s'\n", letters[i], lower);
        } else {
            printf("Letter '%s' not found in trie.\n", letters[i]);
        }
    }

    if (parse_letters(ctx->buf, ctx->tt, ctx->helper_trie)) {
        printf("Parsed line '%s' successfully.\n", ctx->buf->data);
    } else {
        printf("Failed to parse line '%s'.\n", ctx->buf->data);
    }

    if ((lower = get_lower(ctx->tt, letters[1])) != 0) {
        printf("Letter '%s' found in trie, lower-case letter is '%s'\n", letters[1], lower);
    } else {
        printf("Letter '%s' not found in trie.\n", letters[1]);
    }
}

void test_read_translate(struct test_context *ctx) {
    printf("\n---- Read Translate Test ----\n");
    FILE *file = fopen("test/german.tr", "r");
    if (file == NULL) {
        fputs("Could not open german.tr\n", stderr);
        return;
    }

    ctx->params->translate_file = file;

    if (read_translate(ctx->params, ctx->tt)) {
        printf("Translate table read successfully.\n");
    } else {
        printf("Failed to read translate table.\n");
    }
}

void test_parse_word(struct test_context *ctx) {
    printf("\n---- Parse Word Test ----\n");

    strcpy(ctx->buf->data, "te-st");
    ctx->buf->size = strlen(ctx->buf->data);

    if (!default_ascii_mapping(ctx->tt, ctx->helper_trie)) {
        return;
    }

    if (parse_word(ctx->buf, ctx->tt, ctx->params, ctx->word)) {
        char text[10] = {'\0'};
        struct word word = { .translated = text, .length = 0 };
        size_t letter_index;
        char *letter;
        char *word_index = ctx->word->translated;
        for (size_t i = 0; i < ctx->word->length; i++){
            letter_index = convert_byte_sequence(&(word_index));
            letter = ctx->tt->alphabet->data + ctx->tt->index_to_alphabet[letter_index];
            sprintf(word.translated + word.size, "%s", letter);
            word.size += strlen(letter);
        }
        
        printf("Word parsed: '%s', length=%zu\n", word.translated, ctx->word->length);
        printf("True hyphens at positions: ");
        for (size_t i = 0; i < ctx->word->length; i++) {
            if (get_true_hyphen(ctx->word, i) % 4 > 0) {
                printf("%zu ", i);
            }
        }
        printf("\n");
    } else {
        printf("Failed to parse word '%s'.\n", ctx->buf->data);
    }
}

void test_hyphenate_word(struct test_context *ctx){
    printf("\n---- Hyphenate Word Test ----\n");

    strcpy(ctx->buf->data, "\x1b\x14\x05\x13\x14\x1b");
    ctx->buf->size = strlen(ctx->buf->data);

    ctx->params->hyph_level = 1;

    if (!default_ascii_mapping(ctx->tt, ctx->helper_trie)) {
        printf("Failed to load default mapping.\n");
        return;
    }
    printf("Default mapping loaded successfully.\n");

    size_t op_index;
    if (!insert_pattern(ctx->pt->t, "\x14\x05", &op_index, ctx->helper_trie) ||
        !set_output(ctx->pt, op_index, 1, 1)){
        printf("Failed to insert pattern.\n");
        return;
    }

    for (size_t i = 0; i < ctx->buf->size; i++) {
        if (!append_char_to_word(ctx->word, ctx->buf->data[i])) {
            printf("Failed to build word.\n");
            return;
        }
    }
    ctx->word->true_hyphens[1] = 4;
    ctx->word->true_hyphens[2] = 5;
    ctx->params->left_hyphen_min = 1;
    ctx->params->right_hyphen_min = 1;

    if (hyphenate_word(ctx->word, ctx->pt, ctx->params)) {
        printf("Word hyphenated:\n\t");
        output_hyphenated_word(stdout, ctx->word, ctx->tt, ctx->params);
    } else {
        printf("Failed to hyphenate word.\n");
    }
}

void test_patterns(struct test_context *ctx){
    printf("\n---- Patterns Test ----\n");

    strcpy(ctx->buf->data, "\xfe\x02st\xff");
    ctx->buf->size = strlen(ctx->buf->data);

    if (!default_ascii_mapping(ctx->tt, ctx->helper_trie)) {
        return;
    }

    if (!parse_pattern(ctx->buf, ctx->pat, ctx->tt)){
        return;
    }
    printf("Pattern %s parsed.\n", ctx->buf->data);

    struct pass_stats ps;
    if (!insert_new_pattern(ctx->pat, ctx->pt, &ps, ctx->helper_trie)){
        return;
    }
    printf("Pattern inserted successfully.\n");

    struct output retrieved_op = get_pattern_output(ctx->pt, ctx->pat->text);
    if (retrieved_op.value != EMPTY_OP_VALUE) {
        printf("Retrieved output for pattern '%s': value=%zu, position=%zu\n",
               ctx->pat->text, retrieved_op.value, retrieved_op.position);
    } else {
        printf("No output found for pattern '%s'.\n", ctx->pat->text);
    }
}

void test_letter_index(struct test_context *ctx) {
    printf("\n---- Letter Index Test ----\n");
    
    // Load default ASCII mapping
    if (!default_ascii_mapping(ctx->tt, ctx->helper_trie)) {
        printf("Failed to load default mapping.\n");
        return;
    }
    printf("Default mapping loaded successfully.\n");
    
    // Test text
    const char *test_text = "hello";
    printf("Testing with text: '%s'\n", test_text);
    
    // Store original indices for verification
    size_t original_indices[32];
    size_t text_len = strlen(test_text);
    
    // Convert each character to index and then to byte sequence
    for (size_t i = 0; i < text_len; i++) {
        char letter[2] = {test_text[i], '\0'};
        size_t index = get_letter_index(ctx->tt, letter);
        original_indices[i] = index;
        
        printf("  '%c' -> index %zu", test_text[i], index);
        
        if (!convert_index(index, ctx->word)) {
            printf(" - Failed to convert index\n");
            return;
        }
        printf(" -> appended to word\n");
    }
    
    // Verify the word contains the expected byte sequences
    printf("Word contents (size=%zu): ", ctx->word->size);
    for (size_t i = 0; i < ctx->word->size; i++) {
        printf("0x%02x ", (uint8_t)ctx->word->translated[i]);
    }
    printf("\n");
    
    // Convert byte sequences back to indices and verify
    size_t pos = 0;
    char *word_index = ctx->word->translated;
    for (size_t i = 0; i < text_len && pos < ctx->word->size; i++) {
        size_t reconstructed_index = convert_byte_sequence(&word_index);
        
        // Advance position past the byte sequence
        while (ctx->word->translated[pos] == (char)0xff) {
            pos++;
        }
        pos++; // Skip the final byte
        
        printf("  Reconstructed index: %zu (original: %zu)", reconstructed_index, original_indices[i]);
        
        if (reconstructed_index != original_indices[i]) {
            printf(" - MISMATCH!\n");
            return;
        }
        printf(" - OK\n");
    }
    
    printf("Letter index test PASSED\n");
}

void test_german_letter_index(struct test_context *ctx) {
    printf("\n---- German Letter Index Test ----\n");
    
    // Read German translate file
    FILE *file = fopen("test/german.tr", "r");
    if (file == NULL) {
        printf("Failed to open german.tr\n");
        return;
    }
    ctx->params->translate_file = file;
    
    if (!read_translate(ctx->params, ctx->tt)) {
        printf("Failed to read German translate table.\n");
        return;
    }
    printf("German translate table loaded successfully.\n");
    
    // Test with German word containing non-ASCII characters: "über" (with ü)
    const char *test_text = "über";
    printf("Testing with German text: '%s'\n", test_text);
    
    // Store original indices for verification
    size_t original_indices[32];
    size_t text_len = strlen(test_text);
    size_t char_count = 0;
    
    // Convert each character to index and then to byte sequence
    for (size_t i = 0; i < text_len; ) {
        // Handle multi-byte UTF-8 characters
        char letter[8] = {0};
        
        if ((test_text[i] & 0x80) == 0) {
            // Single byte ASCII
            letter[0] = test_text[i];
            i++;
        } else if ((test_text[i] & 0xE0) == 0xC0) {
            // Two byte UTF-8
            letter[0] = test_text[i];
            letter[1] = test_text[i + 1];
            i += 2;
        } else if ((test_text[i] & 0xF0) == 0xE0) {
            // Three byte UTF-8
            letter[0] = test_text[i];
            letter[1] = test_text[i + 1];
            letter[2] = test_text[i + 2];
            i += 3;
        }
        
        size_t index = get_letter_index(ctx->tt, letter);
        original_indices[char_count] = index;
        
        printf("  '%s' -> index %zu", letter, index);
        
        if (!convert_index(index, ctx->word)) {
            printf(" - Failed to convert index\n");
            return;
        }
        printf(" -> appended to word\n");
        char_count++;
    }
    
    // Verify the word contains the expected byte sequences
    printf("Word contents (size=%zu): ", ctx->word->size);
    for (size_t i = 0; i < ctx->word->size; i++) {
        printf("0x%02x ", (uint8_t)ctx->word->translated[i]);
    }
    printf("\n");
    
    // Convert byte sequences back to indices and verify
    size_t pos = 0;
    char *word_index = ctx->word->translated;
    for (size_t i = 0; i < char_count && pos < ctx->word->size; i++) {
        size_t reconstructed_index = convert_byte_sequence(&word_index);
        
        // Advance position past the byte sequence
        while (ctx->word->translated[pos] == (char)0xff) {
            pos++;
        }
        pos++; // Skip the final byte
        
        printf("  Reconstructed index: %zu (original: %zu)", reconstructed_index, original_indices[i]);
        
        if (reconstructed_index != original_indices[i]) {
            printf(" - MISMATCH!\n");
            return;
        }
        printf(" - OK\n");
    }
    
    printf("German letter index test PASSED\n");
}

int main(void) {
    run_test(test_read_line);
    run_test(test_parse_header);
    run_test(test_trie);
    run_test(test_read_letters);
    run_test(test_read_translate);
    run_test(test_parse_word);
    run_test(test_hyphenate_word);
    run_test(test_patterns);
    run_test(test_letter_index);
    run_test(test_german_letter_index);

    return 0;
}

