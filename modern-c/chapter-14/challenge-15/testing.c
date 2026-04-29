#include <assert.h>
#include <stdio.h>
#include <string.h>
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

int main(){
 test_search_text();
 test_replace_text();
}