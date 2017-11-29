#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <assert.h>
#include <sys/time.h>
#include <omp.h>

int compare(const void *a, const void *b) {
     return *(int*)a - *(int*)b;
 }


void* merge_sort(int *A, int *B, int left, int right, int m, int threads) {

    int width = right - left + 1;
    if (width <= m) {
          qsort(A + left, width, sizeof(int), compare);
          return A;
    }

    int middle = (left + right) / 2;

    int *l;
    int *r;
    
     if (threads > 1)
    {	
		#pragma omp parallel sections
		{
			#pragma omp section
			{
				l = (int*)merge_sort(A, B, left, middle, m, threads/2);
			}
			#pragma omp section
			{
				r = (int*)merge_sort(A, B, middle + 1, right, m, threads - threads/2);
			}
		}
    }
    else
    {
		l = (int*)merge_sort(A, B, left, middle, m, 1);
		r = (int*)merge_sort(A, B, middle + 1, right, m, 1);
    }   

    int *res = l == A ? B : A;

    int l_cur = left, r_cur = middle + 1;

    for (int i = left; i <= right; i++){

        if (l_cur <= middle && r_cur <= right) {
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
    
    struct timeval start, end;
    assert(gettimeofday(&start, NULL) == 0);
    int *res = (int*)merge_sort(A, B, 0, n - 1, m, P);
    assert(gettimeofday(&end, NULL) == 0);
    double delta = ((end.tv_sec - start.tv_sec) * 1000000u + end.tv_usec - start.tv_usec) / 1.e6;
    
    for (int i = 0; i < n; i++) {
        fprintf(data, "%d ", res[i]);
    }
    fprintf(data, "\n");    

    FILE *f = fopen("stats.txt", "a");
    fprintf(f, "%.5f %d %d %d\n", delta, n, m, P);
    //fprintf(f, "%.5f %d\n", delta, P);
    
    //assert(gettimeofday(&start, NULL) == 0);	
	//qsort(C, n, sizeof(int), compare);
    //assert(gettimeofday(&end, NULL) == 0);
    //delta = ((end.tv_sec - start.tv_sec) * 1000000u + end.tv_usec - start.tv_usec) / 1.e6;
    //fprintf(f,"%0.5f\n", delta);
	//int flag = 1;
	//for (int i = 0; i < n; i++)
	//	if (res[i] != C[i]){
	//		flag = 0;
	//		fprintf(data, "\n%d %d %d\n", i, A[i], C[i]);
	//	}
	//fprintf(f,"%d\n", flag);	
    //for (int i = 0; i < n; i++) {
	//	fprintf(data, "%d ", C[i]);
    //}
    fprintf(data, "\n");    
    fclose(data);
    
    fclose(f);
    free(A);
    free(B);

    //free(C);
    return 0;
}
