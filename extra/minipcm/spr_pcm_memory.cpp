#include "uncore_pmu_discovery.h"
#include <x86intrin.h>

int main() {
    pcm::UncorePMUDiscovery *uncorePMUDiscovery = new pcm::UncorePMUDiscovery();
    const auto BoxType = pcm::SPR_IMC_BOX_TYPE;
    const size_t socket_ = 0;
    const double tsc_freq = 2.5;
    const auto numBoxes = uncorePMUDiscovery->getNumBoxes(BoxType, socket_);
    std::cout << "numBoxes: " << numBoxes << std::endl;
    std::vector<pcm::UncorePMU> imcPMUs;
    for (size_t pos = 0; pos < numBoxes; ++pos) {
        if (uncorePMUDiscovery->getBoxAccessType(BoxType, socket_, pos) == pcm::UncorePMUDiscovery::accessTypeEnum::MMIO) {
            std::vector<std::shared_ptr<pcm::HWRegister> > CounterControlRegs, CounterValueRegs;
            const auto n_regs = uncorePMUDiscovery->getBoxNumRegs(BoxType, socket_, pos);
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

            auto boxCtlRegister = makeRegister(uncorePMUDiscovery->getBoxCtlAddr(BoxType, socket_, pos), 32);
            if (boxCtlRegister.get()) {
                for (size_t r = 0; r < n_regs; ++r) {
                    CounterControlRegs.push_back(makeRegister(uncorePMUDiscovery->getBoxCtlAddr(BoxType, socket_, pos, r), 32));
                    CounterValueRegs.push_back(makeRegister(uncorePMUDiscovery->getBoxCtrAddr(BoxType, socket_, pos, r), 64));
                }
                std::cout << n_regs << ' ' << pos << std::endl;
                imcPMUs.push_back(pcm::UncorePMU(pcm::SPR,
                    boxCtlRegister,
                    CounterControlRegs,
                    CounterValueRegs,
                    makeRegister(uncorePMUDiscovery->getBoxCtlAddr(BoxType, socket_, pos) + pcm::SERVER_MC_CH_PMON_FIXED_CTL_OFFSET, 32),
                    makeRegister(uncorePMUDiscovery->getBoxCtlAddr(BoxType, socket_, pos) + pcm::SERVER_MC_CH_PMON_FIXED_CTR_OFFSET, 64)));

            }
        }
    }
    uint32_t MCCntConfig[4] = { 0,0,0,0 };

    MCCntConfig[pcm::EventPosition::READ] = MC_CH_PCI_PMON_CTL_EVENT(0x05) + MC_CH_PCI_PMON_CTL_UMASK(0xcf);  // monitor reads on counter 0: CAS_COUNT.RD
    MCCntConfig[pcm::EventPosition::WRITE] = MC_CH_PCI_PMON_CTL_EVENT(0x05) + MC_CH_PCI_PMON_CTL_UMASK(0xf0); // monitor writes on counter 1: CAS_COUNT.WR
    MCCntConfig[pcm::EventPosition::PMM_READ] = MC_CH_PCI_PMON_CTL_EVENT(0xe3);  // monitor PMM_RDQ_REQUESTS on counter 2
    MCCntConfig[pcm::EventPosition::PMM_WRITE] = MC_CH_PCI_PMON_CTL_EVENT(0xe7); // monitor PMM_WPQ_REQUESTS on counter 3

    const uint32_t extraIMC = UNC_PMON_UNIT_CTL_FRZ_EN;

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

    size_t prev_read = 0, prev_write = 0;
    size_t prev_tsc = 0;
    unsigned int dummy;

    for (size_t i = 0;i < 100;i++) {
        size_t now_tsc = __rdtscp(&dummy);
        for (size_t j = 0;j < numBoxes;j++) {
            imcPMUs[j].freeze(UNC_PMON_UNIT_CTL_FRZ_EN);
        }
        size_t now_read = 0, now_write = 0;
        for (size_t j = 0;j < numBoxes;j++) {
            now_read += *imcPMUs[j].counterValue[pcm::EventPosition::READ];
            now_write += *imcPMUs[j].counterValue[pcm::EventPosition::WRITE];
        }

        double read_bw_mb = (now_read - prev_read) * 1.0 * 64 * 1000 * tsc_freq / (now_tsc - prev_tsc);
        double write_bw_mb = (now_write - prev_write) * 1.0 * 64 * 1000 * tsc_freq / (now_tsc - prev_tsc);
        for (size_t j = 0;j < numBoxes;j++) {
            imcPMUs[j].unfreeze(UNC_PMON_UNIT_CTL_FRZ_EN);
        }
        std::cout << "read_bw_mb: " << read_bw_mb << " write_bw_mb: " << write_bw_mb << std::endl;
        prev_read = now_read;
        prev_write = now_write;
        prev_tsc = now_tsc;

        sleep(1);
    }
    exit(0);
}