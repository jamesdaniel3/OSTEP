/*
Assignment:
Write your own version of the command line program stat, which simply calls the stat() system call on a given file or directory. 
Print out file size, number of blocks allocated, reference(link) count, and so forth. 
What is the link count of a directory, as the number of entries in the directory changes? 

Answers:
The link count of a directory is always the number of files (including symbolic links) that have been added to it +2 for . and .. entries.
Below is a simple version of the stat program. I used lstat because I read in the man pages that it uses lstat by default. 
*/

#include <assert.h>
#include <grp.h>
#include <pwd.h>
#include <stdio.h>
#include <sys/stat.h>
#include <time.h>

#define _DARWIN_FEATURE_64_BIT_INODE 1 // this gets us the birth time of the file 
#define TIME_STR_MAX_LEN 25

int main (int argc, char ** argv){
    assert(argc == 2);

    struct stat result;
    int lstat_ret = lstat(argv[1], &result);
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
    printf("%s\n", argv[1]);
}