ALL:
	mpicc -D_OMPI_CUDA_OPA -g cuda_ring.c -lcuda -lcudart -lrt -o cuda_ring
clean:
	rm -f *~ cuda_ring
