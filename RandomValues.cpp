#include <cstdlib>
#include "MyPSO.h"
using namespace std;
double RandomValues::random(int min, int max)
{
    return ((max - min) * ( (double)rand() / (double)RAND_MAX ) + min);
}

void RandomValues::provideRandValues(double **arr, int m, int n)
{
    for(int i=0; i<m; i++)
    {
        for(int j=0; j<n; j++)
        {
            arr[i][j] = random(0,1);
        }
    }
}
