#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "estrutura/lista.h"

#define MENU_W 38
#define FILA_W 32
#define MAX_LINHAS_FILA 16
#define INDENT "  "
#define NOME_RAW_MAX 44

typedef struct {
    char texto[FILA_W + 1];
} LinhaFila;

static void limparTela(void) {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

static void pausar(void) {
    printf("\n%s>> Pressione ENTER para voltar ao totem...", INDENT);
    int c;
    while ((c = getchar()) != '\n' && c != EOF) { }
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

static char prefixoPrioridade(int p) {
    switch (p) {
        case PRIORIDADE_EMERGENCIA:   return 'E';
        case PRIORIDADE_PREFERENCIAL: return 'P';
        case PRIORIDADE_COMUM:        return 'C';
        default:                      return '?';
    }
}

static const char* nomePrioridade(int prioridade) {
    switch (prioridade) {
        case PRIORIDADE_EMERGENCIA:   return "EMERGENCIA";
        case PRIORIDADE_PREFERENCIAL: return "PREFERENCIAL";
        case PRIORIDADE_COMUM:        return "COMUM";
        default:                      return "DESCONHECIDA";
    }
}

static int filaVazia(Fila *fila) {
    return fila->inicio == NULL;
}

static int tamanhoFila(Fila *fila) {
    int n = 0;
    for (No *atual = fila->inicio; atual != NULL; atual = atual->prox) {
        n++;
    }
    return n;
}

static void liberarFila(Fila *fila) {
    while (fila->inicio != NULL) {
        No *n = fila->inicio;
        fila->inicio = n->prox;
        free(n);
    }
    fila->fim = NULL;
}

static void listarFila(Fila *fila) {
    No *atual = fila->inicio;
    int posicao = 1;
    while (atual != NULL) {
        printf("    %02d. %-35s  |  %s\n",
               posicao,
               atual->nome,
               nomePrioridade(atual->prioridade));
        atual = atual->prox;
        posicao++;
    }
}

static void bordaCompleta(char ch) {
    int total = MENU_W + FILA_W + 5;
    printf("%s+", INDENT);
    for (int i = 0; i < total; i++) putchar(ch);
    printf("+\n");
}

static void bordaDividida(char ch) {
    printf("%s+", INDENT);
    for (int i = 0; i < MENU_W + 2; i++) putchar(ch);
    putchar('+');
    for (int i = 0; i < FILA_W + 2; i++) putchar(ch);
    printf("+\n");
}

static void linhaCentralizada(const char *texto) {
    int total = MENU_W + FILA_W + 5;
    int len = (int)strlen(texto);
    if (len > total) len = total;
    int leftPad = (total - len) / 2;
    int rightPad = total - len - leftPad;
    printf("%s|%*s%.*s%*s|\n", INDENT, leftPad, "", len, texto, rightPad, "");
}

static void linhaDupla(const char *esq, const char *dir) {
    printf("%s| %-*.*s | %-*.*s |\n",
           INDENT,
           MENU_W, MENU_W, esq,
           FILA_W, FILA_W, dir);
}

static void cabecalho(void) {
    bordaCompleta('=');
    linhaCentralizada("");
    linhaCentralizada("[ + ]  M E D I C O R E  [ + ]");
    linhaCentralizada("Centro Clinico");
    linhaCentralizada("");
    bordaCompleta('=');
}

static int montarLinhasFila(Fila *fila, LinhaFila *linhas, int max) {
    if (filaVazia(fila)) {
        snprintf(linhas[0].texto, FILA_W + 1, "Nenhuma senha cadastrada");
        return 1;
    }

    int count = 0;
    int prevPrior = -1;
    No *atual = fila->inicio;

    while (atual != NULL && count < max - 1) {
        int p = atual->prioridade;
        if (p != prevPrior) {
            snprintf(linhas[count++].texto, FILA_W + 1,
                     "-- %s --", nomePrioridade(p));
            prevPrior = p;
            if (count >= max - 1) break;
        }
        snprintf(linhas[count++].texto, FILA_W + 1,
                 " %.*s",
                 FILA_W - 2,
                 atual->nome);
        atual = atual->prox;
    }

    if (atual != NULL) {
        int remaining = 0;
        for (No *n = atual; n != NULL; n = n->prox) remaining++;
        snprintf(linhas[count++].texto, FILA_W + 1,
                 "... +%d aguardando", remaining);
    }

    return count;
}

static void menuPrincipal(Fila *fila) {
    const char *menuLinhas[] = {
        "[1] Retirar senha de atendimento",
        "[2] Chamar proximo paciente",
        "[3] Visualizar paciente da frente",
        "[4] Visualizar fila completa",
        "[5] Total de pacientes aguardando",
        "[6] Verificar se a fila esta vazia",
        "",
        "[0] Encerrar sessao do totem",
    };
    int menuCount = (int)(sizeof(menuLinhas) / sizeof(menuLinhas[0]));

    LinhaFila linhasFila[MAX_LINHAS_FILA];
    int filaCount = montarLinhasFila(fila, linhasFila, MAX_LINHAS_FILA);

    bordaCompleta('=');
    linhaCentralizada("");
    linhaCentralizada("[ + ]  M E D I C O R E  [ + ]");
    linhaCentralizada("Centro Clinico");
    linhaCentralizada("");
    bordaDividida('=');
    linhaDupla("MENU DE ATENDIMENTO", "SENHAS NA FILA");
    bordaDividida('-');

    int rows = (menuCount > filaCount) ? menuCount : filaCount;
    for (int i = 0; i < rows; i++) {
        const char *esq = (i < menuCount) ? menuLinhas[i] : "";
        const char *dir = (i < filaCount) ? linhasFila[i].texto : "";
        linhaDupla(esq, dir);
    }

    bordaDividida('-');
    printf("\n%s>> Sua escolha: ", INDENT);
}

static void menuPrioridade(void) {
    cabecalho();
    printf("%s\n", INDENT);
    printf("%s        CLASSIFICACAO DE PRIORIDADE\n", INDENT);
    printf("%s\n", INDENT);
    printf("%s   [ 1 ]  EMERGENCIA     (atendimento imediato)\n", INDENT);
    printf("%s   [ 2 ]  PREFERENCIAL   (idoso, gestante, PCD, crianca)\n", INDENT);
    printf("%s   [ 3 ]  COMUM          (consultas em geral)\n", INDENT);
    printf("%s\n", INDENT);
    printf("%s   [ 0 ]  Cancelar e voltar\n", INDENT);
    printf("\n%s>> Selecione a prioridade: ", INDENT);
}

static void retirarSenha(Fila *fila, int *contadorSenha) {
    int opcao = -1;
    int prioridade;
    char nomeRaw[NOME_RAW_MAX];
    char nomeCompleto[50];

    limparTela();
    menuPrioridade();
    if (scanf("%d", &opcao) != 1) {
        while (getchar() != '\n') { }
        printf("\n%s[!] Entrada invalida.\n", INDENT);
        pausar();
        return;
    }
    while (getchar() != '\n') { }

    switch (opcao) {
        case 1: prioridade = PRIORIDADE_EMERGENCIA;   break;
        case 2: prioridade = PRIORIDADE_PREFERENCIAL; break;
        case 3: prioridade = PRIORIDADE_COMUM;        break;
        case 0:
            printf("\n%sOperacao cancelada.\n", INDENT);
            pausar();
            return;
        default:
            printf("\n%s[!] Prioridade invalida.\n", INDENT);
            pausar();
            return;
    }

    limparTela();
    cabecalho();
    printf("\n%sInforme o nome do paciente: ", INDENT);
    lerLinha(nomeRaw, sizeof(nomeRaw));
    if (nomeRaw[0] == '\0') {
        strcpy(nomeRaw, "Paciente s/ ident.");
    }

    int senha = ++(*contadorSenha);
    snprintf(nomeCompleto, sizeof(nomeCompleto),
             "%c%04d %s",
             prefixoPrioridade(prioridade),
             senha,
             nomeRaw);

    printf("\n");
    enqueue(fila, nomeCompleto, prioridade);

    printf("\n%s+--------------------------------------------------+\n", INDENT);
    printf("%s|           SENHA GERADA COM SUCESSO               |\n", INDENT);
    printf("%s+--------------------------------------------------+\n", INDENT);
    printf("%s   Senha       : %c%04d\n", INDENT, prefixoPrioridade(prioridade), senha);
    printf("%s   Paciente    : %s\n", INDENT, nomeRaw);
    printf("%s   Prioridade  : %s\n", INDENT, nomePrioridade(prioridade));
    printf("%s+--------------------------------------------------+\n", INDENT);
    pausar();
}

static void chamarProximo(Fila *fila) {
    limparTela();
    cabecalho();

    if (filaVazia(fila)) {
        printf("\n%s[i] Nao ha pacientes aguardando no momento.\n", INDENT);
        pausar();
        return;
    }

    char nomeSalvo[50];
    int prioridadeSalva;
    strcpy(nomeSalvo, fila->inicio->nome);
    prioridadeSalva = fila->inicio->prioridade;

    printf("\n");
    dequeue(fila);

    printf("\n%s+--------------------------------------------------+\n", INDENT);
    printf("%s|           CHAMANDO PROXIMO PACIENTE              |\n", INDENT);
    printf("%s+--------------------------------------------------+\n", INDENT);
    printf("%s   Identificacao : %s\n", INDENT, nomeSalvo);
    printf("%s   Prioridade    : %s\n", INDENT, nomePrioridade(prioridadeSalva));
    printf("%s+--------------------------------------------------+\n", INDENT);
    printf("\n%s>> Dirija-se ao consultorio indicado.\n", INDENT);
    pausar();
}

static void verFrente(Fila *fila) {
    limparTela();
    cabecalho();

    if (filaVazia(fila)) {
        printf("\n%s[i] A fila esta vazia.\n", INDENT);
        pausar();
        return;
    }

    printf("\n%s+--------------------------------------------------+\n", INDENT);
    printf("%s|             PROXIMO A SER CHAMADO                |\n", INDENT);
    printf("%s+--------------------------------------------------+\n", INDENT);
    printf("%s   Identificacao : %s\n", INDENT, fila->inicio->nome);
    printf("%s   Prioridade    : %s\n", INDENT, nomePrioridade(fila->inicio->prioridade));
    printf("%s+--------------------------------------------------+\n", INDENT);
    pausar();
}

static void verFilaCompleta(Fila *fila) {
    limparTela();
    cabecalho();
    printf("\n%s------------- FILA DE ATENDIMENTO -------------\n\n", INDENT);
    if (filaVazia(fila)) {
        printf("%s[i] Nenhum paciente na fila.\n", INDENT);
    } else {
        listarFila(fila);
    }
    pausar();
}

static void verTamanho(Fila *fila) {
    limparTela();
    cabecalho();
    printf("\n%sPacientes aguardando atendimento: %d\n", INDENT, tamanhoFila(fila));
    pausar();
}

static void verificarVazia(Fila *fila) {
    limparTela();
    cabecalho();
    if (filaVazia(fila)) {
        printf("\n%s[i] A fila esta VAZIA. Nenhum paciente aguardando.\n", INDENT);
    } else {
        printf("\n%s[i] A fila NAO esta vazia. Ha %d paciente(s) aguardando.\n",
               INDENT, tamanhoFila(fila));
    }
    pausar();
}

int main(void) {
    Fila fila;
    int contadorSenha = 0;
    int opcao = -1;

    iniciarFila(&fila);

    do {
        limparTela();
        menuPrincipal(&fila);

        if (scanf("%d", &opcao) != 1) {
            while (getchar() != '\n') { }
            printf("\n%s[!] Opcao invalida. Digite um numero do menu.\n", INDENT);
            pausar();
            continue;
        }
        while (getchar() != '\n') { }

        switch (opcao) {
            case 1: retirarSenha(&fila, &contadorSenha); break;
            case 2: chamarProximo(&fila);                break;
            case 3: verFrente(&fila);                    break;
            case 4: verFilaCompleta(&fila);              break;
            case 5: verTamanho(&fila);                   break;
            case 6: verificarVazia(&fila);               break;
            case 0:
                limparTela();
                cabecalho();
                printf("\n%sObrigado por utilizar o totem do MediCore Centro Clinico.\n", INDENT);
                printf("%sTenha um excelente atendimento!\n\n", INDENT);
                break;
            default:
                printf("\n%s[!] Opcao inexistente. Tente novamente.\n", INDENT);
                pausar();
                break;
        }
    } while (opcao != 0);

    liberarFila(&fila);
    return 0;
}
