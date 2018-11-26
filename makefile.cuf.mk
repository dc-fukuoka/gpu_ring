all:
	mpif90 -ta=tesla:cc60 cuda_ring.CUF -o cuda_ring_cuf

clean:
	rm -f cuda_ring_cuf *~
