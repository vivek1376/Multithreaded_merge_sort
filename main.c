#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>


#define MAX_ELEMENTS 10000000

struct thread_data {
    long int *arr1, *arr2, *merged;
    long int count1, count2;
    unsigned long int i;
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


void *thread_function(void *arg)
{
    struct thread_data *data = (struct thread_data *)arg;

    mergeArrays(data->arr1, data->count1, data->arr2, data->count2, data->merged);
    
    return NULL;
}


void mergeSort(long int *arr, unsigned long int count)
{
    long int *merged1 = NULL, *merged2 = NULL, *arr1, *arr2;
    unsigned long int count1, count2, s, i;

    pthread_t tid;

    struct thread_data tdata;
    tdata.merged = NULL;
    
    int res;
    
    /* main iterative loop */
    for (s = 1; s < count; s = s*2) {
	printf("h1\n");
	fflush(stdout);
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
	    
	    if (i%2 == 0) {
		/* join */
		printf("h3\n");
		fflush(stdout);
		
		
		if (tdata.merged != NULL) {
		    res = pthread_join(tid, NULL);
		    printf("h1 res=%d\n", res); //d
		    fflush(stdout);		
		    memcpy(arr + tdata.i, tdata.merged, sizeof(long int) * (tdata.count1 + tdata.count2));
		    free(tdata.merged);
		    tdata.merged = merged1 = NULL;
		}
		
		merged1 = (long int *)malloc(sizeof(long int) * (count1 + count2));
		
		tdata.arr1 = arr1;
		tdata.arr2 = arr2;
		tdata.count1 = count1;
		tdata.count2 = count2;
		tdata.merged = merged1;

		res = pthread_create(&tid, NULL, thread_function, (void *)&tdata);
		/* check res */
	    } else {
		merged2 = (long int *)malloc(sizeof(long int) * (count1 + count2));
		
		mergeArrays(arr1, count1, arr2, count2, merged2);
		/* copy */
		memcpy(arr + i, merged2, sizeof(long int) * (count1 + count2));

		free(merged2);
		merged2 = NULL;
		//res = pthread_join(tid, NULL);

		/* check res */
		
	    }
	}
    }
}


int main()
{
    long int n;
    unsigned long int i, j;

    
    long int *arr = (long int *)malloc(MAX_ELEMENTS * sizeof(long int));

    i = 0;
    while (scanf("%ld", &n) != EOF) {
//	printf("%ld\n", n);

	arr[i++] = n;
    }

    if (i > MAX_ELEMENTS) {
	fprintf(stderr, "Too many numbers.\n");
	return -1;
    }

    /* print input */
    /* for(j=0; j<i; j++) */
    /* 	printf("num=%ld\n", arr[j]); */

    mergeSort(arr, i);

    /* print sorted */
    for(j=0; j<i; j++)
	printf("%ld\n", arr[j]);
    
    
    return 0;
}
