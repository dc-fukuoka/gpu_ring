all:
	mpif90 -acc acc_ring.f90

clean:
	rm -f a.out *~
