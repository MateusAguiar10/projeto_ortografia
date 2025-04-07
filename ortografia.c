/*-----------------------------------------------------------------------------------------------------+
| ortografia.c           | Funções de carregamento do dicionário para um vetor                         |
|                        | Função de processamento de um vetor de linhas com suporte a diferentes modos|
|                        | de funcionamento                                                            |
|                        | Função que retorna a primeira sugestão com base no algoritmo de sugestoes   |
+------------------------------------------------------------------------------------------------------+
| Autores: José Mateus Aguiar n. 113754         Rogério Pina n. 114719                                 |
|          LEEC-IST                                                                                    |
| Data: 06 Abril 2025                                                                                  |
+-----------------------------------------------------------------------------------------------------*/
#include "ortografia.h"
#include "processos_diferencas.h"
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdio.h>

/*Nome: carrega_dicionario
  Input: filename: nome do arquivo de dicionário
         words_len: ponteiro para inteiro que VAI guardar o número de palavras
  Output: ponteiro para um array de strings (cada palavra do dicionário)
  Description: Carrega o dicionário a partir do arquivo fornecido.
               - O número de palavras é retornado através do ponteiro words_len.
                - Aloca memória para o array de palavras e lê cada linha do arquivo,
                armazenando as palavras no array.
                - Se o arquivo não puder ser aberto, imprime uma mensagem de erro e sai do programa.
                - Se a alocação de memória falhar, imprime uma mensagem de erro e sai do programa.
*/
char ** carrega_dicionario(char * filename, int * words_len) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        fprintf(stderr, "Erro ao abrir o arquivo de dicionário: %s\n", filename);
        exit(1);
    }

    int count = 0;
    char *line = NULL;
    size_t len = 0;
    ssize_t read;

    // primeiro loop para contar o número de palavras
    while ((read = getline(&line, &len, file)) != -1) {
        count++;
    }
    // Libertar a linha alocada pelo getline após o primeiro loop
    free(line);
    line = NULL;
    len = 0;
    rewind(file);

    // aloca memória para o array de palavras
    char ** words = malloc(count * sizeof(char*));
    if (words == NULL) { 
        fprintf(stderr, "Erro ao alocar memória para words\n");
        exit(1);
    }
    int i = 0;

    // segundo loop para ler as palavras e armazená-las
    while ((read = getline(&line, &len, file)) != -1) {
        // Remove o '\n' do final se presente
        if (read > 0 && line[read - 1] == '\n') {
            line[read - 1] = '\0';
            read--;
        }
        
        words[i] = malloc(read + 1);
        if (words[i] == NULL) {
            fprintf(stderr, "Erro ao alocar memória para palavra\n");
            exit(1);
        }
        strcpy(words[i], line);
        i++;
    }
    
    // Liberar a linha alocada pelo getline após o segundo loop
    free(line);
    fclose(file);

    *words_len = count; 
    return words;
}

/*Nome: processa_linhas
  Input: lines: array de strings (linhas a processar)
         nr_linhas: número de linhas a processar (tamanho de lines)
         dictionary: array de strings (dicionário)
         dict_size: número de palavras no dicionário
         output: ponteiro para o arquivo de saída (para a flag -o)
         mode: modo de operação (1, 2 ou 3)
         max_diff: número máximo de diferenças permitidas (para a flag -n)
         max_suggestions: número máximo de sugestões a imprimir (para a flag -a)

  Output: Não retorna nada, mas imprime os erros e sugestões no arquivo de saída. 
  Description: 
               - Processa cada linha do array lines, verificando cada palavra dessa linha contra o dicionário. (bsearch)
               - Se a palavra não estiver no dicionário, imprime um erro e, se o modo for 2,
                 imprime sugestões de correção.
               - O modo 1 verifica apenas erros, o modo 2 verifica erros e sugere correções,
                 e o modo 3 corrige automaticamente as palavras.
               - O número máximo de diferenças permitidas e o número máximo de sugestões
                 são controlados pelos parâmetros max_diff e max_suggestions, respetivamente.
*/
void processa_linhas(char ** lines, int nr_linhas, char ** dictionary, int dict_size, FILE *output, int mode, int max_diff, int max_suggestions) {
    char * delimitadores = " #*$\t\n/.,;:!?\"-()[]{}\\|<>~`@^&%+=_";

    int primeira = 1;
    for (int i = 0; i < nr_linhas; i++) {
        char * linha = strdup(lines[i]); /* Copia a linha para nao estragar a original */
        if (linha == NULL) {
            fprintf(stderr, "Erro ao alocar memória para linha\n");
            exit(1);
        }

        if (mode == 3) {
            char *linha_corrigida = corrige_linha(linha, dictionary, dict_size, max_diff);
            fprintf(output, "%s\n", linha_corrigida);
            free(linha_corrigida);
            free(linha);
            continue; /* Se o modo for 3, corrige a linha e passa para a próxima antes de chegar ao funcionamento de outros modos */
        }

        char * pch = strtok(linha, delimitadores); /* Divide a linha pelos delimitadores definidos */
        while (pch != NULL) { 
            char *temp = strdup(pch); /*copia temporaria da palavra para dar handle aos apostrofos */
            if (temp == NULL) {
                fprintf(stderr, "Erro ao alocar memória para temp\n");
                exit(1);
            }
            size_t temp_len = strlen(temp);


            /* Remove apóstrofos no início e no fim, se houver. Nao podemos cortar palavras com apostrofo ao meio, por isso a lógica extra */
            int count_apostrofo_inicio = 0;
            while (count_apostrofo_inicio < temp_len && temp[count_apostrofo_inicio] == '\'') {
                count_apostrofo_inicio++;
            }

            int count_apostrofo_final = 0;
            while (count_apostrofo_final < temp_len && temp[temp_len - 1 - count_apostrofo_final] == '\'') {
                count_apostrofo_final++;
            }

            /*se for so apostrofos, ignora a palavra*/
            if (count_apostrofo_inicio == temp_len) {
                free(temp);
                pch = strtok(NULL, delimitadores);
                continue;
            }

            /* como no modo 3 é criado um token sem apostrofos para a pesquisa */
            char *clean_token = NULL;
            size_t new_length = temp_len - count_apostrofo_inicio - count_apostrofo_final;
            clean_token = malloc(new_length + 1);
            if (clean_token == NULL) {
                fprintf(stderr, "Erro ao alocar memória para clean_token\n");
                exit(1);
            }
            if (new_length > 0) {
                strncpy(clean_token, temp + count_apostrofo_inicio, new_length);
            }
            clean_token[new_length] = '\0';
            
            free(temp);
        
            /* Se, após a limpeza, o token está vazio, ignora-o */
            if (clean_token[0] == '\0') {
                free(clean_token);
                pch = strtok(NULL, delimitadores);
                continue;
            }


            if (clean_token[0] == '\0') {
                free(clean_token);
                pch = strtok(NULL, delimitadores);
                continue;
            }

            /* Ignora números */
            if (somente_numero(clean_token)) {
                free(clean_token);
                pch = strtok(NULL, delimitadores);
                continue;
            }

            /* Verifica se a palavra está no dicionário */
            void * res = bsearch(&clean_token, dictionary, dict_size, sizeof(char*), comparacao_helper);
            if (res == NULL) {
                if (primeira == 1) { /* Isto serve para a dar print a linha onde se encontra a palavra somente na primeira palavra errada da linha */
                    fprintf(output, "%d: %s\n", i+1, lines[i]);
                    primeira = 0;
                }
                fprintf(output, "Erro na palavra \"%s\"\n", clean_token);

                int sugestao = 0;
                if (mode == 2) { /* Modo 2: Sugestoes. O parametro que decide se este modo esta ativo é passado nos paramentros da funcao */
                    int difs = max_diff;
                    int count_sugestoes = 0;

                    char **suggested = malloc(max_suggestions * sizeof(char*)); /* Array para guardar sugestoes, para nao se repetir */
                    if (!suggested) {
                        fprintf(stderr, "Erro ao alocar memória para sugestoes\n");
                        exit(1);
                    } 
                    
                    int suggested_count = 0;
                    for (int j = 1; j <= difs && count_sugestoes < max_suggestions; j++) {
                        for (int k = 0; k < dict_size && count_sugestoes < max_suggestions; k++) {
                            char * segunda_palavra = diferenca_com_segunda_palavra(clean_token, dictionary[k], dictionary, dict_size);
                            if (j == 1 && segunda_palavra != NULL) {    /* Primeiro é feito o check de se o erro está em ter faltado algum espaço, isto é ao divir a palavra no primeiro e obtermos duas palavras que constam no dicionario, um erro pode ser um espaço que faltou */
                                char * nova_palavra = malloc(strlen(dictionary[k]) + strlen(segunda_palavra) + 2);
                                if (nova_palavra == NULL) {
                                    fprintf(stderr, "Erro ao alocar memória para nova_palavra\n");
                                    exit(1);
                                }
                                strcpy(nova_palavra, dictionary[k]);
                                strcat(nova_palavra, " ");
                                strcat(nova_palavra, segunda_palavra); /*É necessario criar uma nova string com as duas strings com um espaço no meio para dar print como a sugestao */

                                int jaSugerida = 0;
                                for (int s = 0; s < suggested_count; s++) {
                                    if (strcasecmp(suggested[s], nova_palavra) == 0) {  /* Verifica se a sugestao ja foi dada */
                                        jaSugerida = 1;
                                        break;
                                    }
                                }
                                if (jaSugerida) {
                                    free(nova_palavra);
                                    continue;
                                }
                                suggested[suggested_count++] = nova_palavra;
                                if (sugestao == 0) {
                                    fprintf(output, "%s", nova_palavra);  /* Se for a primeira sugestao, imprime sem virgula */
                                    sugestao = 1;
                                } else {
                                    fprintf(output, ", %s", nova_palavra);  /* Se nao for a primeira sugestao, imprime com virgula */
                                }
                                count_sugestoes++;
                            }
                            int dif = minimo_diferencas(clean_token, dictionary[k], j); /* Verifica se a minimo numero de diferenças entre as duas palavras de acordo com cada um dos processos é igual ao número de diferenças permitidas */
                            if (dif == j) {
                                int jaSugerida = 0;
                                for (int s = 0; s < suggested_count; s++) {
                                    if (strcasecmp(suggested[s], dictionary[k]) == 0) {
                                        jaSugerida = 1;
                                        break;
                                    }
                                }
                                if (jaSugerida)
                                    continue;

                                char * sugestao_str = strdup(dictionary[k]);
                                suggested[suggested_count++] = sugestao_str;
                                if (sugestao_str == NULL) {
                                    fprintf(stderr, "Erro ao alocar memória para sugestao_str\n");
                                    exit(1);
                                }
                                if (sugestao == 0) {
                                    fprintf(output, "%s", sugestao_str); /* Se for a primeira sugestao, imprime sem virgula */
                                    sugestao = 1;
                                } else {
                                    fprintf(output, ", %s", sugestao_str); /* Se nao for a primeira sugestao, imprime com virgula */
                                }
                                count_sugestoes++;
                                if (count_sugestoes >= max_suggestions) {
                                    break;
                                }
                            }
                        }
                    }
                    for (int s = 0; s < suggested_count; s++) {
                        free(suggested[s]);
                    }
                    free(suggested);
                    fprintf(output, "\n");
                }
            }
            free(clean_token);
            pch = strtok(NULL, delimitadores); 
        }
        free(linha);
        primeira = 1;
    }
}

/*Nome: primeira_sugestao
  Input: palavra: palavra a verificar (uso suposto: uma palavra errada)
         dicionario: array de strings (dicionário)
         size_dicionario: número de palavras no dicionário
         max_diferencas: número máximo de diferenças permitidas
  Output: string com a primeira sugestão encontrada ou NULL se não houver sugestões
  Description:
        - Retorna a primeira sugestão encontrada no dicionário para a palavra fornecida
        utilizando o mesmo algoritmo de sugestões que o modo 2 (ver processa_linhas).
*/
char * primeira_sugestao(char *palavra, char **dicionario, int size_dicionario, int max_diferencas) {
    for (int j = 1; j <= max_diferencas; j++) {
        for (int k = 0; k < size_dicionario; k++) {
            // Tenta obter a sugestão combinada (possível falta de espaço)
            char *segunda_palavra = diferenca_com_segunda_palavra(palavra, dicionario[k], dicionario, size_dicionario);
            if (j == 1 && segunda_palavra != NULL) { 
                int nova_palavra_len = strlen(dicionario[k]) + strlen(segunda_palavra) + 2;
                char *nova_palavra = malloc(nova_palavra_len);
                if (nova_palavra == NULL) {
                    fprintf(stderr, "Erro ao alocar memória para nova_palavra\n");
                    exit(1);
                }
                strcpy(nova_palavra, dicionario[k]);
                strcat(nova_palavra, " ");
                strcat(nova_palavra, segunda_palavra);
                
                return nova_palavra;
            }
            // Verifica se a diferença mínima é exatamente j
            int dif = minimo_diferencas(palavra, dicionario[k], j);
            if (dif == j) {
                return strdup(dicionario[k]);
            }
        }
    }
    
    return NULL;
}

/* Nome: corrige_linha
   Input: linha: linha original a ser corrigida
          dictionary: array de strings (dicionário)
          dict_size: número de palavras no dicionário
          max_diff: número máximo de diferenças permitidas
   Output: ponteiro para a linha corrigida (deve ser libertado pelo chamador)
   Description: Corrige automaticamente uma linha substituindo palavras erradas
                pela primeira sugestão disponível no dicionário.
*/
char* corrige_linha(char* linha, char** dictionary, int dict_size, int max_diff) {
    char * delimitadores = " #*$\t\n/.,;:!?\"-()[]{}\\|<>~`@^&%+=_";
    char *linha_corrigida = NULL;
    size_t size_linha_corrigida = 0;
    
    /* Inicializa a linha corrigida */
    linha_corrigida = malloc(1);
    if (linha_corrigida == NULL) { 
        fprintf(stderr, "Erro ao alocar memória\n"); 
        exit(1); 
    }
    linha_corrigida[0] = '\0';
    size_linha_corrigida = 0;
    
    char *ptr = linha;
    while (*ptr != '\0') {
        /* Se o caractere atual for um delimitador, copia-o para a saída */
        if (strchr(delimitadores, *ptr) != NULL) {
            { 
                char *temp = realloc(linha_corrigida, size_linha_corrigida + 2);
                if (temp == NULL) {
                    fprintf(stderr, "Erro ao realocar memória\n");
                    exit(1);
                }
                linha_corrigida = temp;
            }
            linha_corrigida[size_linha_corrigida] = *ptr;
            size_linha_corrigida++;
            linha_corrigida[size_linha_corrigida] = '\0';
            ptr++;
        } else {
            // Extrai o token (palavra)
            size_t token_len = strcspn(ptr, delimitadores);
            char *token = malloc(token_len + 1);
            if (token == NULL) {
                fprintf(stderr, "Erro ao alocar memória para token\n");
                exit(1);
            }
            strncpy(token, ptr, token_len);
            token[token_len] = '\0';

            /*Para lidar com apostrofos no inicio e no final é preciso esta logica toda para nao cortar palavras como can't ao meio*/
            int count_apostrofo_inicio = 0;
            while (count_apostrofo_inicio < token_len && token[count_apostrofo_inicio] == '\'') {
                count_apostrofo_inicio++;
            }

            int count_apostrofo_final = 0;
            while (count_apostrofo_final < token_len && token[token_len - 1 - count_apostrofo_final] == '\'') {
                count_apostrofo_final++;
            }

            if (count_apostrofo_inicio == token_len) {
                char *temp = realloc(linha_corrigida, size_linha_corrigida + token_len + 1);
                if (temp == NULL) {
                    fprintf(stderr, "Erro ao realocar memória\n");
                    exit(1);
                }
                linha_corrigida = temp;
                strncat(linha_corrigida, token, token_len);
                size_linha_corrigida += token_len;
                free(token);
                ptr += token_len;
                continue;
            }

            // Cria uma palavra sem os apóstrofos para a pesquisa
            size_t new_length = token_len - count_apostrofo_inicio - count_apostrofo_final;
            char *clean_token = malloc(new_length + 1);
            if (clean_token == NULL) {
                fprintf(stderr, "Erro ao alocar memória para clean_token\n");
                exit(1);
            }
            if (new_length > 0) {
                strncpy(clean_token, token + count_apostrofo_inicio, new_length);
            }
            clean_token[new_length] = '\0';

            if (somente_numero(clean_token)) {
                /* Se for um numero apenas adiciona o numero */
                { 
                    char *temp = realloc(linha_corrigida, size_linha_corrigida + token_len + 1);
                    if (temp == NULL) {
                        fprintf(stderr, "Erro ao realocar memória\n");
                        exit(1);
                    }
                    linha_corrigida = temp;
                }
                strncat(linha_corrigida, token, token_len);
                size_linha_corrigida += token_len;
            } else {
                void *res = bsearch(&clean_token, dictionary, dict_size, sizeof(char*), comparacao_helper);
                if (res == NULL) {
                    char *sugestao = primeira_sugestao(clean_token, dictionary, dict_size, max_diff);
                    if (sugestao != NULL) {
                        for (int k = 0; k < count_apostrofo_inicio; k++) { //adiciona os apostrofos no final
                            char *temp = realloc(linha_corrigida, size_linha_corrigida + 2);
                            if (temp == NULL) {
                                fprintf(stderr, "Erro ao realocar memória\n");
                                exit(1);
                            }
                            linha_corrigida = temp;
                            linha_corrigida[size_linha_corrigida] = '\'';
                            size_linha_corrigida++;
                            linha_corrigida[size_linha_corrigida] = '\0';
                        }
                        size_t sug_len = strlen(sugestao);
                        { 
                            char *temp = realloc(linha_corrigida, size_linha_corrigida + sug_len + 1);
                            if (temp == NULL) {
                                fprintf(stderr, "Erro ao realocar memória\n");
                                exit(1);
                            }
                            linha_corrigida = temp;
                        }
                        strcat(linha_corrigida, sugestao); /* adiciona a palavra entre os apostrofos */
                        size_linha_corrigida += sug_len;
                        for (int k = 0; k < count_apostrofo_final; k++) {
                            char *temp = realloc(linha_corrigida, size_linha_corrigida + 2);
                            if (temp == NULL) {
                                fprintf(stderr, "Erro ao realocar memória\n");
                                exit(1);
                            }
                            linha_corrigida = temp;
                            linha_corrigida[size_linha_corrigida] = '\'';
                            size_linha_corrigida++;
                            linha_corrigida[size_linha_corrigida] = '\0';
                        }
                        free(sugestao);
                    } else {
                        /* se nao for encontrada uma sugestao no dicionario vai a original */
                        { 
                            char *temp = realloc(linha_corrigida, size_linha_corrigida + token_len + 1);
                            if (temp == NULL) {
                                fprintf(stderr, "Erro ao realocar memória\n");
                                exit(1);
                            }
                            linha_corrigida = temp;
                        }
                        strncat(linha_corrigida, token, token_len);
                        size_linha_corrigida += token_len;
                    }
                } else {
                    /* se a palavra estiver certa (no dicionario) vai a original, que ja ha de ter os apostrofos todos por isso nao é preciso lidar com isso */
                    { 
                        char *temp = realloc(linha_corrigida, size_linha_corrigida + token_len + 1);
                        if (temp == NULL) {
                            fprintf(stderr, "Erro ao realocar memória\n");
                            exit(1);
                        }
                        linha_corrigida = temp;
                    }
                    strncat(linha_corrigida, token, token_len);
                    size_linha_corrigida += token_len;
                }
            }
            free(token);
            free(clean_token);
            ptr += token_len;
        }
    }
    return linha_corrigida;
}
