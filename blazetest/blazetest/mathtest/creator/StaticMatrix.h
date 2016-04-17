//=================================================================================================
/*!
//  \file blazetest/mathtest/creator/StaticMatrix.h
//  \brief Specialization of the Creator class template for StaticMatrix
//
//  Copyright (C) 2013 Klaus Iglberger - All Rights Reserved
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

#ifndef _BLAZETEST_MATHTEST_CREATOR_STATICMATRIX_H_
#define _BLAZETEST_MATHTEST_CREATOR_STATICMATRIX_H_


//*************************************************************************************************
// Includes
//*************************************************************************************************

#include <blaze/math/StaticMatrix.h>
#include <blazetest/mathtest/creator/Default.h>
#include <blazetest/system/Types.h>


namespace blazetest {

//=================================================================================================
//
//  CLASS DEFINITION
//
//=================================================================================================

//*************************************************************************************************
/*!\brief Specialization of the Creator class template for static matrices.
//
// This specialization of the Creator class template is able to create random static matrices.
*/
template< typename T     // Element type of the static matrix
        , size_t M       // Number of rows of the static matrix
        , size_t N       // Number of columns of the static matrix
        , bool SO        // Storage order of the static matrix
        , typename CP >  // Creation policy
class Creator< blaze::StaticMatrix<T,M,N,SO>, CP >
{
 public:
   //**Type definitions****************************************************************************
   typedef blaze::StaticMatrix<T,M,N,SO>  Type;    //!< Type to be created by the Creator.
   typedef CP                             Policy;  //!< Creation policy for the built-in elements.
   //**********************************************************************************************

   //**Constructors********************************************************************************
   /*!\name Constructors */
   //@{
   explicit inline Creator( const Creator<T,CP>& elementCreator = Creator<T,CP>() );
   // No explicitly declared copy constructor.
   //@}
   //**********************************************************************************************

   //**Destructor**********************************************************************************
   // No explicitly declared destructor.
   //**********************************************************************************************

   //**Operators***********************************************************************************
   /*!\name Operators */
   //@{
   // No explicitly declared copy assignment operator.
   const blaze::StaticMatrix<T,M,N,SO> operator()() const;
   //@}
   //**********************************************************************************************

 private:
   //**Member variables****************************************************************************
   /*!\name Member variables */
   //@{
   Creator<T,CP> ec_;  //!< Creator for the elements of the static matrix.
   //@}
   //**********************************************************************************************
};
//*************************************************************************************************




//=================================================================================================
//
//  CONSTRUCTORS
//
//=================================================================================================

//*************************************************************************************************
/*!\brief Constructor for the creator specialization for StaticMatrix.
//
// \param elementCreator The creator for the elements of the static matrix.
*/
template< typename T     // Element type of the static matrix
        , size_t M       // Number of rows of the static matrix
        , size_t N       // Number of columns of the static matrix
        , bool SO        // Storage order of the static matrix
        , typename CP >  // Creation policy
inline Creator< blaze::StaticMatrix<T,M,N,SO>, CP >::Creator( const Creator<T,CP>& elementCreator )
   : ec_( elementCreator )  // Creator for the elements of the static matrix
{}
//*************************************************************************************************




//=================================================================================================
//
//  OPERATORS
//
//=================================================================================================

//*************************************************************************************************
/*!\brief Returns a randomly created static matrix.
//
// \return The randomly generated static matrix.
*/
template< typename T     // Element type of the static matrix
        , size_t M       // Number of rows of the static matrix
        , size_t N       // Number of columns of the static matrix
        , bool SO        // Storage order of the static matrix
        , typename CP >  // Creation policy
inline const blaze::StaticMatrix<T,M,N,SO> Creator< blaze::StaticMatrix<T,M,N,SO>, CP >::operator()() const
{
   blaze::StaticMatrix<T,M,N,SO> matrix;

   // Initialization of a column-major matrix
   if( SO ) {
      for( size_t j=0UL; j<N; ++j )
         for( size_t i=0UL; i<M; ++i )
            matrix(i,j) = ec_();
   }

   // Initialization of a row-major matrix
   else {
      for( size_t i=0UL; i<M; ++i )
         for( size_t j=0UL; j<N; ++j )
            matrix(i,j) = ec_();
   }

   return matrix;
}
//*************************************************************************************************

} // namespace blazetest

#endif
