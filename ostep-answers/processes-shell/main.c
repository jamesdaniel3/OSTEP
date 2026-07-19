#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/wait.h>
#include <ctype.h>
#include <fcntl.h>

char error_message[30] = "An error has occurred\n";
char ** paths = NULL;
size_t path_len = 0;

bool is_built_in_command(char* program){
    return strcmp("exit", program) == 0 || strcmp("cd", program) == 0 || strcmp("path", program) == 0;
}

char* find_path(char* program){
    char * result;
    for(size_t i = 0; i < path_len; i++){
        size_t path_size = strlen(paths[i]) + strlen(program) + 2; 
        result = realloc(NULL, path_size);
        if (result == NULL){
            fwrite(error_message, strlen(error_message), 1, stderr); 
            exit(1);
        }

        memcpy(result, paths[i], strlen(paths[i]));
        result[strlen(paths[i])] = '/';
        memcpy(result + strlen(paths[i]) + 1, program, strlen(program));
        result[path_size - 1] = '\0';

        int access_check = access(result, X_OK);
        if(access_check == 0){
            return result;
        }
    }
    return NULL;
}

char* trim_whitespace(char *str){
    if (str == NULL) {
        return NULL;
    }
    char *end;

    while(isspace((unsigned char)*str)) str++;

    if(*str == 0)  // All spaces?
    return str;

    end = str + strlen(str) - 1;
    while(end > str && isspace((unsigned char)*end)) end--;

    end[1] = '\0';

    return str;
}

typedef struct split_arr split_arr;
struct split_arr {
    char** strings;
    size_t len;
    size_t cap;
};

char* add_space_to_redirectors(char* command){
    size_t result_len = strlen(command);
    char* result = malloc(result_len);
    if (result == NULL){
        fwrite(error_message, strlen(error_message), 1, stderr); 
        exit(1);
    }
    result = strdup(command);

    char* replacement_str = " > ";
    size_t command_index = 0;
    size_t result_index = 0;
    char* location = strchr(command, '>');

    if(location == NULL){
        return result;
    }

    while (location != NULL){
        size_t length = location - (command + command_index);

        result = reallocf(result, result_len + 3);
        if (result == NULL){
            fwrite(error_message, strlen(error_message), 1, stderr); 
            exit(1);
        }
        result_len += 3;

        memcpy(
            result + result_index, 
            command + command_index,
            length
        );
        memcpy(
            result + result_index + length, 
            replacement_str, 
            3
        );

        result_index += (length + 3);
        command_index += (length + 1);
        location = strchr(command + command_index, '>');
    }

    memcpy(
        result + result_index, 
        command + command_index,
        command + strlen(command) - (command + command_index)
    );
    result[result_len - 1] = '\0';

    return result;
}

split_arr split_string(char* line, char* delimiters){
    size_t len = 0;
    size_t cap = 5;
    char** args = malloc(sizeof(char *) * cap);
    if (args == NULL) {
        fwrite(error_message, strlen(error_message), 1, stderr); 
        exit(1);
    }

    char** next_arg = args;

    while ((*next_arg = trim_whitespace(strsep(&line, delimiters))) != NULL){
        if (**next_arg != '\0') {
            len++;
            if (len >= cap - 1) { 
                cap *= 2;
                args = reallocf(args, sizeof(char *) * cap);
                if (args == NULL){
                    fwrite(error_message, strlen(error_message), 1, stderr); 
                    exit(1);
                }
                next_arg = args + len - 1;
            }

            next_arg++;
        }
    }

    args[len] = NULL;
    len++;

    split_arr result = {
        .strings = args,
        .cap = cap, 
        .len = len,
    };

    return result;         
}

typedef struct command_arr command_arr;
struct command_arr {
    split_arr* commands;
    size_t len;
    size_t cap;
};

command_arr split_args_into_commands(char* line){
    split_arr* command_list = malloc(sizeof(split_arr) * 5);
    if (command_list == NULL) {
        fwrite(error_message, strlen(error_message), 1, stderr); 
        exit(1);
    }

    command_arr result = {
        .commands = command_list,
        .len = 0,
        .cap = 5
    };

    split_arr strings_of_commands = split_string(line, "&");

    for(size_t i = 0; i < strings_of_commands.len - 1; i++){ // - 1 to avoid the null termination at the end of the list
        char * cleaned_command = add_space_to_redirectors(strings_of_commands.strings[i]);
        split_arr command = split_string(cleaned_command, " \t\n");

        if (result.len >= result.cap){
            result.cap *= 2;
            result.commands = reallocf(result.commands, sizeof(split_arr*) * result.cap);
            if (result.commands == NULL){
                fwrite(error_message, strlen(error_message), 1, stderr); 
                exit(1);
            }
        }

        result.commands[result.len] = command;
        result.len++;
    }

    return result;
}

int handle_user_command(char* args[static 1]){
    char* path = find_path(args[0]);    

    if (path == NULL) {
        fwrite(error_message, strlen(error_message), 1, stderr); 
        return -1;
    }

    args[0] = path;  

    int pid = fork();

    if (pid == 0) {
        // redirect output if necessary 
        size_t current_index = 0;
        char* redirect_file_name = NULL;

        while (args[current_index] != NULL){
            if (strcmp(args[current_index], ">") != 0) {
                current_index++;
                continue;
            }

            if (!args[current_index + 1] || args[current_index + 2] != NULL){
                fwrite(error_message, strlen(error_message), 1, stderr); 
                return -1;
            }

            redirect_file_name = args[current_index + 1];
            args[current_index] = NULL;
            break;
        }

        if (redirect_file_name) {
            int fd = open(redirect_file_name,
                        O_WRONLY | O_CREAT | O_TRUNC,
                        0644);
            if (fd < 0){
                fwrite(error_message, strlen(error_message), 1, stderr); 
                exit(1);
            }

            dup2(fd, STDOUT_FILENO);
            dup2(fd, STDERR_FILENO);
            close(fd);
        }

        int result = execv(args[0], args);
        if (result < 0) {
            fwrite(error_message, strlen(error_message), 1, stderr); 
        }
        // free(path);
        return 0;
    }

    else {
        // free(path);
        return pid;
    }
}

void handle_built_in_command(char* args[static 1], size_t args_len) {
    if (
        (strcmp(args[0], "exit") == 0 && args_len != 2) || \
        (strcmp(args[0], "cd") == 0 && args_len != 3)
    ) {
        // parsing logic adds a NULL entry to the end of the array
        fwrite(error_message, strlen(error_message), 1, stderr); 
        return;
    }

    if (strcmp(args[0], "exit") == 0) {
        exit(0);
    }

    else if (strcmp(args[0], "cd") == 0) {
        // doesn't seem to be working with ~
        int result = chdir(args[1]);
        if (result < 0){
            fwrite(error_message, strlen(error_message), 1, stderr); 
        }
    }

    else if (strcmp(args[0], "path") == 0) {
        path_len = args_len - 2;
        free(paths);

        paths = malloc(sizeof(char *) * path_len);
        if (paths == NULL) {
            fwrite(error_message, strlen(error_message), 1, stderr); 
            exit(1);
        }
        for(size_t i = 1; i < args_len - 1; i++){
            paths[i - 1] = strdup(args[i]);
        }
    }
}

int main(int argc, char* argv[]){
    if(argc > 2) {
        fwrite(error_message, strlen(error_message), 1, stderr); 
        exit(1);
    }

    bool is_interactive_mode = argc == 1;
    FILE * fp = NULL;
    if (!is_interactive_mode) {
        fp = fopen(argv[1], "r");

        if (fp == NULL) {
            fwrite(error_message, strlen(error_message), 1, stderr); 
            exit(1);
        }
    }

    path_len = 1;
    paths = malloc(sizeof(char *) * path_len);
    if (paths == NULL) {
        fwrite(error_message, strlen(error_message), 1, stderr); 
        exit(1);
    }
    paths[0] = "/bin";

    char* line = NULL;
    size_t line_cap = 0;
    ssize_t linelen;

    if (is_interactive_mode) {
         printf("wish> ");
    }

    while ((linelen = getline(&line, &line_cap, is_interactive_mode ? stdin : fp)) > 0) {
        command_arr command_list = split_args_into_commands(line);
        int* cpids = malloc(sizeof(int *) * command_list.len);
        size_t num_child_processes = 0;

        for (size_t i = 0; i < command_list.len; i++) {
            split_arr current_command = command_list.commands[i];

            if(is_built_in_command(current_command.strings[0])) {
                handle_built_in_command(current_command.strings, current_command.len);
            } else {
                int cpid = handle_user_command(current_command.strings);
                if (cpid > 0) {
                    cpids[num_child_processes] = cpid;
                    num_child_processes++;
                }
            }
        }

        for (size_t i = 0; i < num_child_processes; i++) {
            int stat;
            int cpid = waitpid(cpids[i], &stat, WUNTRACED);
            if (cpid < 0) {
                fwrite(error_message, strlen(error_message), 1, stderr); 
            }
        }

        if (is_interactive_mode) {
            printf("wish> ");
        }

        free(command_list.commands);
    }

    return 0;
}