#include <assert.h>
#include <fcntl.h>
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>
#include "mapreduce.h"

#define STANDARD_FILE_BYTES (16 * 1000 * 1000)
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

void MR_Run(
    int argc, char *argv[], 
    Mapper map, int num_mappers, 
    Reducer reduce, int num_reducers, 
    Partitioner partition
){
    assert(argc >= 2);
    char** args;
    size_t num_args = 0;
    for (int i = 1; i < argc; i++){
        int read_fd = open(argv[argc], O_RDONLY);
        assert(read_fd != -1);

        struct stat sb;
        fstat(read_fd, &sb);

        int new_files = sb.st_size / STANDARD_FILE_BYTES;
        if (sb.st_size % STANDARD_FILE_BYTES > 0){
            new_files++;
        }
        num_args += new_files;

        args = realloc(args, num_args * sizeof(char*));
        assert(args != NULL);

        char file_buffer[STANDARD_FILE_BYTES];
        for(int j = 0; j < new_files; j++){
            char* file_name = malloc(TEMP_FILE_NAME_SIZE);
            assert(file_name != NULL);
            // if we want to later, we could check the number of bytes written and resize the name array
            int name_bytes_written = snprintf(file_name, TEMP_FILE_NAME_SIZE, "temp/temp_%d_%d.txt", i, j); 
            assert(name_bytes_written >= 0);

            int bytes_read = read(read_fd, file_buffer, STANDARD_FILE_BYTES);
            assert(bytes_read != -1);

            // this should maybe have another flag for perms
            int write_fd = open(file_name, O_WRONLY | O_CREAT); 
            assert(write_fd != -1);

            int bytes_written = write(write_fd, file_buffer, bytes_read);
            assert(bytes_written != -1);

            args[num_args - (new_files - j)] = file_name;
        }
    }


    // create a thread pool containing num_mappers threads
    pthread_t* mapper_thread_pool = malloc(sizeof(pthread_t) * num_mappers);
    assert(mapper_thread_pool != NULL);

    // start assigning tasks to threads


    // create a thread pool containing num_reducers threads
    // start reading from the big concurrent data structure 

}

