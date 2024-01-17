/*********************************************************************
 * \file   MemoryMacro.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   May 2022
 *********************************************************************/
#ifndef _EN_MEMORY_MACRO_H
#define _EN_MEMORY_MACRO_H

#include "PlatformConfig.h"

#if TARGET_PLATFORM == PLATFORM_WIN32
#include <new>

#if defined _DEBUG && defined _MSC_VER
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#endif
#if defined _DEBUG && defined _MSC_VER
#ifndef memalloc
#define memalloc(T, count)       ((T*)(_malloc_dbg(sizeof(T)*(count), _NORMAL_BLOCK, __FILE__, __LINE__)))
#define memalloc_p(s, filename, line) _malloc_dbg(s, _NORMAL_BLOCK, filename, line)
#endif
#ifndef merealloc
#define merealloc(p, s)   _realloc_dbg(p, s, _NORMAL_BLOCK, __FILE__, __LINE__)
#endif
#ifndef mefree
#define mefree(p)         _free_dbg(p, _NORMAL_BLOCK)
#endif
#ifndef menew
#define menew new(_NORMAL_BLOCK, __FILE__, __LINE__)
#endif
#ifndef medelete
#define medelete delete
#endif
#define meInitMemoryCheck() \
  _CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF)
#else
#ifndef memalloc
#define memalloc(T, count)  ((T*)(malloc(sizeof(T)*count)))
#define memalloc_p(s, filename, line) malloc(s)
#endif
#ifndef merealloc
#define merealloc realloc
#endif
#ifndef mefree
#define mefree    free
#endif
#ifndef menew
#define menew new
#endif
#ifndef medelete
#define medelete delete
#endif
#define meInitMemoryCheck() (0L)
#endif
#elif TARGET_PLATFORM == PLATFORM_ANDROID
#ifndef memalloc
#define memalloc(T, count)  ((T*)(malloc(sizeof(T)*count)))
#define memalloc_p(s, filename, line) malloc(s)
#endif
#ifndef merealloc
#define merealloc realloc
#endif
#ifndef mefree
#define mefree    free
#endif
#ifndef menew
#define menew new
#endif
#ifndef medelete
#define medelete delete
#endif
#define meInitMemoryCheck() (0L)
#endif

#ifndef SAFE_DELETE
#define SAFE_DELETE(p)       { if(p) { medelete (p);     (p) = nullptr; } }
#endif

#ifndef SAFE_DELETE_ARRAY
#define SAFE_DELETE_ARRAY(p) { if(p) { medelete[] (p);   (p) = nullptr; } }
#endif

#ifndef SAFE_RELEASE
#define SAFE_RELEASE(p)      { if(p) { (p)->Release(); (p) = nullptr; } }
#endif

#ifndef SAFE_FREE
#define SAFE_FREE(p)       { if(p) { mefree(p);     (p) = nullptr; } }
#endif

#endif