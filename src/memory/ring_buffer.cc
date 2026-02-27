/*
  +----------------------------------------------------------------------+
  | OpenSwoole                                                          |
  +----------------------------------------------------------------------+
  | This source file is subject to version 2.0 of the Apache license,    |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.apache.org/licenses/LICENSE-2.0.html                      |
  | If you did not receive a copy of the Apache2.0 license and are unable|
  | to obtain it through the world-wide-web, please send a note to       |
  | hello@swoole.co.uk so we can mail you a copy immediately.            |
  +----------------------------------------------------------------------+
  | Author: Tianfeng Han  <mikan.tenny@gmail.com>                        |
  +----------------------------------------------------------------------+
*/

#include "openswoole.h"
#include "openswoole_memory.h"

namespace openswoole {

struct RingBufferImpl {
    void *memory;
    bool shared;
    uint8_t status;
    uint32_t size;
    uint32_t alloc_offset;
    uint32_t collect_offset;
    uint32_t alloc_count;
    osw_atomic_t free_count;

    void collect();
};

struct RingBufferItem {
    uint16_t lock;
    uint16_t index;
    uint32_t length;
    char data[0];
};

#ifdef OSW_RINGBUFFER_DEBUG
static void swRingBuffer_print(swRingBuffer *object, char *prefix);

static void swRingBuffer_print(swRingBuffer *object, char *prefix) {
    printf("%s: size=%d, status=%d, alloc_count=%d, free_count=%d, offset=%d, next_offset=%d\n",
           prefix,
           impl->size,
           impl->status,
           impl->alloc_count,
           impl->free_count,
           impl->alloc_offset,
           impl->collect_offset);
}
#endif

RingBuffer::RingBuffer(uint32_t size, bool shared) {
    size = OSW_MEM_ALIGNED_SIZE(size);
    void *mem = (shared == 1) ? osw_shm_malloc(size) : osw_malloc(size);
    if (mem == nullptr) {
        throw std::bad_alloc();
    }

    impl = (RingBufferImpl *) mem;
    mem = (char *) mem + sizeof(*impl);
    osw_memset_zero(impl, sizeof(*impl));

    impl->size = size - sizeof(impl);
    impl->shared = shared;
    impl->memory = mem;

    openswoole_debug("memory: ptr=%p", mem);
}

void RingBufferImpl::collect() {
    for (uint32_t i = 0; i < free_count; i++) {
        RingBufferItem *item = (RingBufferItem *) ((char *) memory + collect_offset);
        if (item->lock == 0) {
            uint32_t n_size = item->length + sizeof(RingBufferItem);
            collect_offset += n_size;
            if (collect_offset + sizeof(RingBufferItem) > size || collect_offset >= size) {
                collect_offset = 0;
                status = 0;
            }
            osw_atomic_fetch_sub(&free_count, 1);
        } else {
            break;
        }
    }
}

void *RingBuffer::alloc(uint32_t size) {
    assert(size > 0);

    RingBufferItem *item;
    uint32_t capacity;

    size = OSW_MEM_ALIGNED_SIZE(size);
    uint32_t alloc_size = size + sizeof(RingBufferItem);

    if (impl->free_count > 0) {
        impl->collect();
    }

    if (impl->status == 0) {
        if (impl->alloc_offset + alloc_size >= (impl->size - sizeof(RingBufferItem))) {
            uint32_t skip_n = impl->size - impl->alloc_offset;
            if (skip_n >= sizeof(RingBufferItem)) {
                item = (RingBufferItem *) ((char *) impl->memory + impl->alloc_offset);
                item->lock = 0;
                item->length = skip_n - sizeof(RingBufferItem);
                osw_atomic_t *free_count = &impl->free_count;
                osw_atomic_fetch_add(free_count, 1);
            }
            impl->alloc_offset = 0;
            impl->status = 1;
            capacity = impl->collect_offset - impl->alloc_offset;
        } else {
            capacity = impl->size - impl->alloc_offset;
        }
    } else {
        capacity = impl->collect_offset - impl->alloc_offset;
    }

    if (capacity < alloc_size) {
        return nullptr;
    }

    item = (RingBufferItem *) ((char *) impl->memory + impl->alloc_offset);
    item->lock = 1;
    item->length = size;
    item->index = impl->alloc_count;

    impl->alloc_offset += alloc_size;
    impl->alloc_count++;

    openswoole_debug("alloc: ptr=%p", (void *) (item->data - (char *) impl->memory));

    return item->data;
}

void RingBuffer::free(void *ptr) {
    RingBufferItem *item = (RingBufferItem *) ((char *) ptr - sizeof(RingBufferItem));

    assert(ptr >= impl->memory);
    assert((char *) ptr <= (char *) impl->memory + impl->size);
    assert(item->lock == 1);

    if (item->lock != 1) {
        openswoole_debug("invalid free: index=%d, ptr=%p", item->index, (void *) (item->data - (char *) impl->memory));
    } else {
        item->lock = 0;
    }

    openswoole_debug("free: ptr=%p", (void *) (item->data - (char *) impl->memory));

    osw_atomic_t *free_count = &impl->free_count;
    osw_atomic_fetch_add(free_count, 1);
}

RingBuffer::~RingBuffer() {
    if (impl->shared) {
        osw_shm_free(impl);
    } else {
        osw_free(impl);
    }
}

}  // namespace openswoole
