// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2012-2022, Intel Corporation
// written by Roman Dementiev
//            Patrick Konsor
//

#pragma once

/*!     \file mmio.h
        \brief Interface to access memory mapped IO registers

*/

#include "types.h"

#include <unistd.h>
#include <memory>

namespace pcm {

    class MMIORange {
        int32 fd;
        char *mmapAddr;
        const uint64 size;
        const bool readonly;
        const bool silent;
        const int core;
        MMIORange(const MMIORange &) = delete;
        MMIORange &operator = (const MMIORange &) = delete;
    public:
        MMIORange(const uint64 baseAddr_,
            const uint64 size_,
            const bool readonly_ = true,
            const bool silent_ = false,
            const int core_ = -1);
        uint32 read32(uint64 offset);
        uint64 read64(uint64 offset);
        void write32(uint64 offset, uint32 val);
        void write64(uint64 offset, uint64 val);
        ~MMIORange();
    };

    void mmio_memcpy(void *dest, const uint64 src, const size_t n, const bool checkFailures, const bool silent = false);

} // namespace pcm
