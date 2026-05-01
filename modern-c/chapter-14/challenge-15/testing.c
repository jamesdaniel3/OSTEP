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
    assert(first_range.is_alphabetical == second_range.is_alphabetical);
    assert(first_range.is_case_sensative == second_range.is_case_sensative);
}

void test_get_next_acceptable_chars(){
    char* regex = "[A-j]";
    regex_char_range result = get_next_acceptable_chars(regex);
    regex_char_range expected_result = {
        .min_accetable_char = 'a',
        .max_accetable_char = 'j',
        .is_alphabetical = true,
        .is_case_sensative = false,
    };
    check_char_range_equality(result, expected_result);

    regex = "[J-M]?";
    result = get_next_acceptable_chars(regex);
    expected_result = (regex_char_range){
        .min_accetable_char = 'J',
        .max_accetable_char = 'M',
        .is_alphabetical = true,
        .is_case_sensative = true,
    };
    check_char_range_equality(result, expected_result);

    regex = "[2-5]*";
    result = get_next_acceptable_chars(regex);
    expected_result = (regex_char_range){
        .min_accetable_char = '2',
        .max_accetable_char = '5',
        .is_alphabetical = false,
        .is_case_sensative = false,
    };
    check_char_range_equality(result, expected_result);

    regex = "a";
    result = get_next_acceptable_chars(regex);
    expected_result = (regex_char_range){
        .min_accetable_char = 'a',
        .max_accetable_char = 'a',
        .is_alphabetical = true,
        .is_case_sensative = true,
    };
    check_char_range_equality(result, expected_result);

    regex = "[[:digit]]";
    result = get_next_acceptable_chars(regex);
    expected_result = (regex_char_range){
        .min_accetable_char = '0',
        .max_accetable_char = '9',
        .is_alphabetical = false,
        .is_case_sensative = false,
    };
    check_char_range_equality(result, expected_result);

    regex = "[[:alpha]]";
    result = get_next_acceptable_chars(regex);
    expected_result = (regex_char_range){
        .min_accetable_char = 'a',
        .max_accetable_char = 'z',
        .is_alphabetical = true,
        .is_case_sensative = false,
    };
    check_char_range_equality(result, expected_result);
}

void print_results(regex_match_list results) {
    for (size_t i = 0; i < results.num_matches; i++) {
        printf("Match: %s\n", results.matches[i].text);
    }
}

void test_regex_search(){
    regex_match_list result_list = {
        .num_matches = 0,
        .capacity = 10,
        .matches = calloc(sizeof(regex_match), 10)
    };

    search_text_regex("color and colour", 17, "colou?r", 8, &result_list);
    print_results(result_list);
}

int main(){
 test_search_text();
 test_replace_text();
 test_is_valid_regex();
 test_get_next_acceptable_chars();
 test_regex_search();

}