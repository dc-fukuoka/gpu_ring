program main
  use openacc
  use mpi_f08
  implicit none
  
  integer :: iam, np
  type(mpi_status) :: stat
  integer :: i
  integer :: mydev, ndevs
  integer :: sendto, recvfrom
  real(8),allocatable,dimension(:) :: sendbuf, recvbuf
  integer :: clk(2), clk_rate, clk_max
  real(8) :: time
  character(len=32) :: argv1
  integer :: size

  call mpi_init
  call mpi_comm_rank(mpi_comm_world, iam)
  call mpi_comm_size(mpi_comm_world, np)

  ndevs = acc_get_num_devices(acc_device_nvidia)
  mydev = mod(iam, ndevs)
  call acc_set_device_num(mydev, acc_device_nvidia)
  write(6, *) "iam:", iam, "mydev:", mydev
  
  if (command_argument_count() == 0) then
     size = 1024
  else
     call get_command_argument(1, argv1)
     read(argv1, *) size
  end if

  if (iam == 0) write(6, *) "size:", size
  
  allocate(sendbuf(size), recvbuf(size))

  do i = 1, size
     sendbuf(i) = 10.0d0*i+iam
     recvbuf(i) = 0.0d0
  end do

  sendto = iam + 1
  if (iam == np - 1) sendto = 0
  recvfrom = iam -1
  if (iam == 0) recvfrom = np - 1

  !$acc enter data copyin(sendbuf(:)) create(recvbuf(:))
  call mpi_barrier(mpi_comm_world)
  call system_clock(clk(1), clk_rate, clk_max)
  if (iam == 0) then
     !$acc host_data use_device(sendbuf(:), recvbuf(:))
     call mpi_send(sendbuf, size, mpi_real8, sendto,   0, mpi_comm_world)
     call mpi_recv(recvbuf, size, mpi_real8, recvfrom, 0, mpi_comm_world, stat)
     !$acc end host_data
  else
     !$acc host_data use_device(sendbuf(:), recvbuf(:))
     call mpi_recv(recvbuf, size, mpi_real8, recvfrom, 0, mpi_comm_world, stat)
     call mpi_send(sendbuf, size, mpi_real8, sendto,   0, mpi_comm_world)
     !$acc end host_data
  endif
  call mpi_barrier(mpi_comm_world)
  call system_clock(clk(2), clk_rate, clk_max)
  !$acc update host(recvbuf(:))
  !$acc exit data delete(sendbuf(:), recvbuf(:))
  time = real(clk(2) - clk(1), 8)/clk_rate
  if (iam == 0) write(6, *) "time[s]:", time
  
  deallocate(sendbuf, recvbuf)
  call mpi_finalize
  stop
end program main
