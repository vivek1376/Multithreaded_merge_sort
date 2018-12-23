#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>


#define MAX_ELEMENTS 10000000
#define NUM_THREADS 2


struct thread_data {
    long int *arr1, *arr2, *merged;
    unsigned long int count1, count2;
    unsigned long int i;
};


void mergeArrays(long int *arr1,
		 unsigned long int count1,
		 long int *arr2,
		 unsigned long int count2,
		 long int *merged)
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
    /* printf("thread called\n"); */
    /* fflush(stdout); */
    struct thread_data *tdata = (struct thread_data *)arg;

    mergeArrays(tdata->arr1, tdata->count1, tdata->arr2, tdata->count2, tdata->merged);

    /* memcpy to main array */
    memcpy(tdata->arr1, tdata->merged, sizeof(long int) * (tdata->count1 + tdata->count2));
    free(tdata->merged);
    tdata->merged = NULL;

    /* pthread_exit ??? */
//    pthread_exit("");
    return NULL;
}


void mergeSort(long int *arr, unsigned long int count)
{
    long int *arr1, *arr2;
    unsigned long int count1, count2, s, i;

    pthread_t tid[2];

    volatile int thread_flags[2] = {0};
    
    volatile struct thread_data tdata[2];
    tdata[0].merged = tdata[1].merged = NULL;
    
    volatile int p, res, thNum;
    
    /* main iterative loop */
    for (s = 1; s < count; s = s*2) {
        // printf("\n==h1\n");
        // fflush(stdout);

        for (i = thNum = 0; i < count; i += (2*s)) {
            /* printf("h2\n"); */
            /* fflush(stdout); */

            arr1 = arr + i;
            arr2 = arr + (i + s);
            count1 = count2 = s;

            if (i+s >= count) {
                count1 = count - i;             /* override */
                arr2 = NULL;                    /* override */
                count2 = 0;
            } else if (i + 2*s >= count) {
                count2 = count - (i + s);       /* override */
            }

	    /* printf("thNum : %d\n", thNum); */
	    /* fflush(stdout); */


	    if (thread_flags[thNum] == 1) {
		/* printf("inside\n"); */
		/* fflush(stdout); */
		
		pthread_join(tid[thNum], NULL);
		thread_flags[thNum] = 0;
	    }
	    
	    // tdata[thNum].thFlag = &thread_flags[thNum];
	    tdata[thNum].i = i;
	    tdata[thNum].arr1 = arr1;
	    tdata[thNum].arr2 = arr2;
	    tdata[thNum].count1 = count1;
	    tdata[thNum].count2 = count2;
	    tdata[thNum].merged = (long int *)malloc(sizeof(long int) * (count1 + count2));


	    res = pthread_create(&tid[thNum], NULL, thread_function, (void *)&tdata[thNum]);
	    thread_flags[thNum] = 1;
	    
            // printf(">> ");
            // for (p=0; p<count; p++)
            //     printf("%ld ", arr[p]);
            
            // printf("\n");

	    thNum = (thNum ? 0 : 1);
        }

	/* printf("h3\n"); */
	/* fflush(stdout); */
	for (p = 0; p < NUM_THREADS; p++) {
	    if (thread_flags[p] == 1) {
		/* printf("p = %d\n", p);//d */
		/* fflush(stdout); */
		pthread_join(tid[p], NULL);
		thread_flags[p] = 0;
	    }
	}
	

        // printf(">> after inner loop finish: \n");
        // for (p=0; p<count; p++)
        //     printf("%ld ", arr[p]);
        
        // printf("\n");
    }
}


int main()
{
    long int n;
    unsigned long int i, j;

    
    long int *arr = (long int *)malloc(MAX_ELEMENTS * sizeof(long int));

    i = 0;
    while (scanf("%ld", &n) != EOF) {
//      printf("%ld\n", n);

        arr[i++] = n;
    }

    if (i > MAX_ELEMENTS) {
        fprintf(stderr, "Too many numbers.\n");
        return -1;
    }

    /* print input */
    /* for(j=0; j<i; j++) */
    /*  printf("num=%ld\n", arr[j]); */

    mergeSort(arr, i);

    /* print sorted */
    // printf("sorted:\n");
    for(j=0; j<i; j++)
        printf("%ld\n", arr[j]);
    
    
    return 0;
}
