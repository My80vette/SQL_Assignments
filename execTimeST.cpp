#include <stdio.h>
#include <stdlib.h>
#include <chrono>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <ctime>

using namespace std;
using namespace std::chrono;
high_resolution_clock::time_point t1 = high_resolution_clock::now();


double countWithTime(void) {
    int count = 0;
    for(int i = 0; i < 100000; ++i) {
        count += i;
    }
    high_resolution_clock::time_point t2 = high_resolution_clock::now();
    duration<double> time_span = duration_cast<duration<double>>(t2 - t1);
    return time_span.count();
}


int main(void) {
    ofstream myFile;
    myFile.open("CountTimes_ST.txt");    //CountTimes_SingleThread.txt
    for(int i = 0; i < 100; ++i) {
        myFile << fixed << setw(5) << "Trial#" << i + 1 << ": " << countWithTime() << "\n";
    }
    myFile.close();
return 0;
}
 
/*
    clock_t startTime = clock();
    clock_t endTime = clock();
    double totalTime = ((endTime - startTime)/CLOCKS_PER_SEC); 
    printf("The runtime was: %2.4f Seconds\n", (double)(endTime - startTime)/CLOCKS_PER_SEC);
    printf("The final count was %d", count);
*/
