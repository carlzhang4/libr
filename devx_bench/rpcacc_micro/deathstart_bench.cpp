#include "rpcacc_micro.h"
#include "bench.h"

#define DEATHSTARBENCH
#include "serdes_main.h"

using namespace std;
Metadata metadataVec[1000];
size_t messageSizeVec[1000];

size_t tmp = 0;
int main(int argc, char *argv[]) {
    signal(SIGINT, ctrl_c_handler);
    signal(SIGTERM, ctrl_c_handler);


    M_base *top_msg_ptr = BenchmarkInit(0, metadataVec, messageSizeVec);
    size_t total_size = ByteSizeLong(top_msg_ptr, false);

    M1 *m1 = reinterpret_cast<M1 *>(top_msg_ptr);


    for (size_t index = 0;index < 1000;index++) {
        std::string str = m1->get_first_name() + m1->get_last_name() + m1->get_username() + m1->get_password();


    }
}