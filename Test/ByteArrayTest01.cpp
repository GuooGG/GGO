#include"bytearray.h"
#include"logSystem.h"
#include<vector>
#include<malloc.h>
#define LOG GGO_LOG_INFO(GGO_LOG_ROOT())

#define DATA_COUNT 128

void test_basic(){
    LOG << "basic test begin";
    GGo::ByteArray ba(16);
    for(int i = 0; i < DATA_COUNT; i++){
        ba.writeFixeduint32(i + 1);
    }
    std::vector<uint32_t> vec;
    ba.setPosition(0);
    for(int i = 0; i < DATA_COUNT; i++){
        vec.push_back(ba.readFixedint32());
    }
    for(int i = 0; i <= DATA_COUNT * 4; i++){
        ba.setPosition(i);
        LOG << "READABLE SIZE= " << ba.getReadableSize() << std::endl;
        LOG << std::endl
            << ba.toHexString();
    }

}

int main(){
    test_basic();
    return 0;
}