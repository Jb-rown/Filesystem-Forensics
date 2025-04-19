#include "recovery.h"
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#include "scanner.h"
// Reconstruct the file from the inode and save it to the specified path.
void recover_deleted_file(const char *device_path, uint32_t inode_num, const char *output_path) {
    printf("[+] Recovering deleted file (Inode: %u) to: %s\n", inode_num, output_path);
    reconstruct_file(device_path, inode_num, output_path);
}
