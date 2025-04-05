#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

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

// Compara do início, mas ao encontrar a primeira diferença, 
// salta um número de caracteres (offset) na palavra do dicionário se a palavra errada for menor,
// ou na palavra errada se ela for maior. Se forem do mesmo tamanho, salta em ambas.
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

// Compara do início, mas ao encontrar a primeira diferença, salta o offset na palavra errada.
int diff_offset_wrong(const char *w1, const char *w2, int target) {
    int diff = 0;
    int i = 0, j = 0;
    int len1 = strlen(w1), len2 = strlen(w2);
    while(i < len1 && j < len2) {
        if(tolower(w1[i]) == tolower(w2[j])) {
            i++; j++;
        } else {
            diff++;
            if(diff > target)
                return diff;
            i += target; // salta na palavra errada
            j++;       // avança um na palavra do dicionário
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
    int d1 = diferencas_normais(w1, w2);
    int d2 = diferencas_offset(w1, w2, target);
    // int d3 = diff_offset_wrong(w1, w2, target);
    int d4 = diferencas_para_tras(w1, w2);
    int m = d1;
	printf("Diferenças: %d %d %d\n", d1, d2, d4);
    if(d2 < m) m = d2;
    if(d4 < m) m = d4;
    return m;
}

int main(int argc, char **argv) {
    if(argc < 2) {
        printf("Uso: %s palavra_errada\n", argv[0]);
        return 1;
    }
    
	int found = 0;
    char *wrong = argv[1];
	char *dict = argv[2];
	int maxDif = 2;
    
	int d = minimo_diferencas_diferentes_processos(wrong, dict, maxDif);
	if(d == maxDif) {
		printf("%s\n", dict);
		found = 1;
	}
    if(!found)
        printf("Nenhuma alternativa encontrada.\n");
        
} 
