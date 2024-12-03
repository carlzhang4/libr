#pragma once
#include "types.h"
#include "pci.h"
#include "mmio.h"
namespace pcm {
    class HWRegister {
    public:
        virtual void operator = (uint64 val) = 0; // write operation
        virtual operator uint64 () = 0; //read operation
        virtual ~HWRegister() {}
    };

    class PCICFGRegister64: public HWRegister {
        std::shared_ptr<PciHandleType> handle;
        size_t offset;
    public:
        PCICFGRegister64(const std::shared_ptr<PciHandleType> &handle_, size_t offset_):
            handle(handle_),
            offset(offset_) {
        }
        void operator = (uint64 val) override {
            cvt_ds cvt;
            cvt.ui64 = val;
            handle->write32(offset, cvt.ui32.low);
            handle->write32(offset + sizeof(uint32), cvt.ui32.high);
        }
        operator uint64 ()  override {
            uint64 result = 0;
            handle->read64(offset, &result);
            return result;
        }
    };

    class PCICFGRegister32: public HWRegister {
        std::shared_ptr<PciHandleType> handle;
        size_t offset;
    public:
        PCICFGRegister32(const std::shared_ptr<PciHandleType> &handle_, size_t offset_):
            handle(handle_),
            offset(offset_) {
        }
        void operator = (uint64 val) override {
            handle->write32(offset, (uint32)val);
        }
        operator uint64 () override {
            uint32 result = 0;
            handle->read32(offset, &result);
            return result;
        }
    };

    class MMIORegister64: public HWRegister {
        std::shared_ptr<MMIORange> handle;
        size_t offset;
    public:
        MMIORegister64(const std::shared_ptr<MMIORange> &handle_, size_t offset_):
            handle(handle_),
            offset(offset_) {
        }
        void operator = (uint64 val) override {
            // std::cout << std::hex << "MMIORegister64 writing " << val << " at offset " << offset << std::dec << std::endl;
            handle->write64(offset, val);
        }
        operator uint64 () override {
            const uint64 val = handle->read64(offset);
            // std::cout << std::hex << "MMIORegister64 read " << val << " from offset " << offset << std::dec << std::endl;
            return val;
        }
    };

    class MMIORegister32: public HWRegister {
        std::shared_ptr<MMIORange> handle;
        size_t offset;
    public:
        MMIORegister32(const std::shared_ptr<MMIORange> &handle_, size_t offset_):
            handle(handle_),
            offset(offset_) {
        }
        void operator = (uint64 val) override {
            // std::cout << std::hex << "MMIORegister32 writing " << val << " at offset " << offset << std::dec << std::endl;
            handle->write32(offset, (uint32)val);
        }
        operator uint64 () override {
            const uint64 val = (uint64)handle->read32(offset);
            // std::cout << std::hex << "MMIORegister32 read " << val << " from offset " << offset << std::dec << std::endl;
            return val;
        }
    };

    enum EventPosition {
        READ = 0,
        WRITE = 1,
        READ2 = 2,
        WRITE2 = 3,
        READ_RANK_A = 0,
        WRITE_RANK_A = 1,
        READ_RANK_B = 2,
        WRITE_RANK_B = 3,
        PARTIAL = 2,
        PMM_READ = 2,
        PMM_WRITE = 3,
        MM_MISS_CLEAN = 2,
        MM_MISS_DIRTY = 3,
        NM_HIT = 0,  // NM :  Near Memory (DRAM cache) in Memory Mode
        M2M_CLOCKTICKS = 1
    };

    class UncorePMU {
        typedef std::shared_ptr<HWRegister> HWRegisterPtr;
        HWRegisterPtr unitControl;
    public:
        std::vector<HWRegisterPtr> counterControl;
        std::vector<HWRegisterPtr> counterValue;
        HWRegisterPtr fixedCounterControl;
        HWRegisterPtr fixedCounterValue;
        HWRegisterPtr filter[2];
        enum {
            maxCounters = 8
        };

        UncorePMU(const HWRegisterPtr &unitControl_,
            const std::vector<HWRegisterPtr> &counterControl_,
            const std::vector<HWRegisterPtr> &counterValue_,
            const HWRegisterPtr &fixedCounterControl_ = HWRegisterPtr(),
            const HWRegisterPtr &fixedCounterValue_ = HWRegisterPtr(),
            const HWRegisterPtr &filter0 = HWRegisterPtr(),
            const HWRegisterPtr &filter1 = HWRegisterPtr()
        ):unitControl(unitControl_),
            counterControl{ counterControl_ },
            counterValue{ counterValue_ },
            fixedCounterControl(fixedCounterControl_),
            fixedCounterValue(fixedCounterValue_),
            filter{ filter0 , filter1 } {
            assert(counterControl.size() == counterValue.size());
        };

        UncorePMU() {}
        size_t size() const { return counterControl.size(); }
        virtual ~UncorePMU() {}
        bool valid() const {
            return unitControl.get() != nullptr;
        }
        void cleanup() {
            for (auto &cc : counterControl) {
                if (cc.get()) *cc = 0;
            }
            if (unitControl.get()) *unitControl = 0;
            if (fixedCounterControl.get()) *fixedCounterControl = 0;
        }
        void freeze(const uint32 extra) {
            *unitControl = SPR_UNC_PMON_UNIT_CTL_FRZ;
        }
        bool initFreeze(const uint32 extra, const char *xPICheckMsg = nullptr) {
            if (unitControl.get() == nullptr) {
                return true; // this PMU does not have unit control register => no op
            }
            *unitControl = SPR_UNC_PMON_UNIT_CTL_FRZ; // freeze
            *unitControl = SPR_UNC_PMON_UNIT_CTL_FRZ + SPR_UNC_PMON_UNIT_CTL_RST_CONTROL; // freeze and reset control registers
            return true;
        }
        void unfreeze(const uint32 extra) {
            *unitControl = 0;
        }
        void resetUnfreeze(const uint32 extra) {
            *unitControl = SPR_UNC_PMON_UNIT_CTL_FRZ + SPR_UNC_PMON_UNIT_CTL_RST_COUNTERS; // freeze and reset counter registers
            *unitControl = 0; // unfreeze
            return;
        }
    };

}