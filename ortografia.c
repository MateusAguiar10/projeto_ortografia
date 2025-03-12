#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//decidimos adiantar esta parte
char** dicionario(char* filename) {
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

	char **words = (char**) malloc(count * sizeof(char*));

	int i = 0;
	while (fgets(buffer, sizeof(buffer), file)) {
		// remove o \n do final da palavra
		size_t len = strlen(buffer);
		if (buffer[len - 1] == '\n') {
			buffer[len - 1] = '\0';
		}

		// aloca a palavra e copia o buffer para a palavra
		// desta forma cada palavra tem o espaco certo
		words[i] = (char*) malloc(len);
		strcpy(words[i], buffer);
		i++;
	}
	fclose(file);
	return words;
}

int main(int argc, char **argv) {
	char* filename = "/usr/share/dict/words";

	char** words = dicionario(filename);
	
	// imprime as palavras
	for (int i = 0; i < 10; i++) {
		printf("%s\n", words[i]);
	}
}
