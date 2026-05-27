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

void inicializarFila(Fila *fila);
int  filaVazia(Fila *fila);
int  tamanhoFila(Fila *fila);
void enfileirar(Fila *fila, Paciente paciente);
int  desenfileirar(Fila *fila, Paciente *removido);
int  frenteFila(Fila *fila, Paciente *atual);
void liberarFila(Fila *fila);
void listarFila(Fila *fila);

#endif
