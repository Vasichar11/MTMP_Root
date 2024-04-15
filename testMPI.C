#include <stdio.h>
#include <mpi.h>

int main(int argc, char *argv[]) {
    int rank, size;
    int N = 100;
    int sum = 0;
    
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    int chunk = N / size;
    int start = rank * chunk + 1;
    int end = (rank == size - 1) ? N : start + chunk - 1;

    for (int i = start; i <= end; i++) {
        sum += i;
    }

    int totalSum;
    MPI_Reduce(&sum, &totalSum, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

    if (rank == 0) {
        printf("The sum of numbers from 1 to %d is: %d\n", N, totalSum);
    }

    MPI_Finalize();

    return 0;
}
