#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include "text_blob.h"

split_text_result split_text(text_blob* text_snippet, size_t split_location){
    size_t left_split_size = split_location + 1;
    size_t left_buffer_size = left_split_size * 2;

    size_t right_split_size = text_snippet->text_size + 1 - split_location;
    size_t right_buffer_size = right_split_size * 2;

    char *first_text = malloc(left_buffer_size);
    char *second_text = malloc(right_buffer_size);

    memcpy(
        first_text, 
        text_snippet->text, 
        split_location
    ); 
    memset(
        first_text + left_split_size, 
        '\0', 
        left_buffer_size - left_split_size
    );

    memcpy(
        second_text, 
        text_snippet->text + split_location, 
        right_split_size
    ); 
    memset(
        second_text + (text_snippet->text_size - split_location + 1), 
        '\0', 
        right_buffer_size - right_split_size
    );

    text_blob* left_snippet = malloc(sizeof(text_blob));
    text_blob* right_snippet = malloc(sizeof(text_blob));

    left_snippet->text = first_text;
    left_snippet->text_size = left_split_size;
    left_snippet->buffer_size = left_buffer_size;
    left_snippet->previous = text_snippet->previous;
    left_snippet->next = right_snippet;

    right_snippet->text = second_text;
    right_snippet->text_size = right_split_size;
    right_snippet->buffer_size = right_buffer_size;
    right_snippet->previous = left_snippet;
    right_snippet->next = text_snippet->next;

    free(text_snippet->text);

    split_text_result result = {
        .first_blob = left_snippet,
        .second_blob = right_snippet
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