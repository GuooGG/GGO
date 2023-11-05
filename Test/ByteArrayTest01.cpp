#include"bytearray.h"
#include"logSystem.h"
#include<vector>
#include<malloc.h>
#define LOG GGO_LOG_INFO(GGO_LOG_ROOT())

void test_basic(){
    LOG << "basic test begin";
    GGo::ByteArray ba(512);
    for(int i = 0; i <= 512; i++){
        ba.writeFixeduint32(i);
    }
    std::vector<uint32_t> vec;
    ba.setPosition(0);
    for(int i = 0; i <= 512; i++){
        vec.push_back(ba.readFixedint32());
        ba.setPosition(ba.getPosition()+sizeof(uint32_t));
    }
    for(auto i : vec){
        std::cout << i << std::endl;
    }

}

int main(){
    test_basic();
    return 0;
}