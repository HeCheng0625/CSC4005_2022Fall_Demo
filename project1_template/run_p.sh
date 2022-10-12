mpic++ odd_even_parallel_sort.cpp -o psort
mpirun -np 4 ./psort 20 ./test_data/20a.in
./check 20 ./test_data/20a.in.parallel.out