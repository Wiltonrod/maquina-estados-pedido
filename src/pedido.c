#include "pedido.h"

void pedido_inicializa(Pedido *p) {
    p->estado = CRIADO;
    p->historico[0] = CRIADO;
    p->tam_historico = 1;
}

Resultado pedido_processa_evento(Pedido *p, Evento e) {
    Estado s = p->estado;
    Estado proximo;

    if (s == CRIADO && e == EV_PAGAR) {                      /* T1 */
        proximo = PAGO;
    } else if (s == CRIADO && e == EV_CANCELAR) {            /* T2 */
        proximo = CANCELADO;
    } else if (s == PAGO && e == EV_INICIAR_PREPARO) {       /* T3 */
        proximo = EM_PREPARO;
    } else if (s == PAGO && e == EV_CANCELAR) {              /* T4 */
        proximo = CANCELADO;
    } else if (s == EM_PREPARO && e == EV_FINALIZAR_PREPARO) { /* T5 */
        proximo = PRONTO;
    } else if (s == EM_PREPARO && e == EV_CANCELAR) {        /* T6 */
        proximo = CANCELADO;
    } else if (s == PRONTO && e == EV_DESPACHAR) {           /* T7 */
        proximo = EM_ENTREGA;
    } else if (s == EM_ENTREGA && e == EV_CONFIRMAR_ENTREGA) { /* T8 */
        proximo = ENTREGUE;
    } else {
        return RESULT_TRANSICAO_INVALIDA; /* rota de falha segura: nada é alterado */
    }

    p->estado = proximo;
    p->historico[p->tam_historico++] = proximo;
    return RESULT_OK;
}

int pedido_finalizado(const Pedido *p) {
    return p->estado == ENTREGUE || p->estado == CANCELADO;
}

const char *estado_mensagem_cliente(Estado s) {
    switch (s) {
    case CRIADO:     return "Aguardando pagamento";
    case PAGO:       return "Pagamento confirmado";
    case EM_PREPARO: return "Em preparo na cozinha";
    case PRONTO:     return "Pronto, aguardando o entregador";
    case EM_ENTREGA: return "Saiu para entrega";
    case ENTREGUE:   return "Pedido entregue";
    case CANCELADO:  return "Pedido cancelado";
    default:         return "Estado desconhecido";
    }
}

const char *estado_nome(Estado s) {
    static const char *nomes[NUM_ESTADOS] = {
        "CRIADO", "PAGO", "EM_PREPARO", "PRONTO", "EM_ENTREGA", "ENTREGUE", "CANCELADO"
    };
    return ((int)s >= 0 && (int)s < NUM_ESTADOS) ? nomes[s] : "?";
}

const char *evento_nome(Evento e) {
    static const char *nomes[NUM_EVENTOS] = {
        "pagar", "iniciar_preparo", "finalizar_preparo",
        "despachar", "confirmar_entrega", "cancelar"
    };
    return ((int)e >= 0 && (int)e < NUM_EVENTOS) ? nomes[e] : "?";
}
