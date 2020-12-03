
/*

Progetto n° 7 Sudoku
Approccio multicore usato:  MPI, OpenMP  


*/

#include <iostream>
#include <stdio.h>
#include <omp.h>  // Impostazioni di Visual Studio | Proprietà -> C/c++ -> Linguaggio -> Supporto per OpenMP
#include <mpi.h>  // https://www.microsoft.com/en-us/download/details.aspx?id=100593


int main(int argc, char* argv[]) {
    std::cout << "Hello World!" << std::endl;

    std::cout << "Parametri: ";

    for (int i = 1; i < argc; i++) {
        printf("%s ", argv[i]);
    }

    return 0;
}