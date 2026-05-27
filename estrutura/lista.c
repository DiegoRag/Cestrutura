#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lista.h"

/*
 * Implementacao PROVISORIA da fila com prioridade do MediCore.
 *
 * Estrutura: lista duplamente ligada adaptada como fila.
 *
 * A fila e mantida ordenada por prioridade no momento da insercao:
 *   EMERGENCIA (1)  >  PREFERENCIAL (2)  >  COMUM (3)
 *
 * Dentro da mesma prioridade vale FIFO (quem chegou primeiro
 * fica mais perto da frente). Assim, `desenfileirar` apenas
 * remove o no de `inicio` e ja entrega o paciente correto.
 */

void inicializarFila(Fila *fila) {
    fila->inicio = NULL;
    fila->fim = NULL;
    fila->tamanho = 0;
}

int filaVazia(Fila *fila) {
    return fila->inicio == NULL;
}

int tamanhoFila(Fila *fila) {
    return fila->tamanho;
}

void enfileirar(Fila *fila, Paciente paciente) {
    No *novo = (No *) malloc(sizeof(No));
    if (novo == NULL) {
        return;
    }
    novo->paciente = paciente;
    novo->anterior = NULL;
    novo->proximo = NULL;

    if (fila->inicio == NULL) {
        fila->inicio = novo;
        fila->fim = novo;
        fila->tamanho++;
        return;
    }

    /*
     * Procura o primeiro no com prioridade ESTRITAMENTE menor
     * (numero maior) que a do novo paciente. O `<=` garante que,
     * em caso de empate de prioridade, o recem-chegado entra
     * depois dos que ja estavam la (FIFO dentro do grupo).
     */
    No *atual = fila->inicio;
    while (atual != NULL && atual->paciente.prioridade <= paciente.prioridade) {
        atual = atual->proximo;
    }

    if (atual == NULL) {
        /* Insere no fim. */
        novo->anterior = fila->fim;
        fila->fim->proximo = novo;
        fila->fim = novo;
    } else if (atual == fila->inicio) {
        /* Insere antes do primeiro (nova maior prioridade). */
        novo->proximo = fila->inicio;
        fila->inicio->anterior = novo;
        fila->inicio = novo;
    } else {
        /* Insere entre dois nos existentes. */
        novo->anterior = atual->anterior;
        novo->proximo = atual;
        atual->anterior->proximo = novo;
        atual->anterior = novo;
    }

    fila->tamanho++;
}

int desenfileirar(Fila *fila, Paciente *removido) {
    if (fila->inicio == NULL) {
        return 0;
    }

    No *no = fila->inicio;
    if (removido != NULL) {
        *removido = no->paciente;
    }

    fila->inicio = no->proximo;
    if (fila->inicio != NULL) {
        fila->inicio->anterior = NULL;
    } else {
        fila->fim = NULL;
    }

    free(no);
    fila->tamanho--;
    return 1;
}

int frenteFila(Fila *fila, Paciente *atual) {
    if (fila->inicio == NULL) {
        return 0;
    }
    if (atual != NULL) {
        *atual = fila->inicio->paciente;
    }
    return 1;
}

void liberarFila(Fila *fila) {
    No *atual = fila->inicio;
    while (atual != NULL) {
        No *prox = atual->proximo;
        free(atual);
        atual = prox;
    }
    fila->inicio = NULL;
    fila->fim = NULL;
    fila->tamanho = 0;
}

void listarFila(Fila *fila) {
    No *atual = fila->inicio;
    int posicao = 1;

    while (atual != NULL) {
        const char *prio;
        switch (atual->paciente.prioridade) {
            case PRIORIDADE_EMERGENCIA:   prio = "EMERGENCIA";   break;
            case PRIORIDADE_PREFERENCIAL: prio = "PREFERENCIAL"; break;
            case PRIORIDADE_COMUM:        prio = "COMUM";        break;
            default:                      prio = "DESCONHECIDA"; break;
        }
        printf("    %02d. Senha %04d  |  %-25s  |  %s\n",
               posicao,
               atual->paciente.senha,
               atual->paciente.nome,
               prio);
        atual = atual->proximo;
        posicao++;
    }
}
