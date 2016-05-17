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

#ifndef SDR_POWER_RINGBUFFER_H
#define SDR_POWER_RINGBUFFER_H

#include <stddef.h>
#include <stdint.h>

typedef struct
{
    size_t size;
    void* buffer;
    size_t elem_size;
    size_t read_idx;
    size_t write_idx;
    uint8_t full;
} ring_buffer;

ring_buffer* rb_create(size_t size, size_t elem_size);
ring_buffer* rb_create_i8(size_t size);
ring_buffer* rb_create_i16(size_t size);
void rb_free(ring_buffer* rb);
void rb_reset(ring_buffer* rb);
size_t rb_write(ring_buffer* rb, void* buffer, size_t len);
size_t rb_read(ring_buffer* rb, void* buffer, size_t len);
uint8_t rb_can_read(ring_buffer* rb);

#endif //SDR_POWER_RINGBUFFER_H