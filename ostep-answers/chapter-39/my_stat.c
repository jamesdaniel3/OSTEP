/*
Write your own version of the command line program stat, which simply calls the stat() system call on a given file or directory. 
Print out file size, number of blocks allocated, reference(link) count, and so forth. 
What is the link count of a directory, asthe number of entries in the directory changes? 
*/

#include <assert.h>
#include <errno.h>
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

    struct passwd* user_info = getpwuid(result.st_uid);
    struct group* group_info = getgrgid(result.st_gid);

    char last_access_string[TIME_STR_MAX_LEN];
    char last_modification_string[TIME_STR_MAX_LEN];
    char last_status_change_string[TIME_STR_MAX_LEN];
    char creation_string[TIME_STR_MAX_LEN];

    // this doesn't work; need to create a struct tm object from the struct timespec object to pass to strftime
    int las_ret = strftime(last_access_string, TIME_STR_MAX_LEN, "%b %d %I:%M:%S %G", &result.st_atimespec);
    assert(las_ret != 0);


    // <st_dev, std_ino, st_mode, st_nlink, st_uid, st_gid, st_rdev, st_size, st_atime, st_mtime, st_ctime, st_birthtime, st_blksize, st_block, st_flags NAME>
    printf("%d ", result.st_dev);
    printf("%llu ", result.st_ino);
    printf("%o ", result.st_mode);      // going to need to convert mode to the rwx display
    printf("%d ", result.st_nlink);
    printf("%s ", user_info->pw_name);
    printf("%s ", group_info->gr_name);
    printf("%d ", result.st_rdev);
    printf("%lld", result.st_size);
    printf("%s\n", last_access_string);
}