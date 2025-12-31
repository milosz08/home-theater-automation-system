#include "util.h"
#include "ui.h"

int util_secure_memcmp(const void *s1, const void *s2, size_t n) {
  const unsigned char *p1 = s1, *p2 = s2;
  int ret = 0;
  for (size_t i = 0; i < n; i++) {
    ret |= p1[i] ^ p2[i];
  }
  return ret;
}

esp_err_t util_silent_error_handler(httpd_req_t *req, httpd_err_code_t err)
{
  switch (err) {
    // 4xx
    case HTTPD_400_BAD_REQUEST:               httpd_resp_set_status(req, "400 Bad Request"); break;
    case HTTPD_401_UNAUTHORIZED:              httpd_resp_set_status(req, "401 Unauthorized"); break;
    case HTTPD_403_FORBIDDEN:                 httpd_resp_set_status(req, "403 Forbidden"); break;
    case HTTPD_404_NOT_FOUND:                 httpd_resp_set_status(req, "404 Not Found"); break;
    case HTTPD_405_METHOD_NOT_ALLOWED:        httpd_resp_set_status(req, "405 Method Not Allowed"); break;
    case HTTPD_408_REQ_TIMEOUT:               httpd_resp_set_status(req, "408 Request Timeout"); break;
    case HTTPD_411_LENGTH_REQUIRED:           httpd_resp_set_status(req, "411 Length Required"); break;
    case HTTPD_414_URI_TOO_LONG:              httpd_resp_set_status(req, "414 URI Too Long"); break;
    case HTTPD_431_REQ_HDR_FIELDS_TOO_LARGE:  httpd_resp_set_status(req, "431 Request Header Fields Too Large"); break;
    // 5xx
    case HTTPD_500_INTERNAL_SERVER_ERROR:     httpd_resp_set_status(req, "500 Internal Server Error"); break;
    case HTTPD_501_METHOD_NOT_IMPLEMENTED:    httpd_resp_set_status(req, "501 Method Not Implemented"); break;
    case HTTPD_505_VERSION_NOT_SUPPORTED:     httpd_resp_set_status(req, "505 HTTP Version Not Supported"); break;
    default:                                  httpd_resp_set_status(req, "500 Internal Server Error"); break;
  }
  return httpd_resp_send(req, NULL, 0);
}

esp_err_t util_silent_response(httpd_req_t *req)
{
  httpd_resp_set_status(req, "204 No Content");
  return httpd_resp_send(req, NULL, 0);
}

void util_show_network_connection(void)
{
  esp_netif_ip_info_t ip_info;
  memset(&ip_info, 0, sizeof(esp_netif_ip_info_t));
  if (netif != NULL)
  {
    esp_netif_get_ip_info(netif, &ip_info);
  }
  uint8_t mac[6] = {0};
  esp_read_mac(mac, ESP_MAC_ETH);
  ui_show_network_dashboard(&ip_info, mac);
}
