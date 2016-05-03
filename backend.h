#ifndef SDR_POWER_BACKEND_H
#define SDR_POWER_BACKEND_H

#include <stdint.h>

struct backend
{
    int (*read_sync)(uint8_t *buf, int len, int *n_read);
    uint64_t (*get_center_freq)();
    int (*set_center_freq)(uint64_t freq);
    int (*set_sample_rate)(uint32_t rate);
    int (*close)();

    uint32_t MINIMUM_RATE;
    uint32_t MAXIMUM_RATE;
};

struct backend* initialize_backend(char* opts);

#endif //SDR_POWER_BACKEND_H
