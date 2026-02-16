#ifndef HELPER_H_
#define HELPER_H_

#include <stddef.h>
#include <stdint.h>

/*! \brief Handy macro to get the number of elements in a static array.
 *
 * This macro will not work correctly with pointers, only with static arrays.
 *
 * \param arr The array to measure.
 *
 * \return The number of elements in the array.
 */
#ifndef ARRAY_SIZE
#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))
#endif

/*! \brief Reads an entire file into a heap-allocated buffer.
 *
 * Opens the file, checks its size, mallocs memory, reads it, and closes the file. Warning: caller is responsible for
 * `free()`-ing the returned buffer.
 *
 * \param path Path to the file (e.g., "/spiffs/cert.pem").
 *
 * \return Pointer to the char buffer or NULL if something failed.
 */
char *helper_read_file_to_buffer(const char *path);

/*! \brief Time-constant memory comparison.
 *
 * Used for verifying passwords/tokens to prevent timing attacks. It always takes the same amount of time regardless of
 * where the mismatch occurs.
 *
 * \param s1  First buffer.
 * \param s2  Second buffer.
 * \param n   Number of bytes to compare.
 *
 * \return 0 if identical, non-zero otherwise.
 */
int helper_secure_memcmp(const void *s1, const void *s2, size_t n);

/*! \brief Formats seconds into a readable uptime string.
 *
 * Format: `DDD:HH:MM`.
 *
 * \param total_seconds Raw uptime in seconds.
 * \param[out] dest     Output buffer.
 * \param max_len       Size of the output buffer.
 */
void helper_format_uptime(uint32_t total_seconds, char *dest, size_t max_len);

#endif // HELPER_H_
