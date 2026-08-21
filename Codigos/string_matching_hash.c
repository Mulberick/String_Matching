#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define PRIMO 101   // Numero primo para el calculo del hash
#define BASE  256   // cardinalidad del alfabeto (1 byte = 256 valores posibles)

int MatchingHash(char *T, char *P){

    long n = strlen(T);
    long m = strlen(P);

    if (m == 0 || m > n) {
        fprintf(stderr, "Patron invalido o mas largo que el texto\n");
        return EXIT_FAILURE;
    }

    // h = BASE^(m-1) mod PRIMO -> el "peso" de la primera letra de la mascara
    long peso = 1;
    for (long i = 0; i < m - 1; i++) {
        peso = (peso * BASE) % PRIMO;
    }
    printf(" - Peso h de la primera letra (BASE^%ld mod %d): %ld\n", m-1, PRIMO, peso);

    // Hash inicial del patron y de la primera mascara del texto (con pesos)
    long hashP = 0, hashT = 0;
    for (long i = 0; i < m; i++) {
        hashP = (BASE * hashP + P[i]) % PRIMO;
        hashT = (BASE * hashT + T[i]) % PRIMO;
        printf(" - Valor de hash de P: %ld | Valor de hash de T: %ld\n", hashP, hashT);
    }
    printf(" - Valor de hash de P: %ld | Valor de hash de T: %ld\n", hashP, hashT);
    printf("\n");

    for (long i = 0; i <= n - m; i++) {
        if (hashT == hashP) {
            if (strncmp(T + i, P, m) == 0) {
                printf("Coincidencia encontrada en la posicion: %ld\n", i);
            }
        }
        if (i < n - m) {
            printf("Avanzando: quita '%c', agrega '%c' -> ", T[i], T[i + m]);
            hashT = (BASE * (hashT - T[i] * peso) + T[i + m]) % PRIMO;
            printf("hashT = %ld\n", hashT);
            if (hashT < 0) hashT += PRIMO;  // normalizar modulo negativo
        }
    }

    return EXIT_SUCCESS;
}

int main(int argc, char **argv) {
    if (argc != 3) {
        fprintf(stderr, "Uso: %s <cadena> <cadena a buscar>\n", argv[0]);
        return EXIT_FAILURE;
    }

    char *T = argv[1];
    char *P = argv[2];

    MatchingHash(T, P);

    return EXIT_SUCCESS;
}