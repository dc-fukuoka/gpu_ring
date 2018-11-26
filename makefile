all:
	mpif90 -acc acc_ring.f90 -o acc_ring

clean:
	rm -f acc_ring *~
