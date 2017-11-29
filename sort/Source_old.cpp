#include <stdlib.h>
#include <iostream>
#include <omp.h>
#include <time.h>
#include <windows.h>
using namespace std;





inline void swap(int &a, int &b) {
	int temp = a;
	a = b;
	b = temp;
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
		//cout << left << ' ' << mid << ' '  << right << ' ' << array[mid] << ' ' << x << ' ' << array[right] << '\n';
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
/*
int Search(int x, int *array, int left, int right) {
	if (right < left || x <= array[left]) {
		return left;
	}
	else {
		int i = right + 1;
		while (array[i - 1] >= x)
			--i;
		return i;
	}
}
*/

void mergeMT(int *T, int left1, int right1, int left2, int right2, int * A, int left3, int P) {
	int n1 = right1 - left1 + 1;
	int	n2 = right2 - left2 + 1;
	if (n1 < n2) {
		swap(left1, left2);
		swap(right1, right2);
		swap(n1, n2);
	}
	if (n1 == 0)
		return;
	else {
		int mid1 = (left1 + right1) / 2;
		int mid2 = binarySearch(T[mid1], T, left2, right2);
		int mid3 = left3 + (mid1 - left1) + (mid2 - left2);
		A[mid3] = T[mid1];
		#pragma omp parallel num_threads(P)
		{
			#pragma omp task
			mergeMT(T, left1, mid1 - 1, left2, mid2 - 1, A, left3, P);
			#pragma omp task
			mergeMT(T, mid1 + 1, right1, mid2, right2, A, mid3 + 1, P);
		}
	}
}


void initialization(int *up, int N, int a, int b) {
	srand(time(NULL));
	for (int i = 0; i < N; i++) {
		up[i] = rand() % (b - a + 1) + a;
	}
}

int compare(const void * x1, const void * x2)   // функция сравнения элементов массива
{
	return (*(int*)x1 - *(int*)x2);              // если результат вычитания равен 0, то числа равны, < 0: x1 < x2; > 0: x1 > x2
}

int main() 
{
	/*
	int n, a, b, x;
	int *A;
	cin >> n;
	A = new int[n];

	for (int i = 0; i < n; i++)
		A[i] = i + 1;
	for (int a = 0; a < n; a++)
		for (int b = 0; b < n; b++)
			for (int x = 0; x < n; x++) {
				if (binarySearch(x, A, a - 1, b - 1) != Search(x, A, a - 1, b - 1))
					cout << x << ' ' << binarySearch(x, A, a - 1, b - 1) << ' ' << Search(x, A, a - 1, b - 1) << '\n';
			}
	system("pause");
	*/


	int n, m, P;
	int *a, *b, *c, *d;
	cin >> n >> m >> P;
	omp_set_num_threads(P);
	a = new int[n];
	b = new int[n];
	d = new int[n];
	initialization(a, n, 0, 100);
	for (int i = 0; i < n; ++i)
		d[i] = a[i];

	SYSTEMTIME start, end;
	GetSystemTime(&start);
	#pragma omp parallel
	{
		#pragma omp for
		{
			for (int i = 0; i < n - m; i += m) {
				qsort(a + i, m, sizeof(int), compare);
			}
		}
	}
	if (n % m != 0)
		qsort(a + (n / m) * m, n % m, sizeof(int), compare);
	else 
		qsort(a + ((n - 1) / m) * m, m, sizeof(int), compare);
	for (int i = 0; i < n; ++i)
		b[i] = a[i];
	GetSystemTime(&end);
	double delta1 = (end.wMinute - start.wMinute) * 60 + end.wSecond - start.wSecond;
	delta1 += (end.wMilliseconds - start.wMilliseconds) / 1000.0;
	cout << delta1 << '\n';
	for (int M = m; M < n; M *= 2) {
		#pragma omp parallel for num_threads(P)
		for (int i = 0; i < n - 2 * M; i += 2 * M) {
			mergeMT(a, i, i + M - 1, i + M, i + 2 * M - 1, b, i, P);
		}
		if (n % (2 * M) > M) {
			mergeMT(a, n - n % (2 * M), n - n % M - 1, n - n % M, n - 1, b, n - n % (2 * M), P);
		}
		else if (n % (2 * M) == 0){
			mergeMT(a, n - 2 * M, n - M - 1, n - M, n - 1, b, n - 2 * M, P);
		}
		else {
			for (int j = n - n % (2 * M); j < n; j++)
				b[j] = a[j];
		}
		c = a;
		a = b;
		b = c;
	}
	/*

	for (int i = 0; i < n; ++i)
		cout << a[i] << ' ';
	cout << '\n';
	for (int i = 0; i < n; ++i)
		cout << b[i] << ' ';
	cout << '\n' << '\n';
	*/

	bool success = true;
	//for (int i = 0; i < n; ++i) {
	//	cout << a[i] << ' ';
	//}
	for (int i = 1; i < n; ++i)
		if (a[i - 1] > a[i]) {
			success = false;
			cout << '\n' << i << ' ' << a[i - 1] << ' ' << a[i] << '\n';
		}
	cout << '\n' << success << '\n';
	GetSystemTime(&end);
	double delta = (end.wMinute - start.wMinute) * 60 + end.wSecond - start.wSecond;
	delta += (end.wMilliseconds - start.wMilliseconds) / 1000.0;
	cout << delta << '\n';

	GetSystemTime(&start);
	qsort(a, n, sizeof(int), compare);
	GetSystemTime(&end);
	delta = (end.wMinute - start.wMinute) * 60 + end.wSecond - start.wSecond;
	delta += (end.wMilliseconds - start.wMilliseconds) / 1000.0;
	cout << delta << '\n';
	system("pause");
	delete[] b;
	delete[] a;
	delete[] d;
	return 0;
}