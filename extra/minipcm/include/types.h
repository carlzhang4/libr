#pragma once

#include <iostream>
#include <istream>
#include <sstream>
#include <iomanip>
#include <string.h>
#include <assert.h>
#include <limits>
#include <memory>
namespace pcm {

    typedef unsigned long long uint64;
    typedef signed long long int64;
    typedef unsigned int uint32;
    typedef signed int int32;

    constexpr auto PCM_INTEL_PCI_VENDOR_ID = 0x8086;

#define PCM_ULIMIT_RECOMMENDATION ("try executing 'ulimit -n 1000000' to increase the limit on the number of open files.\n")

    template <unsigned Bytes>
    inline void warnAlignment(const char *call, const bool silent, const uint64 offset) {
        if (silent == false && (offset % Bytes) != 0) {
            std::cerr << "PCM Warning: " << call << " offset " << offset << " is not " << Bytes << "-byte aligned\n";
        }
    }

    inline uint64 roundDownTo4K(uint64 number) {
        return number & ~0xFFFULL; // Mask the lower 12 bits to round down to 4K
    }

    inline uint64 roundUpTo4K(uint64 number) {
        if (number % 4096ULL == 0ULL) {
            // Already a multiple of 4K
            return number;
        } else {
            // Round up to the next multiple of 4K
            return ((number / 4096ULL) + 1ULL) * 4096ULL;
        }
    }

    union cvt_ds {
        typedef uint64 UINT64;
        typedef uint32 DWORD;
        UINT64 ui64;
        struct {
            DWORD low;
            DWORD high;
        } ui32;
    };


    struct MCFGRecord {
        unsigned long long baseAddress;
        unsigned short PCISegmentGroupNumber;
        unsigned char startBusNumber;
        unsigned char endBusNumber;
        char reserved[4];
        MCFGRecord(): baseAddress(0), PCISegmentGroupNumber(0), startBusNumber(0), endBusNumber(0) {
            std::fill(reserved, reserved + 4, 0);
        }
        void print() {
            std::cout << "BaseAddress=" << (std::hex) << "0x" << baseAddress << " PCISegmentGroupNumber=0x" << PCISegmentGroupNumber <<
                " startBusNumber=0x" << (unsigned)startBusNumber << " endBusNumber=0x" << (unsigned)endBusNumber << "\n" << std::dec;
        }
    };

    struct MCFGHeader {
        char signature[4];
        unsigned length;
        unsigned char revision;
        unsigned char checksum;
        char OEMID[6];
        char OEMTableID[8];
        unsigned OEMRevision;
        unsigned creatorID;
        unsigned creatorRevision;
        char reserved[8];

        unsigned nrecords() const {
            return (length - sizeof(MCFGHeader)) / sizeof(MCFGRecord);
        }

        void print() {
            std::cout << "Header: length=" << length << " nrecords=" << nrecords() << "\n";
        }
    };

    constexpr auto SERVER_MC_CH_PMON_SIZE = 0x1000;
    constexpr auto SERVER_MC_CH_PMON_FIXED_CTL_OFFSET = 0x54;
    constexpr auto SERVER_MC_CH_PMON_FIXED_CTR_OFFSET = 0x38;

#define MC_CH_PCI_PMON_CTL_EVENT(x) (x << 0)
#define MC_CH_PCI_PMON_CTL_UMASK(x) (x << 8)
#define MC_CH_PCI_PMON_CTL_RST (1 << 17)
#define MC_CH_PCI_PMON_CTL_EDGE_DET (1 << 18)
#define MC_CH_PCI_PMON_CTL_EN (1 << 22)
#define MC_CH_PCI_PMON_CTL_INVERT (1 << 23)
#define MC_CH_PCI_PMON_CTL_THRESH(x) (x << 24UL)

#define UNC_PMON_UNIT_CTL_RST_CONTROL  (1 << 0)
#define UNC_PMON_UNIT_CTL_RST_COUNTERS     (1 << 1)
#define UNC_PMON_UNIT_CTL_FRZ  (1 << 8)
#define UNC_PMON_UNIT_CTL_FRZ_EN   (1 << 16)
#define UNC_PMON_UNIT_CTL_RSV  ((1 << 16) + (1 << 17))

#define SPR_UNC_PMON_UNIT_CTL_FRZ          (1 << 0)
#define SPR_UNC_PMON_UNIT_CTL_RST_CONTROL  (1 << 8)
#define SPR_UNC_PMON_UNIT_CTL_RST_COUNTERS (1 << 9)

#define MC_CH_PCI_PMON_FIXED_CTL_RST (1 << 19)
#define MC_CH_PCI_PMON_FIXED_CTL_EN (1 << 22)
#define EDC_CH_PCI_PMON_FIXED_CTL_EN (1 << 0)

}