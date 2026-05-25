#include "BlockQueue.hpp"
#include <vector>

int main() {
    BlockQueue<int> bq(5);
    std::atomic<int> counter(0);

    std::vector<std::thread> consumers, producers;

    for (int i = 0; i < 3; i++)
        consumers.emplace_back([&] {
            int val;
            while (bq.pop(val))
                std::cout << std::this_thread::get_id() << " consume: " << val << std::endl;
        });

    for (int i = 0; i < 2; i++)
        producers.emplace_back([&] {
            for (int j = 0; j < 10; j++)
                bq.push(counter++);
        });

    for (auto& t : producers) t.join();
    bq.stop();
    for (auto& t : consumers) t.join();
    return 0;
}
