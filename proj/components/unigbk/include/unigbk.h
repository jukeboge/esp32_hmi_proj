#pragma once

#include <stdbool.h>
#include "sdkconfig.h"
#include "esp_err.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"

#ifdef __cplusplus
extern "C" {
#endif
int utf8togbk(uint8_t* in, uint8_t* out);
#ifdef __cplusplus
}
#endif