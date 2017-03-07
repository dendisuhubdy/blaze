//=================================================================================================
/*!
//  \file blazetest/mathtest/customvector/AlignedUnpaddedTest.h
//  \brief Header file for the aligned/unpadded CustomVector class test
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

#ifndef _BLAZETEST_MATHTEST_CUSTOMVECTOR_ALIGNEDUNPADDEDTEST_H_
#define _BLAZETEST_MATHTEST_CUSTOMVECTOR_ALIGNEDUNPADDEDTEST_H_


//*************************************************************************************************
// Includes
//*************************************************************************************************

#include <sstream>
#include <stdexcept>
#include <string>
#include <blaze/math/constraints/ColumnVector.h>
#include <blaze/math/constraints/DenseVector.h>
#include <blaze/math/constraints/RequiresEvaluation.h>
#include <blaze/math/constraints/RowVector.h>
#include <blaze/math/CustomVector.h>
#include <blaze/util/constraints/SameType.h>
#include <blazetest/system/Types.h>


namespace blazetest {

namespace mathtest {

namespace customvector {

//=================================================================================================
//
//  CLASS DEFINITION
//
//=================================================================================================

//*************************************************************************************************
/*!\brief Auxiliary class for all tests of the aligned/unpadded CustomVector class template.
//
// This class represents a test suite for the specialization of the blaze::CustomVector class
// template for aligned and unpadded custom vectors. It performs a series of both compile time
// as well as runtime tests.
*/
class AlignedUnpaddedTest
{
 public:
   //**Constructors********************************************************************************
   /*!\name Constructors */
   //@{
   explicit AlignedUnpaddedTest();
   // No explicitly declared copy constructor.
   //@}
   //**********************************************************************************************

   //**Destructor**********************************************************************************
   // No explicitly declared destructor.
   //**********************************************************************************************

 private:
   //**Test functions******************************************************************************
   /*!\name Test functions */
   //@{
   void testConstructors();
   void testAssignment  ();
   void testAddAssign   ();
   void testSubAssign   ();
   void testMultAssign  ();
   void testDivAssign   ();
   void testCrossAssign ();
   void testScaling     ();
   void testSubscript   ();
   void testAt          ();
   void testIterator    ();
   void testNonZeros    ();
   void testReset       ();
   void testClear       ();
   void testSwap        ();
   void testIsDefault   ();

   template< typename Type >
   void checkSize( const Type& vector, size_t expectedSize ) const;

   template< typename Type >
   void checkCapacity( const Type& vector, size_t minCapacity ) const;

   template< typename Type >
   void checkNonZeros( const Type& vector, size_t nonzeros ) const;
   //@}
   //**********************************************************************************************

   //**Member variables****************************************************************************
   /*!\name Member variables */
   //@{
   std::string test_;  //!< Label of the currently performed test.
   //@}
   //**********************************************************************************************

   //**Type definitions****************************************************************************
   //! Type of the custom row vector.
   typedef blaze::CustomVector<int,blaze::aligned,blaze::unpadded,blaze::rowVector>  VT;

   //! Type of the custom column vector.
   typedef blaze::CustomVector<int,blaze::aligned,blaze::unpadded,blaze::columnVector>  TVT;

   typedef VT::Rebind<const double>::Other   RVT;   //!< Rebound custom row vector type.
   typedef TVT::Rebind<const double>::Other  TRVT;  //!< Rebound custom column vector type.
   //**********************************************************************************************

   //**Compile time checks*************************************************************************
   /*! \cond BLAZE_INTERNAL */
   BLAZE_CONSTRAINT_MUST_BE_DENSE_VECTOR_TYPE( VT                  );
   BLAZE_CONSTRAINT_MUST_BE_DENSE_VECTOR_TYPE( VT::ResultType      );
   BLAZE_CONSTRAINT_MUST_BE_DENSE_VECTOR_TYPE( VT::TransposeType   );
   BLAZE_CONSTRAINT_MUST_BE_DENSE_VECTOR_TYPE( TVT                 );
   BLAZE_CONSTRAINT_MUST_BE_DENSE_VECTOR_TYPE( TVT::ResultType     );
   BLAZE_CONSTRAINT_MUST_BE_DENSE_VECTOR_TYPE( TVT::TransposeType  );
   BLAZE_CONSTRAINT_MUST_BE_DENSE_VECTOR_TYPE( RVT                 );
   BLAZE_CONSTRAINT_MUST_BE_DENSE_VECTOR_TYPE( RVT::ResultType     );
   BLAZE_CONSTRAINT_MUST_BE_DENSE_VECTOR_TYPE( RVT::TransposeType  );
   BLAZE_CONSTRAINT_MUST_BE_DENSE_VECTOR_TYPE( TRVT                );
   BLAZE_CONSTRAINT_MUST_BE_DENSE_VECTOR_TYPE( TRVT::ResultType    );
   BLAZE_CONSTRAINT_MUST_BE_DENSE_VECTOR_TYPE( TRVT::TransposeType );

   BLAZE_CONSTRAINT_MUST_BE_ROW_VECTOR_TYPE   ( VT                  );
   BLAZE_CONSTRAINT_MUST_BE_ROW_VECTOR_TYPE   ( VT::ResultType      );
   BLAZE_CONSTRAINT_MUST_BE_COLUMN_VECTOR_TYPE( VT::TransposeType   );
   BLAZE_CONSTRAINT_MUST_BE_COLUMN_VECTOR_TYPE( TVT                 );
   BLAZE_CONSTRAINT_MUST_BE_COLUMN_VECTOR_TYPE( TVT::ResultType     );
   BLAZE_CONSTRAINT_MUST_BE_ROW_VECTOR_TYPE   ( TVT::TransposeType  );
   BLAZE_CONSTRAINT_MUST_BE_ROW_VECTOR_TYPE   ( RVT                 );
   BLAZE_CONSTRAINT_MUST_BE_ROW_VECTOR_TYPE   ( RVT::ResultType     );
   BLAZE_CONSTRAINT_MUST_BE_COLUMN_VECTOR_TYPE( RVT::TransposeType  );
   BLAZE_CONSTRAINT_MUST_BE_COLUMN_VECTOR_TYPE( TRVT                );
   BLAZE_CONSTRAINT_MUST_BE_COLUMN_VECTOR_TYPE( TRVT::ResultType    );
   BLAZE_CONSTRAINT_MUST_BE_ROW_VECTOR_TYPE   ( TRVT::TransposeType );

   BLAZE_CONSTRAINT_MUST_NOT_REQUIRE_EVALUATION( VT::ResultType      );
   BLAZE_CONSTRAINT_MUST_NOT_REQUIRE_EVALUATION( VT::TransposeType   );
   BLAZE_CONSTRAINT_MUST_NOT_REQUIRE_EVALUATION( TVT::ResultType     );
   BLAZE_CONSTRAINT_MUST_NOT_REQUIRE_EVALUATION( TVT::TransposeType  );
   BLAZE_CONSTRAINT_MUST_NOT_REQUIRE_EVALUATION( RVT::ResultType     );
   BLAZE_CONSTRAINT_MUST_NOT_REQUIRE_EVALUATION( RVT::TransposeType  );
   BLAZE_CONSTRAINT_MUST_NOT_REQUIRE_EVALUATION( TRVT::ResultType    );
   BLAZE_CONSTRAINT_MUST_NOT_REQUIRE_EVALUATION( TRVT::TransposeType );

   BLAZE_CONSTRAINT_MUST_BE_SAME_TYPE( VT::ElementType,   VT::ResultType::ElementType      );
   BLAZE_CONSTRAINT_MUST_BE_SAME_TYPE( VT::ElementType,   VT::TransposeType::ElementType   );
   BLAZE_CONSTRAINT_MUST_BE_SAME_TYPE( TVT::ElementType,  TVT::ResultType::ElementType     );
   BLAZE_CONSTRAINT_MUST_BE_SAME_TYPE( TVT::ElementType,  TVT::TransposeType::ElementType  );
   BLAZE_CONSTRAINT_MUST_BE_SAME_TYPE( RVT::ElementType,  RVT::ResultType::ElementType     );
   BLAZE_CONSTRAINT_MUST_BE_SAME_TYPE( RVT::ElementType,  RVT::TransposeType::ElementType  );
   BLAZE_CONSTRAINT_MUST_BE_SAME_TYPE( TRVT::ElementType, TRVT::ResultType::ElementType    );
   BLAZE_CONSTRAINT_MUST_BE_SAME_TYPE( TRVT::ElementType, TRVT::TransposeType::ElementType );
   /*! \endcond */
   //**********************************************************************************************
};
//*************************************************************************************************




//=================================================================================================
//
//  TEST FUNCTIONS
//
//=================================================================================================

//*************************************************************************************************
/*!\brief Checking the size of the given custom vector.
//
// \param vector The custom vector to be checked.
// \param expectedSize The expected size of the custom vector.
// \return void
// \exception std::runtime_error Error detected.
//
// This function checks the size of the given custom vector. In case the actual size
// does not correspond to the given expected size, a \a std::runtime_error exception is
// thrown.
*/
template< typename Type >  // Type of the custom vector
void AlignedUnpaddedTest::checkSize( const Type& vector, size_t expectedSize ) const
{
   if( size( vector ) != expectedSize ) {
      std::ostringstream oss;
      oss << " Test: " << test_ << "\n"
          << " Error: Invalid size detected\n"
          << " Details:\n"
          << "   Size         : " << size( vector ) << "\n"
          << "   Expected size: " << expectedSize << "\n";
      throw std::runtime_error( oss.str() );
   }
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Checking the capacity of the given custom vector.
//
// \param vector The custom vector to be checked.
// \param minCapacity The expected minimum capacity of the custom vector.
// \return void
// \exception std::runtime_error Error detected.
//
// This function checks the capacity of the given custom vector. In case the actual capacity
// is smaller than the given expected minimum capacity, a \a std::runtime_error exception is
// thrown.
*/
template< typename Type >  // Type of the custom vector
void AlignedUnpaddedTest::checkCapacity( const Type& vector, size_t minCapacity ) const
{
   if( capacity( vector ) < minCapacity ) {
      std::ostringstream oss;
      oss << " Test: " << test_ << "\n"
          << " Error: Invalid capacity detected\n"
          << " Details:\n"
          << "   Capacity                 : " << capacity( vector ) << "\n"
          << "   Expected minimum capacity: " << minCapacity << "\n";
      throw std::runtime_error( oss.str() );
   }
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Checking the number of non-zero elements of the given custom vector.
//
// \param vector The custom vector to be checked.
// \param expectedNonZeros The expected number of non-zero elements of the custom vector.
// \return void
// \exception std::runtime_error Error detected.
//
// This function checks the number of non-zero elements of the given custom vector. In
// case the actual number of non-zero elements does not correspond to the given expected
// number, a \a std::runtime_error exception is thrown.
*/
template< typename Type >  // Type of the custom vector
void AlignedUnpaddedTest::checkNonZeros( const Type& vector, size_t expectedNonZeros ) const
{
   if( nonZeros( vector ) != expectedNonZeros ) {
      std::ostringstream oss;
      oss << " Test: " << test_ << "\n"
          << " Error: Invalid number of non-zero elements\n"
          << " Details:\n"
          << "   Number of non-zeros         : " << nonZeros( vector ) << "\n"
          << "   Expected number of non-zeros: " << expectedNonZeros << "\n";
      throw std::runtime_error( oss.str() );
   }
}
//*************************************************************************************************




//=================================================================================================
//
//  GLOBAL TEST FUNCTIONS
//
//=================================================================================================

//*************************************************************************************************
/*!\brief Testing the functionality of the aligned/unpadded CustomVector class template.
//
// \return void
*/
void runTest()
{
   AlignedUnpaddedTest();
}
//*************************************************************************************************




//=================================================================================================
//
//  MACRO DEFINITIONS
//
//=================================================================================================

//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Macro for the execution of the aligned/unpadded CustomVector class test.
*/
#define RUN_CUSTOMVECTOR_ALIGNED_UNPADDED_TEST \
   blazetest::mathtest::customvector::runTest()
/*! \endcond */
//*************************************************************************************************

} // namespace customvector

} // namespace mathtest

} // namespace blazetest

#endif
