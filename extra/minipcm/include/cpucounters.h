#pragma once
#include "types.h"
#include "pci.h"
#include "mmio.h"
namespace pcm {

#define PCM_CPU_FAMILY_MODEL(family_, model_) (((family_) << 8) + (model_))

    enum SupportedCPUModels {
        NEHALEM_EP = PCM_CPU_FAMILY_MODEL(6, 26),
        NEHALEM = PCM_CPU_FAMILY_MODEL(6, 30),
        ATOM = PCM_CPU_FAMILY_MODEL(6, 28),
        ATOM_2 = PCM_CPU_FAMILY_MODEL(6, 53),
        CENTERTON = PCM_CPU_FAMILY_MODEL(6, 54),
        BAYTRAIL = PCM_CPU_FAMILY_MODEL(6, 55),
        AVOTON = PCM_CPU_FAMILY_MODEL(6, 77),
        CHERRYTRAIL = PCM_CPU_FAMILY_MODEL(6, 76),
        APOLLO_LAKE = PCM_CPU_FAMILY_MODEL(6, 92),
        GEMINI_LAKE = PCM_CPU_FAMILY_MODEL(6, 122),
        DENVERTON = PCM_CPU_FAMILY_MODEL(6, 95),
        SNOWRIDGE = PCM_CPU_FAMILY_MODEL(6, 134),
        ELKHART_LAKE = PCM_CPU_FAMILY_MODEL(6, 150),
        JASPER_LAKE = PCM_CPU_FAMILY_MODEL(6, 156),
        CLARKDALE = PCM_CPU_FAMILY_MODEL(6, 37),
        WESTMERE_EP = PCM_CPU_FAMILY_MODEL(6, 44),
        NEHALEM_EX = PCM_CPU_FAMILY_MODEL(6, 46),
        WESTMERE_EX = PCM_CPU_FAMILY_MODEL(6, 47),
        SANDY_BRIDGE = PCM_CPU_FAMILY_MODEL(6, 42),
        JAKETOWN = PCM_CPU_FAMILY_MODEL(6, 45),
        IVY_BRIDGE = PCM_CPU_FAMILY_MODEL(6, 58),
        HASWELL = PCM_CPU_FAMILY_MODEL(6, 60),
        HASWELL_ULT = PCM_CPU_FAMILY_MODEL(6, 69),
        HASWELL_2 = PCM_CPU_FAMILY_MODEL(6, 70),
        IVYTOWN = PCM_CPU_FAMILY_MODEL(6, 62),
        HASWELLX = PCM_CPU_FAMILY_MODEL(6, 63),
        BROADWELL = PCM_CPU_FAMILY_MODEL(6, 61),
        BROADWELL_XEON_E3 = PCM_CPU_FAMILY_MODEL(6, 71),
        BDX_DE = PCM_CPU_FAMILY_MODEL(6, 86),
        SKL_UY = PCM_CPU_FAMILY_MODEL(6, 78),
        KBL = PCM_CPU_FAMILY_MODEL(6, 158),
        KBL_1 = PCM_CPU_FAMILY_MODEL(6, 142),
        CML = PCM_CPU_FAMILY_MODEL(6, 166),
        CML_1 = PCM_CPU_FAMILY_MODEL(6, 165),
        ICL = PCM_CPU_FAMILY_MODEL(6, 126),
        ICL_1 = PCM_CPU_FAMILY_MODEL(6, 125),
        RKL = PCM_CPU_FAMILY_MODEL(6, 167),
        TGL = PCM_CPU_FAMILY_MODEL(6, 140),
        TGL_1 = PCM_CPU_FAMILY_MODEL(6, 141),
        ADL = PCM_CPU_FAMILY_MODEL(6, 151),
        ADL_1 = PCM_CPU_FAMILY_MODEL(6, 154),
        RPL = PCM_CPU_FAMILY_MODEL(6, 0xb7),
        RPL_1 = PCM_CPU_FAMILY_MODEL(6, 0xba),
        RPL_2 = PCM_CPU_FAMILY_MODEL(6, 0xbf),
        RPL_3 = PCM_CPU_FAMILY_MODEL(6, 0xbe),
        MTL = PCM_CPU_FAMILY_MODEL(6, 0xAA),
        LNL = PCM_CPU_FAMILY_MODEL(6, 0xBD),
        ARL = PCM_CPU_FAMILY_MODEL(6, 197),
        ARL_1 = PCM_CPU_FAMILY_MODEL(6, 198),
        BDX = PCM_CPU_FAMILY_MODEL(6, 79),
        KNL = PCM_CPU_FAMILY_MODEL(6, 87),
        SKL = PCM_CPU_FAMILY_MODEL(6, 94),
        SKX = PCM_CPU_FAMILY_MODEL(6, 85),
        ICX_D = PCM_CPU_FAMILY_MODEL(6, 108),
        ICX = PCM_CPU_FAMILY_MODEL(6, 106),
        SPR = PCM_CPU_FAMILY_MODEL(6, 143),
        EMR = PCM_CPU_FAMILY_MODEL(6, 207),
        GNR = PCM_CPU_FAMILY_MODEL(6, 173),
        SRF = PCM_CPU_FAMILY_MODEL(6, 175),
        GNR_D = PCM_CPU_FAMILY_MODEL(6, 174),
        GRR = PCM_CPU_FAMILY_MODEL(6, 182),
        END_OF_MODEL_LIST = 0x0ffff
    };

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
        uint32 cpu_family_model_;
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

        UncorePMU(const uint32 cpu_model,
            const HWRegisterPtr &unitControl_,
            const std::vector<HWRegisterPtr> &counterControl_,
            const std::vector<HWRegisterPtr> &counterValue_,
            const HWRegisterPtr &fixedCounterControl_ = HWRegisterPtr(),
            const HWRegisterPtr &fixedCounterValue_ = HWRegisterPtr(),
            const HWRegisterPtr &filter0 = HWRegisterPtr(),
            const HWRegisterPtr &filter1 = HWRegisterPtr()
        ): cpu_family_model_(cpu_model),
            unitControl(unitControl_),
            counterControl{ counterControl_ },
            counterValue{ counterValue_ },
            fixedCounterControl(fixedCounterControl_),
            fixedCounterValue(fixedCounterValue_),
            filter{ filter0 , filter1 } {
            assert(counterControl.size() == counterValue.size());
        };

        UncorePMU(const uint32 cpu_model,
            const HWRegisterPtr &unitControl_,
            const HWRegisterPtr &counterControl0,
            const HWRegisterPtr &counterControl1,
            const HWRegisterPtr &counterControl2,
            const HWRegisterPtr &counterControl3,
            const HWRegisterPtr &counterValue0,
            const HWRegisterPtr &counterValue1,
            const HWRegisterPtr &counterValue2,
            const HWRegisterPtr &counterValue3,
            const HWRegisterPtr &fixedCounterControl_,
            const HWRegisterPtr &fixedCounterValue_
        ):
            cpu_family_model_(cpu_model),
            unitControl(unitControl_),
            counterControl{ counterControl0, counterControl1, counterControl2, counterControl3 },
            counterValue{ counterValue0, counterValue1, counterValue2, counterValue3 },
            fixedCounterControl(fixedCounterControl_),
            fixedCounterValue(fixedCounterValue_) {
            assert(counterControl.size() == counterValue.size());
        }


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
            switch (cpu_family_model_) {
            case SPR:
            case EMR:
            case GNR:
            case GRR:
            case SRF:
                *unitControl = SPR_UNC_PMON_UNIT_CTL_FRZ;
                break;
            default:
                *unitControl = extra + UNC_PMON_UNIT_CTL_FRZ;
            }
        }
        bool initFreeze(const uint32 extra, const char *xPICheckMsg = nullptr) {
            if (unitControl.get() == nullptr) {
                return true; // this PMU does not have unit control register => no op
            }
            switch (cpu_family_model_) {
            case SPR:
            case EMR:
            case GNR:
            case GRR:
            case SRF:
                *unitControl = SPR_UNC_PMON_UNIT_CTL_FRZ; // freeze
                *unitControl = SPR_UNC_PMON_UNIT_CTL_FRZ + SPR_UNC_PMON_UNIT_CTL_RST_CONTROL; // freeze and reset control registers
                return true;
            }
            // freeze enable
            *unitControl = extra;
            if (xPICheckMsg) {
                if ((extra & UNC_PMON_UNIT_CTL_VALID_BITS_MASK) != ((*unitControl) & UNC_PMON_UNIT_CTL_VALID_BITS_MASK)) {
                    unitControl = nullptr;
                    return false;
                }
            }
            // freeze
            *unitControl = extra + UNC_PMON_UNIT_CTL_FRZ;
            return true;
        }

        void unfreeze(const uint32 extra) {
            switch (cpu_family_model_) {
            case SPR:
            case EMR:
            case GNR:
            case GRR:
            case SRF:
                *unitControl = 0;
                break;
            default:
                *unitControl = extra;
            }

        }
        void resetUnfreeze(const uint32 extra) {
            switch (cpu_family_model_) {
            case SPR:
            case EMR:
            case GNR:
            case GRR:
            case SRF:
                *unitControl = SPR_UNC_PMON_UNIT_CTL_FRZ + SPR_UNC_PMON_UNIT_CTL_RST_COUNTERS; // freeze and reset counter registers
                *unitControl = 0; // unfreeze
                return;
            }
            // reset counter values
            *unitControl = extra + UNC_PMON_UNIT_CTL_FRZ + UNC_PMON_UNIT_CTL_RST_COUNTERS;

            // unfreeze counters
            *unitControl = extra;
        }
    };

    void initSocket2Bus(std::vector<std::pair<uint32, uint32> > &socket2bus, uint32 device, uint32 function, const uint32 DEV_IDS[], uint32 devIdsSize);

    PciHandleType *createIntelPerfMonDevice(uint32 groupnr_, int32 bus_, uint32 dev_, uint32 func_, bool checkVendor);

    static const uint32 IMC_DEV_IDS[] = {
    0x03cb0,
    0x03cb1,
    0x03cb4,
    0x03cb5,
    0x0EB4,
    0x0EB5,
    0x0EB0,
    0x0EB1,
    0x0EF4,
    0x0EF5,
    0x0EF0,
    0x0EF1,
    0x2fb0,
    0x2fb1,
    0x2fb4,
    0x2fb5,
    0x2fd0,
    0x2fd1,
    0x2fd4,
    0x2fd5,
    0x6fb0,
    0x6fb1,
    0x6fb4,
    0x6fb5,
    0x6fd0,
    0x6fd1,
    0x6fd4,
    0x6fd5,
    0x2042,
    0x2046,
    0x204a,
    0x7840,
    0x7841,
    0x7842,
    0x7843,
    0x7844,
    0x781f
    };

}