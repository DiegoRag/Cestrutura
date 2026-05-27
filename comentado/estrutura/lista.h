/*
 * ============================================================
 *  lista.h  -  CONTRATO da fila com prioridade
 * ============================================================
 *
 *  Um arquivo .h ("header") guarda apenas DECLARACOES:
 *  os tipos (structs) e os "prototipos" das funcoes.
 *  A implementacao de verdade fica no .c.
 *
 *  Quem precisar usar a fila inclui este arquivo com
 *  #include "lista.h" e ja conhece os tipos e funcoes.
 */

/* "Include guard": impede que este header seja incluido    */
/* duas vezes no mesmo arquivo (causaria erro de redeclaracao). */
#ifndef LISTA_H   /* Se a macro LISTA_H ainda nao existe... */
#define LISTA_H   /* ...define ela e continua processando.   */

/* -------- Constantes simbolicas das prioridades -------- */
/* Sao numeros inteiros, mas usamos NOMES para o codigo    */
/* ficar legivel. Numero menor = prioridade maior.         */
#define PRIORIDADE_EMERGENCIA   1   /* Atendimento imediato */
#define PRIORIDADE_PREFERENCIAL 2   /* Idoso, gestante etc. */
#define PRIORIDADE_COMUM        3   /* Consultas em geral   */

/* -------- Struct que representa um Paciente ---------- */
/* Cada paciente tem 3 dados: numero da senha, nome e a   */
/* prioridade (1, 2 ou 3).                                */
typedef struct Paciente {
    int senha;          /* Numero unico gerado pelo totem  */
    char nome[80];      /* Nome (ate 79 caracteres + '\0') */
    int prioridade;     /* Uma das constantes acima         */
} Paciente;             /* "typedef" permite escrever        */
                        /* "Paciente x;" em vez de           */
                        /* "struct Paciente x;"              */

/* -------- Struct No (no = elemento da lista) -------- */
/* Cada No carrega 1 Paciente + ponteiros para o no    */
/* anterior e o proximo (lista DUPLAMENTE ligada).     */
typedef struct No {
    Paciente paciente;      /* Dado armazenado no no       */
    struct No *anterior;    /* Aponta para o no de tras    */
    struct No *proximo;     /* Aponta para o no da frente  */
} No;

/* -------- Struct Fila (a lista em si) -------- */
/* Guardamos so as pontas e o tamanho, assim     */
/* nao precisamos contar a fila toda toda hora.  */
typedef struct Fila {
    No *inicio;     /* Primeiro no (frente da fila)        */
    No *fim;        /* Ultimo no (final da fila)           */
    int tamanho;    /* Quantos pacientes estao aguardando  */
} Fila;

/* -------- PROTOTIPOS DAS FUNCOES -------- */
/* So a "assinatura" (nome + parametros + retorno).      */
/* O corpo de cada uma esta em lista.c.                  */

void inicializarFila(Fila *fila);        /* Zera a fila              */
int  filaVazia(Fila *fila);              /* 1 se vazia, 0 caso contr.*/
int  tamanhoFila(Fila *fila);            /* Quantos pacientes ha     */
void enfileirar(Fila *fila, Paciente p); /* Adiciona respeitando pri.*/
int  desenfileirar(Fila *fila, Paciente *removido); /* Remove frente */
int  frenteFila(Fila *fila, Paciente *atual);       /* Espia frente  */
void liberarFila(Fila *fila);            /* Libera memoria de tudo   */
void listarFila(Fila *fila);             /* Imprime todos os pacient.*/

#endif  /* Fecha o include guard aberto la em cima */
