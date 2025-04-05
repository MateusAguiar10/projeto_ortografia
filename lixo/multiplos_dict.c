#include <dirent.h>

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

