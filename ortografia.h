/*-----------------------------------------------------------------------------------------------------+
| ortografia.h           | Funções de carregamento do dicionário para um vetor                         |
|                        | Função de processamento de um vetor de linhas com suporte a diferentes modos|
|                        | de funcionamento                                                            |
|                        | Função que retorna a primeira sugestão com base no algoritmo de sugestoes   |
+------------------------------------------------------------------------------------------------------+
| Autores: José Mateus Aguiar n. 113754         Rogério Pina n. 114719                                 |
|          LEEC-IST                                                                                    |
| Data: 06 Abril 2025                                                                                  |
+-----------------------------------------------------------------------------------------------------*/
#ifndef ORTOGRAFIA_H
#define ORTOGRAFIA_H

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
char ** carrega_dicionario(char * filename, int * words_len);

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
void processa_linhas(char ** lines, int nr_linhas, char ** dictionary, int dict_size, FILE *output, int mode, int max_diff, int max_suggestions);

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
char * primeira_sugestao(char * palavra, char ** dicionario, int size_dicionario, int max_diferencas);

/* Nome: corrige_linha
   Input: linha: linha original a ser corrigida
          dictionary: array de strings (dicionário)
          dict_size: número de palavras no dicionário
          max_diff: número máximo de diferenças permitidas
   Output: ponteiro para a linha corrigida (deve ser libertado pelo chamador)
   Description: Corrige automaticamente uma linha substituindo palavras erradas
                pela primeira sugestão disponível no dicionário.
*/
char* corrige_linha(char* linha, char** dictionary, int dict_size, int max_diff);

#endif
