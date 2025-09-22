all:
	mpif90 -cuda cuda_ring.CUF -o cuda_ring_cuf

clean:
	rm -f cuda_ring_cuf *~
