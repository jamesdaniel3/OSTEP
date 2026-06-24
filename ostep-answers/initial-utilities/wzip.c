#include <stdio.h>
#include <stdlib.h>

int main(int argc, char * argv[]){
    if (argc < 2) {
        printf("wzip: file1 [file2 ...]\n");
        exit(1);
    }

    int current_char = 0;
    size_t char_count = 1;
    
    int last_file_char = 0;

    for (int i = 1; i < argc; i++) {
        FILE* fp = fopen(argv[i], "r");

        if (fp == NULL) {
            printf("wzip: cannot open file\n");
            exit(1);
        }

        while((current_char = last_file_char ? last_file_char : fgetc(fp)) != EOF) {
            if (last_file_char) {
                last_file_char = 0;
            }
            
            int next_char = fgetc(fp);

            if (next_char == EOF){
                last_file_char = current_char;
                break; 
            }

            if (next_char == current_char) {
                char_count++;
            }

            else {
                fwrite(&char_count, 4, 1, stdout);
                fwrite(&current_char, 1, 1, stdout);
                char_count = 1;
            }

            ungetc(next_char, fp);
        }
    }

    fwrite(&char_count, 4, 1, stdout);
    fwrite(&current_char, 1, 1, stdout);
    char_count = 1;

    return 0;
}