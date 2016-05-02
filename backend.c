#include <gif_lib.h>
#include <stdio.h>
#include <stdlib.h>
#include "backend.h"

#ifdef RTLSDR_ENABLED
#include "backends/rtlsdr.h"
#endif

#ifdef HACKRF_ENABLED
#include "backends/hackrf.h"
#endif

#include "config.h"

enum radios
{
    RTLSDR = 0,
    BLADERF,
    HACKRF,
    THE_END
};

char *radio_list[] =
{
    [RTLSDR] = "rtlsdr",
    [BLADERF] = "bladerf",
    [HACKRF] = "hackrf",
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
    default:
        fprintf(stderr, "Unsupported radio\n");
        exit(1);
    };
}
