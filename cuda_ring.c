#include <cuda.h>
#include <cuda_runtime.h>
#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define CUDACHECK(CALL)                                                 \
        do {                                                            \
                cudaError_t e = CALL;                                   \
                if (e != cudaSuccess) {                                 \
                        printf("Cuda failure %s:%d: '%s'\n", __FILE__, __LINE__, cudaGetErrorString(e)); \
                        exit(0);                                        \
                }                                                       \
        } while(0)
#if 0
static double dclock(void)
{
	struct timespec tp;
	clock_gettime(CLOCK_REALTIME, &tp);
	return (double)(tp.tv_sec + 1.0e-9*tp.tv_nsec);
	
}
#endif
int main(int argc, char **argv)
{
        int iam, np;
	long i;
        long *hs, *hr;
        long *ds, *dr;
        MPI_Status stat;
	int sendto, recvfrom;
	int ndevs, mydev;
	int size = argv[1] ? atol(argv[1]) : 4;
	double time, t0;

        MPI_Init(&argc, &argv);
        MPI_Comm_rank(MPI_COMM_WORLD, &iam);
        MPI_Comm_size(MPI_COMM_WORLD, &np);

	CUDACHECK(cudaGetDeviceCount(&ndevs));
	mydev = iam % ndevs;
	CUDACHECK(cudaSetDevice(mydev));
#ifdef _DEBUG
	printf("iam: %d mydev: %d\n", iam, mydev);
#endif
	
	if (!iam) printf("size: %d\n", size);

	sendto = iam + 1;
	if (iam == np-1) sendto = 0;
	recvfrom = iam - 1;
	if (iam == 0) recvfrom = np - 1;

        hs = (long*)malloc(sizeof(*hs)*size);
        hr = (long*)malloc(sizeof(*hr)*size);

        for (i=0; i<size; i++) {
                hs[i] = 10*iam + i;
                hr[i] = -1;
        }

        CUDACHECK(cudaMalloc((void**)&ds, sizeof(*ds)*size));
        CUDACHECK(cudaMalloc((void**)&dr, sizeof(*dr)*size));

        CUDACHECK(cudaMemcpy(ds, hs, sizeof(*ds)*size, cudaMemcpyDefault));
        CUDACHECK(cudaMemcpy(dr, hr, sizeof(*dr)*size, cudaMemcpyDefault));

	MPI_Barrier(MPI_COMM_WORLD);
	t0 = MPI_Wtime();
        if (!iam) {
                MPI_Send(ds, size, MPI_LONG, sendto,   0, MPI_COMM_WORLD);
		MPI_Recv(dr, size, MPI_LONG, recvfrom, 0, MPI_COMM_WORLD, &stat);
		
        } else {
                MPI_Recv(dr, size, MPI_LONG, recvfrom, 0, MPI_COMM_WORLD, &stat);
		MPI_Send(ds, size, MPI_LONG, sendto,   0, MPI_COMM_WORLD);
        }
	MPI_Barrier(MPI_COMM_WORLD);
	time = MPI_Wtime() - t0;

        CUDACHECK(cudaMemcpy(hr, dr, sizeof(*dr)*size, cudaMemcpyDefault));

	if (!iam) printf("time[s]: %lf\n", time);
#ifdef _DEBUG
        printf("iam: %d: ", iam);
        for (i=0; i<size; i++)
                printf("%ld ", hr[i]);
	printf("\n");
#endif 

        CUDACHECK(cudaFree(ds));
        CUDACHECK(cudaFree(dr));
        free(hs);
        free(hr);

        MPI_Finalize();
        return 0;
}
