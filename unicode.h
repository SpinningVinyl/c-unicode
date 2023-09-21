#ifndef PRSV_UNICODE_H
#define PRSV_UNICODE_H

#include <stdlib.h>
#include <stdint.h>

static const uint32_t UTF8_ONE_BYTE_MASK = 0x80;
static const uint32_t UTF8_ONE_BYTE_BITS = 0;
static const uint32_t UTF8_TWO_BYTES_MASK = 0xE0;
static const uint32_t UTF8_TWO_BYTES_BITS = 0xC0;
static const uint32_t UTF8_THREE_BYTES_MASK = 0xF0;
static const uint32_t UTF8_THREE_BYTES_BITS = 0xE0;
static const uint32_t UTF8_FOUR_BYTES_MASK = 0xF8;
static const uint32_t UTF8_FOUR_BYTES_BITS = 0xF0;
static const uint32_t UTF8_CONTINUATION_MASK = 0xC0;
static const uint32_t UTF8_CONTINUATION_BITS = 0x80;

/***************************************************************************
 * utf8_rune_size(): Returns the size of a UTF-8 rune (in bytes).          *
 ***************************************************************************/
size_t utf8_rune_size(const uint8_t byte);


/***************************************************************************
 * utf8_strlen(): Returns the length of a UTF-8 string (in codepoints, not *
 *                in bytes). The string should be null-terminated.         *
 ***************************************************************************/
size_t utf8_strlen(const uint8_t *utf8str);


/***************************************************************************
 * utf32_bytelen(): Returns the number of bytes a UTF-32 string would take *
 *                  after conversion to UTF-8. The supplied string should  *
 *                  be null-terminated.                                    *
 ***************************************************************************/
size_t utf32_bytelen(const uint32_t *utf32str);


/***************************************************************************
 * utf8_decode():   Converts a null-terminated UTF-8 string to an array of *
 *                  UTF-32 codepoints. Writes the output to `out'. The     *
 *                  maximum length of the output is equal to `num_runes'.  *
 *                  The size of the output buffer should be at least       *
 *                  (num_runes + 1). The function returns the number of    *
 *                  successfully processed UTF-8 runes.                    *
 ***************************************************************************/
size_t utf8_decode(const uint8_t *in, uint32_t *out, size_t num_runes);


/***************************************************************************
 * utf8_encode():   Converts a null-terminated UTF-32 string to a UTF-8    *
 *                  string. Writes the output to `out'. The maximum length *
 *                  of the output is equal to `byte_len'. The size of the  *
 *                  output buffer should be at least (byte_len + 1). The   *
 *                  function returns the number of bytes written to `out'. *
 ***************************************************************************/
size_t utf8_encode(const uint32_t *in, uint8_t *out, size_t bytelen);


#endif /* PRSV_UNICODE_H */
