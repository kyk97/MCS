#include <iostream>
#include <random>
#include <omp.h>
#include <assert.h>
#include <time.h>
#include <sys/time.h>
using namespace std;

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
		while (X != a && X != b) {
			counter++;
			double p1 = (double)rand_r(&seed[i]) / RAND_MAX;
			if (p1 <= p) {
				X++;
			}
			else {
				X--;
			}
		}
		if (X == b) {
			B++;
		}
	}
	assert(gettimeofday(&end, NULL) == 0);
	double delta = ((end.tv_sec - start.tv_sec) * 1000000u + end.tv_usec - start.tv_usec) / 1.e6;
	//fprintf(f, "%f %f %fs %d %d %d %d %f %d \n", double(B) / N, double(counter) / N, delta, a, b, x, N, p, P);
	fprintf(f, "%fs %d \n", delta, P);
}

int main()
{

	int a, b, x, N, P, counter = 0, B = 0;
	double p;
	FILE *f = fopen("stats.txt", "w");
	a = 0; b = 100; x = 50; p = 0.49; P = 4; N = 100000;
	for (int P = 1; P < 17; P *= 2) {
		random_walk(f, a, b, x, N, p, P);
	}
	fclose(f);
	return 0;
}
