//=================================================================================================
/*!
//  \file blazemark/blaze/init/DynamicStrictlyUpper.h
//  \brief Header file for the Blaze strictly upper dynamic matrix initialization functions
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

#ifndef _BLAZEMARK_BLAZE_INIT_DYNAMICSTRICTLYUPPER_H_
#define _BLAZEMARK_BLAZE_INIT_DYNAMICSTRICTLYUPPER_H_


//*************************************************************************************************
// Includes
//*************************************************************************************************

#include <vector>
#include <blaze/math/DynamicMatrix.h>
#include <blaze/math/StrictlyUpperMatrix.h>
#include <blaze/util/Random.h>
#include <blazemark/system/Types.h>


namespace blazemark {

namespace blaze {

//=================================================================================================
//
//  INITIALIZATION FUNCTIONS
//
//=================================================================================================

//*************************************************************************************************
/*!\name Blaze initialization functions */
//@{
template< typename Type >
void init( ::blaze::StrictlyUpperMatrix< ::blaze::DynamicMatrix<Type,::blaze::rowMajor> >& m );

template< typename Type >
void init( ::blaze::StrictlyUpperMatrix< ::blaze::DynamicMatrix<Type,::blaze::columnMajor> >& m );

template< typename Type, bool SO >
void init( ::std::vector< ::blaze::StrictlyUpperMatrix< ::blaze::DynamicMatrix<Type,SO> > >& v );
//@}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Random initialization of the given row-major strictly upper dynamic matrix.
//
// \param m The row-major strictly upper dynamic matrix to be initialized.
// \return void
//
// This function initializes the given row-major strictly upper dynamic matrix with random values.
*/
template< typename Type >  // Data type of the matrix
void init( ::blaze::StrictlyUpperMatrix< ::blaze::DynamicMatrix<Type,::blaze::rowMajor> >& m )
{
   const size_t N( m.rows() );

   for( size_t i=0UL; i<N; ++i ) {
      for( size_t j=i+1UL; j<N; ++j ) {
         m(i,j) = ::blaze::rand<Type>( 0, 10 );
      }
   }
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Random initialization of the given column-major strictly upper dynamic matrix.
//
// \param m The column-major strictly upper dynamic matrix to be initialized.
// \return void
//
// This function initializes the given column-major strictly upper dynamic matrix with random values.
*/
template< typename Type >  // Data type of the matrix
void init( ::blaze::StrictlyUpperMatrix< ::blaze::DynamicMatrix<Type,::blaze::columnMajor> >& m )
{
   const size_t N( m.columns() );

   for( size_t j=1UL; j<N; ++j ) {
      for( size_t i=0UL; i<j; ++i ) {
         m(i,j) = ::blaze::rand<Type>( 0, 10 );
      }
   }
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Random initialization of the given vector of strictly upper dynamic matrices.
//
// \param v The vector of strictly upper dynamic matrices to be initialized.
// \return void
//
// This function initializes all strictly upper dynamic matrices in the given vector with random
// values.
*/
template< typename Type  // Data type of the matrix
        , bool SO >      // Storage order
void init( ::std::vector< ::blaze::StrictlyUpperMatrix< ::blaze::DynamicMatrix<Type,SO> > >& v )
{
   const size_t size( v.size() );

   for( size_t i=0UL; i<size; ++i ) {
      init( v[i] );
   }
}
//*************************************************************************************************

} // namespace blaze

} // namespace blazemark

#endif
