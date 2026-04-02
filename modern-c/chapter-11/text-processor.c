#include <stddef.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct text_blob text_blob;
struct text_blob {
    size_t size;
    char* text;
    text_blob* next;
    text_blob* previous;
};

typedef struct split_text_result split_text_result;
struct split_text_result {
    text_blob first_blob;
    text_blob second_blob;
};

split_text_result split_text(text_blob text_snippet, size_t split_location){
    size_t left_split_size = split_location;
    size_t right_split_size = text_snippet.size - split_location;

    char *first_text = malloc((left_split_size + 1) * sizeof(char));
    char *second_text = malloc((right_split_size + 1) * sizeof(char));

    // copy from [0, location) like splitting does in python
    memcpy(first_text, text_snippet.text, left_split_size); 
    first_text[left_split_size] = '\0';

    memcpy(second_text, text_snippet.text + split_location, right_split_size); 
    second_text[right_split_size] = '\0';

    free(text_snippet.text);

    text_blob left_blob = {
        .size = left_split_size,
        .text = first_text,
        .previous = text_snippet.previous
    };

    text_blob right_blob = {
        .size = right_split_size,
        .text = second_text,
        .next = text_snippet.next
    };

    left_blob.next = &right_blob;
    right_blob.previous = &left_blob;

    split_text_result result = {
        .first_blob = left_blob,
        .second_blob = right_blob
    };

    return result;
}

int main(){
    char* test_text = malloc(strlen("Hello! My name is James and this is my test.") + 1);
    strcpy(test_text, "Hello! My name is James and this is my test.");

    text_blob initial_blob = {
        .size = strlen(test_text),
        .text = test_text,
        .previous = NULL,
        .next = NULL
    };

    split_text_result text_after_split = split_text(initial_blob, 6);
    printf("The left piece of the split is: %s\n", text_after_split.first_blob.text);
    printf("It has %zu characters\n", text_after_split.first_blob.size);
    
    printf("The right piece of the split is: %s\n", text_after_split.second_blob.text);
    printf("It has %zu characters\n", text_after_split.second_blob.size);
}