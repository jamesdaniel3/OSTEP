#include <stddef.h>
#include <stdbool.h>
#include "regex.h"

int search_text(char const* text_to_search, size_t text_size, char const* target, size_t target_size);

char * replace_text(
    char const* original_string, size_t original_string_size,
    char const* text_to_replace, size_t text_to_replace_size,
    char const* new_text, size_t new_text_size
);

typedef struct regex_match_list regex_match_list;
struct regex_match_list{
    size_t num_matches;
    size_t capacity;
    regex_match* matches;
};

regex_match_list* search_text_regex(
    char const* text_to_search, size_t text_size, 
    char const* regex, size_t regex_size,
    regex_match_list* result_list
);