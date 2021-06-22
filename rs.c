#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <correct.h>


#define N  ((1U << 8) - 1)

void printf_buf(const uint8_t * buf, const uint32_t len)
{
  uint32_t i = 0;
  for (i = 0; i < len; i++) {
    printf("%02X", buf[i]);
  }
  printf("\n");
}

int main(int argc, char ** argv)
{
  const uint32_t nr_roots = 32, prim = 11, fcr = 121;
  const uint32_t poly_f[] = {0, 1, 2, 7, 8};
  const uint32_t K = N - nr_roots;
  uint8_t buf[N], out[N];
  uint16_t poly = 0;
  uint32_t i = 0;
  ssize_t ret = -1;
  int exit_code = EXIT_FAILURE;
  correct_reed_solomon * rs = NULL;

  for (i = 0; i < sizeof(poly_f)/sizeof(poly_f[0]); i++) {
    poly |= (1U << poly_f[i]);
  }

  printf("poly     = 0x%X\n", poly);
  printf("fcr      = %d\n", fcr);
  printf("prim     = %d\n", prim);
  printf("nr_roots = %d\n", nr_roots);

  rs = correct_reed_solomon_create(poly, fcr, prim, nr_roots);
  if (NULL == rs) {
    fprintf(stderr, "ERROR: Unable to initialize RS codec!\n");
    return exit_code;
  }
  memset(buf, 0, N);
  memset(out, 0, N);

  if (argc == 2) {
    if (strlen(argv[1]) > K) {
      fprintf(stderr, "ERROR: Maximum message length is %d\n", K);
      goto cleanup;
    }
    memcpy(buf, argv[1], strlen(argv[1]));
    ret = correct_reed_solomon_encode(rs, buf, K, out);
    if (ret < 0) {
      fprintf(stderr, "ERROR: RS encoding error!\n");
      goto cleanup;
    }
    printf("%s\n", buf);
    printf_buf(out + K, nr_roots);
  } else if (argc == 3) {
    if (strlen(argv[1]) > K) {
      fprintf(stderr, "ERROR: Maximum message length is %d\n", K);
      goto cleanup;
    }
    memcpy(buf, argv[1], strlen(argv[1]));
    if (strlen(argv[2]) != (2 * nr_roots)) {
      fprintf(stderr, "ERROR: Number of parity bytes doesn't match %d\n",
          nr_roots);
      goto cleanup;
    }
    for (i = 0; i < nr_roots; i++) {
      unsigned int v;
      sscanf(&argv[2][i*2], "%02X", &v);
      buf[K + i] = (uint8_t) v;
    }
    ret = correct_reed_solomon_decode(rs, buf, N, out);
    if (ret < 0) {
      fprintf(stderr, "ERROR: RS decoding error!\n");
      goto cleanup;
    }
    printf("%s\n", out);
  } else {
    fprintf(stderr, "Usage:\n%s \"message\" [parity_bytes]\n", argv[0]);
    fprintf(stderr, "  message: message to be encoded/decoded\n");
    fprintf(stderr, "  parity_bytes: parity used for decoding the message\n");
    goto cleanup;
  }

  exit_code = EXIT_SUCCESS;

cleanup:
  correct_reed_solomon_destroy(rs);
  rs = NULL;
  return exit_code;
}
