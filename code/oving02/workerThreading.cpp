#include <iostream>
#include <thread>
#include <vector>
#include <functional>
#include <mutex>
#include <condition_variable>
#include <queue>

class Workers {
public:
    explicit Workers(size_t num_threads) : num_threads(num_threads) {}
    
    // Worker thread start
    void start() {
        for (size_t i = 0; i < num_threads; ++i) {
            threads.emplace_back([this] { worker_loop(); });
        }
    }

    // Worker thread post task
    void post(std::function<void()> task) {
        {
            std::lock_guard<std::mutex> lock(mtx);
            if (stop) return;
            tasks.push(std::move(task));
        }
        cv.notify_one();
    }
    
    // Worker thread join
    void join() {
        {   // lock scope
            std::lock_guard<std::mutex> lock(mtx);
            stop = true;
        }
        cv.notify_all();

        for (std::thread &thread : threads) {
            if (thread.joinable()) thread.join();
        }
        threads.clear();
    }

    // Worker destructor
    ~Workers() {
        join();
    };

private:
    size_t num_threads;
    std::vector<std::thread> threads;
    std::mutex mtx;
    std::condition_variable cv;
    std::queue<std::function<void()>> tasks;
    bool stop = false;

    void worker_loop() {
        while (true) {
            std::function<void()> task;
            {   // lock scope
                std::unique_lock<std::mutex> lock(mtx);
                cv.wait(lock, [this] { return stop || !tasks.empty(); });
                if (stop && tasks.empty()) return;
                task = std::move(tasks.front());
                tasks.pop();
            }
            task();
        }
    }
};

/*  OPPGAVE:

    Workers worker_threads(4);
    Workers event_loop(1);
    worker_threads.start(); // Create 4 internal threads
    event_loop.start(); // Create 1 internal thread
    worker_threads.post([] {
    // Task A
    });
    worker_threads.post([] {
    // Task B
    // Might run in parallel with task A
    });
    event_loop.post([] {
    // Task C
    // Might run in parallel with task A and B
    });
    event_loop.post([] {
    // Task D
    // Will run after task C
    // Might run in parallel with task A and B
    });
    worker_threads.join(); // Calls join () on the worker threads
    event_loop.join(); // Calls join () on the event thread
*/

int main() {
    Workers worker_threads(4);
    Workers event_loop(1);
    worker_threads.start(); // Create 4 internal threads
    event_loop.start(); // Create 1 internal thread
    worker_threads.post([] {
        // Task A
        std::cout << "Task A running in worker thread." << std::endl;
    });
    worker_threads.post([] {
        // Task B
        std::cout << "Task B running in worker thread." << std::endl;
    });
    event_loop.post([] {
        // Task C
        std::cout << "Task C running in event loop thread." << std::endl;
    });
    event_loop.post([] {
        // Task D
        std::cout << "Task D running in event loop thread." << std::endl;
    });
    worker_threads.join(); // Calls join () on the worker threads
    event_loop.join(); // Calls join () on the event thread
    return 0;
}