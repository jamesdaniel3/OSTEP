// CHALLENGE 15

/*
- X Can you search for a given word in a string?
- X Can you replace a word in a string and return a copy with the new contents?
- Can you implement some regular expression-matching functions for strings? 
  For example, find a character class such as [A-Q] or [^0-9] and match with * or ?.
- Can you implement a reglar expression-matching function for POSIX character classes such as [[:alpha]], [[:digit]], and so on?
- Can you stitch all of these functionalities together to search for a regexp in a string?
- Do query-replace with regexp against a specific word?
- Extend a regexp with grouping?
- Extend query-replace with grouping?
*/

#include <string.h>
#include <stddef.h>
#include <stdlib.h>

int search_text(char const* text_to_search, size_t text_size, char const* target, size_t target_size) {
    if (text_to_search == NULL || target == NULL) {
        return -1;
    }

    if (text_size <= 1 || target_size <= 1) {
        return -1;
    }

    if (text_size < target_size) {
        return -1;
    }

    for (size_t i = 0; i <= text_size - target_size; i++) { 
        if (strncmp(text_to_search + i, target, target_size - 1) == 0) {
            return i;
        }
    }

    return -1;
}

// Find and replace a word in a string
    // By replace I mean return a pointer to a modified copy of the string 
    // if the word is not in the string, return a pointer to the original string
char * replace_text(
    char const* original_string, size_t original_string_size,
    char const* text_to_replace, size_t text_to_replace_size,
    char const* new_text, size_t new_text_size
) {
    if (
        original_string == NULL ||
        text_to_replace == NULL ||
        new_text == NULL
    ) {
        return NULL;
    }

    int replacement_start_index = search_text(
        original_string, original_string_size, 
        text_to_replace, text_to_replace_size
    );

    if (replacement_start_index == -1) {
        return (char *)original_string;
    }

    size_t new_string_size = original_string_size; 
    char * result = calloc(new_string_size, 1);
    size_t current_search_index = 0;
    size_t current_result_index = 0;

    while (replacement_start_index != -1) {
        result = realloc(result, new_string_size + (new_text_size - text_to_replace_size));
        new_string_size += (new_text_size - text_to_replace_size);

        memcpy(
            result + current_result_index, 
            original_string + current_search_index, 
            replacement_start_index
        );
        memcpy(
            result + current_result_index + replacement_start_index,
            new_text,
            new_text_size
        );

        current_search_index += (replacement_start_index + text_to_replace_size - 1);
        current_result_index += (replacement_start_index + new_text_size - 1);

        replacement_start_index = search_text(
            original_string + current_search_index, original_string_size - current_search_index, 
            text_to_replace, text_to_replace_size
        );
    }

    if (current_search_index < original_string_size) {
        memcpy(
            result + current_result_index, 
            original_string + current_search_index, 
            original_string_size - current_search_index
        );
    }

    return result;
}