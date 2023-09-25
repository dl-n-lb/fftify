# FFT-IFY
A pipe based system for messing with the fast fourier transform. Example use below, produces the sine wave after FFTing and unFFTing it.

``` bash
$ ./sine -t 2 -s 1024 | ./fft | ./fft -cgb | graph --output-format png -x 0 2000 -y -5 5 > out.png && feh out.png
```

# fft (main.c)
takes an input from a file (--input, -i, default stdin) with the format of floats separated by new lines (or with the -c or --complex flag `float float` separated by new lines). Produces the fourier transform of the input and outputs it to a file (--output, -o, default stdout).

To output in a graphable format, pass --graph or -g, and use with graph from `plotutils`.

To get help just run the command with no arguments.

```bash
$ ./fft
```

!! CURRENTLY UNIMPLEMENTED: 2D transforms

# sine (sine.c)
Outputs a sine wave with properties specified by the parameters passed. 
(--output, -o: default stdout)
(--input, -i: default stdin)
(--time, -t time, "time" in "seconds" for the wave to be generated over)
(--frequency, -f: frequency of the wave in Hz)
(--samples, -s: number of samples to generate)
(--cos, -c: produce a cosine wave instead of a sine wave)
