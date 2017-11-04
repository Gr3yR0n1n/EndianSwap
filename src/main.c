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

#define bswap_16(value) \
((((value) & 0xff) << 8) | ((value) >> 8))

#define bswap_32(value) \
(((uint32_t)bswap_16((uint16_t)((value) & 0xffff)) << 16) | \
(uint32_t)bswap_16((uint16_t)((value) >> 16)))

int main(int argc, char *argv[]) {
  FILE *inputFile, *outputFile;
  uint32_t currentDword;
  
  if (argc != 3) {
    printf("USAGE: endianswap [input file] [output file]\n");
    return EXIT_SUCCESS;
  }

  inputFile = fopen(argv[1], "rb");
  if (inputFile == NULL) {
    fprintf(stderr, "Error: Opening input file %s has failed.", argv[1]);
    return EXIT_FAILURE;
  }

  outputFile = fopen(argv[2], "wb");
  if (outputFile == NULL) {
    fprintf(stderr, "Error: Creating output file %s has failed.", argv[2]);
    return EXIT_FAILURE;
  }

  while (!feof(inputFile)) {
    fread(&currentDword, sizeof(uint32_t), 1, inputFile);
    if (ferror(inputFile)) {
      fprintf(stderr, "Error: Reading input file %s has failed.", argv[1]);
      return EXIT_FAILURE;
    }

    currentDword = bswap_32(currentDword);
    
    fwrite(&currentDword, sizeof(uint32_t), 1, outputFile);
    if (ferror(outputFile)) {
      fprintf(stderr, "Error: Writing to output file %s has failed.", argv[2]);
      return EXIT_FAILURE;
    }
  }

  fclose(inputFile);
  fclose(outputFile);
  
  return EXIT_SUCCESS;
}
