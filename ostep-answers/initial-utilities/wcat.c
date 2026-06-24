#include <stdlib.h>
#include <stdio.h>

int main(int argc, char * argv[]){
    for (int i = 1; i < argc; i++){
        char * current_file = argv[i];

        FILE* fp = fopen(current_file, "r");

        if (fp == NULL) {
            printf("wcat: cannot open file\n");
            exit(1);
        }

        char buffer[100] = {};

        char * result = fgets(buffer, 100, fp);

        while (result != NULL) {
            printf("%s", result);
            result = fgets(buffer, 100, fp);
        }
    }

    return 0;
}