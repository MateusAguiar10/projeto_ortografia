#define DICT_PATHS "/usr/share/dict"

#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//decidimos adiantar esta parte
char ** dicionario(char * filename, int * words_len) {
	FILE *file = fopen(filename, "r");
	if (file == NULL) {
		printf("Erro ao abrir o arquivo\n");
		exit(1);
	}

	// conta o nr de palavras
	int count = 0;
	char buffer[100];

	while (fgets(buffer, sizeof(buffer), file)) {
		count++;
	}

	// aloca o vetor de ponteiros para as palavras com a quantidade de palavras

	rewind(file);

	char ** words = (char**) malloc(count * sizeof(char*));

	int i = 0;
	while (fgets(buffer, sizeof(buffer), file)) {
		// remove o \n do final da palavra
		size_t len = strlen(buffer);
		if (buffer[len - 1] == '\n') {
			buffer[len - 1] = '\0';
		}

		// aloca a palavra e copia o buffer para a palavra
		// desta forma cada palavra tem o espaco certo
		words[i] = (char*) malloc(len + 1);
		strcpy(words[i], buffer);
		i++;
	}
	fclose(file);

	*words_len = count;
	return words;
}

//esta helper function ajuda a juntar todos os dicionarios
char ** todos_dicionarios(int * words_len) {
	const char *dict_dir = DICT_PATHS;
	DIR *dir = opendir(dict_dir);
	if (dir == NULL) {
		printf("Erro ao abrir o diretorio\n");
		exit(1);
	}

	*words_len = 0;
	char ** words = NULL;

	// confesso que nao sabia o que era o dirent entao copiei um pouco do stack overflow
	struct dirent *entry;
	while ((entry = readdir(dir)) != NULL) {
		if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
			continue;
		}

		char filename[400];
		snprintf(filename, sizeof(filename), "%s/%s", dict_dir, entry->d_name);

		int count;
		char ** singular = dicionario(filename, &count);

		words = (char**) realloc(words, (*words_len + count) * sizeof(char*));
		for (int i = 0; i < count; i++) {
			words[*words_len + i] = singular[i];
		}
		*words_len += count;
		free(singular);
	}
	closedir(dir);
	return words;
}

void testar_linhas(char ** linhas, int nr_linhas, char ** dicionario, size_t size_dicionario, int (*compar)(const void *, const void *)) {
	// wtf é feff
	char * delimitadores = " \t\n/.,;:!?\"-\'()[]{}﻿";
	int primeira = 1;
	for (int i = 0; i < nr_linhas; i++) {
		char * linha = strdup(linhas[i]);
		// para nao estragar a linha original tenho de duplicar a linha
		char * pch = strtok(linha, delimitadores);
		// o strtok divide a linha pelos separadores que definimos dentro das aspas
		while (pch != NULL) {
			//tive de adicionar logica para lidar com numeros, caso o atoi seja 0. Ainda há o caso de haver uma palavra com multiplos 000000 mas vou ignorar isso
			if (*pch == '0') {
				pch = strtok(NULL, delimitadores);
				continue;
			} else if (atoi(pch) != 0) {
				pch = strtok(NULL, delimitadores);
				continue;
			} else {
				// o bsearch procura a palavra no dicionario
				void * res = bsearch(&pch, dicionario, size_dicionario, sizeof(char*), compar);

				if (res == NULL) {
					if (primeira == 1) {
						printf("%d: %s\n", i+1, linhas[i]);
						primeira = 0;
					}
					printf("Erro na palavra \"%s\"\n", pch);
				}
				
				pch = strtok(NULL, " \t\n/.,;:!?\"-\'()[]{}");
			}
		}
		free(linha);
		primeira = 1;
	}
}

//resolver o problema da funcao, vi no stack overflow
int compare_strings(const void *a, const void *b) {
    const char *str1 = *(const char **)a;
    const char *str2 = *(const char **)b;
    
    return strcasecmp(str1, str2);
}

int main(int argc, char ** argv) {
	int words_len; 
	char ** words = todos_dicionarios(&words_len);

	qsort(words, words_len, sizeof(char*), compare_strings);	


  /* PARTE DO INPUT */
	char ** lines = NULL;
	char buffer[1000];

	int linhasalocadas = 0;
	int count = 0;

	// eu nem sei como fazer sem dyanmic memory allocation
	while (fgets(buffer, sizeof(buffer), stdin) != NULL) {
		size_t linha_len = strlen(buffer);	
		if (linha_len > 0 && buffer[linha_len - 1] == '\n') {
			buffer[linha_len - 1] = '\0';
			linha_len--;
		}

		// aloca mais 10 linhas se precisar
		if (count >= linhasalocadas) {
			linhasalocadas += 10;
			lines = (char**) realloc(lines, linhasalocadas * sizeof(char*));
		}

		// aloca a linha e copia o buffer para a linha
		lines[count] = (char*) malloc(linha_len + 1);
		if (lines[count] == NULL) {
			printf("Erro ao alocar memoria\n");
			for (int j = 0; j < count; j++) {
				free(lines[j]);
			}
			free(lines);
			exit(1);
		}
		strcpy(lines[count], buffer);
		count++;
	}

	testar_linhas(lines, count, words, words_len, compare_strings);

  /* FREES */
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
