#ifndef __MT_H__
#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

typedef void (*callback_t)(const char *message, const void *user_data);

void register_callback(callback_t callback, const void *user_data);
void start(const uint32_t number_of_threads);
void stop();

#ifdef __cplusplus
}
#endif

#endif
