
/*

Progetto n° 7 Sudoku
Approccio multicore usato: MPI, OpenMP  
Approccio algoritmo usato: Backtracking, ???
Studenti:
    Daniele Fardella        fardella.1642573@studenti.uniroma1.it
    Alessandro Basilici     basilici.1835392@studenti.uniroma1.it

Sorgenti:
    https://en.wikipedia.org/wiki/Sudoku_solving_algorithms
    http://www.ams.org/notices/200904/tx090400460p.pdf

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