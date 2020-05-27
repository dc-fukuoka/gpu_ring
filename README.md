gpu_ring - a test of GPU direct with CUDA/OpenACC.  
           CUDA, PGI compiler, and openmpi are required.
=============

* OpenACC version
-------------
~~~
$ make
$ mpirun -x PSM2_GPUDIRECT=1 -x PSM2_CUDA=1 -x LD_LIBRARY_PATH -np 8 -npernode 4 ./acc_ring $((1024*1024)) 2>/dev/null
 size:      1048576
 time[s]:   4.5307000000000000E-002
~~~~
 
* CUDA C version
-------------
mpicc must use gcc.
~~~
$ make -f Makefile.cuda.mk
$ mpirun -x PSM2_GPUDIRECT=1 -x PSM2_CUDA=1 -x LD_LIBRARY_PATH -np 8 -npernode 4 ./cuda_ring $((1024*1024))
size: 1048576
time[s]: 0.032730
~~~

* CUDA Fortran version
-------------
mpif90 must use pgfortran.
~~~
$ make -f makefile.cuf.mk
$ mpirun -x PSM2_GPUDIRECT=1 -x PSM2_CUDA=1 -x LD_LIBRARY_PATH -np 8 -npernode 4 ./cuda_ring_cuf $((1024*1024)) 2>/dev/null
 size:      1048576
 time[s]:   3.5033409949392080E-002
~~~
