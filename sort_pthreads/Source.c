#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>
#include <assert.h>
#include <sys/time.h>

static int count = 0;

pthread_mutex_t lock;

typedef struct scalar {
    int *A;
    int *B;
    int left;
    int right;
    int m;
} scalar;


int compare(const void *a, const void *b) {
     return *(int*)a - *(int*)b;
 }


void* merge_sort(void *input) {

    scalar *s = input;
    int width = s->right - s->left + 1;
    if (width <= s->m) {
          qsort(s->A + s->left, width, sizeof(int), compare);
          return s->A;
    }

    int middle = (s->left + s->right) / 2;

    int *l;
    int *r;

    scalar left_scal = {
        .A = s->A,
        .B = s->B,
        .left = s->left,
        .right = middle,
        .m = s->m,
    };

    scalar right_scal = {
        .A = s->A,
        .B = s->B,
        .left = middle + 1,
        .right = s->right,
        .m = s->m,
    };

    int child_created = 0;
    pthread_t child;
    void *returnValue;

    pthread_mutex_lock(&lock);
    if(count - 1 > 0) {
        count--;
        child_created = 1;
    }
    pthread_mutex_unlock(&lock);

    if(child_created == 1) {
         pthread_create(&child, NULL, merge_sort, &left_scal);
    }
    else
        l = (int*)merge_sort(&left_scal);

    r = (int*)merge_sort(&right_scal);

    if(child_created == 1) {
         pthread_join(child, &returnValue);
         pthread_mutex_lock(&lock);
         count++;
         pthread_mutex_unlock(&lock);
         l = (int*)returnValue;  		
    }


    int *res = l == s->A ? s->B : s->A;


    int l_cur = s->left, r_cur = middle + 1;

    for (int i = s->left; i <= s->right; i++){

        if (l_cur <= middle && r_cur <= s->right) {
            if (l[l_cur] < r[r_cur]){
                res[i] = l[l_cur];
                l_cur++;
            } else {
                res[i] = r[r_cur];
                r_cur++;
                }
        } else if (l_cur <= middle) {
            res[i] = l[l_cur];
            l_cur++;
        }
        else {
            res[i] = r[r_cur];
            r_cur++;
        }

    }
 
    return res;
}




int main(int argc, char** argv) {
    int n = atoi(argv[1]);
    int m = atoi(argv[2]);
    int P = atoi(argv[3]);

    int *A = (int*)malloc(sizeof(int) * n);
    
    srand(time(NULL));
    for(int i = 0; i < n; i++) {
        A[i] = rand() % (200 + 1);
	}
    pthread_mutex_init(&lock, NULL);
    count = P;

    int *B = (int*)malloc(sizeof(int) * n);
    //int *C = (int*)malloc(sizeof(int) * n);
    
    FILE *data = fopen("data.txt", "w");
    for (int i = 0; i < n; i++) {
        fprintf(data, "%d ", A[i]);
    }
    fprintf(data, "\n");
    
    //for (int i = 0; i < n; i++) {
    //    C[i] = A[i];
    //}
    
     scalar s = {
        .A = A,
        .B = B,
        .left = 0,
        .right = n - 1,
        .m = m,
    };

    struct timeval start, end;
    assert(gettimeofday(&start, NULL) == 0);
    int *res = (int*)merge_sort(&s);
    assert(gettimeofday(&end, NULL) == 0);
    double delta = ((end.tv_sec - start.tv_sec) * 1000000u + end.tv_usec - start.tv_usec) / 1.e6;
    
    for (int i = 0; i < n; i++) {
        fprintf(data, "%d ", res[i]);
    }
    fprintf(data, "\n");    
    fclose(data);

    FILE *f = fopen("stats.txt", "w");
    fprintf(f, "%.5f %d %d %d\n", delta, n, m, P);
    //fprintf(f, "%.5f %d\n", delta, P);
    
    //assert(gettimeofday(&start, NULL) == 0);	
	//qsort(C, n, sizeof(int), compare);
    //assert(gettimeofday(&end, NULL) == 0);
    //delta = ((end.tv_sec - start.tv_sec) * 1000000u + end.tv_usec - start.tv_usec) / 1.e6;
    //fprintf(f,"%0.5f\n", delta);

    fclose(f);
    pthread_mutex_destroy(&lock);
    free(A);
    free(B);
    //free(C);
    return 0;
}
