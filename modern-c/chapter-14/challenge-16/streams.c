/*
CHALLENGE 16 

- X For text processing in streams, can you read on stdin, dump modified text on stdout, and report diagnostics on stderr?
- X Count the occurences of a list of words? 
- X Count the occurences of a regexp?
- X Replace all occurrences of a word with another?
*/

#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include "search_functions.h"

#define INITIAL_BUFFER_SIZE 20 // this should be larger but we need it to be small at first so that we can test incomplete reads

void handle_count_words(char* input, char** word_list, size_t num_words){
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

void handle_count_regex(char* input, char* regex){
    regex_match_list matches = {
        .num_matches = 0,
        .capacity = 10,
        .matches = malloc(sizeof(regex_match) * 10)
    };

    search_text_regex(
        input, strlen(input),
        regex, strlen(regex),
        &matches
    );

    fprintf(stderr, "%s: %zu matches\n", regex, matches.num_matches);
}

void handle_replace_words(char* input, char* word_to_replace, char* replacement){
    /*
    Strlen does not include the null terminator but the replace text function expects it to be 
    included in the length of the string
    */
    char* updated_text = replace_text(
        input, strlen(input) + 1,
        word_to_replace, strlen(word_to_replace) + 1,
        replacement, strlen(replacement) + 1
    );

    fprintf(stdout, "%s", updated_text);
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
    bool replace_word = false;

    if (argc < 3) {
        printf("Invalid args. You must provide a mode and some arguments.\n");
        exit(1);
    }

    if (strncmp(argv[1], "--count-words", 13) == 0) {
        count_words = true;
    }
    if (strncmp(argv[1], "--count-regex", 13) == 0) {
        count_regex = true;
    }
    if (strncmp(argv[1], "--replace-word", 15) == 0) {
        replace_word = true;
    }

    if (!count_words && !count_regex && !replace_word) {
        printf("Invalid args. Second argument must be --count-words or --count-regex if provided.\n");
        exit(1);
    }

    if (replace_word && argc != 4) {
        printf("Invalid args. Replace mode expects a word to find and a replacement.\n");
        exit(1);
    }

    char* buffer = read_next_line();
    while (buffer != NULL) {
        if (count_words) {
            handle_count_words(buffer, argv + 2, argc - 2);
        }
        if (count_regex) {
            handle_count_regex(buffer, argv[2]);
        }
        if(replace_word) {
            handle_replace_words(buffer, argv[2], argv[3]);
        }

        buffer = read_next_line();
    }
}