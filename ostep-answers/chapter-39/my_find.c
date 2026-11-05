/*
Recursive Search: Write a program that prints out the names of
each file and directory in the file system tree, starting at a given
point in the tree. For example, when run without arguments, the
program should start with the current working directory and print
its contents, as well as the contents of any sub-directories, etc., until
the entire tree, root at the CWD, is printed. If given a single argument (of a directory name), 
use that as the root of the tree instead.
Refine your recursive search with more fun options, similar to the
powerful find command line tool. Useful interfaces: figure it out.
*/

#include <assert.h>
#include <dirent.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Node Node;
struct Node {
    char* value;
    Node* next;
};

typedef struct Queue Queue;
struct Queue {
    size_t len;
    Node* head;
};

Node* init_node(char* val){
    Node* node = malloc(sizeof(Node));
    assert(node != NULL);
    node->value = calloc(strlen(val) + 1, 1); // need a null terminator
    assert(node->value != NULL);
    memcpy(node->value, val, strlen(val));
    node->next = NULL;

    return node;
}

void destroy_node(Node* node){
    free(node->value);
    free(node);
}

void run_bfs(char* starting_dir){
    Node* head = init_node(starting_dir);
    Queue queue = {
        .len = 1,
        .head = head
    };

    while (queue.len > 0){
        Node* current_node = queue.head;
        queue.head = queue.head->next;
        queue.len--;

        DIR* current_directory = opendir(current_node->value);
        assert(current_directory != NULL);

        struct dirent* next_dirent_ptr;
        while((next_dirent_ptr = readdir(current_directory)) != NULL){
            if(
                strcmp(next_dirent_ptr->d_name, ".") == 0 || 
                strcmp(next_dirent_ptr->d_name, "..") == 0
            ){
                continue;
            }

            if (next_dirent_ptr->d_type & DT_DIR){
                // add a node to the queue
                char* new_dir_path = calloc(strlen(current_node->value) + strlen(next_dirent_ptr->d_name) + 2, 1);
                assert(new_dir_path != NULL);
                memcpy(new_dir_path, current_node->value, strlen(current_node->value));
                new_dir_path[strlen(current_node->value)] = '/';
                memcpy(new_dir_path + strlen(current_node->value) + 1, next_dirent_ptr->d_name, strlen(next_dirent_ptr->d_name));

                Node* new_node = init_node(new_dir_path);
                free(new_dir_path);

                // we could get rid of this if else if we put a dummy node on the queue 
                if (queue.head == NULL){
                    queue.head = new_node;
                }
                else {
                    Node* current_node = queue.head;
                    while(current_node->next != NULL){
                        current_node = current_node->next;
                    }
                    current_node->next = new_node;
                }
                queue.len++;
            }
            else {
                // print everything that's not a directory
                printf("%s/%s\n", current_node->value, next_dirent_ptr->d_name);
            }
        }

        destroy_node(current_node);
    }
}

int main (int argc, char** argv){
    // by default we should BFS through the firectories from each path given 
    char* current_directory_path;
    if (argc == 1){
        current_directory_path = getcwd(NULL, 0); // size is ignored b/c directory is NULL
        assert(current_directory_path != NULL);
        run_bfs(current_directory_path);
        return 0;
    }

    for (int i = 1; i < argc; i++){
        current_directory_path = argv[i];
        DIR* directory = opendir(current_directory_path);
        if (directory == NULL){
            printf("Couldn't read %s\n", current_directory_path);
            continue;
        }
        closedir(directory);
        run_bfs(current_directory_path);
    }
}