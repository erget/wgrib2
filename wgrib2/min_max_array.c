#include <stdio.h>
#include "wgrib2.h"

/*
 * min_max_array()  returns min/max of the array
 *
 * Public Domain 8/2015 Wesley Ebisuzaki  
 *
 * this is an OpenMP code, does not require OpenMP 3.1
 *
 * return 0:  if min max found
 * return 1:  if min max not found
 */

int min_max_array(float *data, unsigned int n, float *min, float *max) {

    unsigned int first, i;
    float mn, mx, min_val, max_val;

    if (n == 0) return 1;

    for (first = 0; first < n; first++) {
        if (DEFINED_VAL(data[first])) break;
    }
    if (first >= n) return 1;

    mn = mx = data[first];

#pragma omp parallel private(min_val, max_val)
    {
	min_val = max_val = data[first];

#pragma omp for private(i) schedule(static) nowait
	for (i = first+1; i < n; i++) {
	    if (DEFINED_VAL(data[i])) {
                min_val = (min_val > data[i]) ? data[i] : min_val;
                max_val = (max_val < data[i]) ? data[i] : max_val;
            }
	}

#pragma omp critical
	{
	    if (min_val < mn) mn = min_val;
	    if (max_val > mx) mx = max_val;
	}
    }

    *min = mn;
    *max = mx;
    return 0;
}

