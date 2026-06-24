#include <stdio.h>
#include <stdlib.h>

int main(int argc, char * argv[]){
    if (argc < 2) {
        printf("wunzip: file1 [file2 ...]\n");
        exit(1);
    }

    for (int i = 1; i < argc; i++) {
        FILE* fp = fopen(argv[i], "r");

        if (fp == NULL) {
            printf("wunzip: cannot open file\n");
            exit(1);
        }

        int current_count;
        char current_char;

        while(1) {
            int bytes_read = fread(&current_count, 4, 1, fp);

            if (bytes_read < 1) {
                // assuming the input we get is good the program could only break here
                break;
            }

            fread(&current_char, 1, 1, fp);

            for(int i = 0; i < current_count; i++){
                printf("%c", current_char);
            }
        }
    }

    return 0;
}