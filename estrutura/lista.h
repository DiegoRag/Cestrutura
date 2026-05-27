#ifndef LISTA_H
#define LISTA_H

#define PRIORIDADE_EMERGENCIA   1
#define PRIORIDADE_PREFERENCIAL 2
#define PRIORIDADE_COMUM        3

typedef struct Paciente {
    int senha;
    char nome[80];
    int prioridade;
} Paciente;

typedef struct No {
    Paciente paciente;
    struct No *anterior;
    struct No *proximo;
} No;

typedef struct Fila {
    No *inicio;
    No *fim;
    int tamanho;
} Fila;

/*
 * Contrato da fila com prioridade.
 *
 * A fila e adaptada da Lista Duplamente Ligada e mantida ordenada
 * por prioridade:
 *
 *   EMERGENCIA (1)  >  PREFERENCIAL (2)  >  COMUM (3)
 *
 * Dentro da mesma prioridade, a ordem e FIFO (primeiro que chegou,
 * primeiro a ser atendido).
 *
 * Como a ordenacao acontece no momento da insercao, o paciente em
 * `inicio` e sempre o de maior prioridade que chegou primeiro.
 * Por isso, `desenfileirar` apenas remove o no da frente.
 */

void inicializarFila(Fila *fila);
int  filaVazia(Fila *fila);
int  tamanhoFila(Fila *fila);

/* Insere o paciente respeitando a ordem de prioridade descrita acima. */
void enfileirar(Fila *fila, Paciente paciente);

/* Remove o paciente da frente (maior prioridade que chegou primeiro). */
int  desenfileirar(Fila *fila, Paciente *removido);

/* Le, sem remover, o paciente que esta na frente da fila. */
int  frenteFila(Fila *fila, Paciente *atual);

void liberarFila(Fila *fila);
void listarFila(Fila *fila);

#endif
