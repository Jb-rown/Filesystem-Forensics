#ifndef SCANNER_H
#define SCANNER_H

#include <stdint.h>  

void scan_raw(const char *device_path);
void find_deleted(const char *device_path);
void reconstruct_file(const char *device_path, uint32_t inode_num, const char *output_path);

#endif
