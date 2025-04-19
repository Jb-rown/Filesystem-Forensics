#include "scanner.h"
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <time.h>

#define BLOCK_SIZE 4096
#define READ_BLOCKS 1024  // Scan 4MB at a time
#define EXT4_SUPERBLOCK_OFFSET 1024
#define EXT4_SUPERBLOCK_SIZE 1024
#define INODE_SIZE 256  // default for modern ext4
#define INODES_PER_GROUP 8192
#define BLOCK_GROUP_DESC_SIZE 64

// struct to hold deleted inode info
typedef struct {
    uint32_t inode_no;
    uint32_t deletion_time;
} DeletedInode;

void find_deleted(const char *device_path) {
    int fd = open(device_path, O_RDONLY);
    if (fd < 0) {
        perror("open device");
        return;
    }

    uint8_t superblock[EXT4_SUPERBLOCK_SIZE];
    if (pread(fd, superblock, EXT4_SUPERBLOCK_SIZE, EXT4_SUPERBLOCK_OFFSET) != EXT4_SUPERBLOCK_SIZE) {
        perror("read superblock");
        close(fd);
        return;
    }

    // Get first inode table block (simplified: assumes group 0)
    uint32_t inode_table_block = *(uint32_t *)(superblock + 0x100 + 0x8); // offset 0x108
    uint64_t inode_table_offset = inode_table_block * 4096;

    for (uint32_t i = 0; i < INODES_PER_GROUP; i++) {
        uint64_t inode_offset = inode_table_offset + (i * INODE_SIZE);
        uint8_t inode[INODE_SIZE];

        if (pread(fd, inode, INODE_SIZE, inode_offset) != INODE_SIZE) {
            perror("read inode");
            break;
        }

        uint32_t dtime = *(uint32_t *)(inode + 0x60);  // i_dtime offset
        if (dtime != 0) {
            time_t deletion_time = (time_t)dtime;
            printf("[Deleted] Inode #%u | Deleted At: %s", i + 1, ctime(&deletion_time));
        }
    }

    close(fd);
}

void reconstruct_file(const char *device_path, uint32_t inode_num, const char *output_path) {
    int fd = open(device_path, O_RDONLY);
    if (fd < 0) {
        perror("open device");
        return;
    }

    FILE *out = fopen(output_path, "wb");
    if (!out) {
        perror("fopen output");
        close(fd);
        return;
    }

    // Read superblock
    uint8_t superblock[EXT4_SUPERBLOCK_SIZE];
    if (pread(fd, superblock, EXT4_SUPERBLOCK_SIZE, EXT4_SUPERBLOCK_OFFSET) != EXT4_SUPERBLOCK_SIZE) {
        perror("read superblock");
        goto cleanup;
    }

    uint32_t inode_table_block = *(uint32_t *)(superblock + 0x100 + 0x8);  // first inode table block (group 0)
    uint64_t inode_table_offset = inode_table_block * 4096;
    uint32_t inode_index = inode_num - 1;  // 1-based

    uint64_t inode_offset = inode_table_offset + inode_index * INODE_SIZE;
    uint8_t inode[INODE_SIZE];

    if (pread(fd, inode, INODE_SIZE, inode_offset) != INODE_SIZE) {
        perror("read inode");
        goto cleanup;
    }

    uint32_t file_size = *(uint32_t *)(inode + 0x4);  // i_size
    uint32_t *block_ptrs = (uint32_t *)(inode + 0x28);  // i_block (first 12 direct blocks)

    size_t bytes_read = 0;
    for (int i = 0; i < 12; i++) {
        if (block_ptrs[i] == 0 || bytes_read >= file_size)
            break;

        uint8_t buf[4096];
        uint64_t block_offset = (uint64_t)block_ptrs[i] * 4096;
        ssize_t n = pread(fd, buf, 4096, block_offset);
        if (n <= 0) {
            perror("pread block");
            break;
        }

        size_t to_write = (file_size - bytes_read > 4096) ? 4096 : (file_size - bytes_read);
        fwrite(buf, 1, to_write, out);
        bytes_read += to_write;
    }

    printf("[+] Reconstructed %zu bytes to %s\n", bytes_read, output_path);

cleanup:
    fclose(out);
    close(fd);
}


void scan_raw(const char *device_path) {
    int fd = open(device_path, O_RDONLY);
    if (fd < 0) {
        perror("open device");
        return;
    }

    uint8_t *buf = malloc(BLOCK_SIZE * READ_BLOCKS);
    if (!buf) {
        perror("malloc");
        close(fd);
        return;
    }

    off_t offset = 0;
    ssize_t bytes;
    while ((bytes = pread(fd, buf, BLOCK_SIZE * READ_BLOCKS, offset)) > 0) {
        for (ssize_t i = 0; i < bytes - 4; i++) {
            // Example: look for ext4 inode magic number (0xEF53 at offset 0x38 in superblock)
            if (buf[i] == 0x53 && buf[i + 1] == 0xEF) {
                printf("[+] Found ext4 signature at offset: %lld + %ld = %lld\n",
                       (long long)offset, i, (long long)(offset + i));
            }
        }
        offset += bytes;
    }

    if (bytes < 0)
        perror("pread");

    free(buf);
    close(fd);
}

