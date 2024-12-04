#include "minipcm.h"

int main() {
    const size_t socket_ = 0;
    const double tsc_freq = 2.2;

    PcmMemory *pcmMemory = new SKXPcmMemory(socket_, tsc_freq);
    for (size_t i = 0;i < 100;i++) {
        pcmMemory->startRecord();
        sleep(1);
        auto [read_bw_mb, write_bw_mb] = pcmMemory->endRecord();
        std::cout << "NUMA " << socket_ << "  read_bw_mb: " << read_bw_mb << " write_bw_mb: " << write_bw_mb << std::endl;
    }
    exit(0);
}