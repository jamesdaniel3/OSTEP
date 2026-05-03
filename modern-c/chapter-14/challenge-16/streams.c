/*
CHALLENGE 16 

For text processing in streams, can you read on stdin, dump modified text on stdout, and report diagnostics on stderr?
Count the occurences of a list of words? 
Count the occurences of a regexp?
Replace all occurrences of a word with another?
*/

#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include "search_functions.h"

#define INITIAL_BUFFER_SIZE 20 // this should be larger but we need it to be small at first so that we can test incomplete reads

void handle_count_words(char* input, char** word_list, size_t num_words){
    printf("Buffer Contents New: %s\n", input);
    for(size_t i = 0; i < num_words; i++) {
        size_t current_index = 0;
        size_t input_length = strlen(input);
        size_t occurence_count = 0;
        char* word_to_find = word_list[i];

        while (current_index < input_length) {
            int starting_index = search_text(
                input + current_index, input_length - current_index, 
                word_to_find, strlen(word_to_find)
            );

            if (starting_index == -1) {
                break;
            }

            occurence_count++;
            current_index += starting_index + strlen(word_to_find);
        }

        fprintf(stderr, "%s: %zu occurrences\n", word_to_find, occurence_count);
    }
}

char* read_next_line(){
    size_t buffer_size = INITIAL_BUFFER_SIZE;
    char* buffer = calloc(buffer_size, 1);
    size_t buffer_len = 0;

    if (!buffer) {
        return NULL;
    }

    while (1) {
        if (fgets(buffer + buffer_len, buffer_size - buffer_len, stdin) == NULL) {
            if (buffer_len == 0) {
                free(buffer);
                return NULL;
            }

            break;
        }

        buffer_len += strlen(buffer + buffer_len);

        if (buffer_len > 0 && buffer[buffer_len -1] == '\n') {
            break;
        }

        buffer_size *= 2;
        char* new_buffer = realloc(buffer, buffer_size);
        if (!new_buffer) {
            free(buffer);
            return NULL;
        }

        buffer = new_buffer;
    }

    return buffer;
}

int main(int argc, char* argv[argc]){ 
    bool count_words = false;
    bool count_regex = false;

    if (argc > 1) {
        if (strncmp(argv[1], "--count-words", 13) == 0) {
            count_words = true;
        }
        if (strncmp(argv[1], "--count-regex", 13) == 0) {
            count_regex = true;
        }

        if (!count_words && !count_regex) {
            printf("Invalid args. Second argument must be --count-words or --count-regex if provided.\n");
        }
    }

    char* buffer = read_next_line();
    while (buffer != NULL) {
        if (count_words) {
            handle_count_words(buffer, argv + 2, argc - 2);
        }
        buffer = read_next_line();
    }

    // convert output

    // dump to stdout

    // send diagnostic for the given line 

}