#define _GNU_SOURCE
#include <fcntl.h>
#include <sys/syscall.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <linux/dirent.h>
#include <errno.h>
#include <stdint.h>  //For uint32_t

#include "metadata.h"
#include "recovery.h"


#define BUF_SIZE 1024 * 8

struct linux_dirent64 {
    ino64_t        d_ino;
    off64_t        d_off;
    unsigned short d_reclen;
    unsigned char  d_type;
    char           d_name[];
};

void list_dir(const char *path) {
    int dirfd = openat(AT_FDCWD, path, O_RDONLY | O_DIRECTORY);
    if (dirfd < 0) {
        perror("openat");
        return;
    }

    char buf[BUF_SIZE];
    int nread;

    while ((nread = syscall(SYS_getdents64, dirfd, buf, BUF_SIZE)) > 0) {
        int bpos = 0;
        while (bpos < nread) {
            struct linux_dirent64 *d = (struct linux_dirent64 *)(buf + bpos);
            printf("Found: %s\n", d->d_name);
            bpos += d->d_reclen;
        }
    }

    if (nread == -1)
        perror("getdents64");

    close(dirfd);
}

int main(int argc, char *argv[]) {
    if (argc == 3 && strcmp(argv[1], "--scan") == 0) {
        scan_raw(argv[2]);  
        return 0;
    } else if (argc == 3 && strcmp(argv[1], "--deleted") == 0) {
        find_deleted(argv[2]);  
        return 0;
    } else if (argc == 5 && strcmp(argv[1], "--recover") == 0) {
        uint32_t inode_no = (uint32_t)atoi(argv[3]);
        recover_deleted_file(argv[2], inode_no, argv[4]);  // From recovery.h
        return 0;
    }
    
    if (argc < 2) {
        fprintf(stderr, "Usage:\n");
        fprintf(stderr, "  %s <directory>\n", argv[0]);
        fprintf(stderr, "  %s --scan <device>\n", argv[0]);
        fprintf(stderr, "  %s --deleted <device>\n", argv[0]);
        fprintf(stderr, "  %s --recover <device> <inode_no> <output_path>\n", argv[0]);
        return 1;
    }
    
    const char *path = argv[1];
    
    get_all_times(path);  // From metadata.h

    if (check_immutable(path))  // From metadata.h
        printf("File is IMMUTABLE.\n");
    else
        printf("File is NOT immutable.\n");

    list_dir(path);
    return 0;
}
