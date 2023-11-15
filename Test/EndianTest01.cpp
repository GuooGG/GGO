#include<iostream>
#include "GGo.h"
using namespace std;
int main(){
    float value = 3.14159f;
    if(GGO_BYTE_ORDER == GGO_BIG_ENDIAN){
        cout << "this machine is big endian" << endl;
    }else if( GGO_BYTE_ORDER == GGO_LITTLE_ENDIAN){
        cout << "this machine is little dndian" << endl;
    }
    cout << dec << "DEC value= " << value << endl;
    cout << hex << "HEX value= " << value << endl;
    cout << "==========================================" << endl;
    uint32_t swapped = GGo::byteswap(value);
    cout << dec << "DEC swapped= " << swapped << endl;
    cout << hex << "HEX swapped= " << swapped << endl;
    cout << "==========================================" << endl;
    swapped = GGo::byteswap(swapped);
    cout << dec << "DEC swapped= " << swapped << endl;
    cout << hex << "HEX swapped= " << swapped << endl;
    return 0;
}