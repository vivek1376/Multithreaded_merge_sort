#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <time.h>
#include <math.h>
#include <inttypes.h>
#include <sys/sysinfo.h>

#define MAX_ELEMENTS 10000000
#define NUM_THREADS 2
#define THREADS_LIMIT 32

struct thread_data {
    long int *arr;
    unsigned long int count;
};

struct thread_data2 {
    long int *arr1;
    long int *arr2;

    unsigned long int count1;
    unsigned long int count2;
};

void print_current_time_with_ms (void)
{
    long            ms; // Milliseconds
    time_t          s;  // Seconds
    struct timespec spec;

    clock_gettime(CLOCK_REALTIME, &spec);
    
    s  = spec.tv_sec;
    ms = round(spec.tv_nsec / 1.0e6); // Convert nanoseconds to milliseconds
    if (ms > 999) {
        s++;
        ms = 0;
    }
    
    fprintf(stderr, "Current time: %"PRIdMAX".%03ld seconds since the Epoch\n",
           (intmax_t)s, ms);
}


void mergeArrays(long int *arr1, long int count1, long int *arr2, long int count2, long int *merged)
{
    /* printf("mergeArrays-h1\n"); */
    /* fflush(stdout); */

//    return;
    
    long int i, j, m;

    for (i = j = m = 0; i < count1 && j < count2;) {
	if (arr1[i] < arr2[j]) {
	    merged[m++] = arr1[i++];
	} else {
	    merged[m++] = arr2[j++];
	}
    }

    if (i >= count1)
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
	for(i = 0; i < count; i += (2*s)) {
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


void *thread_function(void *arg)
{
    /* printf("thread called\n"); */
    /* fflush(stdout); */
    struct thread_data *tData = (struct thread_data *)arg;
    
    mergeSort(tData->arr, tData->count);
    /* pthread_exit ??? */
//    pthread_exit("");
    return NULL;
}

void *thread_function2(void *arg)
{
    /* printf("in thread2-h1\n"); */
    /* fflush(stdout); */
    
    struct thread_data2 *tData = (struct thread_data2 *)arg;
    
    long int *merged = (long int *)malloc(sizeof(long int) * (tData->count1 + tData->count2));
    
    mergeArrays(tData->arr1, tData->count1, tData->arr2, tData->count2, merged);

    /* printf("in thread2-h2\n"); */
    /* fflush(stdout); */
    /* copy */
    /* printf("tData_arr1: %ld\n", tData->arr1[0]); */
    /* fflush(stdout); */
    /* printf("in thread2-h3\n"); */
    /* fflush(stdout); */

    memcpy(tData->arr1, merged, sizeof(long int) * (tData->count1 + tData->count2));
    
    /* printf("in thread2-h4\n"); */
    /* fflush(stdout); */
        
    free(merged);
    merged = NULL;
    
    return NULL;
}

int main(int argc, char **argv)
{
//    fprintf(stderr, "av: %d\n", get_nprocs());
    
    print_current_time_with_ms();
    
    long int n;
    unsigned long int i, j, arrLen, subArrSize;

    const int numThreads = get_nprocs();

    pthread_t tid[numThreads];
    struct thread_data tData[numThreads];
    struct thread_data2 tData2[numThreads];
	
    int *threadFlag = (int *)malloc(numThreads * sizeof(int));
    memset(threadFlag, 0, numThreads * sizeof(int));
    
    long int *arr = (long int *)malloc(MAX_ELEMENTS * sizeof(long int));


    i = 0;
    while (scanf("%ld", &n) != EOF) {
//	printf("%ld\n", n);

	arr[i++] = n;
    }

    arrLen = i;
    
    if (arrLen > MAX_ELEMENTS) {
	fprintf(stderr, "Too many numbers.\n");
	return -1;
    }

    print_current_time_with_ms();
    
    /* for(j=0; j<i; j++) */
    /* 	printf("num=%ld\n", arr[j]); */

    /* Divide array into as many sub-array as
       the number of threads. */
    for (i = 0, subArrSize = arrLen / numThreads ; i < numThreads; i++) {
	tData[i].arr = arr + (i * subArrSize);

	if (i == numThreads - 1) {
	    tData[i].count = arrLen - (i * subArrSize);
	} else {
	    tData[i].count = subArrSize;
	}

	if (tData[i].count > 0) {
	    pthread_create(&tid[i], NULL, thread_function, (void *)&tData[i]);
	    threadFlag[i] = 1;
	}
    }

    for(i = 0; i< numThreads; i++) {
	if (threadFlag[i] == 1) {
	    pthread_join(tid[i], NULL);
	    threadFlag[i] = 0;
	}
    }

    /* long int *merged = (long int *)malloc(sizeof(long int) * (tData[0].count + tData[0].count)); */

    /* mergeArrays(tData[0].arr, tData[0].count, tData[1].arr, tData[1].count, merged); */
    /* /\* copy *\/ */
    /* memcpy(arr, merged, sizeof(long int) * (tData[0].count + tData[1].count)); */

    /* free(merged); */
    /* merged = NULL; */

    /* for(i=0; i<arrLen; i++) */
    /* 	printf("%ld\n", arr[i]); */

	
    j = numThreads;
    while(j > 1) {
	for (i = 0, subArrSize = arrLen / j; i < j / 2; i++) {
	    tData2[i].arr1 = arr + (2 * i * subArrSize);
	    tData2[i].count1 = subArrSize;
	    
	    tData2[i].arr2 = arr + (2 * i * subArrSize + subArrSize);
	    
	    if (i == ((j / 2) - 1)) {
		tData2[i].count2 = arrLen - (2 * i * subArrSize + subArrSize);
	    } else {
		tData2[i].count2 = subArrSize;
	    }
	    
	    if (tData2[i].count1 > 0 || tData2[i].count2 > 0) {
		pthread_create(&tid[i], NULL, thread_function2, (void *)&tData2[i]);
		threadFlag[i] = 1;
	    }
	}

	for(i = 0; i < numThreads; i++) { /* i < j/2 ??? */
	    if (threadFlag[i] == 1) {
		pthread_join(tid[i], NULL);
		threadFlag[i] = 0;
	    }
	}

	j = j / 2;
    }
    
    print_current_time_with_ms();
    
    for(i=0; i<arrLen; i++)
	printf("%ld\n", arr[i]);
    
    free(threadFlag);
    
    return 0;
}
