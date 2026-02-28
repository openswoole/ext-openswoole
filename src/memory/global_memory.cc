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

#include <vector>
#include <mutex>

#define OSW_MIN_PAGE_SIZE 4096

namespace openswoole {

struct GlobalMemoryImpl {
    bool shared;
    uint32_t pagesize;
    std::mutex lock;
    std::vector<char *> pages;
    uint32_t alloc_offset;
    pid_t create_pid;

  public:
    GlobalMemoryImpl(uint32_t _pagesize, bool _shared);
    char *new_page();
};

struct MemoryBlock {
    uint32_t size;
    uint32_t reserved;
    char memory[0];
};

/**
 * After the memory is allocated,
 * it will not be released until it is recycled by OS when the process exits
 */
GlobalMemory::GlobalMemory(uint32_t pagesize, bool shared) {
    assert(pagesize >= OSW_MIN_PAGE_SIZE);
    impl = new GlobalMemoryImpl(pagesize, shared);
}

GlobalMemoryImpl::GlobalMemoryImpl(uint32_t _pagesize, bool _shared) {
    shared = _shared;
    pagesize = OSW_MEM_ALIGNED_SIZE_EX(_pagesize, OpenSwooleG.pagesize);
    create_pid = OpenSwooleG.pid;

    if (new_page() == nullptr) {
        throw std::bad_alloc();
    }
}

char *GlobalMemoryImpl::new_page() {
    char *page = (char *) (shared ? osw_shm_malloc(pagesize) : osw_malloc(pagesize));
    if (page == nullptr) {
        return nullptr;
    }

    pages.push_back(page);
    alloc_offset = 0;

    return page;
}

/**
 * The returned memory must be initialized to 0
 */
void *GlobalMemory::alloc(uint32_t size) {
    MemoryBlock *block;
    size = OSW_MEM_ALIGNED_SIZE(size);
    uint32_t alloc_size = sizeof(*block) + size;
    std::unique_lock<std::mutex> lock(impl->lock);

    if (alloc_size > impl->pagesize) {
        openswoole_warning("failed to alloc %d bytes, exceed the maximum size[%d]", size, impl->pagesize);
        return nullptr;
    }

    if (impl->shared and impl->create_pid != getpid()) {
        GlobalMemoryImpl *old_impl = impl;
        impl = new GlobalMemoryImpl(old_impl->pagesize, old_impl->shared);
    }

    openswoole_trace("alloc_size=%u, size=%u", alloc_size, size);

    if (impl->alloc_offset + alloc_size > impl->pagesize) {
        char *page = impl->new_page();
        if (page == nullptr) {
            openswoole_warning("alloc memory error");
            return nullptr;
        }
    }

    block = (MemoryBlock *) (impl->pages.back() + impl->alloc_offset);
    impl->alloc_offset += alloc_size;

    block->size = size;

    osw_memset_zero(block->memory, size);
    return block->memory;
}

void GlobalMemory::free(void *ptr) {}

void GlobalMemory::destroy() {
    for (auto page : impl->pages) {
        impl->shared ? ::osw_shm_free(page) : ::osw_free(page);
    }
}

size_t GlobalMemory::capacity() {
    return impl->pagesize - impl->alloc_offset;
}

size_t GlobalMemory::get_memory_size() {
    return impl->pagesize * impl->pages.size();
}

GlobalMemory::~GlobalMemory() {
    delete impl;
}

}  // namespace openswoole
