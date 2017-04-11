#include "measuretime.h"
#include <chrono>
#include <math.h>
#include <stdio.h>

//extern int OverallFrames;


MeasureTime::MeasureTime()
{
    /* for WINDOWS
    if (!QueryPerformanceFrequency((LARGE_INTEGER*)&time_frequency))
	{
		std::cout << "Performance Counter nicht vorhanden" << std::endl;
	}
	QueryPerformanceCounter((LARGE_INTEGER*)&time_last);
    */
	std::fill(record_index, record_index + MEASUREMENT_TYPES, 0);
	//std::cout << measurements[MEASUREMENT_TYPES - 1][MEASUREMENT_COUNT - 1] << std::endl;
}



void MeasureTime::StartMeasure(int measure_type)
{
    //QueryPerformanceCounter((LARGE_INTEGER*)&time_last[measure_type]); for WINDOWS
    last[measure_type] = std::chrono::high_resolution_clock::now();
}



long long MeasureTime::StopMeasure(int measure_type)
{
    //long long elapsed_time;
    next[measure_type] = std::chrono::high_resolution_clock::now();
    //auto diff = next[measure_type] - last[measure_type];
    //cout << chrono::duration <double, micro> (diff).count() << " us" << endl;
    long long elapsed_time = std::chrono::duration_cast<std::chrono::microseconds>(next[measure_type] - last[measure_type]).count();
    if (record_measurements)
	{
		// Save measurement
		measurements[measure_type][record_index[measure_type]] = elapsed_time;
		// Update counter
		if (record_index[measure_type] >= (MEASUREMENT_COUNT - 1))
		{
			record_index[measure_type] = 0;
            valid_measurements = 1;
		}
		else
		{
			record_index[measure_type] = record_index[measure_type] + 1;
		}
	}
	return elapsed_time;
}

void MeasureTime::PrintResults()
{
	double mean[MEASUREMENT_TYPES];
	long long maxval[MEASUREMENT_TYPES];
	for (int i = 0; i < MEASUREMENT_TYPES; i++)
	{
		mean[i] = 0;
		maxval[i] = 0;
		for (int j = 0; j < MEASUREMENT_COUNT; j++)
		{
			mean[i] += measurements[i][j];
			if (measurements[i][j] > maxval[i])
			{
				maxval[i] = measurements[i][j];
			}
		}
        mean[i] = round(mean[i] / MEASUREMENT_COUNT);
		std::cout << "Mean time for measurement #" << i << ": " << mean[i] << "us. Highest: " << maxval[i] << std::endl;
	}
    FILE *pFile;
    pFile = fopen("timings.csv","w+");
    for(int i=0;i<MEASUREMENT_COUNT;i++)
    {

        for(int j=0;j<MEASUREMENT_TYPES;j++)
        {
            fprintf(pFile,"%d,",measurements[j][i]);
        }
        fprintf(pFile,"\n");
    }
}


long long MeasureTime::TimeDiffChrono(int i)
{
	next[i] = high_resolution_clock::now();
	auto duration = duration_cast<microseconds>(next[i] - last[i]).count();
	last[i] = high_resolution_clock::now();
	return duration;
}
