#ifndef NTP_SERVICE_H_
#define NTP_SERVICE_H_

#include <stdbool.h>
#include <stddef.h>
#include <time.h>

#include "esp_err.h"

#define NTP_SYNC_TIMEOUT_MS 10000 /*!< Maximum time to wait for NTP synchronization (in milliseconds). */

/*! \brief Initializes the SNTP service and attempts to synchronize time.
 *
 * This function configures the NTP server and timezone, initializes the SNTP module, and blocks execution until time is
 * synchronized or the timeout occurs.
 *
 * \param server    NTP server address (e.g., "pool.ntp.org").
 * \param timezone  POSIX timezone string (e.g., "CET-1CEST,M3.5.0,M10.5.0/3").
 *
 * \retval ESP_OK               Time synchronized successfully.
 * \retval ESP_ERR_TIMEOUT      Failed to sync within `NTP_SYNC_TIMEOUT_MS`.
 * \retval ESP_ERR_INVALID_ARG  If server or timezone arguments are NULL.
 */
esp_err_t ntp_service_init(const char *server, const char *timezone);

/*! \brief Fills buffers with formatted date and time strings.
 *
 * Helper function to get human-readable date and time. It checks if the system time is valid (year > 2020).
 *
 * \param date_buffer Target buffer for the date string (e.g., "Mon 18.05.2024"). Can be NULL if only time is needed.
 * \param date_len    Size of the date buffer.
 * \param time_buffer Target buffer for the time string (e.g., "14:05:30"). Can be NULL if only date is needed.
 * \param time_len    Size of the time buffer.
 *
 * \retval true   If the system time is synchronized/valid.
 * \retval false  If the time is not synced yet (defaults to epoch 1970).
 */
bool ntp_service_get_time_and_date_strs(char *date_buffer, size_t date_len, char *time_buffer, size_t time_len);

#endif // NTP_SERVICE_H_
