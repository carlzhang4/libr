// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2009-2022, Intel Corporation
// written by Roman Dementiev,
//            Pat Fay
//	      Austen Ott
//            Jim Harris (FreeBSD)

#include <iostream>
#include <stdexcept>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "pci.h"
#include <sys/mman.h>
#include <errno.h>
#include <strings.h>


namespace pcm {
    // Linux implementation


    int openHandle(uint32 groupnr_, uint32 bus, uint32 device, uint32 function) {
        std::ostringstream path(std::ostringstream::out);

        path << std::hex << "/proc/bus/pci/";
        if (groupnr_) {
            path << std::setw(4) << std::setfill('0') << groupnr_ << ":";
        }
        path << std::setw(2) << std::setfill('0') << bus << "/" << std::setw(2) << std::setfill('0') << device << "." << function;

        //    std::cout << "PciHandle: Opening "<<path.str()<<"\n";

        int handle = ::open(path.str().c_str(), O_RDWR);
        if (handle < 0) {
            if (errno == 24) std::cerr << "ERROR: " << PCM_ULIMIT_RECOMMENDATION;
            handle = ::open((std::string("/pcm") + path.str()).c_str(), O_RDWR);
        }
        return handle;
    }

    PciHandle::PciHandle(uint32 groupnr_, uint32 bus_, uint32 device_, uint32 function_):
        fd(-1),
        bus(bus_),
        device(device_),
        function(function_) {
        int handle = openHandle(groupnr_, bus_, device_, function_);
        if (handle < 0) {
            throw std::runtime_error(std::string("PCM error: can't open PciHandle ")
                + std::to_string(groupnr_) + ":" + std::to_string(bus_) + ":" + std::to_string(device_) + ":" + std::to_string(function_));
        }
        fd = handle;

        // std::cout << "DEBUG: Opened "<< path.str().c_str() << " on handle "<< fd << "\n";
    }


    bool PciHandle::exists(uint32 groupnr_, uint32 bus_, uint32 device_, uint32 function_) {
        int handle = openHandle(groupnr_, bus_, device_, function_);

        if (handle < 0) return false;

        ::close(handle);

        return true;
    }

    int32 PciHandle::read32(uint64 offset, uint32 *value) {
        warnAlignment<4>("PciHandle::read32", false, offset);
        return ::pread(fd, (void *)value, sizeof(uint32), offset);
    }

    int32 PciHandle::write32(uint64 offset, uint32 value) {
        warnAlignment<4>("PciHandle::write32", false, offset);
        return ::pwrite(fd, (const void *)&value, sizeof(uint32), offset);
    }

    int32 PciHandle::read64(uint64 offset, uint64 *value) {
        warnAlignment<4>("PciHandle::read64", false, offset);
        size_t res = ::pread(fd, (void *)value, sizeof(uint64), offset);
        if (res != sizeof(uint64)) {
            std::cerr << " ERROR: pread from " << fd << " with offset 0x" << std::hex << offset << std::dec << " returned " << res << " bytes \n";
        }
        return res;
    }

    PciHandle::~PciHandle() {
        if (fd >= 0) ::close(fd);
    }

    int PciHandle::openMcfgTable() {
        const std::vector<std::string> base_paths = { "/sys/firmware/acpi/tables/MCFG", "/sys/firmware/acpi/tables/MCFG1" };
        std::vector<std::string> paths = base_paths;
        for (const auto &p : base_paths) {
            paths.push_back(std::string("/pcm") + p);
        }
        int handle = -1;
        for (const auto &p : paths) {
            if (handle < 0) {
                handle = ::open(p.c_str(), O_RDONLY);
            }
        }
        if (handle < 0) {
            for (const auto &p : paths) {
                std::cerr << "Can't open MCFG table. Check permission of " << p << "\n";
            }
        }
        return handle;
    }

#ifndef PCM_USE_PCI_MM_LINUX

    PciHandleM::PciHandleM(uint32 bus_, uint32 device_, uint32 function_):
        fd(-1),
        bus(bus_),
        device(device_),
        function(function_),
        base_addr(0) {
        int handle = ::open("/dev/mem", O_RDWR);
        if (handle < 0) throw std::exception();
        fd = handle;

        int mcfg_handle = PciHandle::openMcfgTable();
        if (mcfg_handle < 0) throw std::runtime_error("Cannot open any of /[pcm]/sys/firmware/acpi/tables/MCFG* files!");

        int32 result = ::pread(mcfg_handle, (void *)&base_addr, sizeof(uint64), 44);

        if (result != sizeof(uint64)) {
            ::close(mcfg_handle);
            throw std::exception();
        }

        unsigned char max_bus = 0;

        result = ::pread(mcfg_handle, (void *)&max_bus, sizeof(unsigned char), 55);

        ::close(mcfg_handle);
        if (result != sizeof(unsigned char)) {
            throw std::exception();
        }

        if (bus > (unsigned)max_bus) {
            std::cout << "WARNING: Requested bus number " << bus << " is larger than the max bus number " << (unsigned)max_bus << "\n";
            throw std::exception();
        }

        // std::cout << "PCI config base addr: "<< std::hex << base_addr<< "\n" << std::dec;

        base_addr += (bus * 1024ULL * 1024ULL + device * 32ULL * 1024ULL + function * 4ULL * 1024ULL);
    }


    bool PciHandleM::exists(uint32 /*groupnr_*/, uint32 /* bus_*/, uint32 /* device_ */, uint32 /* function_ */) {
        int handle = ::open("/dev/mem", O_RDWR);

        if (handle < 0) {
            perror("error opening /dev/mem");
            return false;
        }

        ::close(handle);

        handle = PciHandle::openMcfgTable();
        if (handle < 0) {
            return false;
        }

        ::close(handle);

        return true;
    }

    int32 PciHandleM::read32(uint64 offset, uint32 *value) {
        warnAlignment<4>("PciHandleM::read32", false, offset);
        return ::pread(fd, (void *)value, sizeof(uint32), offset + base_addr);
    }

    int32 PciHandleM::write32(uint64 offset, uint32 value) {
        warnAlignment<4>("PciHandleM::write32", false, offset);
        return ::pwrite(fd, (const void *)&value, sizeof(uint32), offset + base_addr);
    }

    int32 PciHandleM::read64(uint64 offset, uint64 *value) {
        warnAlignment<4>("PciHandleM::read64", false, offset);
        return ::pread(fd, (void *)value, sizeof(uint64), offset + base_addr);
    }

    PciHandleM::~PciHandleM() {
        if (fd >= 0) ::close(fd);
    }

#endif // PCM_USE_PCI_MM_LINUX

    // mmapped I/O version

    MCFGHeader PciHandleMM::mcfgHeader;
    std::vector<MCFGRecord> PciHandleMM::mcfgRecords;

    const std::vector<MCFGRecord> &PciHandleMM::getMCFGRecords() {
        readMCFG();
        return mcfgRecords;
    }

    void PciHandleMM::readMCFG() {
        if (mcfgRecords.size() > 0)
            return; // already initialized

        int mcfg_handle = PciHandle::openMcfgTable();
        if (mcfg_handle < 0) throw std::runtime_error("cannot open any of /[pcm]/sys/firmware/acpi/tables/MCFG* files!");

        ssize_t read_bytes = ::read(mcfg_handle, (void *)&mcfgHeader, sizeof(MCFGHeader));

        if (read_bytes == 0) {
            ::close(mcfg_handle);
            const auto msg = "PCM Error: Cannot read MCFG-table";
            std::cerr << msg;
            std::cerr << "\n";
            throw std::runtime_error(msg);
        }

        const unsigned segments = mcfgHeader.nrecords();
#ifdef PCM_DEBUG
        mcfgHeader.print();
        std::cout << "PCM Debug: total segments: " << segments << "\n";
#endif

        for (unsigned int i = 0; i < segments; ++i) {
            MCFGRecord record;
            read_bytes = ::read(mcfg_handle, (void *)&record, sizeof(MCFGRecord));
            if (read_bytes == 0) {
                ::close(mcfg_handle);
                const auto msg = "PCM Error: Cannot read MCFG-table (2)";
                std::cerr << msg;
                std::cerr << "\n";
                throw std::runtime_error(msg);
            }
#ifdef PCM_DEBUG
            std::cout << "PCM Debug: segment " << std::dec << i << " ";
            record.print();
#endif
            mcfgRecords.push_back(record);
        }

        ::close(mcfg_handle);
    }

    PciHandleMM::PciHandleMM(uint32 groupnr_, uint32 bus_, uint32 device_, uint32 function_):
        fd(-1),
        mmapAddr(NULL),
        bus(bus_),
        device(device_),
        function(function_),
        base_addr(0) {
        int handle = ::open("/dev/mem", O_RDWR);
        if (handle < 0) throw std::exception();
        fd = handle;

        readMCFG();

        unsigned segment = 0;
        for (; segment < mcfgRecords.size(); ++segment) {
            if (mcfgRecords[segment].PCISegmentGroupNumber == groupnr_
                && mcfgRecords[segment].startBusNumber <= bus_
                && bus <= mcfgRecords[segment].endBusNumber)
                break;
        }
        if (segment == mcfgRecords.size()) {
            std::cerr << "PCM Error: (group " << groupnr_ << ", bus " << bus_ << ") not found in the MCFG table.\n";
            throw std::exception();
        } else {
#ifdef PCM_DEBUG
            std::cout << "PCM Debug: (group " << groupnr_ << ", bus " << bus_ << ") found in the MCFG table in segment " << segment << "\n";
#endif
        }

        base_addr = mcfgRecords[segment].baseAddress;

        base_addr += (bus * 1024ULL * 1024ULL + device * 32ULL * 1024ULL + function * 4ULL * 1024ULL);

        mmapAddr = (char *)mmap(NULL, 4096, PROT_READ | PROT_WRITE, MAP_SHARED, fd, base_addr);

        if (mmapAddr == MAP_FAILED) {
            std::cout << "mmap failed: errno is " << errno << "\n";
            throw std::exception();
        }
    }

    bool PciHandleMM::exists(uint32 /*groupnr_*/, uint32 /*bus_*/, uint32 /*device_*/, uint32 /*function_*/) {
        int handle = ::open("/dev/mem", O_RDWR);

        if (handle < 0) {
            perror("error opening /dev/mem");
            return false;
        }

        ::close(handle);

        handle = PciHandle::openMcfgTable();

        if (handle < 0) {
            return false;
        }

        ::close(handle);

        return true;
    }


    int32 PciHandleMM::read32(uint64 offset, uint32 *value) {
        warnAlignment<4>("PciHandleMM::read32", false, offset);
        *value = *((uint32 *)(mmapAddr + offset));

        return sizeof(uint32);
    }

    int32 PciHandleMM::write32(uint64 offset, uint32 value) {
        warnAlignment<4>("PciHandleMM::write32", false, offset);
        *((uint32 *)(mmapAddr + offset)) = value;

        return sizeof(uint32);
    }

    int32 PciHandleMM::read64(uint64 offset, uint64 *value) {
        warnAlignment<4>("PciHandleMM::read64", false, offset);
        read32(offset, (uint32 *)value);
        read32(offset + sizeof(uint32), ((uint32 *)value) + 1);

        return sizeof(uint64);
    }

    PciHandleMM::~PciHandleMM() {
        if (mmapAddr) munmap(mmapAddr, 4096);
        if (fd >= 0) ::close(fd);
    }


} // namespace pcm
