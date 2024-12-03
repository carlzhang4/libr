#include "uncore_pmu_discovery.h"

int main() {
    pcm::UncorePMUDiscovery *uncorePMUDiscovery = new pcm::UncorePMUDiscovery();
    const auto BoxType = pcm::SPR_IMC_BOX_TYPE;
    const size_t socket_ = 0;

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
                imcPMUs.push_back(pcm::UncorePMU(boxCtlRegister,
                    CounterControlRegs,
                    CounterValueRegs,
                    makeRegister(uncorePMUDiscovery->getBoxCtlAddr(BoxType, socket_, pos) + pcm::SERVER_MC_CH_PMON_FIXED_CTL_OFFSET, 32),
                    makeRegister(uncorePMUDiscovery->getBoxCtlAddr(BoxType, socket_, pos) + pcm::SERVER_MC_CH_PMON_FIXED_CTR_OFFSET, 64)));
                std::cout << "imc push boxCtlRegister" << std::endl;

            }
        }
    }
}