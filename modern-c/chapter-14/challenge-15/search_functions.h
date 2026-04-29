#include <stddef.h>

int search_text(char const* text_to_search, size_t text_size, char const* target, size_t target_size);

char * replace_text(
    char const* original_string, size_t original_string_size,
    char const* text_to_replace, size_t text_to_replace_size,
    char const* new_text, size_t new_text_size
);