#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/wait.h>

size_t num_paths = 1;
char* paths[1] = { "/bin/" };
char error_message[30] = "An error has occurred\n";

void exit_shell(){
    fwrite(error_message, strlen(error_message), 1, stderr); 
    exit(1);
}

char* find_path(char * program){
    char * result;
    for(size_t i = 0; i < num_paths; i++){
        size_t path_size = strlen(paths[i]) + strlen(program) + 1; 
        result = realloc(NULL, path_size);
        if (result == NULL){
            exit_shell();
        }

        memcpy(result, paths[i], strlen(paths[i]));
        memcpy(result + strlen(paths[i]), program, strlen(program));
        result[path_size - 1] = '\0';

        int access_check = access(result, X_OK);
        if(access_check == 0){
            return result;
        }
    }
    return NULL;
}

typedef struct arg_arr arg_arr;
struct arg_arr {
    char** args;
    size_t len;
    size_t cap;
};

arg_arr get_args(char* line){
    size_t len = 0;
    size_t cap = 5;
    char** args = malloc(sizeof(char *) * cap);
    if (args == NULL) {
        exit_shell();
    }

    char** arg_pointer;

    for (arg_pointer = args; (*arg_pointer = strsep(&line, " \t\n")) != NULL; arg_pointer++){
        if (**arg_pointer != '\0') {
            if (++len == cap - 1) {
                cap *= 2;
                args = reallocf(args, sizeof(char *) * cap);
                if (args == NULL){
                    exit_shell();
                }
            }
        }
    }

    args[len] = NULL;
    len++;

    arg_arr result = {
        .args = args,
        .cap = cap, 
        .len = len,
    };

    return result;         
}

int main(int argc, char* argv[]){
    if(argc > 2) {
        exit_shell();
    }

    bool is_interactive_mode = argc == 1;
    FILE * fp = NULL;
    if (!is_interactive_mode) {
        fp = fopen(argv[1], "r");

        if (fp == NULL) {
            exit_shell();
        }
    }

    char* line = NULL;
    size_t line_cap = 0;
    ssize_t linelen;

    if (is_interactive_mode) {
         printf("wish> ");
    }

    while ((linelen = getline(&line, &line_cap, is_interactive_mode ? stdin : fp)) > 0) {
        arg_arr line_args = get_args(line);            // don't forget to clear line_args!!
        char* path = find_path(line_args.args[0]);     // don't forget to clear path!!

        if (path == NULL) {
            exit_shell();
        }
        line_args.args[0] = path;  

        int pid = fork();

        if (pid == 0) {
            int result = execv(line_args.args[0], line_args.args);
            if (result < 0) {
                exit_shell();
            }
        }

        else {
            int stat;
            int cpid = waitpid(pid, &stat, WUNTRACED);
            if (cpid < 0) {
                exit_shell();
            }
        }

        if (is_interactive_mode) {
            printf("wish> ");
        }
    }

    return 0;
}