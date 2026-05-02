#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "search_functions.h"

void test_search_text(){
    // Target in word
    assert(search_text("Hello World", 12, "Worl", 5) == 6);
    assert(search_text("Hello World", 12, "l", 2) == 2);
    assert(search_text("Hello World", 12, "ld", 3) == 9);

    // Target not in word 
    assert(search_text("James Daniel", 13, "Jack", 5) == -1);
    assert(search_text("James Daniel", 13, "james", 6) == -1);

    // Target longer than word
    assert(search_text("123", 4, "12345", 6) == -1);
}

void test_replace_text() {
    // replacement text longer than text to be replaced
    char* original_string = "00110011";
    char* text_to_replace = "11";
    char* replacement = "2222";
    char* new_string = replace_text(original_string, 9, text_to_replace, 3, replacement, 5);
    assert(strcmp(new_string, "002222002222") == 0);

    // replacement text shorter than text to be replaced
    text_to_replace = "00";
    replacement = "a";
    new_string = replace_text(original_string, 9, text_to_replace, 3, replacement, 2);
    assert(strcmp(new_string, "a11a11") == 0);

    // another example of replacement 
    original_string = "hello world";
    text_to_replace = "l";
    replacement = "123";
    new_string = replace_text(original_string, 12, text_to_replace, 2, replacement, 4);
    assert(strcmp(new_string, "he123123o wor123d") == 0);

    // replace with an empty string (delete)
    text_to_replace = "l";
    replacement = "";
    new_string = replace_text(original_string, 12, text_to_replace, 2, replacement, 1);
    assert(strcmp(new_string, "heo word") == 0);

    // handle text to replace not found (return pointer to original string)
    text_to_replace = "33";
    new_string = replace_text(original_string, 9, text_to_replace, 3, replacement, 5);
    assert(new_string == original_string);

    // handle empty text to replace (return pointer to original string)
    text_to_replace = "";
    replacement = "123";
    new_string = replace_text(original_string, 12, text_to_replace, 1, replacement, 4);
    assert(new_string == original_string);

    // handle empty original text (return pointer to original string)
    original_string = "";
    text_to_replace = "l";
    replacement = "123";
    new_string = replace_text(original_string, 1, text_to_replace, 2, replacement, 4);
    assert(new_string == original_string);
}

void test_is_valid_regex(){
    char* regex = "[A-z]+";
    assert(is_valid_regex(regex, 7));

    regex = "[2-8]?";
    assert(is_valid_regex(regex, 7));

    regex = "a*";
    assert(is_valid_regex(regex, 3));

    regex = "Ja mes";
    assert(is_valid_regex(regex, 7));

    regex = "[[:digit]]";
    assert(is_valid_regex(regex, 11));

    regex = "[[:alpha]]";
    assert(is_valid_regex(regex, 11));

    regex = "[A - z]";
    assert(!is_valid_regex(regex, 8));

    regex = "A-z]";
    assert(!is_valid_regex(regex, 5));

    regex = "[1-j]";
    assert(!is_valid_regex(regex, 6));

    regex = "[[";
    assert(!is_valid_regex(regex, 3));

    regex = "[]";
    assert(!is_valid_regex(regex, 3));
}

void check_char_range_equality(regex_char_range first_range, regex_char_range second_range) {
    assert(first_range.min_accetable_char == second_range.min_accetable_char);
    assert(first_range.max_accetable_char == second_range.max_accetable_char);
    assert(first_range.is_case_sensative == second_range.is_case_sensative);
}

void test_get_next_acceptable_chars(){
    char* regex = "[A-j]";
    regex_char_range result = get_next_acceptable_chars(regex);
    regex_char_range expected_result = {
        .min_accetable_char = 'a',
        .max_accetable_char = 'j',
        .is_case_sensative = false,
    };
    check_char_range_equality(result, expected_result);

    regex = "[J-M]?";
    result = get_next_acceptable_chars(regex);
    expected_result = (regex_char_range){
        .min_accetable_char = 'J',
        .max_accetable_char = 'M',
        .is_case_sensative = true,
    };
    check_char_range_equality(result, expected_result);

    regex = "[2-5]*";
    result = get_next_acceptable_chars(regex);
    expected_result = (regex_char_range){
        .min_accetable_char = '2',
        .max_accetable_char = '5',
        .is_case_sensative = false,
    };
    check_char_range_equality(result, expected_result);

    regex = "a";
    result = get_next_acceptable_chars(regex);
    expected_result = (regex_char_range){
        .min_accetable_char = 'a',
        .max_accetable_char = 'a',
        .is_case_sensative = true,
    };
    check_char_range_equality(result, expected_result);

    regex = "[[:digit]]";
    result = get_next_acceptable_chars(regex);
    expected_result = (regex_char_range){
        .min_accetable_char = '0',
        .max_accetable_char = '9',
        .is_case_sensative = false,
    };
    check_char_range_equality(result, expected_result);

    regex = "[[:alpha]]";
    result = get_next_acceptable_chars(regex);
    expected_result = (regex_char_range){
        .min_accetable_char = 'a',
        .max_accetable_char = 'z',
        .is_case_sensative = false,
    };
    check_char_range_equality(result, expected_result);
}

void print_results(regex_match_list results) {
    for (size_t i = 0; i < results.num_matches; i++) {
        printf("Match: %s\n", results.matches[i].text);
    }
}

void check_search_regex_results(
    regex_match_list regex_matches, 
    const char** expected_matches, 
    size_t num_expected_matches
){
    assert(num_expected_matches == regex_matches.num_matches);

    for(size_t i = 0; i < num_expected_matches; i++) {
        const char* expected_match = expected_matches[i];
        const char* found_match = regex_matches.matches[i].text;
        size_t found_match_len = regex_matches.matches[i].length;
        assert(strncmp(expected_match, found_match, found_match_len) == 0);
    }
}

void reset_results(regex_match_list* matches) {
    free(matches->matches);
    matches->num_matches = 0;
    matches->capacity = 10;
    matches->matches = calloc(sizeof(regex_match), 10);
}

void test_regex_search(){
    regex_match_list result_list = {
        .num_matches = 0,
        .capacity = 10,
        .matches = calloc(sizeof(regex_match), 10)
    };

    // Test Question Mark Operator
    search_text_regex("color and colour", 17, "colou?r", 8, &result_list);
    check_search_regex_results(result_list, (const char*[]){"color", "colour"}, 2);
    reset_results(&result_list);

    // Test isalpha posix
    search_text_regex("h3LLo", 6, "9*[[:alpha]]+", 14, &result_list);
    check_search_regex_results(result_list, (const char*[]){"h", "LLo"}, 2);
    reset_results(&result_list);

    search_text_regex("000abcd00ab", 12, "[[:digit]]", 11, &result_list);
    check_search_regex_results(result_list, (const char*[]){"0", "0", "0", "0", "0"}, 5);

    free(result_list.matches);
}

void test_regex_replace() {
    char* original_string = "color and colours";
    char* regex = "colou?r";
    char* replacement = "color";
    char* new_string = replace_text_regex(original_string, 18, regex, 8, replacement, 6);
    assert(strcmp(new_string, "color and colors") == 0);

    original_string = "000abcd00ab";
    regex = "[A-z]*";
    replacement = "XXX";
    new_string = replace_text_regex(original_string, 12, regex, 7, replacement, 4);
    assert(strcmp(new_string, "000XXX00XXX") == 0);

    // replace with an empty string (delete)
    regex = "[[:digit]]";
    replacement = "";
    new_string = replace_text_regex(original_string, 12, regex, 11, replacement, 1);
    assert(strcmp(new_string, "abcdab") == 0);

    // handle regex not found (return identical string)
    regex = "33";
    new_string = replace_text_regex(original_string, 12, regex, 3, replacement, 5);
    assert(strcmp(new_string, original_string) == 0);

    // handle empty regex (return identical string)
    regex = "";
    replacement = "123";
    new_string = replace_text_regex(original_string, 12, regex, 1, replacement, 4);
    assert(strcmp(new_string, original_string) == 0);

    // handle empty original text (return identical string)
    original_string = "";
    regex = "l";
    replacement = "123";
    new_string = replace_text_regex(original_string, 1, regex, 2, replacement, 4);
    assert(strcmp(new_string, original_string) == 0);
}

int main(){
    test_search_text();
    test_replace_text();
    test_is_valid_regex();
    test_get_next_acceptable_chars();
    test_regex_search();
    test_regex_replace();
}