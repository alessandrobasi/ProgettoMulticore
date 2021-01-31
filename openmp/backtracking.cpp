
/*

Progetto n� 7 Sudoku
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
#include <stdio.h>

/*
Progetto Backtracking singolo thread
*/

/////////////////////////////
/*
questo gruppo di funzioni controllano nel sudoku passato se rispettivamente, 
in riga, colonna o quadratro esiste o meno il numero passato,
se esiste ritorna true, altrimentri false
*/
bool _check_row(int sudoku[9][9], int trynum, int riga) {
    bool test = false;
    int incr;
    for (incr = 0; incr < 9; incr++) {
        if (!test && trynum == sudoku[riga][incr]) {
            test = true;
        }
    }
    return test;
}

bool _check_col(int sudoku[9][9], int trynum, int col) {
    bool test = false;
    int incr;
    for (incr = 0; incr < 9; incr++) {
        if (!test && trynum == sudoku[incr][col]) {
            test = true;
        }
    }
    return test;
}

bool _check_square(int sudoku[9][9], int trynum, int riga, int col) {
    riga -= riga % 3;
    col -= col % 3;
    bool test = false;
    int add_riga, add_col;
    for (add_riga = 0; add_riga < 3; add_riga++) {
        for (add_col = 0; add_col < 3; add_col++) {
            if (!test && trynum == sudoku[riga+add_riga][col+add_col]) {
                test = true;
            }
        }
    }
    return test;
}
/////////////////////////////

/*
questa funzione raggruppa i risultati del gruppo di verifica del numero
ritorna fasle se almeno una funzione ritorna true
*/
bool check_space(int sudoku[9][9], int trynum, int row, int col) {

    bool Brow, Bcol, Bsquare;

    Brow = _check_row(sudoku, trynum, row);
    Bcol = _check_col(sudoku, trynum, col);
    Bsquare = _check_square(sudoku, trynum, row, col);
    
    // le funzioni ritornano se il numero � presente nella riga|colonna|quadrato
    // di conseguenza se � presente il posto non va bene
    return !Brow && !Bcol && !Bsquare;
}

/*
dato il sudoku, ritorno il numero di riga e colonna della prima
posizione libera presente
*/
void next_pos_vuoto(int sudoku[9][9], int &row, int &col) {
    int i, j;
    for (i = 0; i < 9; i++) {
        for(j = 0; j < 9; j++){
            if (sudoku[i][j] == 0) {
				if (row < 0 && col < 0) {
					row = i;
					col = j;
				}// if row col
				break;
            }// if sudoku
        }// for j
    }// for i
}// next_pos_vuoto

/*
funzione ricorsiva, chiama la funzione 'next_pos_vuoto' per ottenere una posizione vuota
passo base: controlla se c'è almeno una posizione vuota, se non c'è torna indietro (soluzione trovata)
passo induttivo: controlla se è possibile assegnare un valore alla posizione,
	se è possibile, segna la posizione ed entra in ricorsione
	se non è possibile trovare un numero corretto, un numero precedente è errato, ternima quel ciclo di ricorsione
*/
bool backtracking(int sudoku[9][9]) {
    int row=-1, col=-1;
    // Cerco la prima posizione vuota nello schema
    next_pos_vuoto(sudoku, row, col);
    
    // Controllo se esiste almeno una posizione vuota
    if (row < 0 && col < 0) {
        return true;
    }

    // tento di cercare il numero giusto
    // !! Nessun parallelismo poich� ricorsivo
    // !! Su Windows non � presente la direttiva Task
	int trynum;
    for (trynum = 1; trynum < 10; trynum++) {

        // calcolo se il numero del for pu� essere inserito
        if (check_space(sudoku, trynum, row, col)) {
            sudoku[row][col] = trynum;

            // eseguo la ricorsione avendo bloccato la prima posisione vuota
            if (backtracking(sudoku)){
				return true;
			}else{
				sudoku[row][col] = 0;
			}
            // se tutte le ricorsioni falliscono azzera la posizione
            
        }

    }
    
    
    return false;
}

////////// debug ////////////
void _pprint(int sudoku[9][9]) {
    for (int i = 0; i < 9; i++) {
        for(int j = 0; j<9; j++){
            std::cout << sudoku[i][j] << " ";
        }
        std::cout << std::endl;
    }
    std::cout << "\n" << std::endl;
}
/////////////////////////////


int main(int argc, char* argv[]) {
    // costruzione del sudoku
    int sudoku[9][9] = {
            { 0 , 0 , 0 , 1 , 8 , 0 , 3 , 6 , 0 }, //  0 ...  8
            { 0 , 7 , 0 , 2 , 0 , 4 , 0 , 0 , 8 }, //  9 ... 17
            { 0 , 0 , 0 , 0 , 5 , 7 , 2 , 0 , 9 }, // 18 ... 26
            { 1 , 6 , 0 , 0 , 7 , 0 , 9 , 8 , 0 }, // 27 ... 35
            { 8 , 0 , 3 , 5 , 0 , 1 , 6 , 0 , 2 }, // 36 ... 44
            { 0 , 5 , 7 , 0 , 9 , 0 , 0 , 3 , 4 }, // 45 ... 53
            { 5 , 0 , 2 , 7 , 6 , 0 , 0 , 0 , 0 }, // 54 ... 62
            { 9 , 0 , 0 , 4 , 0 , 8 , 0 , 5 , 0 }, // 63 ... 71
            { 0 , 8 , 6 , 0 , 1 , 5 , 0 , 0 , 0 }, // 72 ... 80
    };

    // inizio algoritmo di backtracking
    backtracking(sudoku);
	
	// possibile soluzione
    _pprint(sudoku);
    return 0;
}