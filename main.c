#include <stdio.h>
#include <stdlib.h>
#include <complex.h>
#include <math.h>

#include "clap.h"
#define NDEBUG
#define VEC_IMPL
#include "vec.h"

typedef float complex cf32;
typedef double complex cf64;

// assumes the lengths of the 2 arrays are equal
// stolen from rosetta code :)
void _fft_impl(cf64 *b1, cf64 *b2, size_t n, size_t step) {
  if (step < n) {
    _fft_impl(b2, b1, n, step * 2);
    _fft_impl(b2 + step, b1 + step, n, step * 2);
    for (size_t i = 0; i < n; i += 2 * step) {
      cf64 t = cexp(-I * M_PI * i / n) * b2[i + step];
      b1[i / 2]     = b2[i] + t;
      b1[(i + n)/2] = b2[i] - t;
    }
  }
}

// accepts a view of doubles
// returns an array of cf32
arr_t fft(view_t in_arr) {
  arr_t inter = arr_new(cf64, in_arr.len);
  arr_t res   = arr_new(cf64, in_arr.len);
  for(size_t i = 0; i < in_arr.len; ++i) {
    *arr_ref(cf64, res, i)   = arr_at(cf64, in_arr, i);
    *arr_ref(cf64, inter, i) = arr_at(cf64, in_arr, i);
  }
  _fft_impl((cf64 *)res.data, (cf64 *)inter.data, in_arr.len, 1);
  arr_free(inter);
  return res; 
}

bool check_pow_of_2(size_t n) {
  return (n != 0) && ((n & (n - 1)) == 0);
}

// pad the vec of doubles to a power of 2 length
void pad(vec_t* in) {
  while(!check_pow_of_2(in->len)) {
    vec_push(cf64, *in, 0);
  }
}

int main(int argc, char **argv) {
  clap_parser parser = clap_parser_init(argc, argv, (clap_parser_opts) {
      .desc = "Takes an input and ffts it"
    });
  clap_arg_add(&parser, (clap_arg) {
      .name = "input",
      .alias = 'i',
      .description = "input file - expected a list of floats separated by newlines",
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
      .name = "dims",
      .alias = 'd',
      .description = "dimensions of input (1 or 2)",
      .default_value = "1",
      .options = CLAP_ARG_OPTIONAL
    });
  clap_arg_add(&parser, (clap_arg) {
      .name = "row-wise",
      .alias = 'r',
      .description = "(2d only) whether to go row-wise or column-wise",
      .default_value = CLAP_FLAG_FALSE,
      .options = CLAP_ARG_FLAG | CLAP_ARG_OPTIONAL
    });
  clap_arg_add(&parser, (clap_arg) {
      .name = "complex",
      .alias = 'c',
      .description = "indicate that the input is complex numbers not floats",
      .default_value = CLAP_FLAG_FALSE,
      .options = CLAP_ARG_FLAG | CLAP_ARG_OPTIONAL
    });
  clap_arg_add(&parser, (clap_arg) {
      .name = "graph",
      .alias = 'g',
      .description = "output data for display on a graph",
      .default_value = CLAP_FLAG_FALSE,
      .options = CLAP_ARG_FLAG | CLAP_ARG_OPTIONAL
    });
  clap_arg_add(&parser, (clap_arg) {
      .name = "backwards",
      .alias = 'b',
      .description = "perform the inverse fourier transform (divide by N)",
      .default_value = CLAP_FLAG_FALSE,
      .options = CLAP_ARG_FLAG | CLAP_ARG_OPTIONAL
    });

  if (!clap_parse(&parser)) {
    clap_print_err(parser);
    clap_destroy(&parser);
    return 1;
  }
  const char *sinf  = clap_get(parser, "input");
  const char *soutf = clap_get(parser, "output");
  const char *sdims = clap_get(parser, "dims");
  const char *sroww = clap_get(parser, "row-wise");
  const char *is_complex = clap_get(parser, "complex");
  const char *graph = clap_get(parser, "graph");
  const char *backwards = clap_get(parser, "backwards");
  
  if (strcmp(sdims, "2") == 0) {
    // in data is 2D
    printf("TODO: Unimplemented\n");
    return 1;
  }
  
  FILE *inf = stdin;
  if (strcmp(sinf, "stdin") != 0) {
    inf = fopen(sinf, "rb");
    if (!inf) {
      fprintf(stderr, "Invalid input file passed\n");
      clap_destroy(&parser);
      return 1;
    }
  }
  
  FILE *outf = stdout;
  if (strcmp(soutf, "stdout") != 0) {
    outf = fopen(soutf, "rb");
    if (!outf) {
      fprintf(stderr, "Invalid output file passed\n");
      clap_destroy(&parser);
      return 1;
    }
  }
  (void)sroww;
  (void)sdims;

  double r, i = 0;
  vec_t in_data = vec_new(cf64, 64);
  if (is_complex != CLAP_FLAG_FALSE) {
    while(fscanf(inf, "%lf %lf", &r, &i) == 2) {
      vec_push(cf64, in_data, r + i * I);
    }
  } else {
    while(fscanf(inf, "%lf\n", &r) == 1) {
      vec_push(cf64, in_data, r + i * I);
    }
  }
  pad(&in_data);

  if (backwards) {
    arr_for_each(cf64, in_data, it) {
      *it = conj(*it);
    }
  }

  arr_t out = fft(in_data.slice);
  size_t idx = 0;
  if (backwards) {
    arr_for_each(cf64, out, it) {
      *it /= out.len;
    }
  }
  arr_for_each(cf64, out, it) {
    if (graph) {
      printf("%zu %f\n", idx, creal(*it));
      idx += 1;
    } else {
      printf("%lf %lf\n", creal(*it), cimag(*it));
    }
  }

  vec_free(in_data);
  arr_free(out);
  clap_destroy(&parser);
  return 0;
}
