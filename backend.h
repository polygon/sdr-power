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
