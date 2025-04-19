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