#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <chrono>
#include <iostream>

#include "constants.h"

using namespace std;
using namespace std::chrono;




class MeasureTime
{


public:
	MeasureTime();

	long long TimeDiffChrono(int i);	
    void StartMeasure(int measure_type);
    long long StopMeasure(int measure_type);
	void PrintResults();
    bool record_measurements = 1;
private:
	// For Chrono
    high_resolution_clock::time_point last[MEASUREMENT_TYPES];
    high_resolution_clock::time_point next[MEASUREMENT_TYPES];
    // general
	int record_index[MEASUREMENT_TYPES];
	long long measurements[MEASUREMENT_TYPES][MEASUREMENT_COUNT];
    bool valid_measurements = 0;
};


void test();


