#include <stdio.h>
#include <stdlib.h>
#include "clap.h"
#include <math.h>

int main(int argc, char **argv) {
  clap_parser parser = clap_parser_init(argc, argv, (clap_parser_opts) {
      .desc = "Composes a sine wave into another wave"
    });
  clap_arg_add(&parser, (clap_arg) {
      .name = "input",
      .alias = 'i',
      .description = "input file",
      .default_value = "stdin",
      .options = CLAP_ARG_OPTIONAL
    });

  clap_arg_add(&parser, (clap_arg) {
      .name = "output",
      .alias = 'o',
      .description = "output file",
      .default_value = "stdout",
      .options = CLAP_ARG_OPTIONAL
    });
  clap_arg_add(&parser, (clap_arg) {
      .name = "time",
      .alias = 't',
      .description = "time to generate",
      .default_value = "1",
      .options = CLAP_ARG_OPTIONAL
    });
  clap_arg_add(&parser, (clap_arg) {
      .name = "frequency",
      .alias = 'f',
      .description = "frequency to generate",
      .default_value = "1",
      .options = CLAP_ARG_OPTIONAL
    });
  clap_arg_add(&parser, (clap_arg) {
      .name = "samples",
      .alias = 's',
      .description = "number of samples to generate",
      .default_value = "64",
      .options = CLAP_ARG_OPTIONAL
    });
  clap_arg_add(&parser, (clap_arg) {
      .name = "cos",
      .alias = 'c',
      .description = "cosine wave",
      .default_value = CLAP_FLAG_FALSE,
      .options = CLAP_ARG_OPTIONAL | CLAP_ARG_FLAG
    });

  if (!clap_parse(&parser)) {
    clap_print_err(parser);
    clap_destroy(&parser);
    return 1;
  }

  const char *sinf = clap_get(parser, "input");
  const char *soutf = clap_get(parser, "output");
  const char *stime = clap_get(parser, "time");
  const char *sfreq = clap_get(parser, "frequency");
  const char *ssamples = clap_get(parser, "samples");
  const char *cosine = clap_get(parser, "cos");

  float time = atof(stime);
  if (time <= 0) {
    fprintf(stderr, "Invalid time passed\n");
    clap_destroy(&parser);
    return 1;
  }
  float freq = atof(sfreq);
  if (freq <= 0) {
    fprintf(stderr, "Invalid frequency passed\n");
    clap_destroy(&parser);
    return 1;
  }
  int samples = atoi(ssamples);
  if (samples <= 0) {
    fprintf(stderr, "Invalid sample count passed\n");
    clap_destroy(&parser);
    return 1;
  }

  FILE *inf = stdin;
  if (strcmp(soutf, "stdin") != 0) {
    inf = fopen(sinf, "rb");
    if (!inf) {
      fprintf(stderr, "Invalid input file passed\n");
      clap_destroy(&parser);
      return 1;
    }
  }


  FILE *outf = stdout;
  if (strcmp(soutf, "stdout") != 0) {
    outf = fopen(soutf, "wb");
    if (!outf) {
      fprintf(stderr, "Invalid output file passed\n");
      clap_destroy(&parser);
      return 1;
    }
  }

  double t = 0.f;
  double v = 0.f;
  assert(fscanf(inf, "%lf\n", &v) == 1);
  double f = 2 * M_PI * freq;
  for (int i = 0; i < samples && fscanf(inf, "%lf\n", &v) == 1; ++i) {
    if (cosine) {
      v += cos(f * t);
    } else {
      v += sin(f * t);
    }
    fprintf(outf, "%f\n", v);
    t += time / samples;
  }
  
  clap_destroy(&parser);
  return 0;
}
