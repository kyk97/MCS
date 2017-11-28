#include <stdlib.h>
#include <omp.h>
#include <assert.h>
#include <time.h>
#include <sys/time.h>
#include <stdio.h>


int compare(const void * x1, const void * x2)
{
	return (*(int*)x1 - *(int*)x2);          
}

void swap(int *a, int *b) {
	int temp = *a;
	*a = *b;
	*b = temp;
}

int binarySearch(int x, int *array, int left, int right) {
	if (right < left || x <= array[left]) {
		return left;
	}
	if (x > array[right]){
		return right + 1;
	}
	int mid;
	while (!(left >= right)) {
		mid = left + (right - left) / 2;
		if (array[right - 1] < x && array[right] >= x) {
			return right;
		}
		if (array[mid - 1] < x && array[mid] >= x) {
			return mid;
		}
		if (array[mid] < x) {
			left = mid;
		}
		else {
			right = mid;
		}
	}
	return -1;
}

void mergeMT(int *T, int left1, int right1, int left2, int right2, int * A, int left3) {
	int n1 = right1 - left1 + 1;
	int	n2 = right2 - left2 + 1;
	if (n1 < n2) {
		swap(&left1, &left2);
		swap(&right1, &right2);
		swap(&n1, &n2);
	}
	if (n1 == 0)
		return;
	else {
		int mid1 = (left1 + right1) / 2;
		int mid2 = binarySearch(T[mid1], T, left2, right2);
		int mid3 = left3 + (mid1 - left1) + (mid2 - left2);
		A[mid3] = T[mid1];
		//#pragma omp parallel
		{
			//#pragma omp task
			mergeMT(T, left1, mid1 - 1, left2, mid2 - 1, A, left3);
			//#pragma omp task
			mergeMT(T, mid1 + 1, right1, mid2, right2, A, mid3 + 1);
		}
	}
}
int* merge_sort(int *up, int *down, int left, int right, int M) {

    int width = right - left;
    if (width <= M) {
          qsort(up + left, width, sizeof(int), compare);
          return up;
    }

    int middle = (int)((left + right) * 0.5);

    int *l_buff;
    int *r_buff;

    //#pragma omp parallel
    //{
	//	#pragma omp single
    //    {
    //        #pragma omp task
    //        {
                l_buff = merge_sort(up, down, left, middle, M);
    //        }
    //        #pragma omp task
    //        {
                r_buff = merge_sort(up, down, middle, right, M);
    //        }

    //    }
    //}


    int mid_1 = (middle + left) / 2;
    int mid_2 = bin_search(r_buff, middle, right, l_buff[mid_1]);
    int mid_3 = left + (mid_1 - left) + (mid_2  - middle);

    int *target = l_buff == up ? down : up;
    //#pragma omp parallel
    //{
    //    #pragma omp single
    //    {
    //        #pragma omp task
    //        {
                mergeMT(target, left, l_buff, left, mid_1, r_buff, middle, mid_2);

    //        }
    //        #pragma omp task
    //        {
                mergeMT(target, mid_3, l_buff, mid_1, middle, r_buff, mid_2, right);
    //        }
    //    }
    //}

    return target;
}

void initialization(int *up, int N, int a, int b) {
	srand(time(NULL));
	for (int i = 0; i < N; i++) {
		up[i] = rand() % (b - a + 1) + a;
	}
}



int main(int argc, char **argv) 
{
	if (argc == 4){
		int n = atoi(argv[1]);
		int m = atoi(argv[2]);
		int P = atoi(argv[3]);
		FILE *f = fopen("stats.txt", "a");

		omp_set_num_threads(P);
		int * a = (int*) malloc(n * sizeof(int));
		int * b = (int*) malloc(n * sizeof(int));
	
		initialization(a, n, 0, 100);


		struct timeval start, end;
		assert(gettimeofday(&start, NULL) == 0);
		int *res = merge_sort(a, b, 0, n, m);
		assert(gettimeofday(&end, NULL) == 0);
		double delta = ((end.tv_sec - start.tv_sec) * 1000000u + end.tv_usec - start.tv_usec) / 1.e6;

		fprintf(f, "%.5f %d %d %d\n", delta, n, m, P);

		FILE *data = fopen("data.txt", "w");
		fprintf(data, "\n");
		for (int i = 0; i < n; i++) {
			fprintf(data, "%d ", res[i]);
		}		
		
		
		assert(gettimeofday(&start, NULL) == 0);
		qsort(a, n, sizeof(int), compare);
		assert(gettimeofday(&end, NULL) == 0);
		delta = ((end.tv_sec - start.tv_sec) * 1000000u + end.tv_usec - start.tv_usec) / 1.e6;
		fprintf(f, "%f \n", delta);
		
		fprintf(data, "\n");
		for (int i = 0; i < n; i++) {
			fprintf(data, "%d ", a[i]);
		}		
		fclose(data);
		fclose(f);
		free(a);
		free(b);
	}
	return 0;
}
