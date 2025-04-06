/*-----------------------------------------------------------------------------------------------------+
| main.c                 |Funcao main, dá handle às opções e ao input, chama a funcao de               |
|                        | processamento de cada linha de input e dá free a toda a memória antes de    |
|                        | dar exit                                                                    |
|                        |                                                                             |
+------------------------------------------------------------------------------------------------------+
| Autores: José Mateus Aguiar n. 113754         Rogério Pina n. 114719                                 |
|          LEEC-IST                                                                                    |
| Data: 06 Abril 2025                                                                                  |
+-----------------------------------------------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>

#include "ortografia.h" 
#include "processos_diferencas.h" 

#define DICT_PATH "words"


// Função para imprimir a ajuda do programa
void print_help() {
    printf("Ortografia: Verificador ortográfico\n");
    printf("Este programa verifica a ortografia de um texto, sugerindo correções para palavras incorretas.\n\n\n\n");
    printf("Uso: programa [-i arquivo_entrada] [-o arquivo_saida] [-d dicionario] [-m modo] [-a max_sugestoes] [-n max_diferencas]\n");
    printf("  -i arquivo_entrada: caminho do arquivo de entrada (padrão: stdin)\n");
    printf("  -o arquivo_saida: caminho do arquivo de saída (padrão: stdout)\n");
    printf("  -d dicionario: caminho do dicionário (padrão: %s)\n", DICT_PATH);
    printf("  -m modo: modo de funcionamento (1, 2 ou 3, padrão: 1)\n");
    printf("  -a max_sugestoes: número máximo de sugestões (padrão: 10)\n");
    printf("  -n max_diferencas: número máximo de diferenças permitidas (padrão: 2)\n");
}


// Função para verificar se uma string é um número válido
int is_valid_number(const char *str) {
    if (str == NULL || *str == '\0') {
        return 0;  // String vazia ou NULL não é um número válido
    }
    
    // Verifica o primeiro caractere (pode ser sinal ou dígito)
    if (*str == '-' || *str == '+') {
        str++;  // Avança para o próximo caractere após o sinal
        // Se só tiver o sinal, não é um número válido
        if (*str == '\0') {
            return 0;
        }
    }
    
    // Verifica se todos os caracteres restantes são dígitos
    while (*str) {
        if (*str < '0' || *str > '9') {
            return 0;  // Encontrou um caractere que não é dígito
        }
        str++;
    }
    
    return 1;  // É um número válido
}

int main(int argc, char **argv) {
    char **lines = NULL;
    char *line = NULL;  
    size_t len = 0;    
    ssize_t read;       
    
    FILE *input = stdin;
    FILE *output = stdout;
    int linhasAlocadas = 0;
    int count = 0;
    
    char *dict_path = DICT_PATH;
    
    // Variáveis para controlar o modo de sugestão
    int modo = 1;         // Padrão: modo 1 (ou modo 2 = 2)
    int modo2_max = 10;   // Padrão: máximo de 10 sugestões
    int modo2_difs = 2;   // Padrão: diferença máxima permitida é 2
    
    int opt;
    while ((opt = getopt(argc, argv, "i:o:d:m:a:n:h")) != -1) {
        switch (opt) {
            case 'h':
                print_help();
                exit(0);
            case 'i': // Path do arquivo de entrada
                input = fopen(optarg, "r");
                if (!input) {
                    fprintf(stderr, "Erro ao abrir o arquivo de entrada: %s\n", optarg);
                    exit(1);
                }
                break;
            case 'o': // Path do arquivo de saída
                output = fopen(optarg, "w");
                if (!output) {
                    fprintf(stderr, "Erro ao abrir o arquivo de saída: %s\n", optarg);
                    exit(1);
                }
                break;
            case 'd': // Path do dicionário
                dict_path = optarg;
                break;
            case 'm': // Modo de funcionamento
                if (!is_valid_number(optarg)) {
                    fprintf(stderr, "Modo deve ser um número inteiro.\n");
                    exit(1);
                }
                modo = atoi(optarg);
                break;
            case 'a': // Número máximo de sugestões
                if (!is_valid_number(optarg)) {
                    fprintf(stderr, "Número máximo de sugestões deve ser um número inteiro.\n");
                    exit(1);
                }
                modo2_max = atoi(optarg);
                break;
            case 'n': // Número máximo de diferenças
                if (!is_valid_number(optarg)) {
                    fprintf(stderr, "Número máximo de diferenças deve ser um número inteiro.\n");
                    exit(1);
                }
                modo2_difs = atoi(optarg);
                break;
            case ':':
                // Se a opção requer um argumento, mas não foi fornecido
                fprintf(stderr, "Opção '-%c' requer um argumento.\n", optopt);
                exit(1);
            case '?':
            default:
                printf("Uso: %s [-i arquivo_entrada] [-o arquivo_saida] [-d dicionario] [-m modo] [-a max_sugestoes] [-n max_diferencas]\n", argv[0]);
                if (input != stdin) {
                    fclose(input);
                }
                if (output != stdout) {
                    fclose(output);
                }
                exit(1);
        }
    }

    if (modo < 1 || modo > 3) {
        fprintf(stderr, "Modo inválido. Use 1, 2 ou 3.\n");
        exit(1);
    }

    if (modo2_max < 1) {
        fprintf(stderr, "Número máximo de sugestões inválido: %d.\n", modo2_max);
        exit(1);
    }
    if (modo2_difs < 0) {
        fprintf(stderr, "Número máximo de diferenças inválido: %d.\n", modo2_difs);
        exit(1);
    }
    
    int words_len;
    char **words = carrega_dicionario(dict_path, &words_len);
    
    /* Ordena o dicionário usando a função comparacao_helper definida no módulo string_difs */
    qsort(words, words_len, sizeof(char*), comparacao_helper);
    
    /* Leitura das linhas de entrada */
    while ((read = getline(&line, &len, input)) != -1) {
        // apaga new line
        if (read > 0 && line[read - 1] == '\n') {
            line[read - 1] = '\0';
            read--;
        }
        
        if (count >= linhasAlocadas) {
            linhasAlocadas += 10;
            char **temp = realloc(lines, linhasAlocadas * sizeof(char*));
            if (temp == NULL) { 
                fprintf(stderr, "Erro ao realocar memória para lines\n");
                exit(1);
            }
            lines = temp;
        }
        
        lines[count] = malloc(read + 1);
        if (lines[count] == NULL) {
            fprintf(stderr, "Erro ao alocar memória para linha\n");
            exit(1);
        }
        strcpy(lines[count], line);
        count++;
    }
    
    // free no buffer da linha
    free(line);
    
    if (input != stdin) {
        fclose(input);
    }
    
    /* Processa as linhas de entrada com a verificação ortográfica */
    processa_linhas(lines, count, words, words_len, output, modo, modo2_difs, modo2_max);
    
    if (output != stdout) {
        fclose(output);
    }
    
    /* Liberta memória alocada */
    for (int j = 0; j < words_len; j++) {
        free(words[j]);
    }
    free(words);
    
    for (int j = 0; j < count; j++) {
        free(lines[j]);
    }
    free(lines);
    
    exit(0);
}
