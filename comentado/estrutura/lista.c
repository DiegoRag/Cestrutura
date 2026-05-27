/*
 * ============================================================
 *  lista.c  -  IMPLEMENTACAO da fila com prioridade
 * ============================================================
 *
 *  Aqui estao os "corpos" das funcoes declaradas em lista.h.
 *  Esse e o arquivo que contem a logica de verdade da fila.
 */

#include <stdio.h>   /* Biblioteca padrao para printf, etc.   */
#include <stdlib.h>  /* Para malloc e free (memoria dinamica) */
#include <string.h>  /* Para funcoes de string (nao usamos    */
                     /* muito aqui, mas e bom ter)             */
#include "lista.h"   /* Importa os tipos Paciente, No, Fila   */
                     /* e os prototipos das funcoes.           */

/* ============================================================
 *  inicializarFila
 *  ------------------------------------------------------------
 *  Deixa a fila em estado "recem-criada": sem nos e tamanho 0.
 *  Sempre chame isso ANTES de usar a fila pela primeira vez,
 *  senao os ponteiros vao ter lixo da memoria e travar tudo.
 * ============================================================ */
void inicializarFila(Fila *fila) {
    fila->inicio = NULL;   /* Sem primeiro no             */
    fila->fim    = NULL;   /* Sem ultimo no               */
    fila->tamanho = 0;     /* Zero pacientes aguardando   */
}

/* ============================================================
 *  filaVazia
 *  ------------------------------------------------------------
 *  Retorna 1 (verdadeiro) se nao ha ninguem na fila,
 *  ou 0 (falso) se existe pelo menos 1 paciente.
 *  Em C, "verdadeiro" e qualquer numero != 0.
 * ============================================================ */
int filaVazia(Fila *fila) {
    /* Se inicio e NULL, nao tem ninguem. A expressao    */
    /* "fila->inicio == NULL" ja vale 0 ou 1.             */
    return fila->inicio == NULL;
}

/* ============================================================
 *  tamanhoFila
 *  ------------------------------------------------------------
 *  So devolve o contador que mantemos atualizado.
 *  Nao precisa percorrer a lista toda (mais eficiente).
 * ============================================================ */
int tamanhoFila(Fila *fila) {
    return fila->tamanho;
}

/* ============================================================
 *  enfileirar
 *  ------------------------------------------------------------
 *  CORACAO da estrutura. Insere o paciente NA POSICAO CORRETA
 *  para que a fila fique sempre ordenada por prioridade:
 *      EMERGENCIA (1)  ANTES de  PREFERENCIAL (2)
 *      PREFERENCIAL (2) ANTES de COMUM (3)
 *  Em caso de empate (mesma prioridade), o novo vai DEPOIS
 *  dos que ja estavam la = FIFO dentro do grupo.
 * ============================================================ */
void enfileirar(Fila *fila, Paciente paciente) {
    /* PASSO 1: alocar memoria para o novo no.            */
    /* malloc reserva um pedaco do heap do tamanho de No. */
    No *novo = (No *) malloc(sizeof(No));

    /* Se malloc falhou (memoria esgotada), aborta.        */
    if (novo == NULL) {
        return;
    }

    /* PASSO 2: preencher o novo no.                        */
    novo->paciente = paciente; /* Copia os dados do paciente */
    novo->anterior = NULL;     /* Ainda nao ligamos em nada  */
    novo->proximo  = NULL;     /* Idem                       */

    /* PASSO 3: CASO ESPECIAL - fila estava vazia.          */
    /* Se nao ha ninguem, o novo vira inicio E fim ao mesmo */
    /* tempo (e o unico no da lista).                       */
    if (fila->inicio == NULL) {
        fila->inicio = novo;
        fila->fim    = novo;
        fila->tamanho++;       /* Atualiza o contador        */
        return;                /* Termina aqui               */
    }

    /* PASSO 4: PROCURAR onde inserir.                      */
    /* Vamos andar do inicio para o fim ate encontrar o      */
    /* primeiro no com prioridade PIOR que a do novo.        */
    /* (PIOR = numero MAIOR)                                 */
    No *atual = fila->inicio;  /* Comeca no primeiro          */

    /* Enquanto NAO chegamos ao fim E o no atual tem         */
    /* prioridade igual ou melhor que a do novo, avancamos.  */
    /* O "<=" e a chave do FIFO: em empate, continuamos      */
    /* andando, entao o novo entra DEPOIS dos iguais.         */
    while (atual != NULL && atual->paciente.prioridade <= paciente.prioridade) {
        atual = atual->proximo;
    }

    /* PASSO 5: INSERIR. Tres casos possiveis:              */

    /* CASO A: atual e NULL = andamos ate o fim.             */
    /* Significa que o novo tem a PIOR prioridade de todos,  */
    /* entao vai para o final da fila.                       */
    if (atual == NULL) {
        novo->anterior     = fila->fim;   /* Liga novo<-fim   */
        fila->fim->proximo = novo;        /* Liga fim->novo   */
        fila->fim          = novo;        /* Atualiza ponteiro*/
    }
    /* CASO B: atual e o inicio = paramos no primeiro no.    */
    /* Significa que o novo tem prioridade MELHOR que todos, */
    /* entao vai para a frente da fila.                      */
    else if (atual == fila->inicio) {
        novo->proximo         = fila->inicio; /* novo->antigo */
        fila->inicio->anterior = novo;        /* antigo->novo */
        fila->inicio          = novo;         /* Novo e a frente */
    }
    /* CASO C: atual esta no meio.                            */
    /* Inserimos o novo ENTRE atual->anterior e atual.        */
    else {
        novo->anterior          = atual->anterior; /* Liga 4  */
        novo->proximo           = atual;           /* ponteiros */
        atual->anterior->proximo = novo;           /* para     */
        atual->anterior          = novo;           /* costurar */
    }

    /* PASSO 6: incrementa o contador.                        */
    fila->tamanho++;
}

/* ============================================================
 *  desenfileirar
 *  ------------------------------------------------------------
 *  Remove e retorna (via ponteiro) o paciente da FRENTE.
 *  Como enfileirar ja ordena, a frente sempre tem o paciente
 *  de maior prioridade que chegou primeiro.
 *  Retorna 1 se conseguiu remover, 0 se a fila estava vazia.
 * ============================================================ */
int desenfileirar(Fila *fila, Paciente *removido) {
    /* Se nao tem ninguem, avisa quem chamou.                 */
    if (fila->inicio == NULL) {
        return 0;
    }

    /* Guarda referencia ao no que sera removido.             */
    No *no = fila->inicio;

    /* Copia o paciente para o ponteiro do chamador,          */
    /* mas SO se ele passou um endereco valido.               */
    if (removido != NULL) {
        *removido = no->paciente;
    }

    /* O segundo no da lista vira o novo inicio.              */
    fila->inicio = no->proximo;

    /* Se ainda sobrou alguem, ajusta o ponteiro "anterior"   */
    /* do novo inicio para NULL (nao tem mais ninguem antes). */
    if (fila->inicio != NULL) {
        fila->inicio->anterior = NULL;
    }
    /* Se a fila ficou vazia, fim tambem vira NULL.           */
    else {
        fila->fim = NULL;
    }

    /* Libera a memoria do no removido (importantissimo!).    */
    /* Esquecer disso causa "memory leak".                    */
    free(no);

    /* Decrementa o contador e avisa que deu certo.           */
    fila->tamanho--;
    return 1;
}

/* ============================================================
 *  frenteFila
 *  ------------------------------------------------------------
 *  Igual ao desenfileirar, mas SEM remover. So "espia" quem
 *  esta na frente. Util para o totem mostrar quem e o proximo
 *  sem efetivamente chamar.
 * ============================================================ */
int frenteFila(Fila *fila, Paciente *atual) {
    if (fila->inicio == NULL) {
        return 0;   /* Vazia, nao tem o que espiar          */
    }
    if (atual != NULL) {
        *atual = fila->inicio->paciente; /* Copia, nao remove */
    }
    return 1;
}

/* ============================================================
 *  liberarFila
 *  ------------------------------------------------------------
 *  Percorre TODOS os nos e libera a memoria de cada um.
 *  Chame antes do programa terminar para nao deixar vazamento.
 * ============================================================ */
void liberarFila(Fila *fila) {
    No *atual = fila->inicio;     /* Comeca pelo primeiro     */

    while (atual != NULL) {       /* Enquanto houver nos      */
        No *prox = atual->proximo; /* Guarda o proximo antes  */
                                   /* de liberar o atual.      */
        free(atual);              /* Libera memoria deste no  */
        atual = prox;             /* Pula para o proximo       */
    }

    /* Deixa a fila no estado inicial novamente.              */
    fila->inicio  = NULL;
    fila->fim     = NULL;
    fila->tamanho = 0;
}

/* ============================================================
 *  listarFila
 *  ------------------------------------------------------------
 *  Imprime na tela cada paciente, na ordem em que serao
 *  atendidos. Usado pelo menu opcao [4].
 * ============================================================ */
void listarFila(Fila *fila) {
    No *atual = fila->inicio;    /* Comeca do inicio          */
    int posicao = 1;             /* Contador 1, 2, 3, ...      */

    while (atual != NULL) {       /* Enquanto houver no        */
        /* Converte o numero da prioridade no texto legivel.   */
        const char *prio;
        switch (atual->paciente.prioridade) {
            case PRIORIDADE_EMERGENCIA:   prio = "EMERGENCIA";   break;
            case PRIORIDADE_PREFERENCIAL: prio = "PREFERENCIAL"; break;
            case PRIORIDADE_COMUM:        prio = "COMUM";        break;
            default:                      prio = "DESCONHECIDA"; break;
        }

        /* Imprime a linha formatada.                          */
        /*   %02d  = inteiro com 2 digitos, completa com zero  */
        /*   %04d  = inteiro com 4 digitos                     */
        /*   %-25s = string, alinhada a esquerda, 25 colunas   */
        printf("    %02d. Senha %04d  |  %-25s  |  %s\n",
               posicao,
               atual->paciente.senha,
               atual->paciente.nome,
               prio);

        atual = atual->proximo;   /* Avanca para o proximo no  */
        posicao++;                /* Incrementa a contagem     */
    }
}
