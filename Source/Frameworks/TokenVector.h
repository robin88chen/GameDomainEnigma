/*********************************************************************
 * \file   TokenVector.h
 * \brief  
 * 
 * \author Lancelot 'Robin' Chen
 * \date   July 2022
 *********************************************************************/
#ifndef TOKEN_VECTOR_H
#define TOKEN_VECTOR_H

#include <string>
#include <vector>

 /** @file TokenVector.h */

 /** @addtogroup type_define Type Define */
 //@{
 /** sting token vector, 字串分解token之後存放成一個vector */
using TokenVector = std::vector<std::string>;
//@}

/** @addtogroup utility_func Utility Functions */
//@{
/** 將字串分解成token
  @param src 來源字串
  @param sep 分解記號 */
extern TokenVector En_Split_Token(const std::string& src, const std::string& sep);

#endif // TOKEN_VECTOR_H
