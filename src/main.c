/*
    Copyright (c) 2017 Jean THOMAS.

    Permission is hereby granted, free of charge, to any person obtaining
    a copy of this software and associated documentation files (the "Software"),
    to deal in the Software without restriction, including without limitation
    the rights to use, copy, modify, merge, publish, distribute, sublicense,
    and/or sell copies of the Software, and to permit persons to whom the Software
    is furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in
    all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
    EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
    OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
    IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
    CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
    TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE
    OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <getopt.h>
#include <errno.h>
#include <ctype.h>

enum swap_type {
  SWAP16,
  SWAP32
};

uint16_t bswap16(uint16_t value) {
  uint8_t byte1, byte2;

  byte1 = value & 0xFF;
  byte2 = (value >> 8) & 0xFF;

  return byte1 << 8 | byte2;
}

uint32_t bswap32(uint32_t value) {
  uint16_t word1, word2;

  word1 = value & 0xFFFF;
  word2 = (value >> 16) & 0xFFFF;

  return bswap16(word1) << 16 | bswap16(word2);
}

int swapFile(char *input_file, char *output_file, enum swap_type swap_type) {
  FILE *input_fh = NULL, *output_fh = NULL;
  uint32_t current_dword;
  uint16_t current_word;
  int file_size, ret;

  input_fh = fopen(input_file, "rb");
  if (!input_fh) {
    goto file_error;
  }

  output_fh = fopen(output_file, "wb");
  if (!output_fh) {
    goto file_error;
  }

  /* Get file length using portable libc code */
  fseek(input_fh, 0, SEEK_END);
  file_size = fseek(input_fh, 0, SEEK_END);
  rewind(input_fh);

  /* Check if the file length is correct */
  if (swap_type == SWAP32 && file_size % 4 != 0) {
    goto length_mult_error;
  } else if (swap_type == SWAP16 && file_size % 2 != 0) {
    goto length_mult_error;
  }

  while (!feof(input_fh)) {
    if (swap_type == SWAP32) {
      ret = fread(&current_dword, sizeof(uint32_t), 1, input_fh);
      if (ret != 4) {
	goto rw_error;
      }

      current_dword = bswap32(current_dword);
      
      ret = fwrite(&current_dword, sizeof(uint32_t), 1, output_fh);
      if (ret != 4) {
	goto rw_error;
      }
    } else if (swap_type == SWAP16) {
      ret = fread(&current_word, sizeof(uint16_t), 1, input_fh);
      if (ret != 2) {
	goto rw_error;
      }

      current_word = bswap16(current_word);

      ret = fwrite(&current_word, sizeof(uint16_t), 1, output_fh);
      if (ret != 2) {
	goto rw_error;
      }
    }
  }

  fclose(input_fh);
  fclose(output_fh);

  return 0;

 file_error:
  perror("Fatal error");
  if (input_fh) {
    fclose(input_fh);
  }
  if (output_fh) {
    fclose(output_fh);
  }
  return -1;
  
 length_mult_error:
  fprintf(stderr, "File length is not a multiple of 2 (16 bits endianness swap)"
	  "or 4 (32 bits endianness swap). Please add NULL bytes using a"
	  "hexadecimal editor.\n");
  fclose(input_fh);
  fclose(output_fh);
  return -2;
  
 rw_error:
  fprintf(stderr, "Read/write error\n");
  fclose(input_fh);
  fclose(output_fh);
  return -3;
}

int main(int argc, char *argv[]) {
  enum swap_type swap_type = SWAP32;
  char c;

  while ((c = getopt(argc, argv, "n:")) != -1) {
    switch (c) {
    case 'n':
      if (strcmp(optarg, "32") == 0) {
	swap_type = SWAP32;
      } else if (strcmp(optarg, "16") == 0) {
	swap_type = SWAP16;
      } else {
	fprintf(stderr, "Wrong swap type !\n");
	return EXIT_FAILURE;
      }
      break;
    case '?':
      if (optopt == 'n') {
	fprintf (stderr, "Option -%c requires an argument.\n", optopt);
      } else if (isprint(optopt)) {
	fprintf (stderr, "Unknown option `-%c'.\n", optopt);
      } else {
	fprintf (stderr, "Unknown option character `\\x%x'.\n", optopt);
      }
      return EXIT_FAILURE;
      break;
    default:
      abort();
    }
  }

  if (argc - optind != 2) {
    printf("Usage: endianswap [-n 16/32] input.bin output.bin\n");
    return EXIT_SUCCESS;
  }

  swapFile(argv[optind], argv[optind+1], swap_type);

  return EXIT_SUCCESS;
}
