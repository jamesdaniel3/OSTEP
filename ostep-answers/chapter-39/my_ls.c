/*
Write a program that lists files in the given directory.
When called without any arguments, the program should just print
the file names. When invoked with the -l flag, the program should
print out information about each file, such as the owner, group, 
permissions, and other information obtained from the stat() system
call. The program should take one additional argument, which is
the directory to read, e.g., myls -l directory. If no directory is
given, the program should just use the current working directory.
Useful interfaces: stat(), opendir(), readdir(), getcwd().
*/

#include <assert.h>
#include <stdbool.h>
#include <dirent.h>
#include <grp.h>
#include <pwd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>

#define _DARWIN_FEATURE_64_BIT_INODE 1 // this gets us the birth time of the file 
#define TIME_STR_MAX_LEN 25

void print_file_info(char* directory_path, char* file_name){
    struct stat result;

    //init filepath
    char* filepath = calloc(strlen(directory_path) + strlen(file_name) + 2, 1);
    memcpy(filepath, directory_path, strlen(directory_path));
    filepath[strlen(directory_path)] = '/';
    memcpy(filepath + strlen(directory_path) + 1, file_name, strlen(file_name));

    int lstat_ret = lstat(filepath, &result);
    assert(lstat_ret == 0);

    int file_type_masks[2] = {0120000, 0040000};
    char file_type_options[2] = {'l', 'd'};
    int mode_masks[3] = {0400, 0200, 0100};
    char full_mode_str[11] = "-rwxrwxrwx";
    char mode_str[11] = "----------";

    // I think there are actually like 5-7 options but I won't be finding more of them right now
    for (int i = 0; i < 2; i++){
        if (result.st_mode & file_type_masks[i]){
            mode_str[0] = file_type_options[i];
        }
    }

    for (int i = 0; i < 9; i++){
        int shift = (i / 3);
        int mask = mode_masks[i % 3] >> (shift * 3);
        if (result.st_mode & mask) {
            mode_str[i + 1] = full_mode_str[i + 1];
        }
    }

    mode_str[10] = '\0';

    struct passwd* user_info = getpwuid(result.st_uid);
    struct group* group_info = getgrgid(result.st_gid);

    char last_access_string[TIME_STR_MAX_LEN];
    char last_modification_string[TIME_STR_MAX_LEN];
    char last_status_change_string[TIME_STR_MAX_LEN];
    char creation_string[TIME_STR_MAX_LEN];

    int acc_str_ret = strftime(last_access_string, TIME_STR_MAX_LEN, "\"%b %e %H:%M:%S %G\"", localtime(&result.st_atimespec.tv_sec)); 
    int mod_str_ret = strftime(last_modification_string, TIME_STR_MAX_LEN, "\"%b %e %H:%M:%S %G\"", localtime(&result.st_mtimespec.tv_sec)); 
    int stat_str_ret = strftime(last_status_change_string, TIME_STR_MAX_LEN, "\"%b %e %H:%M:%S %G\"", localtime(&result.st_ctimespec.tv_sec)); 
    int creat_str_ret = strftime(creation_string, TIME_STR_MAX_LEN, "\"%b %e %H:%M:%S %G\"", localtime(&result.st_birthtimespec.tv_sec)); 
    assert(acc_str_ret != 0 && mod_str_ret != 0 && stat_str_ret != 0 && creat_str_ret != 0);

    printf("%d ", result.st_dev);
    printf("%llu ", result.st_ino);
    printf("%s ", mode_str);      
    printf("%d ", result.st_nlink);
    printf("%s ", user_info->pw_name);
    printf("%s ", group_info->gr_name);
    printf("%d ", result.st_rdev);
    printf("%lld ", result.st_size);
    printf("%s ", last_access_string);
    printf("%s ", last_modification_string);
    printf("%s ", last_status_change_string);
    printf("%s ", creation_string);
    printf("%d ", result.st_blksize);
    printf("%lld ", result.st_blocks);
    printf("%d ", result.st_flags);
    printf("%s\n", file_name);
}

int main(int argc, char** argv){
    assert(argc <= 3);
    char* directory = NULL;
    bool is_long_mode = argc >= 2 && strcmp(argv[1], "-l") == 0;
    bool directory_allocated = false;
    if (argc == 1 || (is_long_mode && argc == 2)){
        directory = getcwd(NULL, 0); // size is ignored b/c directory is NULL
        assert(directory != NULL);
        directory_allocated = true;
    }
    else {
        if (is_long_mode && argc == 3){
            directory = argv[2];
        }
        else {
            directory = argv[1];
        }
    }

    DIR* dir_ptr = opendir(directory);
    assert(dir_ptr != NULL);

    struct dirent* next_dirent_ptr = NULL;

    while((next_dirent_ptr = readdir(dir_ptr)) != NULL){
        if (
            strcmp(next_dirent_ptr->d_name, ".") == 0 || 
            strcmp(next_dirent_ptr->d_name, "..") == 0
        ){
            // We need to skip . and .. entries if we are mimmicking ls
            continue;
        }
        if (is_long_mode){
            print_file_info(directory, next_dirent_ptr->d_name); 
        }
        else{
            printf("%s\n", next_dirent_ptr->d_name);
        }
    }

    closedir(dir_ptr);
    if (directory_allocated){
        free(directory);
    }
}