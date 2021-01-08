
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

typedef std::vector<std::vector<std::vector<int>>> sudonote;

bool check_row(int* sudoku, int trynum, int riga) {

    for (int incr = 0; incr < 9; incr++) {
        if (trynum == sudoku[riga * 9 + incr]) {
            return true;
        }
    }
    return false;
}

bool check_col(int* sudoku, int trynum, int col) {
    for (int incr = 0; incr < 9; incr++) {
        if (trynum == sudoku[col + 9 * incr]) {
            return true;
        }
    }
    return false;
}

bool check_square(int* sudoku, int trynum, int riga, int col) {
    riga /= 3;
    col /= 3;

    for (int add_riga = 0; add_riga < 3; add_riga++) {
        for (int add_col = 0; add_col < 3; add_col++) {
            if (trynum == sudoku[9 * (3 * riga + add_riga) + (3 * col + add_col)]) {
                return true;
            }
        }
    }
    return false;
}

int* solving(int* _sudoku, int _thread_count) {
    //int my_rank = omp_get_thread_num();
    //int thread_count = omp_get_num_threads();
    int threads = _thread_count;
    int* sudoku = _sudoku;
    /*#pragma omp critical
    std::cout << my_rank << "/" << thread_count << std::endl;*/
    
    for (int pos = 0; pos < 81; pos++) {
        if (sudoku[pos] == 0) {
            
            int col = pos % 9;
            int row = pos / 9;
            #pragma omp parallel for firstprivate(sudoku)
            for (int trynum = 1; trynum < 10; trynum++) {
                // verifico se mettendo il numero trynum ho una violazione
                if (!check_col(sudoku, trynum, col) &&
                    !check_row(sudoku, trynum, row) &&
                    !check_square(sudoku, trynum, row, col)) {

                    sudoku[pos] = trynum; // tenta la posizione del numero

                    // ricorsione, sperando che il numero messo non porti a un fallimento ti tutti i numeri di una futura cella
                    int* result;
                    #pragma omp parallel private(result)
                    //#pragma omp task private(result)
                    {
                        result = solving(sudoku, threads);
                    }
                    

                    if (result == NULL) { // se il numero immesso prima (trynum) è sbagliato, azzera la posizione del sudoku
                        sudoku[pos] = 0;
                    }
                    else {
                        return result;
                    }
                }
            }
            // nessun numero va bene, quelli precedenti sono errati
            return NULL;
        }
    }
    return sudoku;
}

int* solve2(int* _sudoku) {
    return NULL;
}

////////// debug
void _pprint(int* sudoku) {
    for (int i = 0; i < 81; i++) {

        if (i % 9 == 0) {
            std::cout << std::endl;
        }

        std::cout << sudoku[i] << " ";

    }
    std::cout << "\n\n" << std::endl;
}

int main(int argc, char* argv[]) {
    
    if (argc <= 1) {
        std::cout << "\nInserire come argomento al programma il numero di thread\n" << argv[0] << " <Num thread>\n\n" << std::endl;
        return 0;
    }
    int thread_count = atoi(argv[1]);

    int sudoku[81] = {
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
    _pprint(sudoku);

    //#pragma omp parallel num_threads(thread_count)
    //{
    //    int my_rank = omp_get_thread_num();
    //    //int thread_count = omp_get_num_threads();
    //    #pragma omp critical
    //    std::cout << my_rank << "/" << thread_count << std::endl;
    //}
    
    //#pragma omp parallel num_threads(thread_count) 


    //int* end = solving(sudoku, thread_count);


    for (int pos = 0; pos < 81; pos++) {
        if (sudoku[pos] == 0) {

            int col = pos % 9;
            int row = pos / 9;

            #pragma omp parallel for firstprivate(sudoku)
            for (int trynum = 1; trynum < 10; trynum++) {
                if (!check_col(sudoku, trynum, col) &&
                    !check_row(sudoku, trynum, row) &&
                    !check_square(sudoku, trynum, row, col)) {

                    sudoku[pos] = trynum;

                }
            }
        }
    }

    _pprint(sudoku);

    return 0;
}