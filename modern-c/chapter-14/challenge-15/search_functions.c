// CHALLENGE 15

/*
- X Can you search for a given word in a string?
- X Can you replace a word in a string and return a copy with the new contents?
- X Can you implement some regular expression-matching functions for strings? 
  For example, find a character class such as [A-Q] or [^0-9] and match with * or ?.
- X Can you implement a reglar expression-matching function for POSIX character classes such as [[:alpha]], [[:digit]], and so on?
- X Can you stitch all of these functionalities together to search for a regexp in a string?
- Do query-replace with regexp against a specific word?
- Extend a regexp with grouping?
- Extend query-replace with grouping?
*/

/*
Note: Regex Parsing could spiral into a much larger scope than I think is intended with this challenge involving a 
grammer and an AST and all that good stuff. Would be really cool to do in the future but I think out of scope at the 
moment. 

I am just going to handle a few basic things. 

*/

#include <string.h>
#include <stddef.h>
#include <stdlib.h>
#include "search_functions.h"

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
char* replace_text(
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

int update_match_list(regex_match_list* result_list, match_position_info new_match, char const* text) {
    char* match = calloc(new_match.length, 1);
    if (match == NULL) {
        return -1;
    }

    memcpy(match, text + new_match.starting_index, new_match.length);

    regex_match result = {
        .length = new_match.length,
        .text = match
    };

    if (
        result_list->num_matches >= result_list->capacity - 5 ||
        result_list->num_matches >= result_list->capacity * .8
    ) {
        result_list->matches = realloc(result_list->matches, 2*result_list->capacity*sizeof(regex_match)); // check for failure
        if (result_list->matches == NULL) {
            return -1;
        }

        result_list->capacity *= 2;
    }

    result_list->matches[result_list->num_matches] = result;
    result_list->num_matches += 1;

    return 0;
}

regex_match_list* search_text_regex(
    char const* text_to_search, size_t text_size, 
    char const* regex, size_t regex_size,
    regex_match_list* result_list
) {
    if (!is_valid_regex(regex, regex_size)) {
        return NULL;
    }

    size_t starting_index = 0;

    while (starting_index < text_size) {
        match_position_info new_match = get_regex_match(
            text_to_search, text_size, 
            regex, regex_size,
            starting_index, starting_index
        );

        if (new_match.length == 0) {
            starting_index++;
            continue;
        }
        starting_index += new_match.length;

        int error = update_match_list(result_list, new_match, text_to_search);

        if (error == -1) {
            return NULL;
        }

    }

    return result_list;
}
