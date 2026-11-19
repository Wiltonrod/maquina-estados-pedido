#include "helpers.h"

const Transicao VALIDAS[NUM_VALIDAS] = {
    {CRIADO,     EV_PAGAR,             PAGO},
    {CRIADO,     EV_CANCELAR,          CANCELADO},
    {PAGO,       EV_INICIAR_PREPARO,   EM_PREPARO},
    {PAGO,       EV_CANCELAR,          CANCELADO},
    {EM_PREPARO, EV_FINALIZAR_PREPARO, PRONTO},
    {EM_PREPARO, EV_CANCELAR,          CANCELADO},
    {PRONTO,     EV_DESPACHAR,         EM_ENTREGA},
    {EM_ENTREGA, EV_CONFIRMAR_ENTREGA, ENTREGUE},
};

int destino_esperado(Estado s, Evento e) {
    for (int i = 0; i < NUM_VALIDAS; i++) {
        if (VALIDAS[i].origem == s && VALIDAS[i].evento == e) {
            return (int)VALIDAS[i].destino;
        }
    }
    return -1;
}

/* Menor sequência de eventos que leva um pedido novo a cada estado. */
static const Evento CAMINHO[NUM_ESTADOS][5] = {
    [CRIADO]     = {0},
    [PAGO]       = {EV_PAGAR},
    [EM_PREPARO] = {EV_PAGAR, EV_INICIAR_PREPARO},
    [PRONTO]     = {EV_PAGAR, EV_INICIAR_PREPARO, EV_FINALIZAR_PREPARO},
    [EM_ENTREGA] = {EV_PAGAR, EV_INICIAR_PREPARO, EV_FINALIZAR_PREPARO, EV_DESPACHAR},
    [ENTREGUE]   = {EV_PAGAR, EV_INICIAR_PREPARO, EV_FINALIZAR_PREPARO, EV_DESPACHAR,
                    EV_CONFIRMAR_ENTREGA},
    [CANCELADO]  = {EV_CANCELAR},
};
static const int TAM_CAMINHO[NUM_ESTADOS] = {
    [CRIADO] = 0, [PAGO] = 1, [EM_PREPARO] = 2, [PRONTO] = 3,
    [EM_ENTREGA] = 4, [ENTREGUE] = 5, [CANCELADO] = 1,
};

Pedido pedido_em(Estado alvo) {
    Pedido p;
    pedido_inicializa(&p);
    for (int i = 0; i < TAM_CAMINHO[alvo]; i++) {
        pedido_processa_evento(&p, CAMINHO[alvo][i]);
    }
    return p;
}
