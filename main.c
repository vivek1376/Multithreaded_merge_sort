#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>


#define MAX_ELEMENTS 10000000
#define NUM_THREADS 2


struct thread_data {
    long int *arr;
    unsigned long int count;
};




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


int main()
{
    long int n;
    unsigned long int i, arrLen;

    pthread_t tid[2];
    struct thread_data tData[NUM_THREADS];
    
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
    
    /* for(j=0; j<i; j++) */
    /* 	printf("num=%ld\n", arr[j]); */

    tData[0].arr = arr;
    tData[0].count = arrLen/2;

    tData[1].arr = arr + arrLen/2;
    tData[1].count = arrLen - (int)arrLen/2;

    pthread_create(&tid[0], NULL, thread_function, (void *)&tData[0]);
    pthread_create(&tid[1], NULL, thread_function, (void *)&tData[1]);

    /* for (i = 0; i < NUM_THREADS; i++) { */
    /* 	mergeSort(arr, i); */
	
    /* } */

    pthread_join(tid[0], NULL);
    pthread_join(tid[1], NULL);

    long int *merged = (long int *)malloc(sizeof(long int) * (tData[0].count + tData[0].count));

    mergeArrays(tData[0].arr, tData[0].count, tData[1].arr, tData[1].count, merged);
    /* copy */
    memcpy(arr, merged, sizeof(long int) * (tData[0].count + tData[1].count));

    free(merged);
    merged = NULL;
    
    for(i=0; i<arrLen; i++)
	printf("%ld\n", arr[i]);
    

    
    return 0;
}
