#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//decidimos adiantar esta parte
char** dicionario(char * filename, int * words_len) {
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

//resolver o problema da funcao, vi no stack overflow
int compare_strings(const void *a, const void *b) {
    const char *str1 = *(const char **)a;
    const char *str2 = *(const char **)b;
    
    return strcasecmp(str1, str2);
}

int main(int argc, char **argv) {
	char* filename = "/usr/share/dict/words";

	int words_len; 
	char** words = dicionario(filename, &words_len);

	qsort(words, words_len, sizeof(char*), compare_strings);	

/* PARTE DO INPUT */
	char ** lines = NULL;
	char buffer[1000];

	int linhasalocadas = 0;
	int count = 0;

	while (fgets(buffer, sizeof(buffer), stdin) != NULL) {
		size_t linha_len = strlen(buffer);	
		if (linha_len > 0 && buffer[linha_len - 1] == '\n') {
			buffer[linha_len - 1] = '\0';
			linha_len--;
		}

		if (count >= linhasalocadas) {
			linhasalocadas += 10;
			lines = (char**) realloc(lines, linhasalocadas * sizeof(char*));
		}

		lines[count] = (char*) malloc(linha_len + 1);
		if (lines[count] == NULL) {
			printf("Erro ao alocar memoria\n");
			free(lines);
			for (int j = 0; j < count; j++) {
				free(lines[j]);
			}
			exit(1);
		}
		strcpy(lines[count], buffer);
		count++;
	}

	for (int i = 0; i < count; i++) {
		printf("%s\n", lines[i]);
	}

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
