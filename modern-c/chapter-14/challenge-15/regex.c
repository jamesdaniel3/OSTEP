#include <stddef.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include <limits.h>
#include "regex.h"

#include <stdio.h>

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
size_t get_next_regex_token_size(char const* regex){
    //printf("Regex: %s\n", regex);
    if (regex[0] != '[') {
        return 1;
    }

    if (
        strncmp(regex, "[[:alpha]]", 10) == 0 ||
        strncmp(regex, "[[:digit]]", 10) == 0
    ) {
        return 10;
    }

    return 5;
}

// This function assumes it is receiving valid regex 
regex_char_range get_next_acceptable_chars(char const* regex){
    regex_char_range result = {
        .min_accetable_char = 0,
        .max_accetable_char = 0,
        .is_case_sensative = false,
    };

    if (regex[0] == '*') {
        result.max_accetable_char = CHAR_MAX;
        return result;
    }

    if (regex[0] != '[') {
        result.min_accetable_char = regex[0];
        result.max_accetable_char = regex[0];

        // these won't be super applicable in this case
        result.is_case_sensative = true; 

        return result;
    }

    if (strncmp(regex, "[[:alpha]]", 10) == 0) {
        result.min_accetable_char = 'a';
        result.max_accetable_char = 'z';
        result.is_case_sensative = false;

        return result;
    }

    if (strncmp(regex, "[[:digit]]", 10) == 0) {
        result.min_accetable_char = '0';
        result.max_accetable_char = '9';
        result.is_case_sensative = false;

        return result;
    }

    if (isdigit(regex[1])) {
        result.min_accetable_char = regex[1];
        result.max_accetable_char = regex[3];
        result.is_case_sensative = false;

        return result;
    }

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

// This function assumes it is receiving valid regex
match_position_info get_regex_match(
    char const* text_to_search, size_t text_size, 
    char const* regex, size_t regex_size,
    size_t current_result_start, size_t current_text_index
){
    //printf("Current Text Index: %zu\n", current_text_index);
    if (regex_size <= 1 || current_text_index == text_size - 1) {
        if (current_result_start == current_text_index) {
            match_position_info no_match = {
                .starting_index = 0,
                .length = 0
            };

            return no_match;
        }

        match_position_info match = {
            .starting_index = current_result_start,
            .length = current_text_index - current_result_start
        };
        
        return match;
    }

    // get valid character range 
    regex_char_range valid_char_range = get_next_acceptable_chars(regex);
    size_t regex_token_size = get_next_regex_token_size(regex);
    char next_char = text_to_search[current_text_index];
    char prev_char = current_text_index > 0 ? text_to_search[current_text_index - 1] : 0;

    // printf("Next Char: %c\n", next_char);
    // printf("Regex Token: %.*s\n", regex_token_size, regex[regex_token_size]);

    if (
        !valid_char_range.is_case_sensative
    ) {
        next_char = tolower(next_char);
        prev_char = tolower(prev_char);
    }

    if (
        next_char > valid_char_range.max_accetable_char ||
        next_char < valid_char_range.min_accetable_char
    ) {
        if (regex_token_size + 1 < regex_size && 
            (
                regex[regex_token_size] == '*' || 
                regex[regex_token_size] == '?'
            )
        ) {
            return get_regex_match(
                text_to_search, text_size,
                regex + (regex_token_size + 1), regex_size - (regex_token_size + 1),
                current_result_start, current_text_index
            );
        }

        if (
            regex_token_size + 1 < regex_size && 
            regex[regex_token_size] == '+' && 
            current_text_index > 0 && 
            prev_char <= valid_char_range.max_accetable_char &&
            prev_char >= valid_char_range.min_accetable_char
        ) {
            return get_regex_match(
                text_to_search, text_size,
                regex + (regex_token_size + 1), regex_size - (regex_token_size + 1),
                current_result_start, current_text_index
            );
        }

        match_position_info no_match = {
            .starting_index = 0,
            .length = 0
        };

        return no_match;
    }

    if (regex_token_size + 1 < regex_size && regex[regex_token_size] == '+') {
        return get_regex_match(
            text_to_search, text_size,
            regex, regex_size,
            current_result_start, current_text_index + 1
        );
    }

    if (regex_token_size + 1 < regex_size && regex[regex_token_size] == '*') {
        return get_regex_match(
            text_to_search, text_size,
            regex, regex_size,
            current_result_start, current_text_index + 1
        );
    }

    if (regex_token_size + 1 < regex_size && regex[regex_token_size] == '?') {
        return get_regex_match(
            text_to_search, text_size,
            regex + (regex_token_size + 1), regex_size - (regex_token_size + 1),
            current_result_start, current_text_index + 1
        );
    }

    return get_regex_match(
        text_to_search, text_size,
        regex + regex_token_size, regex_size - regex_token_size,
        current_result_start, current_text_index + 1
    );
}

