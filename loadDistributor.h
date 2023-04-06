#pragma once

#include <functional>
#include <thread>
#include <vector>

#include <iostream>

class LoadDistributor {
public:
    using IndexProcessor = std::function<bool (int)>;

    void distributeLoadEvenly(int loadSize, int preferredNumberOfThreads, IndexProcessor indexProcessor) {
        const auto processBatch = [&indexProcessor](int start, int stop) {
            for (int i = start; i < stop; i++) {
                if (!indexProcessor(i)) {
                    break;
                }
            }
        };
        
        std::vector<std::thread> threads;
        const int length = static_cast<int>(std::ceil(static_cast<float>(loadSize) / preferredNumberOfThreads));
        int start = 0;
        int stop = start + length;
        while (stop <= loadSize) {
            threads.emplace_back([start, stop, processBatch] { std::cout << "Starting thread\n"; processBatch(start, stop); });
            start += length;
            stop += length;
        }

        if (start < loadSize) {
            threads.emplace_back([start, stop = loadSize, processBatch] { std::cout << "Starting thread\n"; processBatch(start, stop); });
        }

        for (std::thread& thread: threads) {
            thread.join();
        }
    }
};