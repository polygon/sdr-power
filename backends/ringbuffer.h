#ifndef SDR_POWER_RINGBUFFER_H
#define SDR_POWER_RINGBUFFER_H

#include <stddef.h>
#include <stdint.h>

typedef struct
{
    size_t size;
    int8_t* buffer;
    size_t read_idx;
    size_t write_idx;
    uint8_t full;
} ring_buffer;

ring_buffer* rb_create(size_t size);
void rb_free(ring_buffer* rb);
void rb_reset(ring_buffer* rb);
size_t rb_write(ring_buffer* rb, int8_t* buffer, size_t len);
size_t rb_read(ring_buffer* rb, int8_t* buffer, size_t len);
uint8_t rb_can_read(ring_buffer* rb);

#endif //SDR_POWER_RINGBUFFER_H