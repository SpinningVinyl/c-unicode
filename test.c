#include <stdint.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include "unicode.h"

size_t read_line(uint8_t *out, size_t maxlen);

int main(void) {

  size_t utf32len, utf8len;

  uint8_t *utf8string;
  uint32_t *utf32string;

  uint8_t input[] = "охайо годзаимасу";
  
  
  utf32len = utf8_strlen(input);
  printf("Length of the UTF-8 string (in runes): %ld\n", utf32len);
  utf32string = malloc((utf32len + 1) * sizeof(uint32_t));
  utf8_decode(input, utf32string, utf32len);

  int i = 0;
  while (utf32string[i] != 0) {
    printf("%08X\n", utf32string[i]);
    i += 1;
  }

  utf8len = utf32_bytelen(utf32string);
  printf("Length of the UTF-32 string (in bytes after conversion): %ld\n", utf8len);
  utf8string = malloc((utf8len + 1) * sizeof(uint8_t));

  utf8_encode(utf32string, utf8string, utf8len);
  free(utf32string);

  printf("%s\n", utf8string);

  return 0;
}

size_t read_line(uint8_t *out, size_t maxlen) {
  size_t i = 0;
  int ch;
  while (ch != '\n' && ch != EOF) {
    if (i < maxlen) {
      out[i++] = ch;
    }
    ch = getchar();
  }
  out[i] = '\0';
  return i;

}
