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
#include <string.h>
#include "ringbuffer.h"

ring_buffer* rb_create(size_t size, size_t elem_size)
{
    ring_buffer* rb = malloc(sizeof(ring_buffer));
    rb->buffer = malloc(size * elem_size);
    rb->size = size;
    rb_reset(rb);
    return rb;
}

ring_buffer* rb_create_i8(size_t size)
{
    return rb_create(size, 1);
}

ring_buffer* rb_create_i16(size_t size)
{
    return rb_create(size, 2);
}

void rb_free(ring_buffer* rb)
{
    free(rb->buffer);
    free(rb);
}

void rb_reset(ring_buffer* rb)
{
    rb->read_idx = 0;
    rb->write_idx = 0;
    rb->full = 0;
}

size_t rb_write(ring_buffer* rb, void* buffer, size_t elems)
{
    size_t written = 0;
    if (rb->full)
	return 0;

    //fprintf(stderr, "Write Status: write_idx: %i, read_idx: %i\n", rb->write_idx, rb->read_idx);	
    if (rb->write_idx >= rb->read_idx)
    {
	// Write towards end of buffer
	size_t space = rb->size - rb->write_idx;
	if (space <= elems)
	{
	    memcpy(rb->buffer + rb->write_idx * rb->elem_size, buffer, space * rb->elem_size);
	    written = space;
	    rb->write_idx = 0;
	}
	else
	{
	    memcpy(rb->buffer + rb->write_idx * rb->elem_size, buffer, elems * rb->elem_size);
	    rb->write_idx += elems;
	    written = elems;
	}
    }
    
    if ((rb->write_idx < rb->read_idx) && (written < elems))
    {
	// Write towards read_idx
	size_t remaining = elems - written;
	size_t space = rb->read_idx - rb->write_idx;
	if (space > 0)
	{
	    if (space <= remaining)
	    {
		memcpy(rb->buffer + rb->write_idx * rb->elem_size, buffer + written * rb->elem_size, space * rb->elem_size);
		written += space;
		rb->write_idx += space;
	    }
	    else
	    {
		memcpy(rb->buffer + rb->write_idx * rb->elem_size, buffer + written * rb->elem_size, remaining * rb->elem_size);
		rb->write_idx += remaining;
		written += remaining;
	    }
	}
    }
    
    if (rb->write_idx == rb->read_idx)
      rb->full = 1;
    
    return written;
}

size_t rb_read(ring_buffer* rb, void* buffer, size_t elems)
{
    size_t read = 0;
    
    if ((rb->write_idx == rb->read_idx) && (!rb->full))
      return 0;
    if (elems == 0)
      return 0;
    
    if (rb->read_idx >= rb->write_idx)
    {
	// Read towards buffer end
	size_t space = rb->size - rb->read_idx;
	if (space <= elems)
	{		
	    memcpy(buffer, rb->buffer + rb->read_idx * rb->elem_size, space * rb->elem_size);
	    read = space;
	    rb->read_idx = 0;
	}
	else
	{
	    memcpy(buffer, rb->buffer + rb->read_idx * rb->elem_size, elems * rb->elem_size);
	    read = elems;
	    rb->read_idx += elems;
	}
    }
    
    if ((rb->read_idx < rb->write_idx) && (read < elems))
    {
	// Read towards write_ptr
	size_t remaining = elems - read;
	size_t space = rb->write_idx - rb->read_idx;
	if (space <= remaining)
	{
	    memcpy(buffer + read * rb->elem_size, rb->buffer + rb->read_idx * rb->elem_size, space * rb->elem_size);
	    read += space;
	    rb->read_idx += space;
	}
	else
	{
	    memcpy(buffer + read * rb->elem_size, rb->buffer + rb->read_idx * rb->elem_size, remaining * rb->elem_size);
	    read += remaining;
	    rb->read_idx += remaining;
	}
    }
    
    if (read > 0)
      rb->full = 0;
    
    return read;
}

uint8_t rb_can_read(ring_buffer* rb)
{
    return (rb->full || (rb->read_idx != rb->write_idx));
}