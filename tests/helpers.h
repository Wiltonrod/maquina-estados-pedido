#ifndef HELPERS_H
#define HELPERS_H

#include "../src/pedido.h"

/* Oráculo dos testes: tabela de transições escrita de forma independente do código. */
typedef struct {
    Estado origem;
    Evento evento;
    Estado destino;
} Transicao;

#define NUM_VALIDAS 8
extern const Transicao VALIDAS[NUM_VALIDAS];

/* Devolve o destino esperado ou -1 se (estado, evento) não está na tabela. */
int destino_esperado(Estado s, Evento e);

/* Cria um pedido e o leva ao estado desejado pelo caminho mais curto. */
Pedido pedido_em(Estado alvo);

#endif
