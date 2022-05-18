/*********************************************************************
 * \file   MemoryMacro.h
 * \brief  
 * 
 * \author Lancelot 'Robin' Chen
 * \date   May 2022
 *********************************************************************/
#ifndef _EN_MEMORY_MACRO_H
#define _EN_MEMORY_MACRO_H

#include "MemoryAllocMacro.h"

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