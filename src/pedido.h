#ifndef PEDIDO_H
#define PEDIDO_H

/*
 * Máquina de estados do ciclo de vida de um pedido de delivery.
 *
 * Regras:
 *  - todo pedido nasce em CRIADO;
 *  - o pedido avança na ordem: pagar -> iniciar preparo -> finalizar preparo
 *    -> despachar -> confirmar entrega;
 *  - cancelar só é permitido em CRIADO, PAGO e EM_PREPARO;
 *  - ENTREGUE e CANCELADO são finais: nenhum evento é aceito neles;
 *  - evento fora da tabela devolve RESULT_TRANSICAO_INVALIDA e NÃO altera o pedido.
 */

typedef enum {
    CRIADO,
    PAGO,
    EM_PREPARO,
    PRONTO,
    EM_ENTREGA,
    ENTREGUE,
    CANCELADO,
    NUM_ESTADOS
} Estado;

typedef enum {
    EV_PAGAR,
    EV_INICIAR_PREPARO,
    EV_FINALIZAR_PREPARO,
    EV_DESPACHAR,
    EV_CONFIRMAR_ENTREGA,
    EV_CANCELAR,
    NUM_EVENTOS
} Evento;

typedef enum {
    RESULT_OK = 0,
    RESULT_TRANSICAO_INVALIDA = -1
} Resultado;

/* Maior caminho possível: 6 estados (CRIADO ... ENTREGUE). Sobra folga. */
#define MAX_HISTORICO 8

typedef struct {
    Estado estado;
    Estado historico[MAX_HISTORICO]; /* estados percorridos, do inicial ao atual */
    int tam_historico;
} Pedido;

void pedido_inicializa(Pedido *p);

/* Estilo Mealy: o resultado depende do estado atual E do evento recebido. */
Resultado pedido_processa_evento(Pedido *p, Evento e);

/* 1 se o pedido está em ENTREGUE ou CANCELADO. */
int pedido_finalizado(const Pedido *p);

/* Estilo Moore: a saída depende apenas do estado (texto exibido ao cliente). */
const char *estado_mensagem_cliente(Estado s);

const char *estado_nome(Estado s);
const char *evento_nome(Evento e);

#endif
