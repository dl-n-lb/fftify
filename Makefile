fft: main.c
	gcc -o fft main.c -lm -Wall -Wextra

sine: sine.c
	gcc -o sine sine.c -lm -Wall -Wextra

comp: comp.c
	gcc -o comp comp.c -lm -Wall -Wextra

.PHONY: fft sine comp

clean:
	rm fft sine
