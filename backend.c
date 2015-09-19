#include <gif_lib.h>
#include <stdio.h>
#include <stdlib.h>
#include "backend.h"
#include "backends/rtlsdr.h"

enum radios
{
    RTLSDR = 0,
    BLADERF,
    RAD1O,
    THE_END
};

char *radio_list[] =
{
    [RTLSDR] = "rtlsdr",
    [BLADERF] = "bladerf",
    [RAD1O] = "rad1o",
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
        return rtlsdr_initialize_backend(opts);
    default:
        fprintf(stderr, "Unsupported radio\n");
        exit(1);
    };
}
