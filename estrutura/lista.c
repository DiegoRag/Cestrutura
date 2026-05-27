#include <stdio.h>
#include <stdlib.h>
#include <string.h>



/* Definindo a estrutura do Nó base */
typedef struct No{

    char nome[50]; /* Vetor de caracteres para nome */

    int prioridade; /* 1: Alta, 2: Média, 3: Baixa */

    struct No* prox; /* Armazena o endereço do próximo nó */
    struct No* ant; /* Armazena o endereço do nó anterior*/

}No; /* Nomeia a estrutura base como "No" */



/* Definindo a estrutura da Fila */
typedef struct {

    No* inicio; /* Ponteiro que aponta para primeiro elemento */
    No* fim; /* Ponteiro que aponta para o último elemento */

}Fila; /* Nomeia a estrutura como Fila */



/* Iniciar uma lista vazia */
void iniciarFila(Fila* f){
    /* Garante que não haja lixo de memória */
    f -> inicio = NULL;
    f -> fim = NULL;
}



/* Insere um novo elemento no final da fila */
void enqueue(Fila* f, const char* info, int prioridade){

    No* novo = (No*)malloc(sizeof(No)); /* Aloca dinamicamente um bloco de memória do tamanho da estrutura */

    if(novo == NULL) return; /* Prevenção contra estouro de memória */

    strcpy(novo -> nome, info); /* Armazena a informação "info" na variável nome do novo nó */

    novo -> prioridade = prioridade; /* Atribui o índice de prioridade */

    novo -> prox = NULL; /* "prox", do novo nó, aponta para nenhum endereço, já que não há ningúem depois do último da fila */


    /* Se lista vazia */
    if (f -> inicio == NULL) {

        f -> inicio = novo; /* Se a fila tiver vazia o novo nó também será o início da fila */

        f -> fim = novo; /* Valor do fim da fila se torna o novo nó */

        return;
    }



    /* Percorre a lista a partir do início da lista */
    No* atual = f -> inicio;
    /* Enquanto houver elemento na fila e a prioridade for menor que o elemento atual */
    while (atual != NULL && atual -> prioridade <= prioridade) {
        atual = atual -> prox; /* Anda para o próximo elemento */
    }



    /* Inserir no final - Se percorrer tudo e o atual ficou NULL */
    if (atual == NULL) {

        novo -> ant = f -> fim; /* O ponteiro "anterior" do novo nó recebe o endereço do atual último elemento da fila */

        f -> fim -> prox = novo; /* O ponteiro "próximo" do antigo último elemento passa a apontar para o novo nó */

        f -> fim = novo; /* O ponteiro principal que marca o fim da estrutura da fila é atualizado para ser o novo nó */
    }



    /* Inserir no início - O primeiro da fila já tem prioridade menor do que o novo */
    else if (atual == f -> inicio) {

        novo -> prox = f -> inicio; /* O ponteiro "próximo" do novo nó aponta para quem era o primeiro da fila */

        f -> inicio -> ant = novo; /* O ponteiro "anterior" do antigo primeiro da fila passa a apontar para o novo nó */

        novo -> ant = NULL; /* Garante que o novo primeiro não tem ninguém atrás dele */

        f -> inicio = novo; /* O ponteiro principal que marca o início da estrutura da fila é atualizado para ser o novo nó */
    }



    /* Inserir no meio - O novo nó vai entrar exatamente antes do nó "atual" */
    else {

        novo -> prox = atual; /* O novo nó se conecta ao nó da frente, "atual", através de "prox" */

        novo -> ant = atual -> ant; /* O novo nó se conecta ao nó de trás, que era quem vinha antes do "atual" */

        atual -> ant -> prox = novo; /* O nó de trás se conecta ao novo nó também, reconhecendo ele como seu "prox" */

        atual -> ant = novo; /* O nó da frente, "atual", se conecta ao novo nó, reconhecendo ele como seu "ant" */
    }
}



/* Remove o primeiro elemento da fila */
void dequeue(Fila* f){

    /* Se a lista estiver vazia não existe elementos para serem removidos */
    if(f -> inicio == NULL){
        return;
    }


    No* removido = f -> inicio; /* Armazena endereço do primeiro elemento da fila em "removido", auxiliar */

    f -> inicio = removido -> prox; /* Inicio da fila se torna o segundo elemento, novo primeiro elemento */


    if(f -> inicio != NULL){
        f -> inicio -> ant =  NULL; /* Novo primeiro elemento não deve ter ninguém a sua frente */
    } else{
        f -> fim = NULL; /* Se não houver mais elementos na fila, o fim deve apontar para ninguém */
    }

    free(removido); /* Limpa memória - Remove elemento da fila */

}
