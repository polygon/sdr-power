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
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#include "dummy.h"
#include "ringbuffer.h"

// Implementation of interface functions

#define RX_BUFFER_SIZE 1048576

static uint64_t last_center_freq = 0;
static uint8_t file_mode = 0;
static char* filename;
static int iqfile;
static uint8_t* iqdata;
static uint64_t iqsize;
static uint64_t iqpos;

struct backend backend_dummy;

int read_sync_dummy(uint8_t *buf, int len, int *n_read)
{
    if (!file_mode)
    {
	memset(buf, 0, len);
    }
    else
    {
	int read = 0;
	while (read < len)
	{
	    int remaining = len - read;
	    if (remaining > iqsize - iqpos)
	    {
		memcpy(&buf[read], &iqdata[iqpos], iqsize - iqpos);
		read += iqsize - iqpos;
		iqpos = 0;
	    }
	    else
	    {
		memcpy(&buf[read], &iqdata[iqpos], remaining);
		read += remaining;
		iqpos = (iqpos + remaining) % iqsize;
	    }
	}
    }
    *n_read = len;
    return 0;
}

uint64_t get_center_freq_dummy()
{
    return last_center_freq;
}

int set_center_freq_dummy(uint64_t freq)
{
    //fprintf(stderr, "Retune to %i Hz\n", freq);	
    last_center_freq = freq;
    return 0;
}

int set_sample_rate_dummy(uint32_t rate)
{
    if ((rate < backend_dummy.MINIMUM_RATE) || (rate > backend_dummy.MAXIMUM_RATE))
	return -1;
    return 0;
}

int close_dummy()
{
    if (file_mode)
    {
	munmap(iqdata, iqsize);
	close(iqfile);
    }
}

// Backend definition
struct backend backend_dummy = {
        .read_sync = read_sync_dummy,
        .get_center_freq = get_center_freq_dummy,
        .set_center_freq = set_center_freq_dummy,
        .set_sample_rate = set_sample_rate_dummy,
        .close = close_dummy,
        .MINIMUM_RATE = 2000000,
        .MAXIMUM_RATE = 20000000
};

static enum options
{
    MIN_RATE = 0,
    MAX_RATE = 1,
    FILE_MODE = 2,
    SAMPLERATE = 3,
    THE_END
};

static char *options_list[] =
        {
                [MIN_RATE] = "min_rate",
		[MAX_RATE] = "max_rate",
		[FILE_MODE] = "file",
		[SAMPLERATE] = "samplerate",
                [THE_END] = NULL
        };

void dummy_parse_opts(char* opts)
{
    char* value;
    while (*opts != '\0')
    {
        switch(getsubopt(&opts, options_list, &value))
        {
            case MIN_RATE:
                if (value != NULL)
                    backend_dummy.MINIMUM_RATE = atoi(value);
                break;
	    case MAX_RATE:
		if (value != NULL)
		    backend_dummy.MAXIMUM_RATE = atoi(value);
		break;
	    case SAMPLERATE:
		if (value != NULL) {
		    backend_dummy.SAMPLERATE = atoi(value);
		    backend_dummy.RADIO_TYPE = FIXED_SAMPLERATE;
		}
		break;
		 
	    case FILE_MODE:
		if (value != NULL)
		{
		    filename = strdup(value);
		    file_mode = 1;
		}
		break;
        }
    }
}

struct backend *dummy_initialize_backend(char *opts)
{
    dummy_parse_opts(opts);
    if (file_mode)
    {
	iqfile = open(filename, O_RDONLY);
	if (iqfile < 0)
	{
	    fprintf(stderr, "Failed to open file \"%s\"\n", filename);
	    exit(1);
	}
	struct stat iqstat;
	if (fstat(iqfile, &iqstat) < 0)
	{
	    fprintf(stderr, "Failed to stat \"%s\"\n", filename);
	    close(iqfile);
	    exit(1);
	}
	iqdata = mmap(NULL, iqstat.st_size, PROT_READ, MAP_SHARED, iqfile, 0);
	if (iqdata < 0)
	{
	    fprintf(stderr, "Failed to mmap");
	    close(iqfile);
	    exit(1);
	}
	iqpos = 0;
	iqsize = iqstat.st_size;
	fprintf(stderr, "Using iqfile \"%s\" of size %lld\n", filename, iqsize);
    }
    return &backend_dummy;
}