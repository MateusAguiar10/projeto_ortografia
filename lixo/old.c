#define DICT_PATH "words"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <ctype.h>

// vi isto no stack overflow
int compare_strings(const void *a, const void *b) {
    const char *str1 = *(const char **)a;
    const char *str2 = *(const char **)b;
    
    return strcasecmp(str1, str2);
}

// Compara letra a letra (do início) sem saltos ("offset") e conta as diferenças.
int diferencas_normais(const char *w1, const char *w2) {
    int diff = 0;
    int i = 0, j = 0;
    int len1 = strlen(w1), len2 = strlen(w2);
    while(i < len1 && j < len2) {
        if(tolower(w1[i]) == tolower(w2[j])) {
            i++; j++;
        } else {
            diff++;
            i++; j++;
        }
    }
    // Se uma palavra acabar antes, cada caractere extra conta como diferença.
    diff += (len1 - i) + (len2 - j);
    return diff;
}


int diferencas_offset(const char *w1, const char *w2, int target) {
    int diff = 0;
    int i = 0, j = 0;
    int len1 = strlen(w1), len2 = strlen(w2);
    while(i < len1 && j < len2) {
        if(tolower(w1[i]) == tolower(w2[j])) {
            i++; j++;
        } else {
            diff += target;
            if(diff > target)
                return diff;
            if(len1 < len2) {
                j += target; // salta na palavra do dicionário
            } else if(len1 > len2) {
                i += target; // salta na palavra errada
            } else {
                i += target;
                j += target;
            }
        }
    }
    diff += (len1 - i) + (len2 - j);
    return diff;
}

// Compara as palavras de trás para a frente, contando as diferenças.
int diferencas_para_tras(const char *w1, const char *w2) {
    int diff = 0;
    int len1 = strlen(w1), len2 = strlen(w2);
    int i = len1 - 1, j = len2 - 1;
    while(i >= 0 && j >= 0) {
        if(tolower(w1[i]) == tolower(w2[j])) {
            i--; j--;
        } else {
            diff++;
            i--; j--;
        }
    }
    diff += (i + 1) + (j + 1);
    return diff;
}

// Calcula a menor diferença encontrada pelos métodos acima.
int minimo_diferencas_diferentes_processos(const char *w1, const char *w2, int target) {
	// Calcula as diferenças normais, offset e para trás.
    int d1 = diferencas_normais(w1, w2);
    int d2 = diferencas_offset(w1, w2, target);
    int d4 = diferencas_para_tras(w1, w2);
    int m = d1;
    if(d2 < m) m = d2;
    if(d4 < m) m = d4;

    return m;
}

char * diferenca_com_palavra_dicionario(char *w1, char *w2, char **dicionario, size_t size_dicionario) {
	int len2 = strlen(w2);
    int j;
	for (j = 0; j < len2; j++) {
        if (tolower(w1[j]) != tolower(w2[j])) {
            break;
        }
    }

    if (j != len2) {
        return NULL;
    }

	char * nova_palavra = w1 + j;

	void * res = bsearch(&nova_palavra, dicionario, size_dicionario, sizeof(char*), compare_strings);
	if (res != NULL) {
		return nova_palavra;
	}

	return NULL;
}

void testar_linhas(char ** linhas, int nr_linhas, char ** dicionario, size_t size_dicionario, FILE *output, int modo, int modo2_difs, int modo2_max) {
    // este caracter feff aparece num texto qualquer de um teste, 'e um caracter invisivel que nao tem nenhuma razao para exisitr
    char * delimitadores = " \t\n/.,;:!?\"-()[]{}﻿";
    int primeira = 1;
    for (int i = 0; i < nr_linhas; i++) {
        char * linha = strdup(linhas[i]);
        // para nao estragar a linha original tenho de duplicar a linha
        char * pch = strtok(linha, delimitadores);
        // o strtok divide a linha pelos separadores que definimos dentro das aspas
        while (pch != NULL) {

            // se a palavra tiver aspas no inicio e no fim, remove as aspas, só pq algumas palavras tem um apostrofo no meio e nao quero dividi-las

            size_t len = strlen(pch);

            if (len > 0 && pch[0] == '\'') {
                pch++;
                len--;
            }

            if (len > 0 && pch[len - 1] == '\'') {
                pch[len - 1] = '\0';
            }

            if (pch[0] == '\0') {
                pch = strtok(NULL, delimitadores);
                continue;
            }

            //tive de adicionar logica para lidar com numeros, caso o atoi seja 0. Ainda há o caso de haver uma palavra com multiplos 000000 mas vou ignorar isso porque nao é relevante
            if (pch[0] == '0' || atoi(pch) != 0) {
                pch = strtok(NULL, delimitadores);
                continue;
            } 

            void * res = bsearch(&pch, dicionario, size_dicionario, sizeof(char*), compare_strings);

            if (res == NULL) {
                if (primeira == 1) {
                    fprintf(output, "%d: %s\n", i+1, linhas[i]);
                    primeira = 0;
                }
                fprintf(output, "Erro na palavra \"%s\"\n", pch);

                int sugestao = 0;
                if (modo == 2) {
                    int difs = modo2_difs;
                    int count_sugestoes = 0;
                    char * suggested[modo2_max]; 
                    int suggested_count = 0;
                    for (int j = 1; j <= difs && count_sugestoes < modo2_max; j++) {
                        for (int k = 0; k < size_dicionario && count_sugestoes < modo2_max; k++) {
                            char * segunda_palavra = diferenca_com_palavra_dicionario(pch, dicionario[k], dicionario, size_dicionario);
                            if (j == 1 && segunda_palavra != NULL) {
                                char * nova_palavra = malloc(strlen(dicionario[k]) + strlen(segunda_palavra) + 2);
                                strcpy(nova_palavra, dicionario[k]);
                                strcat(nova_palavra, " ");
                                strcat(nova_palavra, segunda_palavra);

                                int jaSugerida = 0;
                                for (int s = 0; s < suggested_count; s++) {
                                    if (strcasecmp(suggested[s], nova_palavra) == 0) {
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
                                    fprintf(output, "%s", nova_palavra);
                                    sugestao = 1;
                                } else {
                                    fprintf(output, ", %s", nova_palavra);
                                }
                                count_sugestoes++;
                                
                            }
                            int dif = minimo_diferencas_diferentes_processos(pch, dicionario[k], j);
                            if (dif == j) {
                                int jaSugerida = 0;
                                for (int s = 0; s < suggested_count; s++) {
                                    if (strcasecmp(suggested[s], dicionario[k]) == 0) {
                                        jaSugerida = 1;
                                        break;
                                    }
                                }
                                if (jaSugerida)
                                    continue;

                                char * sugestao_str = strdup(dicionario[k]);
                                suggested[suggested_count++] = sugestao_str;
                                if (sugestao == 0) {
                                    fprintf(output, "%s", sugestao_str);
                                    sugestao = 1;
                                } else {
                                    fprintf(output, ", %s", sugestao_str);
                                }
                                count_sugestoes++;
                                if (count_sugestoes >= modo2_max) {
                                    break;
                                }
                            }
                        }
                    }

                    for (int s = 0; s < suggested_count; s++) {
                        free(suggested[s]);
                    }
                    
                    fprintf(output, "\n");

					
                }
            }
            
            pch = strtok(NULL, delimitadores);
    
        }
        free(linha);
        primeira = 1;
    }
}

char ** dicionario(char * filename, int * words_len) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        printf("Erro ao abrir o arquivo\n");
        exit(1);
    }

    int count = 0;
    char buffer[100];

    while (fgets(buffer, sizeof(buffer), file)) {
        count++;
    }


    rewind(file);

    char ** words = malloc(count * sizeof(char*));

    int i = 0;
    while (fgets(buffer, sizeof(buffer), file)) {
        size_t len = strlen(buffer);
        if (buffer[len - 1] == '\n') {
            buffer[len - 1] = '\0';
        }

        words[i] = malloc(len + 1);
        strcpy(words[i], buffer);
        i++;
    }
    fclose(file);

    *words_len = count;
    return words;
}

int main(int argc, char ** argv) {
    char ** lines = NULL;
    char buffer[1000];
    FILE *input = stdin; 
    FILE *output = stdout;
    int linhasalocadas = 0;
    int count = 0;
    
    char *dict_path = DICT_PATH;
    
    // Variáveis para controlar o modo de sugestão
    int modo = 0;         // Default: modo 1 (modo 2 = 2)
    int modo2_max = 10;    // Default: 10 sugestões máximas 
    int modo2_difs = 2;    // Default: diferença máxima de 2
    
    int opt;
    while ((opt = getopt(argc, argv, "i:o:d:m:a:n:")) != -1) {
        switch (opt) {
            case 'i':
                input = fopen(optarg, "r");
                if (input == NULL) {
                    printf("Erro ao abrir o arquivo de entrada: %s\n", optarg);
                    exit(1);
                }
                break;
            case 'o':
                output = fopen(optarg, "w");
                if (output == NULL) {
                    printf("Erro ao abrir o arquivo de saída: %s\n", optarg);
            
                    if (input != stdin) {
                        fclose(input);
                    }
                    
                    exit(1);
                }
                break;
            case 'd':
                dict_path = optarg;
                break;
            case 'm':
                modo = atoi(optarg);
                if (modo < 1 || modo > 3) {
                    printf("Modo inválido. Use 1 ou 2 ou 3.\n");
                    
                    if (input != stdin) {
                        fclose(input);
                    }
                    if (output != stdout) {
                        fclose(output);
                    }
                    
                    exit(1);
                }
                break;
            case 'a':
                modo2_max = atoi(optarg);
                break;
            case 'n':
                modo2_difs = atoi(optarg);
                break;
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
    
    int words_len; 
    char ** words = dicionario(dict_path, &words_len);
    
    qsort(words, words_len, sizeof(char*), compare_strings);	

    while (fgets(buffer, sizeof(buffer), input) != NULL) {
        size_t linha_len = strlen(buffer);	
        if (linha_len > 0 && buffer[linha_len - 1] == '\n') {
            buffer[linha_len - 1] = '\0';
            linha_len--;
        }

        if (count >= linhasalocadas) {
            linhasalocadas += 10;
            lines = realloc(lines, linhasalocadas * sizeof(char*));
        }

        lines[count] = malloc(linha_len + 1);
        if (lines[count] == NULL) {
            printf("Erro ao alocar memoria\n");
            for (int j = 0; j < count; j++) {
                free(lines[j]);
            }
            free(lines);
            
            if (input != stdin) {
                fclose(input);
            }
            
            for (int j = 0; j < words_len; j++) {
                free(words[j]);
            }
            free(words);
            
            exit(1);
        }
        strcpy(lines[count], buffer);
        count++;
    }
    
    if (input != stdin) {
        fclose(input);
    }


    testar_linhas(lines, count, words, words_len, output, modo, modo2_difs, modo2_max);

    if (output != stdout) {
        fclose(output);
    }

    for (int j = 0; j < words_len; j++) {
        free(words[j]);
    }

    free(words);
  
    for (int j = 0; j < count; j++) {
        free(lines[j]);
    }

    free(lines);

    return 0;
}
