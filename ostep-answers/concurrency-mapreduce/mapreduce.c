#include <assert.h>
#include <fcntl.h>
#include <pthread.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include "mapreduce.h"

// I want to use 16 MB slices in the future but I don't feel like getting that 
    // much txt right now 
#define STANDARD_FILE_BYTES (1 * 1000 * 1000) 
#define TEMP_FILE_NAME_SIZE 50

// we should start out by locking at the partition level and only move to 
// hand-over-hand locking if we know it will work and improve the result 
typedef struct Node Node;
struct Node {
    char* key;
    size_t key_size;
    char** values;
    size_t value_capacity;
    size_t value_count;
    Node* next;
};

typedef struct PartitionHead PartitionHead;
struct PartitionHead {
    Node* head;
    pthread_mutex_t lock;
};

PartitionHead* partitions;
size_t num_partitions;
Partitioner partition_function;
Mapper map_function;

Node* init_node(char* key, char* value){
    Node* new_node = malloc(sizeof(Node));
    assert(new_node != NULL);
    
    assert(key != NULL);
    new_node->key_size = strlen(key); // excludes /0
    new_node->key = malloc(new_node->key_size + 1);
    assert(new_node->key != NULL);
    strcpy(new_node->key, key);

    assert(value != NULL);
    new_node->value_capacity = 10;
    new_node->values = malloc(sizeof(char*) * 10);
    assert(new_node->values != NULL);
    new_node->values[0] = malloc(strlen(value) + 1);
    assert(new_node->values[0] != NULL);
    strcpy(new_node->values[0], value);
    new_node->value_count = 1;

    return new_node;
}

void destroy_node(Node* node){
    assert(node != NULL);
    for (size_t i = 0; i < node->value_count; i++){
        free(node->values[i]);
    }

    free(node->key);
    free(node);
}

void destroy_partitions(){
    for (size_t i = 0; i < num_partitions; i++){
        PartitionHead current_partition = partitions[i];
        Node* current_node = current_partition.head;

        while (current_node != NULL){
            Node* temp = current_node->next;
            destroy_node(current_node);
            current_node = temp;
        }

        // should check for errors
        pthread_mutex_destroy(&current_partition.lock);
    }
}

Node* locate_key(Node* head, char* key){
    for(;;){
        if (head == NULL){
            break;
        }

        if (strcmp(head->key, key) == 0){
            break;
        }

        head = head->next;
    }

    return head;
}

void MR_Emit(char *key, char *value){
    unsigned long partition_number = partition_function(key, num_partitions);
    PartitionHead partition_head = partitions[partition_number];
    Node* current_node = partition_head.head;
    pthread_mutex_lock(&partition_head.lock);
    Node* key_node = locate_key(current_node, key);

    // I think I can simplify locking below
    if (key_node == NULL){
        // we are creating a new node in the linked list 
        key_node = init_node(key, value);
        
        if(current_node == NULL){ 
            // first key at partition
            partition_head.head = key_node;
            return;
        }
        
        // this lookup should be binary search in the future
        while(
            current_node != NULL && 
            current_node->next != NULL &&
            strcmp(current_node-> key, key) < 0
        ){
            current_node = current_node->next;
        }

        if(current_node->next == NULL){
            // adding key to end of list 
            current_node->next = key_node;
            return;
        }

        // inserting node in list 
       key_node->next = current_node->next;
       current_node->next = key_node;
       return;
    }

    // key node has been found, we are adding to the value list
    if (key_node->value_count >= .8 * key_node->value_capacity){
        key_node->values = realloc(key_node->values, key_node->value_capacity * 2);
        assert(key_node->values != NULL);
        key_node->value_capacity *= 2;
    }

    // I think they may have asked to sort values as well as keys but for now
        // I am just storing values in the order they arrive
    char* new_value = malloc(strlen(value) + 1);
    assert(new_value != NULL);
    strcpy(new_value, value);
    key_node->values[key_node->value_count] = new_value;
    key_node->value_count++;
    pthread_mutex_unlock(&partition_head.lock);
    return;
}

unsigned long MR_DefaultHashPartition(char *key, int num_partitions) {
    unsigned long hash = 5381;
    int c;
    while ((c = *key++) != '\0')
        hash = hash * 33 + c;
    return hash % num_partitions;
}

void create_files_to_map(
    int num_files, char** input_files,
    char*** results_loc, size_t* num_results_loc
){
    assert(num_files >= 1);
    mkdir("temp", 0777);  // I think these group perms just mean everyone has everything

    for (int i = 0; i < num_files; i++){
        int read_fd = open(input_files[i], O_RDONLY);
        assert(read_fd != -1);

        struct stat sb;
        fstat(read_fd, &sb);

        int new_files = sb.st_size / STANDARD_FILE_BYTES;
        if (sb.st_size % STANDARD_FILE_BYTES > 0){
            new_files++;
        }
        *num_results_loc += new_files;

        *results_loc = realloc(*results_loc, *num_results_loc * sizeof(char*));
        assert(*results_loc != NULL);

        char file_buffer[STANDARD_FILE_BYTES / 8];
        for(int j = 0; j < new_files; j++){
            char* file_name = malloc(TEMP_FILE_NAME_SIZE);
            assert(file_name != NULL);
            // if we want to later, we could check the number of bytes written and resize the name array
            int name_bytes_written = snprintf(file_name, TEMP_FILE_NAME_SIZE, "temp/temp_%d_%d.txt", i, j); 
            assert(name_bytes_written >= 0);

            // this should maybe have another flag for perms
            int write_fd = open(file_name, O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR); 
            assert(write_fd != -1);
            
            for (size_t x = 0; x < 8; x++){
                int bytes_read = read(read_fd, file_buffer, STANDARD_FILE_BYTES / 8);
                assert(bytes_read != -1);

                int bytes_written = write(write_fd, file_buffer, bytes_read);
                assert(bytes_written != -1);
            }

            size_t insertion_index = *num_results_loc - (new_files - j);
            (*results_loc)[insertion_index] = file_name;
        }
    }
}

void* map_wrapper(void* arg){
    assert(arg != NULL);
    char* cleaned_arg = (char *)arg;
    map_function(cleaned_arg);
    return NULL;
}

void MR_Run(
    int argc, char *argv[], 
    Mapper map, int num_mappers, 
    Reducer reduce, int num_reducers, 
    Partitioner partition
){
    char** thread_args = NULL;
    size_t num_thread_args = 0;
    num_partitions = num_reducers;
    partition_function = partition;
    map_function = map;

    create_files_to_map(argc - 1, argv + 1, &thread_args, &num_thread_args);

    // create a thread pool containing num_mappers threads
    pthread_t* mapper_thread_pool = malloc(sizeof(pthread_t) * num_mappers);
    assert(mapper_thread_pool != NULL);

    partitions = calloc(num_partitions, sizeof(PartitionHead));
    assert(partitions != NULL);
    for (size_t i = 0; i < num_partitions; i++){
        int rc = pthread_mutex_init(&partitions[i].lock, NULL);
        assert(rc == 0);
    }

    // need error checking for thread functions
    for (int i = 0; i < 1; i++){
        pthread_create(
            mapper_thread_pool + i, 
            NULL, 
            map_wrapper, 
            (void *) thread_args[i]
        );
    }

    for (int i = 0; i < 1; i++){
        pthread_join(mapper_thread_pool[i], NULL);
    }

    // create a thread pool containing num_reducers threads
    // start reading from the big concurrent data structure 
    destroy_partitions();
}

