#include <stddef.h>
#include <stdbool.h>

int search_text(char const* text_to_search, size_t text_size, char const* target, size_t target_size);

char * replace_text(
    char const* original_string, size_t original_string_size,
    char const* text_to_replace, size_t text_to_replace_size,
    char const* new_text, size_t new_text_size
);

/*
These functions could be moved to an internal only header because they really should be private but since this isn't
real prod code I am juse going to include them here. 
*/

typedef struct regex_char_range regex_char_range;
struct regex_char_range {
    char min_accetable_char;
    char max_accetable_char;
    bool is_alphabetical;
    bool is_case_sensative;
};

regex_char_range get_next_acceptable_chars(char const* regex, size_t regex_size);
bool is_valid_regex(char const* regex, size_t regex_size);