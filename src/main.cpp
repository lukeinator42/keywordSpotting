#include <iostream>
#include <keywordSpotter/EmbeddingNetwork.h>

int main() {

    EmbeddingNetwork e("Hello World!");
    
    std::cout << e.getMsg() << std::endl;

    return 0;
}