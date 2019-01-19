/*?*************************************************************************
*                           Switch_StdLibExtras.h
*                           -----------------------
*    copyright            : (C) 2013 by Wouter Charle
*    email                : wouter.charle@gmail.com
*
*    DISCLAIMER OF DAMAGES
*    ---------------------
*    Wouter Charle has made every effort possible to ensure that the software
*    is free of any bugs or errors, however in no way is the software to
*    be considered error or bug free. You assume all responsibility for
*    any damages or lost data that may result from any errors or bugs in
*    the software.
*
*    IN NO EVENT WILL VISION++ BE LIABLE TO YOU FOR ANY GENERAL, SPECIAL,
*    INDIRECT, CONSEQUENTIAL, INCIDENTAL OR OTHER DAMAGES ARISING OUT OF
*    THIS LICENSE.
*
*    In no case shall Wouter Charle's liability exceed the purchase price for
*    the software or services.
*
***************************************************************************/

#ifndef SWITCH_STDLIBEXTRAS_H_INCLUDED
#define SWITCH_STDLIBEXTRAS_H_INCLUDED

#include "Switch_CompilerConfiguration.h"
#include "Switch_Debug.h"

/*
 * The reinterpret_cast<Type1*>([pointer to Type2]) expressions - where
 * sizeof(Type1) > sizeof(Type2) - cause the following warning on ARM with GCC:
 * increases required alignment of target type.
 *
 * An implicit or an extra static_cast<void*> bypasses the warning.
 * For more info see the following bugzilla entries:
 * - https://bugs.webkit.org/show_bug.cgi?id=38045
 * - http://gcc.gnu.org/bugzilla/show_bug.cgi?id=43976
 */
template<typename Type>
bool isPointerTypeAlignmentOkay (Type* ptr)
{
  return !(reinterpret_cast<intptr_t>(ptr) % __alignof__(Type));
}

template<typename TypePtr>
TypePtr reinterpret_cast_ptr (void* ptr)
{
  SWITCH_ASSERT (isPointerTypeAlignmentOkay(reinterpret_cast<TypePtr>(ptr)));
  return reinterpret_cast<TypePtr>(ptr);
}

template<typename TypePtr>
TypePtr reinterpret_cast_ptr (const void* ptr)
{
  SWITCH_ASSERT (isPointerTypeAlignmentOkay(reinterpret_cast<TypePtr>(ptr)));
  return reinterpret_cast<TypePtr>(ptr);
}

#endif // SWITCH_STDLIBEXTRAS_H_INCLUDED
