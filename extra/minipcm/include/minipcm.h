#pragma once

#include "cpucounters.h"
#include "uncore_pmu_discovery.h"
#include <x86intrin.h>

constexpr auto SKX_MC0_CH0_REGISTER_DEV_ADDR = 10;
constexpr auto SKX_MC0_CH1_REGISTER_DEV_ADDR = 10;
constexpr auto SKX_MC0_CH2_REGISTER_DEV_ADDR = 11;
constexpr auto SKX_MC0_CH3_REGISTER_DEV_ADDR = -1; //Does not exist
constexpr auto SKX_MC0_CH0_REGISTER_FUNC_ADDR = 2;
constexpr auto SKX_MC0_CH1_REGISTER_FUNC_ADDR = 6;
constexpr auto SKX_MC0_CH2_REGISTER_FUNC_ADDR = 2;
constexpr auto SKX_MC0_CH3_REGISTER_FUNC_ADDR = -1; //Does not exist

constexpr auto SKX_MC1_CH0_REGISTER_DEV_ADDR = 12;
constexpr auto SKX_MC1_CH1_REGISTER_DEV_ADDR = 12;
constexpr auto SKX_MC1_CH2_REGISTER_DEV_ADDR = 13;
constexpr auto SKX_MC1_CH3_REGISTER_DEV_ADDR = -1; //Does not exist
constexpr auto SKX_MC1_CH0_REGISTER_FUNC_ADDR = 2;
constexpr auto SKX_MC1_CH1_REGISTER_FUNC_ADDR = 6;
constexpr auto SKX_MC1_CH2_REGISTER_FUNC_ADDR = 2;
constexpr auto SKX_MC1_CH3_REGISTER_FUNC_ADDR = -1; //Does not exist


class PcmMemory {
public:
    std::vector<pcm::UncorePMU>imcPMUs;
    uint32_t socketid;
    uint32_t cpuid;
    double tsc_freq;
    uint32_t extraIMC;

    size_t start_tsc;
    std::vector<size_t> start_read_value;
    std::vector<size_t> start_write_value;
    PcmMemory(uint32_t socketid_, uint32_t cpuid_, double tsc_freq_): socketid(socketid_), cpuid(cpuid_), tsc_freq(tsc_freq_) {
        if (cpuid != pcm::SPR && cpuid != pcm::SKX) {
            std::cerr << "Unsupported CPU model" << std::endl;
            exit(1);
        }
        if (cpuid == pcm::SPR) {
            extraIMC = UNC_PMON_UNIT_CTL_FRZ_EN;
        } else if (cpuid == pcm::SKX) {
            extraIMC = UNC_PMON_UNIT_CTL_RSV;
        }
    }
    void startRecord() {
        start_tsc = __rdtscp(&socketid);
        for (size_t i = 0;i < imcPMUs.size();i++) {
            imcPMUs[i].freeze(extraIMC);
        }

        for (size_t i = 0;i < imcPMUs.size();i++) {
            start_read_value[i] = *imcPMUs[i].counterValue[pcm::EventPosition::READ];
            start_write_value[i] = *imcPMUs[i].counterValue[pcm::EventPosition::WRITE];
        }

        for (size_t i = 0; i < imcPMUs.size(); i++) {
            imcPMUs[i].unfreeze(extraIMC);
        }
    }
    // return read_bw_mb, write_bw_mb between startRecord and endRecord
    std::pair<double, double> endRecord() {
        size_t end_tsc = __rdtscp(&socketid);
        size_t total_read = 0, total_write = 0;
        for (size_t i = 0;i < imcPMUs.size();i++) {
            imcPMUs[i].freeze(extraIMC);
        }
        for (size_t i = 0;i < imcPMUs.size();i++) {
            total_read += *imcPMUs[i].counterValue[pcm::EventPosition::READ] - start_read_value[i];
            total_write += *imcPMUs[i].counterValue[pcm::EventPosition::WRITE] - start_write_value[i];
        }
        for (size_t i = 0; i < imcPMUs.size(); i++) {
            imcPMUs[i].unfreeze(extraIMC);
        }

        double read_bw_mb = total_read * 64.0 * 1000 * tsc_freq / (end_tsc - start_tsc);
        double write_bw_mb = total_write * 64.0 * 1000 * tsc_freq / (end_tsc - start_tsc);
        return std::make_pair(read_bw_mb, write_bw_mb);
    }
    virtual ~PcmMemory() {}
};

class SKXPcmMemory: public PcmMemory {
    int32_t iMCbus;
    uint32_t groupnr;
    std::vector<std::vector< std::pair<uint32_t, uint32_t> > > MCRegisterLocation;
    std::vector<std::pair<uint32_t, uint32_t> > socket2iMCbus{};
    std::vector<std::shared_ptr<pcm::PciHandleType> > imcHandles;

public:
    SKXPcmMemory(uint32_t socketid_, double tsc_freq_):PcmMemory(socketid_, pcm::SKX, tsc_freq_) {
        PCM_PCICFG_MC_INIT(0, 0, SKX);
        PCM_PCICFG_MC_INIT(0, 1, SKX);
        PCM_PCICFG_MC_INIT(0, 2, SKX);
        PCM_PCICFG_MC_INIT(0, 3, SKX);
        PCM_PCICFG_MC_INIT(1, 0, SKX);
        PCM_PCICFG_MC_INIT(1, 1, SKX);
        PCM_PCICFG_MC_INIT(1, 2, SKX);
        PCM_PCICFG_MC_INIT(1, 3, SKX);

        pcm::initSocket2Bus(socket2iMCbus, MCRegisterLocation[0][0].first, MCRegisterLocation[0][0].second, pcm::IMC_DEV_IDS, (uint32_t)sizeof(pcm::IMC_DEV_IDS) / sizeof(pcm::IMC_DEV_IDS[0]));

        assert(socket2iMCbus.size() == 2);

        groupnr = socket2iMCbus[socketid].first;
        iMCbus = socket2iMCbus[socketid].second;
        for (auto &ctrl : MCRegisterLocation) {
            for (auto &channel : ctrl) {
                pcm::PciHandleType *handle = pcm::createIntelPerfMonDevice(groupnr, iMCbus, channel.first, channel.second, true);
                if (handle) imcHandles.push_back(std::shared_ptr<pcm::PciHandleType>(handle));
            }
        }
        for (auto &handle : imcHandles) {
            imcPMUs.push_back(
                pcm::UncorePMU(
                    cpuid,
                    std::make_shared<pcm::PCICFGRegister32>(handle, pcm::XPF_MC_CH_PCI_PMON_BOX_CTL_ADDR),
                    std::make_shared<pcm::PCICFGRegister32>(handle, pcm::XPF_MC_CH_PCI_PMON_CTL0_ADDR),
                    std::make_shared<pcm::PCICFGRegister32>(handle, pcm::XPF_MC_CH_PCI_PMON_CTL1_ADDR),
                    std::make_shared<pcm::PCICFGRegister32>(handle, pcm::XPF_MC_CH_PCI_PMON_CTL2_ADDR),
                    std::make_shared<pcm::PCICFGRegister32>(handle, pcm::XPF_MC_CH_PCI_PMON_CTL3_ADDR),
                    std::make_shared<pcm::PCICFGRegister64>(handle, pcm::XPF_MC_CH_PCI_PMON_CTR0_ADDR),
                    std::make_shared<pcm::PCICFGRegister64>(handle, pcm::XPF_MC_CH_PCI_PMON_CTR1_ADDR),
                    std::make_shared<pcm::PCICFGRegister64>(handle, pcm::XPF_MC_CH_PCI_PMON_CTR2_ADDR),
                    std::make_shared<pcm::PCICFGRegister64>(handle, pcm::XPF_MC_CH_PCI_PMON_CTR3_ADDR),
                    std::make_shared<pcm::PCICFGRegister32>(handle, pcm::XPF_MC_CH_PCI_PMON_FIXED_CTL_ADDR),
                    std::make_shared<pcm::PCICFGRegister64>(handle, pcm::XPF_MC_CH_PCI_PMON_FIXED_CTR_ADDR))
            );
        }

        uint32_t MCCntConfig[4] = { 0,0,0,0 };
        MCCntConfig[pcm::EventPosition::READ] = MC_CH_PCI_PMON_CTL_EVENT(0x04) + MC_CH_PCI_PMON_CTL_UMASK(3);  // monitor reads on counter 0: CAS_COUNT.RD
        MCCntConfig[pcm::EventPosition::WRITE] = MC_CH_PCI_PMON_CTL_EVENT(0x04) + MC_CH_PCI_PMON_CTL_UMASK(12); // monitor writes on counter 1: CAS_COUNT.WR
        MCCntConfig[pcm::EventPosition::PMM_READ] = MC_CH_PCI_PMON_CTL_EVENT(0xe3);
        MCCntConfig[pcm::EventPosition::PMM_WRITE] = MC_CH_PCI_PMON_CTL_EVENT(0xe7);

        for (size_t i = 0;i < imcPMUs.size();i++) {
            imcPMUs[i].initFreeze(extraIMC);
            //enable fixed counter (DRAM clocks) 
            *imcPMUs[i].fixedCounterControl = MC_CH_PCI_PMON_FIXED_CTL_EN;
            // reset it
            *imcPMUs[i].fixedCounterControl = MC_CH_PCI_PMON_FIXED_CTL_EN + MC_CH_PCI_PMON_FIXED_CTL_RST;

            for (size_t j = 0;j < 4;j++) {
                auto ctrl = imcPMUs[i].counterControl[j];
                if (ctrl.get() != nullptr) {
                    *ctrl = MC_CH_PCI_PMON_CTL_EN;
                    *ctrl = MC_CH_PCI_PMON_CTL_EN | MCCntConfig[j];
                }
            }
            imcPMUs[i].resetUnfreeze(extraIMC);
        }
        start_read_value.resize(imcPMUs.size());
        start_write_value.resize(imcPMUs.size());
    }

    ~SKXPcmMemory() {}
};

class SPRPcmMemory: public PcmMemory {
    pcm::UncorePMUDiscovery *uncorePMUDiscovery;
    const unsigned int BoxType = pcm::SPR_IMC_BOX_TYPE;
public:
    SPRPcmMemory(uint32_t socketid_, double tsc_freq_):PcmMemory(socketid_, pcm::SPR, tsc_freq_) {
        uncorePMUDiscovery = new pcm::UncorePMUDiscovery();
        const auto numBoxes = uncorePMUDiscovery->getNumBoxes(BoxType, socketid);
        for (size_t pos = 0; pos < numBoxes; ++pos) {
            if (uncorePMUDiscovery->getBoxAccessType(BoxType, socketid, pos) == pcm::UncorePMUDiscovery::accessTypeEnum::MMIO) {
                std::vector<std::shared_ptr<pcm::HWRegister> > CounterControlRegs, CounterValueRegs;
                const auto n_regs = uncorePMUDiscovery->getBoxNumRegs(BoxType, socketid, pos);
                auto makeRegister = [](const uint64_t rawAddr, const uint32_t bits) -> std::shared_ptr<pcm::HWRegister> {
                    const auto mapSize = pcm::SERVER_MC_CH_PMON_SIZE;
                    const auto alignedAddr = rawAddr & ~4095ULL;
                    const auto alignDelta = rawAddr & 4095ULL;
                    try {
                        auto handle = std::make_shared<pcm::MMIORange>(alignedAddr, mapSize, false);
                        assert(handle.get());
                        switch (bits) {
                        case 32:
                            return std::make_shared<pcm::MMIORegister32>(handle, (size_t)alignDelta);
                        case 64:
                            return std::make_shared<pcm::MMIORegister64>(handle, (size_t)alignDelta);
                        }
                    }
                    catch (...) {
                    }
                    return std::shared_ptr<pcm::HWRegister>();
                    };

                auto boxCtlRegister = makeRegister(uncorePMUDiscovery->getBoxCtlAddr(BoxType, socketid, pos), 32);
                if (boxCtlRegister.get()) {
                    for (size_t r = 0; r < n_regs; ++r) {
                        CounterControlRegs.push_back(makeRegister(uncorePMUDiscovery->getBoxCtlAddr(BoxType, socketid, pos, r), 32));
                        CounterValueRegs.push_back(makeRegister(uncorePMUDiscovery->getBoxCtrAddr(BoxType, socketid, pos, r), 64));
                    }
                    std::cout << n_regs << ' ' << pos << std::endl;
                    imcPMUs.push_back(pcm::UncorePMU(pcm::SPR,
                        boxCtlRegister,
                        CounterControlRegs,
                        CounterValueRegs,
                        makeRegister(uncorePMUDiscovery->getBoxCtlAddr(BoxType, socketid, pos) + pcm::SERVER_MC_CH_PMON_FIXED_CTL_OFFSET, 32),
                        makeRegister(uncorePMUDiscovery->getBoxCtlAddr(BoxType, socketid, pos) + pcm::SERVER_MC_CH_PMON_FIXED_CTR_OFFSET, 64)));

                }
            }
        }
        uint32_t MCCntConfig[4] = { 0,0,0,0 };

        MCCntConfig[pcm::EventPosition::READ] = MC_CH_PCI_PMON_CTL_EVENT(0x05) + MC_CH_PCI_PMON_CTL_UMASK(0xcf);  // monitor reads on counter 0: CAS_COUNT.RD
        MCCntConfig[pcm::EventPosition::WRITE] = MC_CH_PCI_PMON_CTL_EVENT(0x05) + MC_CH_PCI_PMON_CTL_UMASK(0xf0); // monitor writes on counter 1: CAS_COUNT.WR
        MCCntConfig[pcm::EventPosition::PMM_READ] = MC_CH_PCI_PMON_CTL_EVENT(0xe3);  // monitor PMM_RDQ_REQUESTS on counter 2
        MCCntConfig[pcm::EventPosition::PMM_WRITE] = MC_CH_PCI_PMON_CTL_EVENT(0xe7); // monitor PMM_WPQ_REQUESTS on counter 3

        for (size_t i = 0;i < imcPMUs.size();i++) {
            imcPMUs[i].initFreeze(extraIMC);
            //enable fixed counter (DRAM clocks) 
            *imcPMUs[i].fixedCounterControl = MC_CH_PCI_PMON_FIXED_CTL_EN;
            // reset it
            *imcPMUs[i].fixedCounterControl = MC_CH_PCI_PMON_FIXED_CTL_EN + MC_CH_PCI_PMON_FIXED_CTL_RST;

            for (size_t j = 0;j < 4;j++) {
                auto ctrl = imcPMUs[i].counterControl[j];
                if (ctrl.get() != nullptr) {
                    *ctrl = MCCntConfig[j];
                }
            }
            imcPMUs[i].resetUnfreeze(extraIMC);
        }
    }

    ~SPRPcmMemory() {
        delete uncorePMUDiscovery;
    }
};