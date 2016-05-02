#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ringbuffer.h"

ring_buffer* rb_create(size_t size)
{
    ring_buffer* rb = malloc(sizeof(ring_buffer));
    rb->buffer = malloc(size);
    rb->size = size;
    rb_reset(rb);
    return rb;
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

size_t rb_write(ring_buffer* rb, int8_t* buffer, size_t len)
{
    size_t written = 0;
    if (rb->full)
	return 0;

    //fprintf(stderr, "Write Status: write_idx: %i, read_idx: %i\n", rb->write_idx, rb->read_idx);	
    if (rb->write_idx >= rb->read_idx)
    {
	// Write towards end of buffer
	size_t space = rb->size - rb->write_idx;
	if (space <= len)
	{
	    memcpy(&rb->buffer[rb->write_idx], buffer, space);
	    written = space;
	    rb->write_idx = 0;
	}
	else
	{
	    memcpy(&rb->buffer[rb->write_idx], buffer, len);
	    rb->write_idx += len;
	    written = len;
	}
    }
    
    if ((rb->write_idx < rb->read_idx) && (written < len))
    {
	// Write towards read_idx
	size_t remaining = len - written;
	size_t space = rb->read_idx - rb->write_idx;
	if (space > 0)
	{
	    if (space <= remaining)
	    {
		memcpy(&rb->buffer[rb->write_idx], &buffer[written], space);
		written += space;
		rb->write_idx += space;
	    }
	    else
	    {
		memcpy(&rb->buffer[rb->write_idx], &buffer[written], remaining);
		rb->write_idx += remaining;
		written += remaining;
	    }
	}
    }
    
    if (rb->write_idx == rb->read_idx)
      rb->full = 1;
    
    return written;
}

size_t rb_read(ring_buffer* rb, int8_t* buffer, size_t len)
{
    size_t read = 0;
    
    if ((rb->write_idx == rb->read_idx) && (!rb->full))
      return 0;
    if (len == 0)
      return 0;
    
    if (rb->read_idx >= rb->write_idx)
    {
	// Read towards buffer end
	size_t space = rb->size - rb->read_idx;
	if (space <= len)
	{		
	    memcpy(buffer, &rb->buffer[rb->read_idx], space);
	    read = space;
	    rb->read_idx = 0;
	}
	else
	{
	    memcpy(buffer, &rb->buffer[rb->read_idx], len);
	    read = len;
	    rb->read_idx += len;
	}
    }
    
    if ((rb->read_idx < rb->write_idx) && (read < len))
    {
	// Read towards write_ptr
	size_t remaining = len - read;
	size_t space = rb->write_idx - rb->read_idx;
	if (space <= remaining)
	{
	    memcpy(&buffer[read], &rb->buffer[rb->read_idx], space);
	    read += space;
	    rb->read_idx += space;
	}
	else
	{
	    memcpy(&buffer[read], &rb->buffer[rb->read_idx], remaining);
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