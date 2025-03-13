ortografia: ortografia.c
	gcc -o ortografia ortografia.c -Wall -O3 -g

.PHONY: clean
clean:
	rm -f ortografia