#ifndef RECOVERY_H
#define RECOVERY_H

#include <stdint.h>  // for uint32_t

void recover_deleted_file(const char *device_path, uint32_t inode_num, const char *output_path);

#endif

