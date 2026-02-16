#include "app_api.h"

#include <string.h>
#include <stdio.h>

#include "cJSON.h"
#include "esp_log.h"

// private api ---------------------------------------------------------------------------------------------------------

static const char *TAG = "API";

static const char* resolve_error_name(const api_ctx_t *ctx)
{
  if (ctx->error_name_str == NULL) return esp_err_to_name(ctx->error_code);

  static const char *forbidden_names[] = { API_GENERIC_VAR_NAMES };
  const size_t count = sizeof(forbidden_names) / sizeof(forbidden_names[0]);

  for (size_t i = 0; i < count; i++)
  {
    if (strcmp(ctx->error_name_str, forbidden_names[i]) == 0)
    {
      const char *esp_name = esp_err_to_name(ctx->error_code);
      return (esp_name != NULL) ? esp_name : "UNKNOWN_ERROR";
    }
  }
  return ctx->error_name_str;
}

// public api ----------------------------------------------------------------------------------------------------------

void *app_api_create_ctx(httpd_req_t *req)
{
  api_ctx_t *ctx = malloc(sizeof(api_ctx_t));
  if (ctx)
  {
    ctx->http_status = 204;
    ctx->error_code = ESP_OK;
    ctx->error_name_str = NULL;
    ctx->manual_response = false;
  }
  return (void*)ctx;
}

esp_err_t app_api_handle_response(httpd_req_t *req, void *void_ctx, esp_err_t handler_res)
{
  api_ctx_t *ctx = (api_ctx_t *)void_ctx;

  const https_endpoint_t *ep = (const https_endpoint_t *)req->user_ctx;
  const char *ep_name = (ep && ep->name) ? ep->name : "UNKNOWN";
  if (!ctx)
  {
    ESP_LOGE(TAG, "[%s] critical: context is NULL", ep_name);
    httpd_resp_send_500(req);
    return ESP_FAIL;
  }
  if (handler_res != ESP_OK)
  {
    ctx->http_status = 500;
    ctx->error_code = handler_res;
    ctx->error_name_str = NULL;
  }
  if (!ctx->manual_response)
  {
    if (ctx->http_status == 204 && ctx->error_code == ESP_OK)
    {
      httpd_resp_set_status(req, "204 No Content");
      httpd_resp_send(req, NULL, 0);
    }
    else
    {
      cJSON *root = cJSON_CreateObject();

      char hex_code[16];
      snprintf(hex_code, sizeof(hex_code), "0x%X", ctx->error_code);
      cJSON_AddStringToObject(root, "error_code", hex_code);

      const char *final_name = resolve_error_name(ctx);
      cJSON_AddStringToObject(root, "error_name", final_name);

      const char *json_str = cJSON_PrintUnformatted(root);
      char status_str[8];
      snprintf(status_str, sizeof(status_str), "%d", ctx->http_status);

      httpd_resp_set_status(req, status_str);
      httpd_resp_set_type(req, "application/json");
      httpd_resp_send(req, json_str, strlen(json_str));

      free((void*)json_str);
      cJSON_Delete(root);

      ESP_LOGW(TAG, "[%s] error: %s (%s) -> HTTP %d", ep_name, final_name, hex_code, ctx->http_status);
    }
  }
  esp_err_t result_for_middleware = (handler_res != ESP_OK) ? handler_res : ctx->error_code;
  free(ctx);
  return result_for_middleware;
}

esp_err_t _api_return_err_helper(httpd_req_t *req, int status, esp_err_t code, const char *name_str)
{
  api_ctx_t *ctx = (api_ctx_t *)req->user_ctx;
  if (ctx)
  {
    ctx->http_status = status;
    ctx->error_code = code;
    ctx->error_name_str = name_str;
    ctx->manual_response = false;
  }
  return ESP_OK;
}

esp_err_t api_return_ok(httpd_req_t *req)
{
  api_ctx_t *ctx = (api_ctx_t *)req->user_ctx;
  if (ctx)
  {
    ctx->http_status = 204;
    ctx->error_code = ESP_OK;
    ctx->manual_response = false;
  }
  return ESP_OK;
}

void api_set_manual_response(httpd_req_t *req)
{
  api_ctx_t *ctx = (api_ctx_t *)req->user_ctx;
  if (ctx) ctx->manual_response = true;
}
