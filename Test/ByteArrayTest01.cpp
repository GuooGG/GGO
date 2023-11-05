#include"bytearray.h"
#include"logSystem.h"
#include<malloc.h>
#define LOG GGO_LOG_INFO(GGO_LOG_ROOT())

void test_basic(){
    LOG << "basic test begin";
    GGo::ByteArray ba(512);
    ba.showInfo();
    int* buffer = new int(0x12345678);
    for(int i = 0; i < 100; i++){
        ba.write(buffer,32);
    }
    ba.showInfo();
}

int main(){
    test_basic();
    return 0;
}