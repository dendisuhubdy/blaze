//=================================================================================================
/*!
//  \file blaze/util/typetraits/AddReference.h
//  \brief Header file for the AddReference type trait
//
//  Copyright (C) 2012-2019 Klaus Iglberger - All Rights Reserved
//
//  This file is part of the Blaze library. You can redistribute it and/or modify it under
//  the terms of the New (Revised) BSD License. Redistribution and use in source and binary
//  forms, with or without modification, are permitted provided that the following conditions
//  are met:
//
//  1. Redistributions of source code must retain the above copyright notice, this list of
//     conditions and the following disclaimer.
//  2. Redistributions in binary form must reproduce the above copyright notice, this list
//     of conditions and the following disclaimer in the documentation and/or other materials
//     provided with the distribution.
//  3. Neither the names of the Blaze development group nor the names of its contributors
//     may be used to endorse or promote products derived from this software without specific
//     prior written permission.
//
//  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY
//  EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
//  OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT
//  SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
//  INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
//  TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
//  BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
//  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
//  ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH
//  DAMAGE.
*/
//=================================================================================================

#ifndef _BLAZE_UTIL_TYPETRAITS_ADDREFERENCE_H_
#define _BLAZE_UTIL_TYPETRAITS_ADDREFERENCE_H_


//*************************************************************************************************
// Includes
//*************************************************************************************************

#include <type_traits>


namespace blaze {

//=================================================================================================
//
//  CLASS DEFINITION
//
//=================================================================================================

//*************************************************************************************************
/*!\brief Addition of a top level reference.
// \ingroup type_traits
//
// In case the given type \a T is not a reference type, the AddReference type trait adds a top
// level reference to the given type \a T. Else the resulting type \a Type is \a T.

   \code
   blaze::AddReference<int>::Type         // Results in 'int&'
   blaze::AddReference<int const&>::Type  // Results in 'int const&'
   blaze::AddReference<int*>::Type        // Results in 'int*&'
   blaze::AddReference<int*&>::Type       // Results in 'int*&'
   \endcode
*/
template< typename T >
struct AddReference
{
 public:
   //**********************************************************************************************
   /*! \cond BLAZE_INTERNAL */
   using Type = typename std::add_lvalue_reference<T>::type;
   /*! \endcond */
   //**********************************************************************************************
};
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Auxiliary alias declaration for the AddReference type trait.
// \ingroup type_traits
//
// The AddReference_t alias declaration provides a convenient shortcut to access the nested \a Type
// of the AddReference class template. For instance, given the type \a T the following two type
// definitions are identical:

   \code
   using Type1 = typename blaze::AddReference<T>::Type;
   using Type2 = blaze::AddReference_t<T>;
   \endcode
*/
template< typename T >
using AddReference_t = typename AddReference<T>::Type;
//*************************************************************************************************

} // namespace blaze

#endif
