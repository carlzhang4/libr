#include "cpucounters.h"
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

int main() {
    const size_t socket_ = 0;
    const double tsc_freq = 2.2;

    std::vector<std::vector< std::pair<uint32_t, uint32_t> > > MCRegisterLocation;
    std::vector<std::pair<uint32_t, uint32_t> > socket2iMCbus{};
    std::vector<uint32_t> num_imc_channels; // number of memory channels in each memory controller
    int32_t iMCbus;
    uint32_t groupnr;

    std::vector<pcm::UncorePMU> imcPMUs;

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

    groupnr = socket2iMCbus[socket_].first;
    iMCbus = socket2iMCbus[socket_].second;

    std::vector<std::shared_ptr<pcm::PciHandleType> > imcHandles;

    auto lastWorkingChannels = imcHandles.size();
    for (auto &ctrl : MCRegisterLocation) {
        for (auto &channel : ctrl) {
            std::cout << "iMCBus " << iMCbus << std::endl;
            pcm::PciHandleType *handle = pcm::createIntelPerfMonDevice(groupnr, iMCbus, channel.first, channel.second, true);
            if (handle) imcHandles.push_back(std::shared_ptr<pcm::PciHandleType>(handle));
        }
        if (imcHandles.size() > lastWorkingChannels) {
            num_imc_channels.push_back((uint32_t)(imcHandles.size() - lastWorkingChannels));
        }
        lastWorkingChannels = imcHandles.size();
    }

    for (auto &handle : imcHandles) {
        imcPMUs.push_back(
            pcm::UncorePMU(
                pcm::SKX,
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
    MCCntConfig[pcm::EventPosition::PARTIAL] = MC_CH_PCI_PMON_CTL_EVENT(0x04) + MC_CH_PCI_PMON_CTL_UMASK(2);

    const uint32_t extraIMC = UNC_PMON_UNIT_CTL_RSV;
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

    size_t prev_read = 0, prev_write = 0;
    size_t prev_tsc = 0;
    unsigned int dummy;

    for (size_t i = 0;i < 10;i++) {
        size_t now_tsc = __rdtscp(&dummy);
        for (size_t j = 0;j < imcPMUs.size();j++) {
            imcPMUs[j].freeze(UNC_PMON_UNIT_CTL_RSV);
        }
        size_t now_read = 0, now_write = 0;
        for (size_t j = 0;j < imcPMUs.size();j++) {
            now_read += *imcPMUs[0].counterValue[pcm::EventPosition::READ];
            now_write += *imcPMUs[0].counterValue[pcm::EventPosition::WRITE];
        }

        double read_bw_mb = (now_read - prev_read) * 1.0 * 64 * 1000 * tsc_freq / (now_tsc - prev_tsc);
        double write_bw_mb = (now_write - prev_write) * 1.0 * 64 * 1000 * tsc_freq / (now_tsc - prev_tsc);
        for (size_t j = 0;j < imcPMUs.size();j++) {
            imcPMUs[j].unfreeze(UNC_PMON_UNIT_CTL_RSV);
        }
        std::cout << "read_bw_mb: " << read_bw_mb << " write_bw_mb: " << write_bw_mb << std::endl;
        prev_read = now_read;
        prev_write = now_write;
        prev_tsc = now_tsc;

        sleep(1);
    }
    exit(0);

}   