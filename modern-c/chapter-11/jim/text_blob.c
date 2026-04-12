#include <stddef.h>
#include <string.h>
#include <stdlib.h>

typedef struct text_blob text_blob;
struct text_blob {
    size_t text_size;
    size_t buffer_size;
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
    size_t left_split_size = split_location + 1;
    size_t left_buffer_size = left_split_size * 2;

    size_t right_split_size = text_snippet.text_size + 1 - split_location;
    size_t right_buffer_size = right_split_size * 2;

    char *first_text = malloc(left_buffer_size);
    char *second_text = malloc(right_buffer_size);

    memcpy(
        first_text, 
        text_snippet.text, 
        split_location
    ); 
    memset(
        first_text + left_split_size, 
        '\0', 
        left_buffer_size - left_split_size
    );

    memcpy(
        second_text, 
        text_snippet.text + split_location, 
        right_split_size
    ); 
    memset(
        second_text + (text_snippet.text_size - split_location + 1), 
        '\0', 
        right_buffer_size - right_split_size
    );

    free(text_snippet.text);

    text_blob left_blob = {
        .text_size = left_split_size,
        .buffer_size = left_split_size * 2,
        .text = first_text,
        .previous = text_snippet.previous
    };

    text_blob right_blob = {
        .text_size = right_split_size,
        .buffer_size = right_split_size * 2,
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

text_blob join_text(text_blob first_snippet, text_blob second_snippet) {
    size_t text_size = first_snippet.text_size + second_snippet.text_size - 1;
    char *combined_buffer = malloc((text_size * 2) * sizeof(char));

    memcpy(combined_buffer, first_snippet.text, first_snippet.text_size - 1); 
    memcpy(combined_buffer + first_snippet.text_size - 1, second_snippet.text, second_snippet.text_size); 

    text_blob result = {
        .text_size = text_size,
        .buffer_size = text_size * 2,
        .text = combined_buffer,
        .previous = first_snippet.previous,
        .next = second_snippet.next
    };

    free(first_snippet.text);
    free(second_snippet.text);

    return result;
}