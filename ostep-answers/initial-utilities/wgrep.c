#include <stdio.h>
#include <string.h>
#include <stdlib.h>

void parse_file_lines(){}

int main(int argc, char* argv[]){
    if (argc == 1) {
        printf("wgrep: searchterm [file ...]\n");
        exit(1);
    }

    char * search_term = argv[1];
    int search_len = strlen(search_term);

    if (argc == 2) {
        char * line = NULL;
        size_t line_capacity = 0;
        int line_len = 0;

        while ((line_len = getline(&line, &line_capacity, stdin)) > 0) {
            for(int j = 0; j < line_len; j++){
                if (strncmp(search_term, line + j, search_len) == 0){
                    // print the line
                    printf("%s", line);
                    break;
                }
            }
        }
    }

    for (int i = 2; i < argc; i++) {
        char * current_file = argv[i];
        FILE* fp = fopen(current_file, "r");

        if (fp == NULL) {
            printf("wgrep: cannot open file\n");
            exit(1);
        }

        char * line = NULL;
        size_t line_capacity = 0;
        int line_len = 0;

        while ((line_len = getline(&line, &line_capacity, fp)) > 0) {
            for(int j = 0; j < line_len; j++){
                if (strncmp(search_term, line + j, search_len) == 0){
                    // print the line
                    printf("%s", line);
                    break;
                }
            }
        }
    }

    return 0;
}