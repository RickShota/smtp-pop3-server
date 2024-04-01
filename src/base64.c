/**
 * @file base64.c
 * @author 黄瑞
 * @date 2024.3.21
 * @details base64编码解码源文件
*/
#include "base64.h"

unsigned int base64_alphabet[] = {
'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I',
'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R',
'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', 'a',
'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j',
'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's',
't', 'u', 'v', 'w', 'x', 'y', 'z', '0', '1',
'2', '3', '4', '5', '6', '7', '8', '9', '+',
'/' };

unsigned int base64_suffix_map[256] = {
     255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
     255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
     255, 255, 255,  62, 255, 255, 255,  63,  52,  53,  54,  55,  56,  57,  58,  59,  60,  61, 255, 255,
     255, 255, 255, 255, 255,  0,   1,    2,   3,   4,   5,   6,   7,   8,   9,  10,  11,  12,  13,  14,
     15,   16,  17,  18,  19,  20,  21,  22,  23,  24,  25, 255, 255, 255, 255, 255, 255,  26,  27,  28,
     29,   30,  31,  32,  33,  34,  35,  36,  37,  38,  39,  40,  41,  42,  43,  44,  45,  46,  47,  48,
     49,   50,  51, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
     255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
     255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
     255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
     255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
     255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
     255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255 };


static int c_move_bits(unsigned char src, unsigned l_num, unsigned r_num) {
  src <<= l_num;   src >>= r_num;
  return src;
}


char *base64_encode(const char *data) {
  char *ret, *ret_pos;
  int m, pad_num = 0, retsize, dlen = strlen(data);

  if(dlen == 0) return NULL;

  /* Account the result buffer size and alloc the memory for it. */
  if((dlen % 3) != 0)
    pad_num = 3 - dlen % 3;
  retsize = (dlen + pad_num) + ((dlen + pad_num) * 1 / 3) + 1;
  if((ret = malloc(retsize)) == NULL)
    return NULL;
  ret_pos = ret;

  /* Starting to convert the originality characters to BASE64 characters.
     Converting process keep to 4->6 principle. */
  for(m = 0; m < (dlen + pad_num); m += 3) {
    /* When data is not suffice 24 bits then pad 0 and the empty place pad '='. */
    *(ret_pos) = base64_alphabet[c_move_bits(*data, 0, 2)];
    if(m == dlen + pad_num - 3 && pad_num != 0) {  /* Whether the last bits-group suffice 24 bits. */
      if(pad_num == 1) {   /* 16bit need pad one '='. */
        *(ret_pos + 1) = base64_alphabet[c_move_bits(*data, 6, 2) + c_move_bits(*(data + 1), 0, 4)];
        *(ret_pos + 2) = base64_alphabet[c_move_bits(*(data + 1), 4, 2)];
        *(ret_pos + 3) = BASE64_PAD64;
      } else if(pad_num == 2) { /* 8bit need pad two'='. */
        *(ret_pos + 1) = base64_alphabet[c_move_bits(*data, 6, 2)];
        *(ret_pos + 2) = BASE64_PAD64;
        *(ret_pos + 3) = BASE64_PAD64;
      }
    } else {  /* 24bit normal. */
      *(ret_pos + 1) = base64_alphabet[c_move_bits(*data, 6, 2) + c_move_bits(*(data + 1), 0, 4)];
      *(ret_pos + 2) = base64_alphabet[c_move_bits(*(data + 1), 4, 2) + c_move_bits(*(data + 2), 0, 6)];
      *(ret_pos + 3) = base64_alphabet[*(data + 2) & 0x3f];
    }

    ret_pos += 4;
    data += 3;
  }

  ret[retsize - 1] = 0;
  return ret;
}


char *base64_decode(const char *bdata) {
  char *ret = NULL, *ret_pos;
  int m, pad_num = 0, retsize, bd_len = strlen(bdata);

  if(bd_len == 0) return NULL;
  if(bd_len % 4 != 0) return NULL;

  /* Whether the data have invalid base-64 characters? */
  for(m = 0; m < bd_len; ++m) {
    if(bdata[m] != BASE64_PAD64 && base64_suffix_map[(int)(bdata[m])] == 255)
      goto LEND;
  }

  /* Account the output size. */
  if(bdata[bd_len - 1] == '=')  pad_num = 1;
  if(bdata[bd_len - 1] == '=' && bdata[bd_len - 2] == '=') pad_num = 2;
  retsize = (bd_len - 4) - (bd_len - 4) / 4 + (3 - pad_num) + 1;
  ret = malloc(retsize);
  if(ret == NULL)
    return NULL;
  ret_pos = ret;

  /* Begging to decode. */
  for(m = 0; m < bd_len; m += 4) {
    *ret_pos = c_move_bits(base64_suffix_map[(int)(*bdata)], 2, 0) + c_move_bits(base64_suffix_map[(int)(*(bdata + 1))], 0, 4);
    if(m == bd_len - 4 && pad_num != 0) {  /* Only deal with last four bits. */
      if(pad_num == 1)   /* Have one pad characters, only two availability characters. */
        *(ret_pos + 1) = c_move_bits(base64_suffix_map[(int)(*(bdata + 1))], 4, 0) + c_move_bits(base64_suffix_map[(int)(*(bdata + 2))], 0, 2);
      /*
      Have two pad characters, only two availability characters.
      if(pad_num == 2) {
      }
      */
      ret_pos += 3 - pad_num;
    } else {
      *(ret_pos + 1) = c_move_bits(base64_suffix_map[(int)(*(bdata + 1))], 4, 0) + c_move_bits(base64_suffix_map[(int)(*(bdata + 2))], 0, 2);
      *(ret_pos + 2) = c_move_bits(base64_suffix_map[(int)(*(bdata + 2))], 6, 0) + base64_suffix_map[(int)(*(bdata + 3))];
      ret_pos += 3;
    }
    bdata += 4;
  }

  ret[retsize - 1] = 0;

LEND: return ret;
}
