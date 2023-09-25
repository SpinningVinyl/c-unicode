#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include "unicode.h"

size_t utf8_rune_size(const uint8_t byte) {
	if ((byte & UTF8_ONE_BYTE_MASK) == UTF8_ONE_BYTE_BITS) {
		return 1;
	}

	if ((byte & UTF8_TWO_BYTES_MASK) == UTF8_TWO_BYTES_BITS) {
		return 2;
	}

	if ((byte & UTF8_THREE_BYTES_MASK) == UTF8_THREE_BYTES_BITS) {
		return 3;
	}

	if ((byte & UTF8_FOUR_BYTES_MASK) == UTF8_FOUR_BYTES_BITS) {
		return 4;
	}

	return 0;
}

size_t utf8_strlen(const uint8_t *utf8str) {

  size_t rune_size;
  size_t rune_count = 0;
  const uint8_t *p = utf8str;
  int panic = 0;
  
  while (*p != 0) {
    rune_size = utf8_rune_size(*p);

    if (0 == rune_size) {
      fprintf(stderr, "Invalid UTF-8 rune at byte %lu\n", p - utf8str);
      break;
    }

    // check multibyte UTF-8 runes for validity
    if (rune_size > 1) {
      for (unsigned int n = 1; n < rune_size; n++) {
        if (*(p + n) == 0) {
          fprintf(stderr, "Null terminator in the middle of a UTF-8 rune.\n");
          panic = 1;
	  break;
        } else if ((*(p + n) & UTF8_CONTINUATION_MASK) !=
                   UTF8_CONTINUATION_BITS) {
          fprintf(stderr, "Invalid byte in a UTF-8 rune.\n");
          panic = 1;
	  break;
	}
      }
    }

    if (panic) {
      break;
    }

    p += rune_size;
    rune_count += 1;
  } // end while

  return rune_count;
  
}


size_t utf8_decode(const uint8_t *in, uint32_t *out, size_t num_runes) {

  size_t inlen = utf8_strlen(in);
  size_t rune_size;
  size_t rune_count = 0;

  const uint8_t *p = in;
  uint32_t *q = out;

  /* test the output buffer */
  if (!out) {
    fprintf(stderr, "utf8_decode: Output buffer is NULL.\n");
    return 0;
  }

  while (rune_count <= inlen && (q - out) <= num_runes) {
    rune_size = utf8_rune_size(*p);
    if (1 == rune_size) {
      *q = ((uint32_t) (*p & ~UTF8_ONE_BYTE_MASK));
    } else if (2 == rune_size) {
      *q = ((uint32_t) (*p & ~UTF8_TWO_BYTES_MASK)) << 6 |
           ((uint32_t) *(p + 1) & ~UTF8_CONTINUATION_MASK);
    } else if (3 == rune_size) {
      *q = (((uint32_t) *p & ~UTF8_THREE_BYTES_MASK) << 12) |
	   (((uint32_t) (*(p + 1) & ~UTF8_CONTINUATION_MASK)) << 6) |
           ((uint32_t) (*(p + 2) & ~UTF8_CONTINUATION_MASK));
    } else if (4 == rune_size) {
      *q = ((uint32_t) *p & ~UTF8_FOUR_BYTES_MASK) << 18 |
           (((uint32_t) *(p + 1) & ~UTF8_CONTINUATION_MASK) << 12) |
           (((uint32_t) *(p + 2) & ~UTF8_CONTINUATION_MASK) << 6) |
           ((uint32_t) *(p + 3) & ~UTF8_CONTINUATION_MASK);
    }
    p += rune_size;
    q += 1;
    rune_count += 1;
  } // end while

  *q = '\0'; // append null character to the end of the output buffer
  return (q - out - 1);
}



size_t utf32_cp_bytesize(const uint32_t codepoint) {
  if (codepoint <= 0x7F) {
    return 1;
  } else if (codepoint <= 0x7FF) {
    return 2;
  } else if (codepoint <= 0xFFFF) {
    return 3;
  } else if (codepoint <= 0x10FFF) {
    return 4;
  }
  return 0;
}


size_t utf32_bytelen(const uint32_t *utf32str) {
  size_t byte_count = 0;
  int rune_size;
  const uint32_t *p = utf32str;

  while (*p != 0) {
    rune_size = utf32_cp_bytesize(*p);
    if (rune_size != 0) {
      byte_count += rune_size;
    } else {
      fprintf(stderr, "utf32_bytelen: Invalid UTF-32 codepoint at %lu.\n", p - utf32str);
      return 0;
    }
    p += 1;
  }

  return byte_count;
}

size_t utf32_cp_to_utf8_rune(uint32_t codepoint, uint8_t *out) {
  size_t rune_size = utf32_cp_bytesize(codepoint);
  if (0 == rune_size) {
    fprintf(stderr, "utf32_cp_to_utf8_rune: Invalid UTF-32 codepoint.\n");
    return 0;
  }
  if (1 == rune_size) {
    out[0] = codepoint;
  } else if (2 == rune_size) {
    out[0] = UTF8_TWO_BYTES_BITS | (codepoint >> 6);
    out[1] = UTF8_CONTINUATION_BITS | (codepoint & ~UTF8_CONTINUATION_MASK);
  } else if (3 == rune_size) {
    out[0] = UTF8_THREE_BYTES_BITS | (codepoint >> 12);
    out[1] =
      UTF8_CONTINUATION_BITS | ((codepoint >> 6) & ~UTF8_CONTINUATION_MASK);
    out[2] = UTF8_CONTINUATION_BITS | (codepoint & ~UTF8_CONTINUATION_MASK);
  } else if (4 == rune_size) {
    out[0] = UTF8_FOUR_BYTES_BITS | (codepoint >> 18);
    out[1] =
      UTF8_CONTINUATION_BITS | ((codepoint >> 12) & ~UTF8_CONTINUATION_MASK);
    out[2] =
      UTF8_CONTINUATION_BITS | ((codepoint >> 6) & ~UTF8_CONTINUATION_MASK);
    out[3] = UTF8_CONTINUATION_BITS | (codepoint & ~UTF8_CONTINUATION_MASK);
  }

  return rune_size;
}

size_t utf8_encode(const uint32_t *in, uint8_t *out, size_t bytelen) {

  size_t rune_size;
  int i;
  uint8_t cbuf[4] = {0};

  // test the output buffer
  if (!out) {
    fprintf(stderr, "utf8_encode: Output buffer is NULL.\n");
    return 0;
  }
    
  const uint32_t *p = in;
  uint8_t *q = out;

  while ((*p != '\0') && (q - out) <= bytelen) {
    rune_size = utf32_cp_to_utf8_rune(*p, cbuf);
    for (i = 0; i < rune_size; i++) {
      if ((q + i - out) <= bytelen) {
        *(q + i) = *(cbuf + i);
      } else {
        fprintf(stderr, "utf8_encode: Output buffer overflow.\n");
	return 0;
      }
    }
    q += rune_size;
    p += 1;
  } // end while

  *q = 0; // terminate the output
  return (q - out - 1);  
}
