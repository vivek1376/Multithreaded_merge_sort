#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_ELEMENTS 10000000

void mergeArrays(long int *arr1, long int count1, long int *arr2, long int count2, long int *merged)
{
    long int i, j, m;

    for (i = j = m = 0; i < count1 && j < count2;) {
	if (arr1[i] < arr2[j]) {
	    merged[m++] = arr1[i++];
	} else {
	    merged[m++] = arr2[j++];
	}
    }

    if (i>=count1)
	while(j<count2)
	    merged[m++] = arr2[j++];
    else
	while(i<count1)
	    merged[m++]=arr1[i++];
}


void mergeSort(long int *arr, unsigned long int count)
{
    long int *merged = NULL, *arr1, *arr2;
    unsigned long int count1, count2, s, i;

    /* main iterative loop */
    for (s = 1; s < count; s = s*2) {
	printf("h1\n");
	fflush(stdout);
	for(i = 0; i < count; i += (2*s)) {
	    printf("h2\n");
	    fflush(stdout);
	    arr1 = arr + i;
	    arr2 = arr + (i + s);
	    count1 = count2 = s;
	    if (i+s >= count) {
		count1 = count - i;		/* override */
		arr2 = NULL;			/* override */
		count2 = 0;
	    } else if (i + 2*s >= count) {
		count2 = count - (i + s); 	/* override */
	    }
	    
	    merged = (long int *)malloc(sizeof(long int) * (count1 + count2));
	
	    mergeArrays(arr1, count1, arr2, count2, merged);

	    /* copy */
	    memcpy(arr + i, merged, sizeof(long int) * (count1 + count2));

	    free(merged);
	    merged = NULL;
	}
    }
}


int main()
{
    long int i, j, n;

    long int *arr = (long int *)malloc(MAX_ELEMENTS * sizeof(long int));

    i = 0;
    while (scanf("%ld", &n) != EOF) {
//	printf("%ld\n", n);

	arr[i++] = n;
    }

    for(j=0; j<i; j++)
	printf("num=%ld\n", arr[j]);

    mergeSort(arr, i);

    printf("sorted:\n");
    for(j=0; j<i; j++)
	printf("num=%ld\n", arr[j]);
    
    
    return 0;
}
