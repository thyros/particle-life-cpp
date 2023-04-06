#include "loadDistributor.h"
#include <iostream>

int main() {

    const auto indexProcessor = [](int index) -> bool 
    { 
        // std::cout << "Index: " << index << "\n"; 
        std::this_thread::sleep_for(std::chrono::milliseconds(10)); 
        return true; 
    };

    LoadDistributor loadDistributor;

    loadDistributor.distributeLoadEvenly(1009, 10, indexProcessor);
}