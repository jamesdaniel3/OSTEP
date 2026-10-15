#include <assert.h>
#include <fcntl.h>
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>
#include "mapreduce.h"

// I want to use 16 MB slices in the future but I don't feel like getting that 
    // much txt right now 
#define STANDARD_FILE_BYTES (1 * 1000 * 1000) 
#define TEMP_FILE_NAME_SIZE 50

void MR_Emit(char *key, char *value){
  // store the key value pairs in a way that lets the reducers access them (some big concurrent data structure)
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
    char** results, size_t* num_results
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
        *num_results += new_files;

        results = realloc(results, *num_results * sizeof(char*));
        assert(results != NULL);

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

            results[*num_results - (new_files - j)] = file_name;
        }
    }
}

void MR_Run(
    int argc, char *argv[], 
    Mapper map, int num_mappers, 
    Reducer reduce, int num_reducers, 
    Partitioner partition
){
    char** thread_args = NULL;
    size_t num_thread_args = 0;

    create_files_to_map(argc - 1, argv + 1, thread_args, &num_thread_args);


    // create a thread pool containing num_mappers threads
    pthread_t* mapper_thread_pool = malloc(sizeof(pthread_t) * num_mappers);
    assert(mapper_thread_pool != NULL);

    // start assigning tasks to threads


    // create a thread pool containing num_reducers threads
    // start reading from the big concurrent data structure 

}

