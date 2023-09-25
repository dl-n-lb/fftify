fft: main.c
	gcc -o fft main.c -lm -Wall -Wextra

sine: sine.c
	gcc -o sine sine.c -lm -Wall -Wextra

clean:
	rm fft sine
