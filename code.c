#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define PULA_CABECALHO(arq, buf) fgets(buf, sizeof(buf), arq)

int sequence_id = 0;

struct cadastro
{
    int id;
    char nome[100];
    int idade;
    float saldo;
};

int get_last_id() {
    FILE *arquivo = fopen("clientes.csv", "r");
    if (arquivo == NULL) return 0;

    char linha[256];
    int ultimo_id = 0;

    PULA_CABECALHO(arquivo, linha);
    while (fgets(linha, sizeof(linha), arquivo)) {
        sscanf(linha, "%d", &ultimo_id);
    }
    fclose(arquivo);
    return ultimo_id;
}

struct cadastro *find_user_id(int user_id) {
    FILE *arquivo = fopen("clientes.csv", "r");

    if (arquivo == NULL) return NULL;

    char linha[256];

    int id = 0;
    char nome[100]; 
    int idade; 
    float saldo;

    PULA_CABECALHO(arquivo, linha);
    while (fgets(linha, sizeof(linha), arquivo)) {
        sscanf(linha, "%d,%99[^,],%d,%f", &id, nome, &idade, &saldo);
        if (id == user_id) break;
    }
    fclose(arquivo);

    if (id != 0 && id == user_id){
        struct cadastro *usuarios;
        usuarios = malloc(sizeof(struct cadastro));
        if (usuarios == NULL) {
            printf("Erro de memoria\n");
            return NULL;
        }
        usuarios->id = id;
        strcpy(usuarios->nome,nome);
        usuarios->idade = idade;
        usuarios->saldo = saldo;
        return usuarios;
    }

    return NULL;
}

void change_saldo_user(int user_id, float new_saldo) {
    FILE *arquivo = fopen("clientes.csv", "r");
    FILE *temp = fopen("temp.csv", "w");

    if (arquivo == NULL || temp == NULL) return;

    char linha[256];

    int id, idade;
    char nome[100];
    float saldo;

    PULA_CABECALHO(arquivo, linha);
    fputs(linha, temp);
    while (fgets(linha, sizeof(linha), arquivo)) {
        sscanf(linha, "%d,%99[^,],%d,%f", &id, nome, &idade, &saldo);
        if (id == user_id) saldo = new_saldo;
        fprintf(temp, "%d,%s,%d,%.2f\n", id, nome, idade, saldo);
    }

    fclose(arquivo);
    fclose(temp);

    remove("clientes.csv");
    rename("temp.csv", "clientes.csv");
}

int generate_id() {
    sequence_id = get_last_id();
    sequence_id++;
    return sequence_id;
}

void insert_user(int qtd) {
    struct cadastro *usuarios;
    usuarios = malloc(qtd * sizeof(struct cadastro));
    if (usuarios == NULL) {
        printf("Erro de memoria\n.");
        return;
    }

    int id = generate_id();
    for (int i = 0; i < qtd; i++) {
        usuarios[i].id = id;

        scanf(" %99[^,], %d, %f",
            usuarios[i].nome,
            &usuarios[i].idade,
            &usuarios[i].saldo);
        
        if (usuarios[i].idade <= 0 || usuarios[i].saldo < 0) {
            printf("Erro: informações inseridas invalidas, cancelando operação\n");
            free(usuarios);
            return;
        }
        
        id++;
    }

    FILE *arquivo = fopen("clientes.csv", "a");
    if (arquivo == NULL) {
        printf("Erro ao abrir arquivo\n.");
        free(usuarios);
        return;
    }

    for (int i = 0; i < qtd; i++) {
        fprintf(
            arquivo,
            "%d,%s,%d,%.2f\n",
            usuarios[i].id,
            usuarios[i].nome,
            usuarios[i].idade,
            usuarios[i].saldo
        );
        
    }
    if (qtd > 1) {
        printf("Usuários inseridos com id %d", usuarios[0].id);
        for (int i = 1; i < qtd; i++) {
            if (i == qtd - 1)printf(" e %d", usuarios[i].id);
            else printf(" ,%d", usuarios[i].id);
        }
        printf("\n");
    }
    else {printf("Usuário inserido com id %d.\n", usuarios[0].id);}


    fclose(arquivo);
    free(usuarios);
}

void info_user(int id) {
    struct cadastro *usuarios = find_user_id(id);
    if (usuarios == NULL) {printf("Erro: Usuário %d não encontrado.\n", id);}
    else {
        printf("Usuário %d tem saldo de R$%.2f.\n", usuarios->id, usuarios->saldo);
        free(usuarios);
    }
}

void transferencia(int id_origem, int id_destino, float valor) {
    struct cadastro *user_origem = find_user_id(id_origem);
    struct cadastro *user_destino = find_user_id(id_destino);

    if (user_origem == NULL) { printf("Erro: Pagador não encontrado.\n"); }
    else if (user_destino == NULL) { printf("Erro: Recebedor não encontrado.\n"); }
    else if (user_origem->saldo < valor) { printf("Erro: Saldo insuficiente.\n"); }
    else {
        change_saldo_user(id_origem, user_origem->saldo - valor);
        change_saldo_user(id_destino, user_destino->saldo + valor);
        printf("Transação realizada com sucesso.\n");
    }

    free(user_origem);
    free(user_destino);
}

void remove_user(int user_id){
    FILE *arquivo = fopen("clientes.csv", "r");
    FILE *temp = fopen("temp.csv", "w");
    if (arquivo == NULL || temp == NULL) return;

    char linha[256];
    int id, idade, encontrado = 0;
    char nome[100];
    float saldo;

    PULA_CABECALHO(arquivo, linha);
    fputs(linha, temp);
    while (fgets(linha, sizeof(linha), arquivo)) {
        sscanf(linha, "%d,%99[^,],%d,%f", &id, nome, &idade, &saldo);
        if (id == user_id) {encontrado = 1; continue;}
        fprintf(temp, "%d,%s,%d,%.2f\n", id, nome, idade, saldo);
    }

    fclose(arquivo);
    fclose(temp);
    remove("clientes.csv");
    rename("temp.csv", "clientes.csv");
    if (encontrado) {printf("Usuário %d removido com sucesso.\n", user_id);}
    else {printf("Erro: Usuário não encontrado.\n");}
}

int main() {
    int exec = 1;
        printf("═══════════════════════════\n");
        printf("       MENU UAIBANK        \n");
        printf("1 → Novo usuário          \n");
        printf("2 → Vários usuários       \n");
        printf("3 → Consultar usuário     \n");
        printf("4 → Transferência         \n");
        printf("5 → Remover usuário       \n");
        printf("6 → Sair                  \n");
        printf("═══════════════════════════\n");
    while (exec != 6){
        scanf("%d", &exec);
        int id, qtd, id_origem, id_destino;
        float valor;
        switch (exec)
        {
            case 1:
                insert_user(1);
                break;
            case 2:
                scanf("%d", &qtd);
                if (qtd <= 0) {
                    printf("Erro: Insira um número valido de usuários\n");
                    break;
                }
                insert_user(qtd);
                break;
            case 3:
                scanf("%d", &id);
                info_user(id);
                break;
            case 4:
                scanf("%d", &id_origem);
                scanf("%d", &id_destino);
                scanf("%f", &valor);
                if (valor <= 0) {
                    printf("Erro: Valor de transferência negativo ou zerado\n");
                    break;
                }
                transferencia(id_origem, id_destino, valor);
                break;
            case 5:
                scanf("%d", &id);
                remove_user(id);
                break;
            case 6:
                return 0;
            default:
                break;
        }
    }
    FILE *arquivo = fopen("clientes.csv", "r");
    if (arquivo == NULL) {
        printf("Erro ao abrir o arquivo!\n");
        return 1;
    }
    fclose(arquivo);
    return 0;
}