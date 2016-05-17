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

#include <stdio.h>
#include <stdlib.h>
#include "backend.h"

#ifdef RTLSDR_ENABLED
#include "backends/rtlsdr.h"
#endif

#ifdef HACKRF_ENABLED
#include "backends/hackrf.h"
#endif

#include "backends/dummy.h"
#ifdef AIRSPY_ENABLED
#include "backends/airspy.h"
#endif

#include "config.h"

enum radios
{
    RTLSDR = 0,
    BLADERF,
    HACKRF,
    DUMMY,
    AIRSPY,
    THE_END
};

char *radio_list[] =
{
    [RTLSDR] = "rtlsdr",
    [BLADERF] = "bladerf",
    [HACKRF] = "hackrf",
    [DUMMY] = "dummy",
    [AIRSPY] = "airspy",
    [THE_END] = NULL
};

struct backend* initialize_backend(char *opts)
{
    char* radio;
    if (*opts == '\0')
    {
        fprintf(stderr, "No radio identifier given\n");
        exit(1);
    }
    switch (getsubopt(&opts, radio_list, &radio))
    {
    case RTLSDR:
#ifdef RTLSDR_ENABLED
        return rtlsdr_initialize_backend(opts);
#else
        fprintf(stderr, "rtlsdr backend not built\n");
        exit(1);
#endif
    case HACKRF:
#ifdef HACKRF_ENABLED
	return hackrf_initialize_backend(opts);
#else
	fprintf(stderr, "hackrf backend not built\n");
	exit(1);
#endif
    case DUMMY:
	return dummy_initialize_backend(opts);
    case AIRSPY:
#ifdef AIRSPY_ENABLED
	return airspy_initialize_backend(opts);
#else
	fprintf(stderr, "airspy backend not built\n");
	exit(1);
#endif
    default:
        fprintf(stderr, "Unsupported radio\n");
        exit(1);
    };
}
