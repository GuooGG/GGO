#include"bytearray.h"
#include"logSystem.h"
#include<vector>
#include<malloc.h>
#define LOG GGO_LOG_INFO(GGO_LOG_ROOT())

#define DATA_COUNT 128
#define BLOCK_SIZE 4096

void test_strings(){
    LOG << "test strings start";
    std::string hello = "hello";
    std::string world = "world";
    GGo::ByteArray ba(BLOCK_SIZE);
    ba.writeStringVarint(hello);
    ba.writeStringVarint(world);
    ba.setPosition(0);
    LOG << "string with varint size : " << ba.getSize();
    LOG << ba.readStringVarint() << ba.readStringVarint();
    ba.clear();
    ba.writeStringFixed32(hello);
    ba.writeStringFixed32(world);
    ba.setPosition(0);
    LOG << "string with uint32_t size : " << ba.getSize();
    LOG << ba.readStringVarint()  << " " << ba.readStringVarint();
    LOG << "test strings end";
}
 
void test_float(){
    LOG << "test float start";
    GGo::ByteArray ba(BLOCK_SIZE);
    float f1 = 3.1;
    float f2 = 3.14;
    float f3 = 3.141;
    ba.writeFloat(f1);
    ba.writeFloat(f2);
    ba.writeFloat(f3);
    LOG << "ba size : " << ba.getSize();
    ba.setPosition(0);
    LOG << ba.readFloat() << "  " << ba.readFloat() << "  " << ba.readFloat();
    LOG << "test float end";
}

void test_double()
{
    LOG << "test double start";
    GGo::ByteArray ba(BLOCK_SIZE);
    double f1 = 3.141592653;
    double f2 = 3.1417453453;
    double f3 = 3.141543453;
    ba.writeDouble(f1);
    ba.writeDouble(f2);
    ba.writeDouble(f3);
    LOG << "ba size : " << ba.getSize();
    ba.setPosition(0);
    LOG << ba.readDouble() << "  " << ba.readDouble() << "  " << ba.readDouble();
    LOG << "test double end";
}

void test_basic(){
    LOG << "basic test begin";
    GGo::ByteArray ba(BLOCK_SIZE);
    for(int i = 0; i < DATA_COUNT; i++){
        ba.writeFixeduint32(i + 1);
    }
    std::vector<uint32_t> vec;
    ba.setPosition(0);
    for(int i = 0; i < DATA_COUNT; i++){
        vec.push_back(ba.readFixedint32());
    }
    ba.setPosition(0);
    LOG << "READABLE SIZE= " << ba.getReadableSize() << std::endl;
    LOG << std::endl
        << ba.toString();

}

void test_file(){
    LOG << "test file start";
    GGo::ByteArray ba(BLOCK_SIZE);
    for (int i = 0; i < DATA_COUNT; i++)
    {
        ba.writeUint32(i + 1);
    }
    ba.setPosition(0);
    ba.writeToFile("../Test/tmp/ByteArrayTest01");
    LOG << "test file end";
}

void test_iovec(){
    LOG << "test iovec begin";
    GGo::ByteArray ba(BLOCK_SIZE);
    std::vector<iovec> iovecs;
    ba.getWriteBuffers(iovecs, 1024);
    LOG << iovecs.size();
    readv(STDOUT_FILENO, &iovecs[0], iovecs.size());
    // LOG << ba.getSize();
    // ba.setPosition(0);
    // ba.writeToFile("../Test/tmp/ByteArrayTest01.txt");
    LOG << "test iovec end";
}


int main(){
    // test_strings();
    // test_float();
    // test_double();
    // test_basic();
    // test_file();
    // test_iovec();
    return 0;
}