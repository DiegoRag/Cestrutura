#ifndef LISTA_H
#define LISTA_H

#define PRIORIDADE_EMERGENCIA   1
#define PRIORIDADE_PREFERENCIAL 2
#define PRIORIDADE_COMUM        3

typedef struct No {
    char nome[50];
    int prioridade;
    struct No* prox;
    struct No* ant;
} No;

typedef struct {
    No* inicio;
    No* fim;
} Fila;

void iniciarFila(Fila* f);
void enqueue(Fila* f, const char* info, int prioridade);
void dequeue(Fila* f);

#endif
