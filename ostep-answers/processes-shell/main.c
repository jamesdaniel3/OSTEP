#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/wait.h>

char error_message[30] = "An error has occurred\n";

void exit_shell(){
    fwrite(error_message, strlen(error_message), 1, stderr); 
    exit(1);
}

bool is_built_in_command(char* program){
    return strcmp("exit", program) == 0 || strcmp("cd", program) == 0 || strcmp("path", program) == 0;
}

char* find_path(char* program, char** paths, size_t num_paths){
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

void handle_user_command(char* args[static 1], char** paths, size_t num_paths){
    char* path = find_path(args[0], paths, num_paths);    

    if (path == NULL) {
        exit_shell();
    }
    args[0] = path;  

    int pid = fork();

    if (pid == 0) {
        int result = execv(args[0], args);
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

    free(path);
}

void handle_built_in_command(char* args[static 1], size_t args_len, char** paths, size_t num_paths) {
    if (
        (strcmp(args[0], "exit") == 0 && args_len != 2) || \
        (strcmp(args[0], "cd") == 0 && args_len != 3)
    ) {
        // parsing logic adds a NULL entry to the end of the array
        exit_shell();
    }

    if (strcmp(args[0], "exit") == 0) {
        exit(0);
    }

    else if (strcmp(args[0], "cd") == 0) {
        // doesn't seem to be working with ~
        int result = chdir(args[1]);
        if (result < 0){
            exit_shell();
        }
    }

    else if (strcmp(args[0], "path") == 0) {
        num_paths = args_len - 2;
        free(paths);

        paths = malloc(sizeof(char *) * num_paths);
        if (paths == NULL) {
            exit_shell();
        }
        for(size_t i = 1; i < args_len - 1; i++){
            paths[i - 1] = args[i];
        }
    }
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

    size_t num_paths = 1;
    char** paths = malloc(sizeof(char *) * num_paths);
    if (paths == NULL) {
        exit_shell();
    }
    paths[0] = "/bin/";

    char* line = NULL;
    size_t line_cap = 0;
    ssize_t linelen;

    if (is_interactive_mode) {
         printf("wish> ");
    }

    while ((linelen = getline(&line, &line_cap, is_interactive_mode ? stdin : fp)) > 0) {
        arg_arr line_args = get_args(line); 
        if(is_built_in_command(line_args.args[0])) {
            handle_built_in_command(line_args.args, line_args.len, paths, num_paths);
        } else {
            handle_user_command(line_args.args, paths, num_paths);
        }

        if (is_interactive_mode) {
            printf("wish> ");
        }

        free(line_args.args);
    }

    return 0;
}