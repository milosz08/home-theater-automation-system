#include "helper.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "esp_log.h"

// private api ---------------------------------------------------------------------------------------------------------

static const char *TAG = "HELPER";

// public api ----------------------------------------------------------------------------------------------------------

char *helper_read_file_to_buffer(const char *path)
{
  FILE *f = fopen(path, "r");
  if (f == NULL)
  {
    ESP_LOGE(TAG, "failed to open file: %s", path);
    return NULL;
  }

  fseek(f, 0, SEEK_END);
  long length = ftell(f);
  fseek(f, 0, SEEK_SET);

  if (length <= 0)
  {
    fclose(f);
    return NULL;
  }

  char *buffer = (char *)malloc(length + 1);
  if (buffer == NULL)
  {
    ESP_LOGE(TAG, "failed to allocate memory for file buffer");
    fclose(f);
    return NULL;
  }

  fread(buffer, 1, length, f);
  buffer[length] = '\0'; // null-terminator

  fclose(f);
  return buffer;
}

int helper_secure_memcmp(const void *s1, const void *s2, size_t n)
{
  const unsigned char *p1 = s1,
                      *p2 = s2;
  int ret = 0;
  for (size_t i = 0; i < n; i++) ret |= p1[i] ^ p2[i];
  return ret;
}

void helper_format_uptime(uint32_t total_seconds, char *dest, size_t max_len)
{
  uint32_t days = total_seconds / 86400;
  uint32_t hours = (total_seconds % 86400) / 3600;
  uint32_t minutes = (total_seconds % 3600) / 60;
  snprintf(dest, max_len, "%02u:%02u:%02u",
           (unsigned int)days,
           (unsigned int)hours,
           (unsigned int)minutes);
}
