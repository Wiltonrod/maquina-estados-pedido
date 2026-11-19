#ifndef TESTLIB_H
#define TESTLIB_H

/* Mini biblioteca de testes (sem dependências externas). */

#include <stdio.h>

typedef struct {
    const char *nome;
    int total;
    int falhas;
} Suite;

void suite_inicia(Suite *s, const char *nome);

/* Registra UM caso de teste: imprime [OK]/[FALHOU] e contabiliza. */
void caso(Suite *s, const char *id, int condicao);

/* Cada suíte devolve o número de falhas e acumula o total de casos executados. */
int suite_estados(int *casos);
int suite_transicoes(int *casos);
int suite_caminhos(int *casos);

#endif
