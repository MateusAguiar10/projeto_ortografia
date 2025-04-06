# Linking
ortografia: main.o ortografia.o processos_diferencas.o
	gcc -Wall -O3 -g -o ortografia main.o ortografia.o processos_diferencas.o


# Dependências object files
main.o: main.c ortografia.h processos_diferencas.h
	gcc -Wall -O3 -g -c main.c

ortografia.o: ortografia.c ortografia.h processos_diferencas.h
	gcc -Wall -O3 -g -c ortografia.c

string_diff.o: processos_diferencas.c processos_diferencas.h
	gcc -Wall -O3 -g -c processos_diferencas.c
	
.PHONY: clean
clean:
	rm -f ortografia