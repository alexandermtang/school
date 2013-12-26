#include  "my_malloc.h"
//#include  <unistd.h>
#include  <stdio.h>
#include  <string.h>


#define malloc(x) my_malloc(x)
#define free(x) my_free(x)

#define TA_TEST_BLOCKSIZE 100

int main(int argc, char*argv[]){

    int casenum;


    if(argc != 2){
        printf("Usage: ./assignment6_ta_tests <int>casenum\n");
    }
    casenum = atoi(argv[1]);


    printf("Case num: %d\n",casenum);

    if(casenum == 1){
        //    1. free pointer never allocated
        int i;
        int *ip;
        printf("\tTA: Mallocing pointer\n");
        ip = malloc(sizeof(int));
        printf("\tTA: Freeing pointer %p\n",&i);
        free(&i);
        printf("Done\n");

    }else if(casenum == 2){
        //    2. free pointer in middle of allocated memory block
        int *ip;
        printf("\tTA: Mallocing pointer\n");
        ip = malloc(10*sizeof(int));
        printf("\tTA: Freeing pointer %p\n",ip+7);
        free(ip+7);
        printf("Done\n");

    }else if(casenum == 3){
        //    3. free pointer between two memory blocks

        char *cp;
        int *ipnext;
        printf("\tTA: Mallocing pointer\n");
        cp = malloc(10*sizeof(char));
        printf("\tTA: Mallocing pointer\n");
        ipnext = malloc(10*sizeof(int));
        printf("\tTA: Freeing pointer %p\n",cp+12);
        free(cp+12);
        printf("Done\n");

    }else if(casenum == 4){
        //    4. freeing pointer a couple bytes before memory block (so into MemEntry struct). Then attempt another malloc.
        char* cp;
        int*ip;
        printf("\tTA: Mallocing pointer\n");
        cp = malloc(15*sizeof(char));
        printf("\tTA: Freeing pointer %p\n",cp-2);
        free(cp-2);
        printf("\tTA: Mallocing pointer\n");
        ip = malloc(16*sizeof(int));
        printf("Done\n");

    }else if(casenum == 5){
        //    5.  redundant freeing of same pointer.
        int *ip1;
        printf("\tTA: Mallocing pointer\n");
        ip1 = malloc(10*sizeof(int));
        printf("\tTA: Freeing pointer %p\n",ip1);
        free(ip1);
        printf("\tTA: Freeing pointer %p\n",ip1);
        free(ip1);
        printf("Done\n");


    }else if(casenum == 6){
        //    6.  saturation with a while loop.  Program should not fail.
        int pcount = 0;
        while(malloc(TA_TEST_BLOCKSIZE*sizeof(int))){
      pcount++;
      printf("Allocated %d pointers of size %d\n",pcount,TA_TEST_BLOCKSIZE);
    }
        printf("Done\n");


    }else if(casenum == 7){
        //    7.  Saturation, then freeing everything, then saturation with same list of malloc's (should yield same # pointers)
        int*ip;
        int pcount = 0;
        int**doubleip;
        int **dip_leftover = 0;
        while(ip = malloc(TA_TEST_BLOCKSIZE*sizeof(int))){
            pcount++;
            if(pcount == 1){
                doubleip = calloc(1,sizeof(int*));
                *doubleip = ip;
            }else{
        if((dip_leftover=realloc(doubleip,pcount*sizeof(int*))) != 0){
          doubleip = dip_leftover;
          doubleip[pcount-1] = ip;
          dip_leftover = 0;
        }else{
          dip_leftover = &ip;
          printf("Allocated 1 extra but couldn't realloc a pointer\n");
          break;
        }

            }
            printf("TA: allocated %d blocks of size %d so far\n", pcount, TA_TEST_BLOCKSIZE);
        }

        //free
        int i;
        int cap = pcount;
        if(dip_leftover != 0){
      cap -= 1;
      free(*dip_leftover);
    }
        for(i = 0; i < cap; i++){
      free(doubleip[i]);
    }

        //malloc again
        int secondpcount = 0;
        while(malloc(TA_TEST_BLOCKSIZE*sizeof(int))){
            secondpcount++;
        }

        if(pcount == secondpcount){
            printf("\tPassed.\n");
        }else{
            printf("\tFailed\n");
        }

        printf("Done\n");

    }else if(casenum == 8){
        //    8.  Regular call to malloc (with some modifications of pointer)

        int *x;
        int *y;
        printf("\tTA: Mallocing pointer\n");
        x = malloc(sizeof(int));
        printf("\tTA: Mallocing pointer\n");
        y = malloc(sizeof(int));

        *x = 5;
        *y = 7;
        printf("\tBefore swap: x=%d,y=%d\n",*x,*y);
        int temp;
        temp = *x;
        *x = *y;
        *y = temp;
        printf("\tAfter swap: x=%d,y=%d\n",*x,*y);
        printf("Done\n");
    }else if(casenum == 9){
        //    9.  Regular call to free (with some modifications of pointer before free)
        int *x;
        int *y;
        printf("\tTA: Mallocing pointer\n");
        x = malloc(sizeof(int));
        printf("\tTA: Mallocing pointer\n");
        y = malloc(sizeof(int));

        *x = 5;
        *y = 7;
        printf("\tBefore swap: x=%d,y=%d\n",*x,*y);
        int temp;
        temp = *x;
        *x = *y;
        *y = temp;
        printf("\tAfter swap: x=%d,y=%d\n",*x,*y);
        printf("\tTA: Freeing pointer %p\n",x);
        free(x);
        printf("\tTA: Freeing pointer %p\n",y);
        free(y);
        printf("Done\n");

    }else if(casenum == 10){
        //   10.  A trickier set of malloc's and free's.
        int **matrix;
        printf("\tAllocating matrix\n");
        matrix = malloc(10*sizeof(int*));
        int i;
        for(i = 0; i < 10; i++){
            matrix[i] = malloc(10*sizeof(int));
        }
        printf("\tFreeing matrix\n");
        for(i = 0; i < 10; i++){
            free(matrix[i]);
        }
        free(matrix);
        printf("Done\n");
    }

}
