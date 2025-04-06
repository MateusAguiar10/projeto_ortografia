/*-----------------------------------------------------------------------------------------------------+
| processos_diferencas.c | Contém as funções que calculam os diferentes tipo de erro em cada palavra:  |
|                        | - Letras diferentes, letras a mais, espaço em falta                         |
|                        | E a função helper que dá cast aos tipos aceites na função bsearch na função |
|                        | de comparação                                                               |
+------------------------------------------------------------------------------------------------------+
| Autores: José Mateus Aguiar n. 113754         Rogério Pina n. 114719                                 |
|          LEEC-IST                                                                                    |
| Data: 06 Abril 2025                                                                                  |
+-----------------------------------------------------------------------------------------------------*/
#include "processos_diferencas.h"
#include <ctype.h>
#include <string.h>
#include <stdlib.h>

/*Nome: comparacao_helper
  Input: a: ponteiro para o primeiro elemento a comparar
         b: ponteiro para o segundo elemento a comparar
  Output: int que indica a ordem entre os dois elementos
  Description: 
               - Função auxiliar para comparação de strings, usada na função bsearch.
               - Compara duas strings ignorando diferenças entre maiúsculas e minúsculas.
               - Retorna um valor negativo se a primeira string for menor, 
                 um valor positivo se a segunda string for menor, 
                 e zero se forem iguais.
               - Usada porque a função bsearch não aceita ponteiros para ponteiros como argumento de comparação.
*/
int comparacao_helper(const void *a, const void *b) {
    const char *str1 = *(const char **) a;
    const char *str2 = *(const char **) b;
    return strcasecmp(str1, str2);
}

/*Nome: diferencas_normais
  Input: a: ponteiro para o primeiro elemento a comparar
         b: ponteiro para o segundo elemento a comparar
  Output: o número de diferenças entre as duas palavras
  Description: 
                - Compara duas palavras letra a letra, ignorando diferenças entre maiúsculas e minúsculas.
                - Retorna o número de diferenças encontradas.
                - Se as palavras forem de tamanhos diferentes, conta as letras restantes como diferenças.
*/
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
    diff += (len1 - i) + (len2 - j);
    return diff;
}

/*Nome: diferencas_offset
  Input: a: ponteiro para o primeiro elemento a comparar
         b: ponteiro para o segundo elemento a comparar
  Output: o número de diferenças entre as duas palavras
  Description: 
                - Compara duas palavras letra a letra, ignorando diferenças entre maiúsculas e minúsculas.
                - Se as letras forem diferentes, conta como uma diferença e avança o índice da palavra mais longa.
                - Se as letras forem iguais, avança ambos os índices.
                - No fundo, conta as letras a mais ou a menos como diferenças.
*/
int diferencas_offset(const char *w1, const char *w2, int target) {
    if (target < 1) {
        target = 1;
    }
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
                j += target;
            } else if(len1 > len2) {
                i += target;
            } else {
                i += target;
                j += target;
            }
        }
    }
    if (i < len1) {
        diff += (len1 - i);
    }
    if (j < len2) {
        diff += (len2 - j);
    }
    return diff;
}

/*Nome: diferencas_para_tras
  Input: a: ponteiro para o primeiro elemento a comparar
         b: ponteiro para o segundo elemento a comparar
  Output: o número de diferenças entre as duas palavras
  Description: 
                - Compara duas palavras letra a letra, da direita para a esquerda, ignorando diferenças entre maiúsculas e minúsculas.
                - Retorna o número de diferenças encontradas.
                - Se as palavras forem de tamanhos diferentes, conta as letras restantes como diferenças.
*/
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

/*Nome: minimo_diferencas
  Input: a: ponteiro para o primeiro elemento a comparar
         b: ponteiro para o segundo elemento a comparar
  Output: o mínimo número de diferenças entre as duas palavras considerando as funções diferencas_normais, 
          diferencas_offset e diferencas_para_tras.
  Description: 
                - Compara duas palavras usando três métodos diferentes de comparação.
                - Retorna o menor número de diferenças encontradas entre os três métodos.
                - Se as palavras forem de tamanhos diferentes, conta as letras restantes como diferenças.
*/
int minimo_diferencas(const char *w1, const char *w2, int target) {
    int d1 = diferencas_normais(w1, w2);
    int d2 = diferencas_offset(w1, w2, target);
    int d4 = diferencas_para_tras(w1, w2);
    int m = d1;
    if(d2 < m) m = d2;
    if(d4 < m) m = d4;
    return m;
}

/*Nome: diferenca_com_segunda_palavra
  Input: a: ponteiro para o primeiro elemento a comparar
         b: ponteiro para o segundo elemento a comparar
  Output: string com a primeira sugestão encontrada ou NULL se não houver sugestões
  Description: 
                - Verifica se existe uma palavra que possa completar w1 no dicionário e se w2 com len = x 
                  corresponder aos x primeiros caracteres de w1.
                - Se sim, retorna a nova palavra.
                - Se não, retorna NULL.
*/
char * diferenca_com_segunda_palavra(char * w1, char * w2, char ** dictionary, size_t dict_size) {
    size_t len1 = strlen(w1);
    size_t len2 = strlen(w2);
    // Se a primeira palavra for menor ou igual à do dicionario, nao há segunda palavra de certeza
    if (len1 <= len2) {
        return NULL;
    }

    // Se a palavra do dicionario nao constar na sua totalidade na primeira palavra, nao ha segunda palavra
    for (size_t j = 0; j < len2; j++) {
        if (tolower((unsigned char) w1[j]) != tolower((unsigned char) w2[j])) {
            return NULL;
        }
    }

    char *nova_palavra = &w1[len2];

    void *res = bsearch(&nova_palavra, dictionary, dict_size, sizeof(char*), comparacao_helper);
    if (res != NULL) {
        return nova_palavra;
    }
    return NULL;
}

/*Nome: somente_numero
  Input: s: string a verificar
  Output: int que indica se a string é numérica (1) ou não (0)
  Description: 
                - Verifica se a string contém apenas dígitos.
                - Retorna 1 se a string for numérica, 0 caso contrário.
                - Se a string estiver vazia, retorna 0.
*/
int somente_numero(const char *s) {
    if (*s == '\0') return 0;
    for (int i = 0; s[i] != '\0'; i++) {
        if (!isdigit((unsigned char)s[i])) return 0;
    }
    return 1;
}