/**
  ******************************************************************************
  * @file    util.c
  * @brief   常用函数及printf重定向
  ******************************************************************************
  */
#include "util.h"
#include "usart.h"

/**
 * @brief converts arbitrary data *bb to an unterminated string *xp of n hexadecimal digits
 * 
 * @param xp 
 * @param bb 
 * @param n 
 */
void btox(char *xp, const unsigned char *bb, int n)
{
  const char xx[] = "0123456789ABCDEF";
  while (--n >= 0)
    xp[n] = xx[(bb[n >> 1] >> ((1 - (n & 1)) << 2)) & 0xF];
}
