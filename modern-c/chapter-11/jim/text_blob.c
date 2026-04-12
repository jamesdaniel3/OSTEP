#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include "text_blob.h"

split_text_result split_text(text_blob* text_snippet, size_t split_location){
    size_t left_split_size = split_location;
    size_t left_buffer_size = left_split_size * 2 < 10 ? 10 : left_split_size * 2;

    size_t right_split_size = text_snippet->text_size - split_location; // 0 when at the end of a line 
    size_t right_buffer_size = right_split_size * 2 < 10 ? 10 : right_split_size * 2;

    char *first_text = malloc(left_buffer_size);
    char *second_text = malloc(right_buffer_size);

    memcpy(
        first_text, 
        text_snippet->text, 
        left_split_size
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
        second_text + right_split_size, 
        '\0', 
        right_buffer_size - right_split_size
    );

    if (right_split_size == 0) {
        second_text[0] = '\n';
    } 

    text_blob* right_snippet = malloc(sizeof(text_blob));

    free(text_snippet->text);

    text_snippet->text = first_text;
    text_snippet->text_size = left_split_size;
    text_snippet->buffer_size = left_buffer_size;

    right_snippet->text = second_text;
    right_snippet->text_size = right_split_size;
    right_snippet->buffer_size = right_buffer_size;
    right_snippet->previous = text_snippet;

    right_snippet->next = text_snippet->next;
    text_snippet->next = right_snippet;

    split_text_result result = {
        .first_blob = text_snippet,
        .second_blob = right_snippet
    };

    return result;
}

text_blob* join_text(text_blob* first_snippet, text_blob* second_snippet) {
    if(first_snippet-> text_size == 0 && second_snippet->text_size == 0){
        // added because the logic below was struggling to deal with delting when in between new lines
        // I think the logic can be rewritten in a more elegant way so that we don't need code specifically for this edge case,
        // but I am okay with this for now
        first_snippet->next = second_snippet->next;
        free(second_snippet);
        return first_snippet;
    }

    size_t text_size = first_snippet->text_size + second_snippet->text_size + 1;
    size_t buffer_size = text_size * 2;
    char *combined_buffer = malloc(buffer_size);

    memcpy(combined_buffer, first_snippet->text, first_snippet->text_size);
    combined_buffer[first_snippet->text_size] = ' ';
    memcpy(combined_buffer + first_snippet->text_size + 1, second_snippet->text, second_snippet->text_size);
    memset(
        combined_buffer + first_snippet->text_size + second_snippet->text_size + 1, // two strings, space, and start after that  
        '\0', 
        buffer_size - (first_snippet->text_size + second_snippet->text_size + 1)
    );

    free(first_snippet->text);

    first_snippet->text_size = text_size;
    first_snippet->buffer_size = buffer_size;
    first_snippet->text = combined_buffer;
    first_snippet->next = second_snippet->next;

    free(second_snippet);

    return first_snippet;
}