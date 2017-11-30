#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <time.h>

#define MASTER 0

void MPI_io(int l, int a, int b, int N, int rank, int size) 
{
    int seed;
    int *res  = calloc(l * l * a * b, sizeof(int));
    int *seeds = (int*)malloc(size * sizeof(int));   
    if (rank == MASTER) 
    {
        srand(time(NULL));
        for (int i = 0; i < size; ++i) 
        {
            seeds[i] = rand();
        }
    }
	
    MPI_Scatter(seeds, 1, MPI_UNSIGNED, &seed, 1, MPI_UNSIGNED, 0, MPI_COMM_WORLD);
	
    srand(seed);
    for (int i = 0; i < N; i++) 
    {
        int x = rand() % l;
        int y = rand() % l;
        int r = rand() % a * b;
        res[(y * l + x) * a * b + r] += 1;
    }

    MPI_File data;
    MPI_Datatype contiguous, view;
    MPI_File_open(MPI_COMM_WORLD, "data.bin", MPI_MODE_CREATE | MPI_MODE_WRONLY, MPI_INFO_NULL, &data);
    MPI_File_set_size(data, 0);
    MPI_Type_contiguous(l * a * b, MPI_INT, &contiguous);
    MPI_Type_create_resized(contiguous, 0, a * l * a * b * sizeof(int), &view);
    MPI_Type_commit(&view);
	
    int whole = rank / a;
    int remainder = rank % a;

    MPI_File_set_view(data, (whole * a * l + remainder) * l * a * b * sizeof(int), MPI_INT, view, "native", MPI_INFO_NULL);
    MPI_File_write_all(data, res, l * l * a * b, MPI_INT, MPI_STATUS_IGNORE);

    MPI_File_close(&data);

    free(seeds);
    free(res);
}

int main (int argc, char** argv) {
		
    int l = atoi(argv[1]);
    int a = atoi(argv[2]);
    int b = atoi(argv[3]);
    int N = atoi(argv[4]);

    MPI_Init(&argc, &argv);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    double start, end;
    if(rank == MASTER) 
    {
        start =  MPI_Wtime();
    }
	
    MPI_io(l, a, b, N, rank, size);

    if(rank == MASTER) 
    {
        end = MPI_Wtime();
        double delta = end - start;
        FILE *f = fopen("stats.txt", "w");
        fprintf(f, "%d %d %d %d %f", l, a, b, N, delta);
        fclose(f);
    }

    MPI_Finalize();
    return 0;
}


