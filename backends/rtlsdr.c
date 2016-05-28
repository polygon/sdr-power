/*
 * Copyright (C) 2016 by Jan Dohl <polygon@c3d2.de>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <rtl-sdr.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include "rtlsdr.h"
#include "rtlsdr_convenience.h"

// Implementation of interface functions
static rtlsdr_dev_t *dev = NULL;

int read_sync_rtlsdr(uint8_t *buf, int len, int *n_read)
{
    return rtlsdr_read_sync(dev, buf, len, n_read);
}

uint64_t get_center_freq_rtlsdr()
{
    return rtlsdr_get_center_freq(dev);
}

int set_center_freq_rtlsdr(uint64_t freq)
{
    return rtlsdr_set_center_freq(dev, freq);
}

int set_sample_rate_rtlsdr(uint32_t rate)
{
    return rtlsdr_set_sample_rate(dev, rate);
}

int close_rtlsdr()
{
    return rtlsdr_close(dev);
}

// Backend definition
const struct backend backend_rtlsdr = {
        .read_sync = read_sync_rtlsdr,
        .get_center_freq = get_center_freq_rtlsdr,
        .set_center_freq = set_center_freq_rtlsdr,
        .set_sample_rate = set_sample_rate_rtlsdr,
        .close = close_rtlsdr,
        .MINIMUM_RATE = 1000000,
        .MAXIMUM_RATE = 2800000,
	.RADIO_TYPE = VARIABLE_SAMPLERATE
};

// Backend configuration
static char* idx;
static int idx_set = 0;
static int gain;
static int gain_set = 0;
static int ppm;
static int ppm_set = 0;
static int dsampling_set = 0;
static int otune_set = 0;

static enum options
{
    INDEX = 0,
    GAIN,
    PPM,
    DSAMPLING,
    OTUNE,
    THE_END
};

static char *options_list[] =
        {
                [INDEX] = "idx",
                [GAIN] = "gain",
                [PPM] = "ppm_error",
                [DSAMPLING] = "dsampling",
                [OTUNE] = "otune",
                [THE_END] = NULL
        };

void rtlsdr_parse_opts(char* opts)
{
    char* value;
    while (*opts != '\0')
    {
        switch(getsubopt(&opts, options_list, &value))
        {
            case INDEX:
                if (value != NULL)
                {
                    idx_set = 1;
                    idx = value;
                }
                break;
            case GAIN:
                if (value != NULL)
                {
                    gain_set = 1;
                    gain = atoi(value);
                }
                break;
            case PPM:
                if (value != NULL)
                {
                    ppm_set = 1;
                    ppm = atoi(value);
                }
                break;
            case DSAMPLING:
                dsampling_set = 1;
                break;
            case OTUNE:
                otune_set = 1;
                break;
        }
    }
}

struct backend * rtlsdr_initialize_backend(char *opts)
{
    int device;
    rtlsdr_parse_opts(opts);

    if (idx_set)
    {
        device = rtlsdr_verbose_device_search(idx);
    }
    else
    {
        device = rtlsdr_verbose_device_search("0");
    }

    if (rtlsdr_open(&dev, device) < 0)
    {
        fprintf(stderr, "Failed to open rtlsdr device #%d.\n", device);
        exit(1);
    }

    if (dsampling_set)
    {
        rtlsdr_verbose_direct_sampling(dev, 1);
    }

    if (otune_set)
    {
        rtlsdr_verbose_offset_tuning(dev);
    }

    if (gain_set)
    {
        gain = rtlsdr_nearest_gain(dev, gain);
        rtlsdr_verbose_gain_set(dev, gain);
    }
    else
    {
        rtlsdr_verbose_auto_gain(dev);
    }

    if (ppm_set)
    {
        rtlsdr_verbose_ppm_set(dev, ppm);
    }

    rtlsdr_verbose_reset_buffer(dev);
    return &backend_rtlsdr;
}
