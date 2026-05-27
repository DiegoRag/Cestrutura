#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "estrutura/lista.h"

static void limparTela(void) {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

static void pausar(void) {
    printf("\n   Pressione ENTER para voltar ao totem...");
    int c;
    while ((c = getchar()) != '\n' && c != EOF) { }
    getchar();
}

static void lerLinha(char *destino, int tamanho) {
    if (fgets(destino, tamanho, stdin) != NULL) {
        size_t len = strlen(destino);
        if (len > 0 && destino[len - 1] == '\n') {
            destino[len - 1] = '\0';
        }
    } else {
        destino[0] = '\0';
    }
}

static void cabecalho(void) {
    printf("\n");
    printf("   +============================================================+\n");
    printf("   |                                                            |\n");
    printf("   |              [ + ]  M E D I C O R E  [ + ]                 |\n");
    printf("   |                                                            |\n");
    printf("   |                  Centro Clinico                            |\n");
    printf("   |                                                            |\n");
    printf("   +============================================================+\n");
    printf("   |   Bem-vindo(a) ao totem de atendimento                     |\n");
    printf("   |   Toque em uma das opcoes abaixo para continuar            |\n");
    printf("   +============================================================+\n");
}

static void menuPrincipal(void) {
    cabecalho();
    printf("   |                                                            |\n");
    printf("   |   [ 1 ]  Retirar senha de atendimento                      |\n");
    printf("   |   [ 2 ]  Chamar proximo paciente                           |\n");
    printf("   |   [ 3 ]  Visualizar paciente na frente da fila             |\n");
    printf("   |   [ 4 ]  Visualizar fila completa                          |\n");
    printf("   |   [ 5 ]  Total de pacientes aguardando                     |\n");
    printf("   |   [ 6 ]  Verificar se a fila esta vazia                    |\n");
    printf("   |                                                            |\n");
    printf("   |   [ 0 ]  Encerrar sessao do totem                          |\n");
    printf("   |                                                            |\n");
    printf("   +============================================================+\n");
    printf("\n   >> Sua escolha: ");
}

static void menuPrioridade(void) {
    cabecalho();
    printf("   |                                                            |\n");
    printf("   |              CLASSIFICACAO DE PRIORIDADE                   |\n");
    printf("   |                                                            |\n");
    printf("   |   [ 1 ]  EMERGENCIA     (atendimento imediato)             |\n");
    printf("   |   [ 2 ]  PREFERENCIAL   (idoso, gestante, PCD, crianca)    |\n");
    printf("   |   [ 3 ]  COMUM          (consultas em geral)               |\n");
    printf("   |                                                            |\n");
    printf("   |   [ 0 ]  Cancelar e voltar                                 |\n");
    printf("   |                                                            |\n");
    printf("   +============================================================+\n");
    printf("\n   >> Selecione a prioridade: ");
}

static const char* nomePrioridade(int prioridade) {
    switch (prioridade) {
        case PRIORIDADE_EMERGENCIA:   return "EMERGENCIA";
        case PRIORIDADE_PREFERENCIAL: return "PREFERENCIAL";
        case PRIORIDADE_COMUM:        return "COMUM";
        default:                      return "DESCONHECIDA";
    }
}

static void retirarSenha(Fila *fila, int *contadorSenha) {
    int opcao = -1;
    Paciente novo;

    limparTela();
    menuPrioridade();
    if (scanf("%d", &opcao) != 1) {
        while (getchar() != '\n') { }
        printf("\n   [!] Entrada invalida.\n");
        pausar();
        return;
    }
    while (getchar() != '\n') { }

    switch (opcao) {
        case 1:
            novo.prioridade = PRIORIDADE_EMERGENCIA;
            break;
        case 2:
            novo.prioridade = PRIORIDADE_PREFERENCIAL;
            break;
        case 3:
            novo.prioridade = PRIORIDADE_COMUM;
            break;
        case 0:
            printf("\n   Operacao cancelada.\n");
            pausar();
            return;
        default:
            printf("\n   [!] Prioridade invalida.\n");
            pausar();
            return;
    }

    limparTela();
    cabecalho();
    printf("\n   Informe o nome do paciente: ");
    lerLinha(novo.nome, sizeof(novo.nome));
    if (novo.nome[0] == '\0') {
        strcpy(novo.nome, "Paciente sem identificacao");
    }

    novo.senha = ++(*contadorSenha);
    enfileirar(fila, novo);

    printf("\n   +--------------------------------------------------------+\n");
    printf("   |               SENHA GERADA COM SUCESSO                 |\n");
    printf("   +--------------------------------------------------------+\n");
    printf("   |  Senha       : %04d                                    \n", novo.senha);
    printf("   |  Paciente    : %s\n", novo.nome);
    printf("   |  Prioridade  : %s\n", nomePrioridade(novo.prioridade));
    printf("   +--------------------------------------------------------+\n");
    pausar();
}

static void chamarProximo(Fila *fila) {
    Paciente atendido;
    limparTela();
    cabecalho();

    if (desenfileirar(fila, &atendido)) {
        printf("\n   +--------------------------------------------------------+\n");
        printf("   |                CHAMANDO PROXIMO PACIENTE               |\n");
        printf("   +--------------------------------------------------------+\n");
        printf("   |  Senha       : %04d                                    \n", atendido.senha);
        printf("   |  Paciente    : %s\n", atendido.nome);
        printf("   |  Prioridade  : %s\n", nomePrioridade(atendido.prioridade));
        printf("   +--------------------------------------------------------+\n");
        printf("\n   >> Dirija-se ao consultorio indicado.\n");
    } else {
        printf("\n   [i] Nao ha pacientes aguardando no momento.\n");
    }
    pausar();
}

static void verFrente(Fila *fila) {
    Paciente atual;
    limparTela();
    cabecalho();

    if (frenteFila(fila, &atual)) {
        printf("\n   +--------------------------------------------------------+\n");
        printf("   |                PROXIMO A SER CHAMADO                   |\n");
        printf("   +--------------------------------------------------------+\n");
        printf("   |  Senha       : %04d                                    \n", atual.senha);
        printf("   |  Paciente    : %s\n", atual.nome);
        printf("   |  Prioridade  : %s\n", nomePrioridade(atual.prioridade));
        printf("   +--------------------------------------------------------+\n");
    } else {
        printf("\n   [i] A fila esta vazia.\n");
    }
    pausar();
}

static void verFilaCompleta(Fila *fila) {
    limparTela();
    cabecalho();
    printf("\n   ----------------- FILA DE ATENDIMENTO -----------------\n\n");
    if (filaVazia(fila)) {
        printf("   [i] Nenhum paciente na fila.\n");
    } else {
        listarFila(fila);
    }
    pausar();
}

static void verTamanho(Fila *fila) {
    limparTela();
    cabecalho();
    printf("\n   Pacientes aguardando atendimento: %d\n", tamanhoFila(fila));
    pausar();
}

static void verificarVazia(Fila *fila) {
    limparTela();
    cabecalho();
    if (filaVazia(fila)) {
        printf("\n   [i] A fila esta VAZIA. Nenhum paciente aguardando.\n");
    } else {
        printf("\n   [i] A fila NAO esta vazia. Ha %d paciente(s) aguardando.\n", tamanhoFila(fila));
    }
    pausar();
}

int main(void) {
    Fila fila;
    int contadorSenha = 0;
    int opcao = -1;

    inicializarFila(&fila);

    do {
        limparTela();
        menuPrincipal();

        if (scanf("%d", &opcao) != 1) {
            while (getchar() != '\n') { }
            printf("\n   [!] Opcao invalida. Digite um numero do menu.\n");
            pausar();
            continue;
        }
        while (getchar() != '\n') { }

        switch (opcao) {
            case 1:
                retirarSenha(&fila, &contadorSenha);
                break;
            case 2:
                chamarProximo(&fila);
                break;
            case 3:
                verFrente(&fila);
                break;
            case 4:
                verFilaCompleta(&fila);
                break;
            case 5:
                verTamanho(&fila);
                break;
            case 6:
                verificarVazia(&fila);
                break;
            case 0:
                limparTela();
                cabecalho();
                printf("\n   Obrigado por utilizar o totem do MediCore Centro Clinico.\n");
                printf("   Tenha um excelente atendimento!\n\n");
                break;
            default:
                printf("\n   [!] Opcao inexistente. Tente novamente.\n");
                pausar();
                break;
        }
    } while (opcao != 0);

    liberarFila(&fila);
    return 0;
}
