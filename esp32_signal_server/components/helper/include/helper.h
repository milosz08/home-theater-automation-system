#ifndef HELPER_H_
#define HELPER_H_

#include <stddef.h>
#include <stdint.h>

char *helper_read_file_to_buffer(const char *path);
int helper_secure_memcmp(const void *s1, const void *s2, size_t n);
void helper_format_uptime(uint32_t total_seconds, char *dest, size_t max_len); // seconds to DDd:HH:MM

#endif
