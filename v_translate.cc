#include <fcntl.h>
#include "config.hh"
uint64_t get_pfn(uint64_t entry) {
    return ((entry) & 0x3fffffffffffff);
}

uint64_t virt_to_phys(uint64_t v_addr) 
{
    uint64_t entry; 
    uint64_t offset = (v_addr/4096) * sizeof(entry);
    uint64_t pfn; 
    int fd = open("/proc/self/pagemap", O_RDONLY);
    assert(fd >= 0);
    int bytes_read = pread(fd, &entry, sizeof(entry), offset);
    close(fd);
    assert(bytes_read == 8);
    assert(entry & (1ULL << 63));
    pfn = get_pfn(entry);
    assert(pfn != 0);
    return (pfn*4096) | (v_addr & 4095); 
}
