#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <assert.h>
#include <time.h>
#include <sys/time.h>

void random_walk(FILE *f, int a, int b, int x, int N, double p, int P) {
	struct timeval start, end;
	assert(gettimeofday(&start, NULL) == 0);
	int B = 0;
	int counter = 0;
	unsigned int * seed = (unsigned int*) malloc(N * sizeof(unsigned int));
	unsigned int s = (unsigned int)(time(NULL));
	for(int i = 0; i < N; i++) {
		seed[i] = rand_r(&s);
	}
	omp_set_num_threads(P);
	#pragma omp parallel for reduction (+: counter)
	for (int i = 0; i < N; i++) {
		int X = x;
		unsigned int s = seed[i];
		while (X != a && X != b) {
			counter++;
			double p1 = ((double)rand_r(&s)) / RAND_MAX;
			if (p1 <= p) {
				X++;
			}
			else {
				X--;
			}
		}
		if (X == b) {
			#pragma omp atomic
			B++;
		}
	}
	assert(gettimeofday(&end, NULL) == 0);
	double delta = ((end.tv_sec - start.tv_sec) * 1000000u + end.tv_usec - start.tv_usec) / 1.e6;
	double p1 = ((double)B) / N;
	double mean_time = ((double)counter) / N;
	fprintf(f, "%f %f %fs %d %d %d %d %f %d \n", p1, mean_time, delta, a, b, x, N, p, P);
	free(seed);
}

int main(int argc, char **argv) {
	if( argc == 7 ) {
		int a = atoi(argv[1]);
		int b = atoi(argv[2]);
		int x = atoi(argv[3]);
		int N = atoi(argv[4]);
		double p = atof(argv[5]);
		int P = atoi(argv[6]);
		
		if(a >= b || N == 0 || x < a || x > b || P == 0 || p > 1 || p < 0)
			return 0;
		FILE *f = fopen("stats.txt", "w");
		if (f != NULL) {
			random_walk(f, a, b, x, N, p, P);
		}
		fclose(f);
	}
	return 0;
}
