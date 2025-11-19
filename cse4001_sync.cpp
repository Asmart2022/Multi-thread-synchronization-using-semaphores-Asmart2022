#include <iostream>
#include <thread>
#include <vector>
#include <mutex>
#include <condition_variable>
#include <chrono>
#include <cstdlib>
#include <string>

using namespace std;

class Semaphore {
private:
    mutex mtx;
    condition_variable cv;
    int count;

public:
    explicit Semaphore(int init = 0) : count(init) {}

    // Allow reinitialization after construction
    void set(int val) {
        count = val;
    }

    void wait() {
        unique_lock<mutex> lock(mtx);
        cv.wait(lock, [&]{ return count > 0; });
        count--;
    }

    void signal() {
        unique_lock<mutex> lock(mtx);
        count++;
        cv.notify_one();
    }
};

class Lightswitch {
private:
    int counter;
    mutex mtx;

public:
    Lightswitch() : counter(0) {}

    void lock(Semaphore &sem) {
        lock_guard<mutex> lock(mtx);
        counter++;
        if (counter == 1) {
            sem.wait();
        }
    }

    void unlock(Semaphore &sem) {
        lock_guard<mutex> lock(mtx);
        counter--;
        if (counter == 0) {
            sem.signal();
        }
    }
};

mutex printMutex;

void log_msg(const string &msg) {
    lock_guard<mutex> lock(printMutex);
    cout << msg << endl;
}

void think_or_work(int min_ms = 50, int max_ms = 150) {
    int dur = min_ms + rand() % (max_ms - min_ms + 1);
    this_thread::sleep_for(chrono::milliseconds(dur));
}


Lightswitch readSwitch_p1;
Semaphore roomEmpty_p1(1);
Semaphore turnstile_p1(1);

const int NUM_READERS = 5;
const int NUM_WRITERS = 5;
const int NUM_ITER = 3; // shorter for demonstration

void reader_p1(int id) {
    for (int i = 0; i < NUM_ITER; i++) {
        turnstile_p1.wait();
        turnstile_p1.signal();
        readSwitch_p1.lock(roomEmpty_p1);

        log_msg("Problem 1 - Reader " + to_string(id) + ": reading");
        think_or_work();
        log_msg("Problem 1 - Reader " + to_string(id) + ": done");

        readSwitch_p1.unlock(roomEmpty_p1);
        think_or_work();
    }
}

void writer_p1(int id) {
    for (int i = 0; i < NUM_ITER; i++) {
        turnstile_p1.wait();
        roomEmpty_p1.wait();

        log_msg("Problem 1 - Writer " + to_string(id) + ": writing");
        think_or_work();
        log_msg("Problem 1 - Writer " + to_string(id) + ": done");

        roomEmpty_p1.signal();
        turnstile_p1.signal();
        think_or_work();
    }
}

void run_problem1() {
    log_msg("=== Problem 1: No-starve Readers/Writers ===");

    vector<thread> threads;
    for (int i = 0; i < NUM_READERS; i++) threads.emplace_back(reader_p1, i);
    for (int i = 0; i < NUM_WRITERS; i++) threads.emplace_back(writer_p1, i);
    for (auto &t : threads) t.join();

    log_msg("=== Problem 1 Complete ===");
}


Lightswitch readSwitch_p2;
Lightswitch writeSwitch_p2;
Semaphore noReaders_p2(1);
Semaphore noWriters_p2(1);

void reader_p2(int id) {
    for (int i = 0; i < NUM_ITER; i++) {
        noReaders_p2.wait();
        readSwitch_p2.lock(noWriters_p2);
        noReaders_p2.signal();

        log_msg("Problem 2 - Reader " + to_string(id) + ": reading");
        think_or_work();
        log_msg("Problem 2 - Reader " + to_string(id) + ": done");

        readSwitch_p2.unlock(noWriters_p2);
        think_or_work();
    }
}

void writer_p2(int id) {
    for (int i = 0; i < NUM_ITER; i++) {
        writeSwitch_p2.lock(noReaders_p2);
        noWriters_p2.wait();

        log_msg("Problem 2 - Writer " + to_string(id) + ": writing");
        think_or_work();
        log_msg("Problem 2 - Writer " + to_string(id) + ": done");

        noWriters_p2.signal();
        writeSwitch_p2.unlock(noReaders_p2);
        think_or_work();
    }
}

void run_problem2() {
    log_msg("=== Problem 2: Writer-Priority Readers/Writers ===");

    vector<thread> threads;
    for (int i = 0; i < NUM_READERS; i++) threads.emplace_back(reader_p2, i);
    for (int i = 0; i < NUM_WRITERS; i++) threads.emplace_back(writer_p2, i);
    for (auto &t : threads) t.join();

    log_msg("=== Problem 2 Complete ===");
}

const int NUM_PHIL = 5;
const int NUM_MEALS = 4;

Semaphore footman_p3(4);
Semaphore forks_p3[NUM_PHIL];

int left(int i) { return i; }
int right(int i) { return (i + 1) % NUM_PHIL; }

void init_problem3() {
    for (int i = 0; i < NUM_PHIL; i++) {
        forks_p3[i].set(1);
    }
}

void get_forks_p3(int i) {
    footman_p3.wait();
    forks_p3[right(i)].wait();
    forks_p3[left(i)].wait();
}

void put_forks_p3(int i) {
    forks_p3[right(i)].signal();
    forks_p3[left(i)].signal();
    footman_p3.signal();
}

void philosopher_p3(int id) {
    for (int m = 0; m < NUM_MEALS; m++) {
        log_msg("Problem 3 - Philosopher " + to_string(id) + ": thinking");
        think_or_work();

        log_msg("Problem 3 - Philosopher " + to_string(id) + ": trying forks");
        get_forks_p3(id);

        log_msg("Problem 3 - Philosopher " + to_string(id) + ": eating");
        think_or_work();

        put_forks_p3(id);
        log_msg("Problem 3 - Philosopher " + to_string(id) + ": finished eating");
    }
}

void run_problem3() {
    log_msg("=== Problem 3: Dining Philosophers (Footman) ===");
    init_problem3();

    vector<thread> threads;
    for (int i = 0; i < NUM_PHIL; i++) threads.emplace_back(philosopher_p3, i);
    for (auto &t : threads) t.join();

    log_msg("=== Problem 3 Complete ===");
}


Semaphore forks_p4[NUM_PHIL];

void init_problem4() {
    for (int i = 0; i < NUM_PHIL; i++) {
        forks_p4[i].set(1);
    }
}

void philosopher_p4(int id) {
    for (int m = 0; m < NUM_MEALS; m++) {
        log_msg("Problem 4 - Philosopher " + to_string(id) + ": thinking");
        think_or_work();

        log_msg("Problem 4 - Philosopher " + to_string(id) + ": grabbing forks");

        if (id % 2 == 0) {
            forks_p4[left(id)].wait();
            forks_p4[right(id)].wait();
        } else {
            forks_p4[right(id)].wait();
            forks_p4[left(id)].wait();
        }

        log_msg("Problem 4 - Philosopher " + to_string(id) + ": eating");
        think_or_work();

        forks_p4[left(id)].signal();
        forks_p4[right(id)].signal();
        log_msg("Problem 4 - Philosopher " + to_string(id) + ": done eating");
    }
}

void run_problem4() {
    log_msg("=== Problem 4: Dining Philosophers (Asymmetric) ===");
    init_problem4();

    vector<thread> threads;
    for (int i = 0; i < NUM_PHIL; i++) threads.emplace_back(philosopher_p4, i);
    for (auto &t : threads) t.join();

    log_msg("=== Problem 4 Complete ===");
}


int main(int argc, char *argv[]) {
    srand(time(NULL));

    if (argc != 2) {
        cerr << "Usage: " << argv[0] << " <problem #>\n";
        cerr << " 1 - No-starve Readers/Writers\n";
        cerr << " 2 - Writer-priority Readers/Writers\n";
        cerr << " 3 - Dining Philosophers #1 (Footman)\n";
        cerr << " 4 - Dining Philosophers #2 (Asymmetric)\n";
        return 1;
    }

    int p = atoi(argv[1]);
    switch (p) {
        case 1: run_problem1(); break;
        case 2: run_problem2(); break;
        case 3: run_problem3(); break;
        case 4: run_problem4(); break;
        default:
            cerr << "Invalid problem number.\n";
            return 1;
    }

    return 0;
}
