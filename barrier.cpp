/******************************************************************************
 *  Compilation:  g++ -o barrier.cpp -std=c++11 -g
 *  Execution:    ./a.out
 *  Dependencies: 
 *
 *  a synchronization mechanism that causes threads to wait until
 *  the required number of threads has reached the barrier.
 *  Once all the threads have reached the barrier, they’re all
 *  unblocked and may proceed.
 *
 *  % ./a.out
 *  this thread: 0x700008f75000 sleep for 650 ms
 *  this thread: 0x700008ff8000 sleep for 886 ms
 *  this thread: 0x70000907b000 sleep for 992 ms
 *  this thread: 0x700008f75000 finished.
 *  this thread: 0x700008ff8000 finished.
 *  this thread: 0x70000907b000 finished.
 *
 ******************************************************************************/
 
#include <cstdlib>
#include <thread>
#include <chrono>
#include <atomic>
#include <iostream>

class barrier {
  unsigned int const count;
  std::atomic<unsigned int> spaces;
  std::atomic<unsigned int> generation;

public:
  explicit barrier(unsigned int const count_) :
      count(count_), spaces(count), generation(0)
  {}

  void wait() {
    unsigned int const my_generation = generation;
    if (--spaces == 0) {
      spaces = count;
      ++generation;
    } else {
      while (my_generation == generation)
        std::this_thread::yield();
    }
  }
};

void fun(barrier* bar) {
  std::srand(std::time(0));
  int rand_ms = std::rand() % 1000;
  std::this_thread::sleep_for(std::chrono::milliseconds(rand_ms));
  std::cout << "this thread: " << std::this_thread::get_id() << " sleep for "
    << rand_ms << " ms" << std::endl;
  bar->wait();
  std::this_thread::sleep_for(std::chrono::milliseconds(rand_ms));
  std::cout << "this thread: " << std::this_thread::get_id()
      << " finished."<< std::endl;
}

int main(int argc, char* argv[]) {
  barrier bar(3);
  std::thread t1(fun, &bar);
  std::thread t2(fun, &bar);
  std::thread t3(fun, &bar);
  t1.join();
  t2.join();
  t3.join();
}
