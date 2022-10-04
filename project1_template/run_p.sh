mpic++ odd_even_parallel_sort.cpp -o psort
mpirun -np 32 ./psort 131072 ./test_data/131072a.in
./check 131072 ./test_data/131072a.in.parallel.out