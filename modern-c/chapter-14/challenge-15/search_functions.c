// CHALLENGE 15

/*
- X Can you search for a given word in a string?
- X Can you replace a word in a string and return a copy with the new contents?
- Can you implement some regular expression-matching functions for strings? 
  For example, find a character class such as [A-Q] or [^0-9] and match with * or ?.
- X Can you implement a reglar expression-matching function for POSIX character classes such as [[:alpha]], [[:digit]], and so on?
- Can you stitch all of these functionalities together to search for a regexp in a string?
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
#include <ctype.h>
#include <stdbool.h>
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

bool is_valid_regex(char const* regex, size_t regex_size){
    /*
    This is not a great approach to this check but for the current simple set of regex operations it works 
    I am trying to avoid writing a regex parser just for this challenge
    */
    size_t current_index = 0;

    while (current_index < regex_size) {
        if (regex[current_index] == ']') {
            return false;
        }
        
        if (regex[current_index] == '[') {
            if (current_index + 4 >= regex_size) {
                return false;
            }

            if (regex[current_index + 1] == '[') {
                if (
                    strncmp(&regex[current_index], "[[:alpha]]", 10) == 0 || 
                    strncmp(&regex[current_index], "[[:digit]]", 10) == 0
                ) {
                    current_index += 10;
                    continue;
                }
                else {
                    return false;
                }
            }

            if (regex[current_index + 2] != '-') {
                return false;
            }

            if (regex[current_index + 4] != ']') {
                return false;
            }

            char first_val = regex[current_index + 1];
            char second_val = regex[current_index + 3];

            if (!isalnum(first_val) || !isalnum(second_val)) {
                return false;
            }

            if (isdigit(first_val) != isdigit(second_val)) {
                return false;
            }

            current_index += 5;
        }

        current_index++;
    }

    return true;
}

// This function assumes it is receiving valid regex 
regex_char_range get_next_acceptable_chars(char const* regex, size_t regex_size){
    regex_char_range result = {
        .min_accetable_char = 0,
        .max_accetable_char = 0,
        .is_alphabetical = false,
        .is_case_sensative = false,
    };

    if (regex[0] != '[') {
        result.min_accetable_char = regex[0];
        result.max_accetable_char = regex[0];

        // these won't be super applicable in this case
        result.is_case_sensative = true; 
        result.is_alphabetical = isalpha(regex[0]);

        return result;
    }

    if (strncmp(regex, "[[:alpha]]", 10) == 0) {
        result.min_accetable_char = 'a';
        result.max_accetable_char = 'z';
        result.is_alphabetical = true;
        result.is_case_sensative = false;

        return result;
    }

    if (strncmp(regex, "[[:digit]]", 10) == 0) {
        result.min_accetable_char = '0';
        result.max_accetable_char = '9';
        result.is_alphabetical = false;
        result.is_case_sensative = false;

        return result;
    }

    if (isdigit(regex[1])) {
        result.min_accetable_char = regex[1];
        result.max_accetable_char = regex[3];
        result.is_alphabetical = false;
        result.is_case_sensative = false;

        return result;
    }

    result.is_alphabetical = true;
    result.is_case_sensative = islower(regex[1]) == islower(regex[3]);

    if (result.is_case_sensative) {
        result.min_accetable_char = regex[1];
        result.max_accetable_char = regex[3];

        return result;
    }

    result.min_accetable_char = tolower(regex[1]);
    result.max_accetable_char = tolower(regex[3]);

    return result;

}

int search_text_regex(
    char const* text_to_search, size_t text_size, 
    char const* regex, size_t regex_size
) {
    // call is_valid_regex and throw if not 
    // get possible chars
    // compare current char to possible chars 

    // shift char pointer and regex pointer and call
    // shift only char pointer and call if the regex was followed by a * or + 
}