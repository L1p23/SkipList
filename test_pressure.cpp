#include "SkipList.h"
#include <iostream>
#include <thread>
#include <ctime>
using namespace std;

#define NUM_THREAD 16
#define NUM_TEST 50000
#define NUM_PER_TH NUM_TEST/NUM_THREAD

skiplist<int, int> sl;

void insertTest() {
    for (int i = 0; i < NUM_PER_TH; i++)
        sl.insert(rand()%NUM_TEST, i);
}

void searchTest() {
    int value;
    for (int i = 0; i < NUM_PER_TH; i++)
        sl.search(rand()%NUM_TEST, value);
}

int main() {
    cout<<"test count = "<<NUM_TEST<<endl;
    srand(time(NULL));
    thread T[NUM_THREAD];
    clock_t start = clock();
    for (int i = 0; i < NUM_THREAD; i++)
        T[i] = thread(insertTest);
    for (int i = 0; i < NUM_THREAD; i++)
        T[i].join();
    cout<<"insert time: "<<(double)(clock() - start) / CLOCKS_PER_SEC<<'s'<<endl;
    start = clock();
    for (int i = 0; i < NUM_THREAD; i++)
        T[i] = thread(searchTest);
    for (int i = 0; i < NUM_THREAD; i++)
        T[i].join();
    cout<<"search time: "<<(double)(clock() - start) / CLOCKS_PER_SEC<<'s'<<endl;
    return 0;
}