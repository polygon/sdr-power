#include <rtl-sdr.h>
#include <stddef.h>
#include "rtlsdr.h"

// Implementation of interface functions
static rtlsdr_dev_t *dev = NULL;

int read_sync_rtlsdr(uint8_t *buf, int len, int *n_read)
{
    return rtlsdr_read_sync(dev, buf, len, n_read);
}

uint32_t get_center_freq_rtlsdr()
{
    return rtlsdr_get_center_freq(dev);
}

int set_center_freq_rtlsdr(uint32_t freq)
{
    return rtlsdr_set_center_freq(dev, freq);
}

int set_sample_rate_rtlsdr(uint32_t rate)
{
    return rtlsdr_set_sample_rate(dev, rate);
}

int open_rtlsdr(uint32_t index)
{
    return rtlsdr_open(dev, index);
}

int close_rtlsdr()
{
    return rtlsdr_close(dev);
}

const struct backend backend_rtlsdr = {
        .read_sync = read_sync_rtlsdr,
        .get_center_freq = get_center_freq_rtlsdr,
        .set_center_freq = set_center_freq_rtlsdr,
        .set_sample_rate = set_sample_rate_rtlsdr,
        .open = open_rtlsdr,
        .close = close_rtlsdr,
        .MINIMUM_RATE = 1000000,
        .MAXIMUM_RATE = 2800000
};