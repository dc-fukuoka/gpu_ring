all:
	mpif90 -cuda -gpu=rdc cuda_ring.CUF -o cuda_ring_cuf

clean:
	rm -f cuda_ring_cuf *~
