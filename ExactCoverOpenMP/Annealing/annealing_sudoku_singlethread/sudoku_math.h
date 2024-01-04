// SD of a population
#include <math.h>
#include <stdio.h>

float calculateSD(float data[ANNEAL_TEMP_SAMPLE])
{
    float sum = 0.0, mean, SD = 0.0;
    int i;
    for (i = 0; i < ANNEAL_TEMP_SAMPLE; ++i)
    {
        //printf("SD %d -> %f\n", i, data[i]);
        sum += data[i];
    }
    mean = sum / ANNEAL_TEMP_SAMPLE;
    for (i = 0; i < ANNEAL_TEMP_SAMPLE; ++i)
    {
        SD += pow(data[i] - mean, 2);
    }

    return sqrt(SD / ANNEAL_TEMP_SAMPLE);
}
