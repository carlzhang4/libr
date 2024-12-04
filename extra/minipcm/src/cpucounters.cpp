#include "cpucounters.h"

namespace pcm {
    void initSocket2Bus(std::vector<std::pair<uint32, uint32> > &socket2bus, uint32 device, uint32 function, const uint32 DEV_IDS[], uint32 devIdsSize) {
        if (device == PCM_INVALID_DEV_ADDR || function == PCM_INVALID_FUNC_ADDR) {
            return;
        }
        if (!socket2bus.empty()) return;

        forAllIntelDevices(
            [&devIdsSize, &DEV_IDS, &socket2bus](const uint32 group, const uint32 bus, const uint32 /* device */, const uint32 /* function */, const uint32 device_id) {
                for (uint32 i = 0; i < devIdsSize; ++i) {
                    // match
                    if (DEV_IDS[i] == device_id) {
                        // std::cout << "DEBUG: found bus " << std::hex << bus << " with device ID " << device_id << std::dec << "\n";
                        socket2bus.push_back(std::make_pair(group, bus));
                        break;
                    }
                }
            }, device, function);
        //std::cout << std::flush;
    }
    PciHandleType *createIntelPerfMonDevice(uint32 groupnr_, int32 bus_, uint32 dev_, uint32 func_, bool checkVendor) {
        std::cout << groupnr_ << " " << bus_ << " " << dev_ << " " << func_ << " " << checkVendor << std::endl;
        if (PciHandleType::exists(groupnr_, (uint32)bus_, dev_, func_)) {
            std::cout << "exists" << std::endl;
            PciHandleType *handle = new PciHandleType(groupnr_, bus_, dev_, func_);

            if (!checkVendor) return handle;

            uint32 vendor_id = 0;
            handle->read32(PCM_PCI_VENDOR_ID_OFFSET, &vendor_id);
            vendor_id &= 0x0ffff;

            if (vendor_id == PCM_INTEL_PCI_VENDOR_ID) return handle;

            if (handle) {
                delete handle;
                handle = nullptr;
            }
        }
        return NULL;
    }
}