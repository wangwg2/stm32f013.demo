/**
  ******************************************************************************
  * @file    util.h
  * @brief   常用函数
  ******************************************************************************
  */
#ifndef __UTIL_H__
#define __UTIL_H__

#include "main.h"

#ifdef __cplusplus
extern "C"
{
#endif

// byte to char * (hex )
void btox(char *xp, const unsigned char *bb, int n);

#ifdef __cplusplus
}
#endif

#endif /* __UTIL_H__ */