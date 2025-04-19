#define _GNU_SOURCE
#include "metadata.h"
#include <stdio.h>
#include <fcntl.h>
#include <linux/fs.h>
#include <linux/stat.h>
#include <sys/stat.h>
#include <sys/syscall.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <time.h>

void get_all_times(const char *filepath) {
    struct statx stx;
    int ret = syscall(SYS_statx, AT_FDCWD, filepath,
                      AT_STATX_SYNC_AS_STAT, STATX_ALL, &stx);
    if (ret < 0) {
        perror("statx");
        return;
    }

    printf("File: %s\n", filepath);
    printf("Access Time : %lld\n", (long long)stx.stx_atime.tv_sec);
    printf("Modify Time : %lld\n", (long long)stx.stx_mtime.tv_sec);
    printf("Change Time : %lld\n", (long long)stx.stx_ctime.tv_sec);
    printf("Birth Time  : %lld\n", (long long)stx.stx_btime.tv_sec);
}

bool check_immutable(const char *filepath) {
    int fd = open(filepath, O_RDONLY);
    if (fd < 0) {
        perror("open");
        return false;
    }

    int flags;
    if (ioctl(fd, FS_IOC_GETFLAGS, &flags) < 0) {
        perror("ioctl FS_IOC_GETFLAGS");
        close(fd);
        return false;
    }

    close(fd);
    return (flags & FS_IMMUTABLE_FL);
}

