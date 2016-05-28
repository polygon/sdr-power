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

//#include <gif_lib.h>
#include <hackrf.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include "hackrf.h"
#include "ringbuffer.h"

// Implementation of interface functions

#define RX_BUFFER_SIZE 1048576

static uint64_t last_center_freq = 0;
static hackrf_device *dev = NULL;
static pthread_mutex_t lock;
static ring_buffer* rb;

int read_sync_hackrf(uint8_t *buf, int len, int *n_read)
{
    //fprintf(stderr, "Want to read %i bytes\n", len);
    // Wait until data is available, may want to add timeout here
    uint8_t can_read = 0;
    int8_t* i8buf = (int8_t*)buf;
    while (can_read == 0)
    {
        pthread_mutex_lock(&lock);
	can_read = rb_can_read(rb);
	if (!can_read)
	    pthread_mutex_unlock(&lock);
    }
    size_t read = rb_read(rb, buf, len);
    for (uint32_t i = 0; i < read; i++)
    {
	buf[i] = (uint8_t)((int16_t)i8buf[i] + 127);
    }
    *n_read = (int)read;
    pthread_mutex_unlock(&lock);
    return 0;
}

uint64_t get_center_freq_hackrf()
{
    return last_center_freq;
}

int set_center_freq_hackrf(uint64_t freq)
{
    //fprintf(stderr, "Retune to %i Hz\n", freq);	
    last_center_freq = freq;
    int status = hackrf_set_freq(dev, freq);
    if (status == 0)
    {
	pthread_mutex_lock(&lock);
	rb_reset(rb);
	pthread_mutex_unlock(&lock);
    }
    return status;
}

int set_sample_rate_hackrf(uint32_t rate)
{
    int status = hackrf_set_sample_rate(dev, rate);
    status |= hackrf_set_baseband_filter_bandwidth(dev, hackrf_compute_baseband_filter_bw(rate));
    if (status == 0)
    {
	pthread_mutex_lock(&lock);
	rb_reset(rb);
	pthread_mutex_unlock(&lock);
    }
    return status;
}

int close_hackrf()
{   pthread_mutex_lock(&lock);
    hackrf_stop_rx(dev);
    hackrf_close(dev);
    hackrf_exit();
    pthread_mutex_unlock(&lock);
}

// Backend definition
const struct backend backend_hackrf = {
        .read_sync = read_sync_hackrf,
        .get_center_freq = get_center_freq_hackrf,
        .set_center_freq = set_center_freq_hackrf,
        .set_sample_rate = set_sample_rate_hackrf,
        .close = close_hackrf,
        .MINIMUM_RATE = 2000000,
        .MAXIMUM_RATE = 20000000,
	.RADIO_TYPE = VARIABLE_SAMPLERATE
};

// Backend configuration
static int idx = 0;
static uint8_t amp = 0;
static uint8_t lna = 0;
static uint8_t vga = 0;

static enum options
{
    INDEX = 0,
    AMP = 1,
    LNA = 2,
    VGA = 3,
    THE_END
};

static char *options_list[] =
        {
                [INDEX] = "idx",
		[AMP] = "amp",
		[LNA] = "lna",
		[VGA] = "vga",
                [THE_END] = NULL
        };

void hackrf_parse_opts(char* opts)
{
    char* value;
    while (*opts != '\0')
    {
        switch(getsubopt(&opts, options_list, &value))
        {
            case INDEX:
                if (value != NULL)
                    idx = atoi(value);
                break;
	    case AMP:
		if (value != NULL)
		    amp = atoi(value);
		break;
	    case LNA:
		if (value != NULL)
		    lna = atoi(value);
		break;
	    case VGA:
		if (value != NULL)
		    vga = atoi(value);
		break;
        }
    }
}

int hackrf_rx_callback(hackrf_transfer* transfer)
{
    pthread_mutex_lock(&lock);
    size_t written = rb_write(rb, transfer->buffer, transfer->valid_length);
    pthread_mutex_unlock(&lock);
    //fprintf(stderr, "Written %i of %i bytes\n", written, transfer->valid);
    return 0;
}

struct backend *hackrf_initialize_backend(char *opts)
{
    hackrf_parse_opts(opts);
    rb = rb_create_i8(RX_BUFFER_SIZE);
    
    if (pthread_mutex_init(&lock, NULL) != 0)
    {
	fprintf(stderr, "Failed to create memory buffer mutex");
	exit(1);
    }
  
    int status = hackrf_init();
    if (status < 0)
    {
	fprintf(stderr, "HackRF init failed, error %i", status);
	exit(1);
    }
    
    hackrf_device_list_t* devices = hackrf_device_list();
    status = hackrf_device_list_open(devices, idx, &dev);
    if (status < 0)
    {
	fprintf(stderr, "HackRF open failed, error %i", status);
	exit(1);
    }
    hackrf_device_list_free(devices);
    
    status = hackrf_start_rx(dev, hackrf_rx_callback, NULL);
    hackrf_set_amp_enable(dev, amp);
    hackrf_set_lna_gain(dev, lna);
    hackrf_set_vga_gain(dev, vga);
    
    
    return &backend_hackrf;
}
