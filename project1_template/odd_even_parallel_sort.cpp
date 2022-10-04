#include <mpi.h>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <chrono>

void swap(int nums[], int i, int j) {
    int temp = nums[i];
    nums[i] = nums[j];
    nums[j] = temp;
}

int main (int argc, char **argv){

    MPI_Init(&argc, &argv); 

    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);  
    
    int world_size;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);

    int num_elements; // number of elements to be sorted
    
    num_elements = atoi(argv[1]); // convert command line argument to num_elements

    int elements[num_elements]; // store elements
    int sorted_elements[num_elements]; // store sorted elements

    if (rank == 0) { // read inputs from file (master process)
        std::ifstream input(argv[2]);
        int element;
        int i = 0;
        while (input >> element) {
            elements[i] = element;
            i++;
        }
        std::cout << "actual number of elements:" << i << std::endl;
    }

    std::chrono::high_resolution_clock::time_point t1;
    std::chrono::high_resolution_clock::time_point t2;
    std::chrono::duration<double> time_span;
    if (rank == 0){ 
        t1 = std::chrono::high_resolution_clock::now(); // record time
    }

    /* TODO BEGIN*/

    int num_my_element = num_elements / world_size; // number of elements allocated to each process
    int my_element[num_my_element]; // store elements of each process
    MPI_Scatter(elements, num_my_element, MPI_INT, my_element, num_my_element, MPI_INT, 0, MPI_COMM_WORLD); // distribute elements to each process
    
    int odd_even = 0;
    int l_num, r_num, ll_num, rr_num;
    int k = 0;
    int all_sorted = 0;
    int sorted = 0;
    while (!all_sorted) {
        all_sorted = 1;
        sorted = 1;
        for (int i = odd_even; i < num_my_element - 1; i += 2) {
            if (my_element[i] > my_element[i+1]) {
                swap(my_element, i, i+1);
                sorted = 0;
            }
        }
        l_num = my_element[0];
        r_num = my_element[num_my_element - 1];

        if (rank == 0) {
            MPI_Send(&r_num, 1, MPI_INT, rank + 1, 0, MPI_COMM_WORLD);
            MPI_Recv(&rr_num, 1, MPI_INT, rank + 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

            if (rr_num < r_num) {
                my_element[num_my_element - 1] = rr_num;
                sorted = 0;
            }
        }
        else if (rank == world_size - 1)
        {
            MPI_Recv(&ll_num, 1, MPI_INT, rank - 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            MPI_Send(&l_num, 1, MPI_INT, rank - 1, 0, MPI_COMM_WORLD);

            if (ll_num > l_num) {
                my_element[0] = ll_num;
                sorted = 0;
            }
        }
        else {
            MPI_Recv(&ll_num, 1, MPI_INT, rank - 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            MPI_Send(&l_num, 1, MPI_INT, rank - 1, 0, MPI_COMM_WORLD);
            
            MPI_Send(&r_num, 1, MPI_INT, rank + 1, 0, MPI_COMM_WORLD);
            MPI_Recv(&rr_num, 1, MPI_INT, rank + 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

            if (ll_num > l_num) {
                my_element[0] = ll_num;
                sorted = 0;
            }
            if (rr_num < r_num) {
                my_element[num_my_element - 1] = rr_num;
                sorted = 0;
            }
        }

        MPI_Barrier(MPI_COMM_WORLD);

        if (rank != 0) MPI_Send(&sorted, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);

        if (rank == 0) {
            if (sorted == 0) all_sorted = 0;
            for (int i = 1; i < world_size; i++) {
                MPI_Recv(&sorted, 1, MPI_INT, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                if (sorted == 0) all_sorted = 0;
            }
        }

        MPI_Barrier(MPI_COMM_WORLD);

        MPI_Bcast(&all_sorted, 1, MPI_INT, 0, MPI_COMM_WORLD);

        odd_even = (odd_even + 1)%2;
        k++;
    }
    
    
    MPI_Gather(my_element, num_my_element, MPI_INT, sorted_elements, num_my_element, MPI_INT, 0, MPI_COMM_WORLD); // collect result from each process
    
    /* TODO END */

    if (rank == 0){ // record time (only executed in master process)
        t2 = std::chrono::high_resolution_clock::now();  
        time_span = std::chrono::duration_cast<std::chrono::duration<double>>(t2 - t1);
        std::cout << "Student ID: " << "119010319" << std::endl; // replace it with your student id
        std::cout << "Name: " << "Yuancheng Wang" << std::endl; // replace it with your name
        std::cout << "Assignment 1" << std::endl;
        std::cout << "Run Time: " << time_span.count() << " seconds" << std::endl;
        std::cout << "Input Size: " << num_elements << std::endl;
        std::cout << "Process Number: " << world_size << std::endl; 
    }

    if (rank == 0){ // write result to file (only executed in master process)
        std::ofstream output(argv[2]+std::string(".parallel.out"), std::ios_base::out);
        for (int i = 0; i < num_elements; i++) {
            output << sorted_elements[i] << std::endl;
        }
    }
    
    MPI_Finalize();
    
    return 0;
}


