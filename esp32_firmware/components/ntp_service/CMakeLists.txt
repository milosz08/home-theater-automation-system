idf_component_register(
  SRCS
    "ntp_service.c"
  
  INCLUDE_DIRS
    "include"

  REQUIRES
    log
    lwip
)
