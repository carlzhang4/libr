// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2009-2022, Intel Corporation
// written by Roman Dementiev,
//            Patrick Konsor
//

#include <iostream>
#include <string.h>
#include <sys/types.h>

#include <sys/stat.h>
#include <fcntl.h>
#include "pci.h"
#include "mmio.h"

#include <sys/mman.h>
#include <errno.h>
#include <exception>
#include <assert.h>

namespace pcm {
    MMIORange::MMIORange(const uint64 baseAddr_, const uint64 size_, const bool readonly_, const bool silent_, const int core_):
        fd(-1),
        mmapAddr(NULL),
        size(size_),
        readonly(readonly_),
        silent(silent_),
        core(core_) {
        const int oflag = readonly ? O_RDONLY : O_RDWR;
        int handle = ::open("/dev/mem", oflag);
        if (handle < 0) {
            std::ostringstream strstr;
            strstr << "opening /dev/mem failed: errno is " << errno << " (" << strerror(errno) << ")\n";
            if (!silent) {
                std::cerr << strstr.str();
            }
            throw std::runtime_error(strstr.str());
        }
        fd = handle;

        const int prot = readonly ? PROT_READ : (PROT_READ | PROT_WRITE);
        mmapAddr = (char *)mmap(NULL, size, prot, MAP_SHARED, fd, baseAddr_);

        if (mmapAddr == MAP_FAILED) {
            std::ostringstream strstr;
            strstr << "mmap failed: errno is " << errno << " (" << strerror(errno) << ")\n";
            if (1 == errno) {
                strstr << "Try to add 'iomem=relaxed' parameter to the kernel boot command line and reboot.\n";
            }
            if (!silent) {
                std::cerr << strstr.str();
            }
            throw std::runtime_error(strstr.str());
        }
    }

    uint32 MMIORange::read32(uint64 offset) {
        warnAlignment<4>("MMIORange::read32", silent, offset);
        return *((uint32 *)(mmapAddr + offset));
    }

    uint64 MMIORange::read64(uint64 offset) {
        warnAlignment<8>("MMIORange::read64", silent, offset);
        return *((uint64 *)(mmapAddr + offset));
    }

    void MMIORange::write32(uint64 offset, uint32 val) {
        warnAlignment<4>("MMIORange::write32", silent, offset);
        if (readonly) {
            std::cerr << "PCM Error: attempting to write to a read-only MMIORange\n";
            return;
        }
        *((uint32 *)(mmapAddr + offset)) = val;
    }
    void MMIORange::write64(uint64 offset, uint64 val) {
        warnAlignment<8>("MMIORange::write64", silent, offset);
        if (readonly) {
            std::cerr << "PCM Error: attempting to write to a read-only MMIORange\n";
            return;
        }
        *((uint64 *)(mmapAddr + offset)) = val;
    }

    MMIORange::~MMIORange() {
        if (mmapAddr) munmap(mmapAddr, size);
        if (fd >= 0) ::close(fd);
    }


    void mmio_memcpy(void *dest_, const uint64 src, const size_t n, const bool checkFailures, const bool silent) {
        assert((src % sizeof(uint32)) == 0);
        assert((n % sizeof(uint32)) == 0);

        const uint64 end = src + n;
        const uint64 mapBegin = roundDownTo4K(src);
        const uint64 mapSize = roundUpTo4K(end) - mapBegin;
        uint32 *dest = (uint32 *)dest_;
        MMIORange range(mapBegin, mapSize, true, silent);

        for (uint64 i = src; i < end; i += sizeof(uint32), ++dest) {
            const auto value = range.read32(i - mapBegin);
            if (checkFailures && value == ~uint32(0)) {
                // a bad read
                std::ostringstream strstr;
                strstr << "Failed to read memory at 0x" << std::hex << i << std::dec << "\n";
                if (!silent) {
                    std::cerr << strstr.str();
                }
                throw std::runtime_error(strstr.str());
            }
            *dest = value;
        }
    }

} // namespace pcm
