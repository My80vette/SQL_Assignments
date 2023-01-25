#include <stdio.h>
#include <stdlib.h>
#include <chrono>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <ctime>
#include <thread>

using namespace std;
using namespace std::chrono;

int sum = 0;


void sum_integers(int start, int end) {
    for (int i = start; i <= end; i++) {
        sum += i;
    }
}
    
int main(void) {
    ofstream myFile;
    myFile.open("CountTimes_MT.txt");    //CountTimes_MultiThread.txt
    for(int i = 1; i <= 100; ++i) {

        high_resolution_clock::time_point t1 = high_resolution_clock::now();
        thread first (sum_integers, 0, 50000);
        thread second (sum_integers, 50001, 100000);        
        first.join();
        second.join();
        high_resolution_clock::time_point t2 = high_resolution_clock::now();
        duration<double> time_span = duration_cast<duration<double>>(t2 - t1);
        myFile << fixed << setw(5) << "Trial #" << i << ": " << time_span.count() << "\n";
        //myFile << time_span << "\n";
    }
    myFile.close();

return 0;
}
 

















/*
    printf("The runtime was: %2.4f Seconds\n", (double)(endTime - startTime)/CLOCKS_PER_SEC);
    printf("The final count was %d", count);


    int count = 0;
    high_resolution_clock::time_point t1 = high_resolution_clock::now();
    for(int i = startCount; i <= (startCount + 2000); ++i) {
        count += i;
    }
    high_resolution_clock::time_point t2 = high_resolution_clock::now();
    duration<double> time_span = duration_cast<duration<double>>(t2 - t1);
    return *time_span.count();


    ofstream myFile;
    myFile.open("CountTimes_MT.txt");    //CountTimes_MultiThread.txt
    for(int i = 0; i < 100; ++i) {
        myFile << "Trial#" << i + 1 << ": " <<countWithTime() << "\n";
    }
    myFile.close();
*/
