/*
Write a program that prints out the last few lines of a file. The
program should be efficient, in that it seeks to near the end of the
file, reads in a block of data, and then goes backwards until it finds
the requested number of lines; at this point, it should print out those
lines from beginning to the end of the file. To invoke the program,
one should type: mytail -n file, where n is the number of lines
at the end of the file to print. Useful interfaces: stat(), lseek(),
open(), read(), close().
*/

#include <assert.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#define MIN(X, Y) (((X) < (Y)) ? (X) : (Y))

long parse_lines_from_buffer(char* buffer, ssize_t buffer_size, char** results, long results_size){
    long lines_found = 0;
    size_t next_line_start = 0;
    for(ssize_t i = 0; i <= buffer_size; i++){
        if (buffer[i] == '\n' || buffer[i] == '\0'){
            char* new_line = calloc(i - next_line_start + 1, 1);
            assert(new_line != NULL);
            memcpy(new_line, buffer + next_line_start, i - next_line_start + 1);
            if (buffer[i] == '\0'){
                new_line[i - next_line_start] = '\n';
            }

            size_t insertion_index = lines_found % results_size;
            if (lines_found >= results_size) {
                free(results[insertion_index]);
            }
            results[insertion_index] = new_line;
            lines_found++;
            next_line_start = i + 1;
        }
    }

    return lines_found;
}

int main(int argc, char** argv){
    assert(argc == 4);

    // could ofc have much better error handling here
    long num_lines = strtol(argv[2], NULL, 10);
    assert(num_lines > 0);

    struct stat file_info;
    int ret = lstat(argv[3], &file_info);  // gives files size in bytes and blocks alllocated 
    assert(ret == 0);

    int fd = open(argv[3], O_RDONLY);
    assert(fd >= 0);

    char** result_lines = malloc(sizeof(char*) * num_lines);
    assert(result_lines != NULL);

    long lines_found = 0;
    size_t multiplier = 1;
    char* buffer = NULL;
    for(;;) {
        off_t desired_offset = 0; // estimating 100 bytes per line 
        if (file_info.st_size > (long long)(num_lines * 100 * multiplier)){
            desired_offset = file_info.st_size - (num_lines * 100 * multiplier);
        }

        off_t bytes_offset = lseek(fd, desired_offset, SEEK_SET);
        assert(bytes_offset != -1);

        ssize_t buffer_size = MIN(file_info.st_size, (long long)(num_lines * 100 * multiplier));
        buffer = realloc(buffer, buffer_size);
        assert(buffer != NULL);

        ssize_t bytes_read = read(fd, buffer, buffer_size);
        assert(bytes_read == buffer_size);

        lines_found = parse_lines_from_buffer(buffer, buffer_size, result_lines, num_lines);

        if (lines_found <= num_lines && desired_offset != 0){
            lines_found = 0;
            multiplier++;
            continue;
        }
        
        break;
    }

    size_t starting_index = 0;
    if (lines_found >= num_lines){
        starting_index = lines_found % num_lines;
    }
    for(long i = 0; i < num_lines; i++){
        if (i >= lines_found){
            break;
        }

        printf("%s", result_lines[(starting_index + i) % num_lines]);
        free(result_lines[(starting_index + i) % num_lines]);
    }

    free(result_lines);
}