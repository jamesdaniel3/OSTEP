#include <stddef.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "text_blob.h"

text_blob* parse_file(const char* file_path, text_blob lines_of_text[static 1]){
    size_t current_blob_index = 0;
    FILE * fstream = fopen(file_path, "r");

    if (!fstream) {
        perror("Error opening file");
        exit(1);
    }

    char * next_line = NULL;
    size_t capacity = 0;
    ssize_t line_length;

    while ((line_length = getline(&next_line, &capacity, fstream)) != -1) {
        size_t text_size = line_length + 1;
        size_t buffer_size = text_size * 2;
        char *copy = malloc(buffer_size);
        memcpy(copy, next_line, text_size);
        memset(copy + text_size, '\0', buffer_size - text_size);

        lines_of_text[current_blob_index] = (text_blob){
            .buffer_size = buffer_size,
            .text_size = text_size - 1,
            .text = copy,
            .next = NULL,
            .previous = (current_blob_index == 0 ? NULL : &lines_of_text[current_blob_index - 1])
        };

        if (current_blob_index != 0){
            lines_of_text[current_blob_index - 1].next = &lines_of_text[current_blob_index];
        }

        current_blob_index++;
    }

    free(next_line);
    fclose(fstream);
    return lines_of_text;
}

void output_file(const char* file_path, text_blob current_text_blob[static 1]){
    FILE * fstream = fopen(file_path, "w");

    if (!fstream) {
        perror("Error opening file");
        exit(1);
    }

    while (current_text_blob != NULL) {
        fprintf(fstream, "%s\n", current_text_blob->text);
        free(current_text_blob->text);
        current_text_blob = current_text_blob->next;
    }

    fclose(fstream);
}