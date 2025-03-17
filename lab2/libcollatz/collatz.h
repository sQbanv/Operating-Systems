#ifndef COLLATZ_H
#define COLLATZ_H

//Funkcja realizująca regułę Collatza
int collatz_conjecture(int input);

// Funkcja sprawdza po ilu wywołaniach collatze_conjecture zbiega się do 1
int test_collatz_convergence(int input, int max_iter);

#endif