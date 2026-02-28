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
#include "openswoole_file.h"
#include "openswoole_memory.h"

#include <sys/mman.h>

#if defined(MAP_ANON) && !defined(MAP_ANONYMOUS)
#define MAP_ANONYMOUS MAP_ANON
#endif

namespace openswoole {

struct SharedMemory {
    size_t size_;

    static void *alloc(size_t size);
    static void free(void *ptr);

    static SharedMemory *fetch_object(void *ptr) {
        return (SharedMemory *) ((char *) ptr - sizeof(SharedMemory));
    }
};

void *SharedMemory::alloc(size_t size) {
    void *mem;
    int tmpfd = -1;
    int flags = MAP_SHARED;
    SharedMemory object;

    size = OSW_MEM_ALIGNED_SIZE(size);
    size += sizeof(SharedMemory);

#ifdef MAP_ANONYMOUS
    flags |= MAP_ANONYMOUS;
#else
    File zerofile("/dev/zero", O_RDWR);
    if (!zerofile.ready()) {
        return nullptr;
    }
    tmpfd = zerofile.get_fd();
#endif
    mem = mmap(nullptr, size, PROT_READ | PROT_WRITE, flags, tmpfd, 0);
#ifdef MAP_FAILED
    if (mem == MAP_FAILED)
#else
    if (!mem)
#endif
    {
        openswoole_sys_warning("mmap(%lu) failed", size);
        return nullptr;
    } else {
        object.size_ = size;
        memcpy(mem, &object, sizeof(object));
        return (char *) mem + sizeof(object);
    }
}

void SharedMemory::free(void *ptr) {
    SharedMemory *object = SharedMemory::fetch_object(ptr);
    size_t size = object->size_;
    if (munmap(object, size) < 0) {
        openswoole_sys_warning("munmap(%p, %lu) failed", object, size);
    }
}

}  // namespace openswoole

using openswoole::SharedMemory;

void *osw_shm_malloc(size_t size) {
    return SharedMemory::alloc(size);
}

void *osw_shm_calloc(size_t num, size_t _size) {
    return SharedMemory::alloc(num * _size);
}

int osw_shm_protect(void *ptr, int flags) {
    SharedMemory *object = SharedMemory::fetch_object(ptr);
    return mprotect(object, object->size_, flags);
}

void osw_shm_free(void *ptr) {
    SharedMemory::free(ptr);
}

void *osw_shm_realloc(void *ptr, size_t new_size) {
    SharedMemory *object = SharedMemory::fetch_object(ptr);
    void *new_ptr = osw_shm_malloc(new_size);
    if (new_ptr == nullptr) {
        return nullptr;
    }
    memcpy(new_ptr, ptr, object->size_);
    SharedMemory::free(ptr);
    return new_ptr;
}
