#ifndef METADATA_H
#define METADATA_H

#include <stdbool.h>

void get_all_times(const char *filepath);
bool check_immutable(const char *filepath);
void scan_raw(const char *device_path);       
void find_deleted(const char *device_path);   

#endif
