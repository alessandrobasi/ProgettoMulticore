
/*

Progetto n° 7 Sudoku
Approccio multicore usato: MPI, OpenMP  
Approccio algoritmo usato: Backtracking, Algoritmo di Crook?
Studenti:
    Daniele Fardella        fardella.1642573@studenti.uniroma1.it
    Alessandro Basilici     basilici.1835392@studenti.uniroma1.it

Sorgenti:
    https://en.wikipedia.org/wiki/Sudoku_solving_algorithms
    http://www.ams.org/notices/200904/tx090400460p.pdf
    https://docs.microsoft.com/it-it/cpp/parallel/openmp/openmp-in-visual-cpp?view=msvc-160

*/

#include <iostream>
#include <vector>
#include <array>
#include <stdio.h>
#include <omp.h>  // Impostazioni di Visual Studio | Proprietà -> C/c++ -> Linguaggio -> Supporto per OpenMP

typedef std::array<int, 81> sudo;
typedef std::vector<std::vector<std::vector<int>>> sudonote;

sudo solving(sudo sudoku) {
    int my_rank = omp_get_thread_num();
    //int thread_count = omp_get_num_threads();
#pragma omp critical
    std::cout << my_rank << "/" << thread_count << std::endl;
}


int main(int argc, char* argv[]) {
    
    if (argc <= 1) {
        std::cout << "\nInserire come argomento al programma il numero di thread\n" << argv[0] << " <Num thread>\n\n" << std::endl;
        return 0;
    }
    int thread_count = atoi(argv[1]);

    sudo sudoku = {
             0 , 0 , 0 , 1 , 8 , 0 , 3 , 6 , 0 , //  0 ...  8
             0 , 7 , 0 , 2 , 0 , 4 , 0 , 0 , 8 , //  9 ... 17
             0 , 0 , 0 , 0 , 5 , 7 , 2 , 0 , 9 , // 18 ... 26
             1 , 6 , 0 , 0 , 7 , 0 , 9 , 8 , 0 , // 27 ... 35
             8 , 0 , 3 , 5 , 0 , 1 , 6 , 0 , 2 , // 36 ... 44
             0 , 5 , 7 , 0 , 9 , 0 , 0 , 3 , 4 , // 45 ... 53
             5 , 0 , 2 , 7 , 6 , 0 , 0 , 0 , 0 , // 54 ... 62
             9 , 0 , 0 , 4 , 0 , 8 , 0 , 5 , 0 , // 63 ... 71
             0 , 8 , 6 , 0 , 1 , 5 , 0 , 0 , 0 , // 72 ... 80
    };


    #pragma omp parallel num_threads(thread_count)
    {
        int my_rank = omp_get_thread_num();
        //int thread_count = omp_get_num_threads();
        #pragma omp critical
        std::cout << my_rank << "/" << thread_count << std::endl;
    }
    
    #pragma omp parallel num_threads(thread_count) 
    solving(sudoku);


    return 0;
}