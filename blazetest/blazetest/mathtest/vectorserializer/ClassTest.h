//=================================================================================================
/*!
//  \file blazetest/mathtest/vectorserializer/ClassTest.h
//  \brief Header file for the VectorSerializer class test
//
//  Copyright (C) 2011 Klaus Iglberger - All Rights Reserved
//
//  This file is part of the Blaze library. This library is free software; you can redistribute
//  it and/or modify it under the terms of the GNU General Public License as published by the
//  Free Software Foundation; either version 3, or (at your option) any later version.
//
//  This library is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
//  without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
//  See the GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License along with a special
//  exception for linking and compiling against the Blaze library, the so-called "runtime
//  exception"; see the file COPYING. If not, see http://www.gnu.org/licenses/.
*/
//=================================================================================================

#ifndef _BLAZETEST_MATHTEST_VECTORSERIALIZER_CLASSTEST_H_
#define _BLAZETEST_MATHTEST_VECTORSERIALIZER_CLASSTEST_H_


//*************************************************************************************************
// Includes
//*************************************************************************************************

#include <sstream>
#include <stdexcept>
#include <string>
#include <blaze/math/CompressedVector.h>
#include <blaze/math/constraints/Vector.h>
#include <blaze/math/DynamicVector.h>
#include <blaze/math/StaticVector.h>
#include <blaze/math/typetraits/IsDenseVector.h>
#include <blaze/util/Random.h>
#include <blaze/util/serialization/Archive.h>


namespace blazetest {

namespace mathtest {

namespace vectorserializer {

//=================================================================================================
//
//  CLASS DEFINITION
//
//=================================================================================================

//*************************************************************************************************
/*!\brief Auxiliary class for all tests of the VectorSerializer class.
//
// This class represents a test suite for the blaze::VectorSerializer class. It performs a
// series of runtime tests with different vector types to test the serialization of both
// dense and sparse vectors.
*/
class ClassTest
{
 public:
   //**Constructors********************************************************************************
   /*!\name Constructors */
   //@{
   explicit ClassTest();
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
   void testEmptyVectors ();
   void testRandomVectors();
   void testFailures     ();

   template< size_t N, typename VT >
   void runAllTests( const VT& src );

   template< size_t N, typename VT >
   void runStaticVectorTests( const VT& src );

   template< typename VT >
   void runDynamicVectorTests( const VT& src );

   template< typename VT >
   void runCompressedVectorTests( const VT& src );

   template< typename VT1, typename VT2 >
   void runTest( const VT1& src, VT2& dst );

   template< typename Archive, typename VT >
   void testSerialization( Archive& archive, const VT& src );

   template< typename Archive, typename VT >
   void testDeserialization( Archive& archive, VT& dst );

   template< typename VT1, typename VT2 >
   void compareVectors( const VT1& src, const VT2& dst );
   //@}
   //**********************************************************************************************

   //**Member variables****************************************************************************
   /*!\name Member variables */
   //@{
   std::string test_;  //!< Label of the currently performed test.
   //@}
   //**********************************************************************************************
};
//*************************************************************************************************




//=================================================================================================
//
//  TEST FUNCTIONS
//
//=================================================================================================

//*************************************************************************************************
/*!\brief Execution of several (de-)serialization tests with the given source vector.
//
// \param src The source vector to be tested.
// \return void
// \exception std::runtime_error Error detected.
//
// This function tests the vector (de-)serialization with the given vector. The vector is
// serialized and deserialized several times, using instances of StaticVector, DynamicVector,
// and CompressedVector as destination vector type. In case an error is detected, a
// \a std::runtime_error exception is thrown.
*/
template< size_t N       // Size of the vector
        , typename VT >  // Type of the vector
void ClassTest::runAllTests( const VT& src )
{
   BLAZE_CONSTRAINT_MUST_BE_VECTOR_TYPE( VT );

   runStaticVectorTests<N> ( src );
   runDynamicVectorTests   ( src );
   runCompressedVectorTests( src );
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Execution of several (de-)serialization tests with the given source vector.
//
// \param src The source vector to be tested.
// \return void
// \exception std::runtime_error Error detected.
//
// This function tests the vector (de-)serialization with the given vector. The vector is
// serialized and deserialized several times, using instances of StaticVector as destination
// vector type. In case an error is detected, a \a std::runtime_error exception is thrown.
*/
template< size_t N       // Size of the vector
        , typename VT >  // Type of the vector
void ClassTest::runStaticVectorTests( const VT& src )
{
   BLAZE_CONSTRAINT_MUST_BE_VECTOR_TYPE( VT );

   typedef typename VT::ElementType  ET;

   {
      blaze::StaticVector<ET,N> dst;
      runTest( src, dst );
   }

   {
      blaze::StaticVector<ET,N> dst;
      randomize( dst );
      runTest( src, dst );
   }
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Execution of several (de-)serialization tests with the given source vector.
//
// \param src The source vector to be tested.
// \return void
// \exception std::runtime_error Error detected.
//
// This function tests the vector (de-)serialization with the given vector. The vector is
// serialized and deserialized several times, using instances of DynamicVector as destination
// vector type. In case an error is detected, a \a std::runtime_error exception is thrown.
*/
template< typename VT >  // Type of the vector
void ClassTest::runDynamicVectorTests( const VT& src )
{
   BLAZE_CONSTRAINT_MUST_BE_VECTOR_TYPE( VT );

   typedef typename VT::ElementType  ET;

   {
      blaze::DynamicVector<ET> dst;
      runTest( src, dst );
   }

   {
      blaze::DynamicVector<ET> dst( 43UL );
      randomize( dst );
      runTest( src, dst );
   }
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Execution of several (de-)serialization tests with the given source vector.
//
// \param src The source vector to be tested.
// \return void
// \exception std::runtime_error Error detected.
//
// This function tests the vector (de-)serialization with the given vector. The vector is
// serialized and deserialized several times, using instances of CompressedVector as destination
// vector type. In case an error is detected, a \a std::runtime_error exception is thrown.
*/
template< typename VT >  // Type of the vector
void ClassTest::runCompressedVectorTests( const VT& src )
{
   BLAZE_CONSTRAINT_MUST_BE_VECTOR_TYPE( VT );

   typedef typename VT::ElementType  ET;

   {
      blaze::CompressedVector<ET> dst;
      runTest( src, dst );
   }

   {
      blaze::CompressedVector<ET> dst( 43UL );
      randomize( dst );
      runTest( src, dst );
   }
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Running a single (de-)serialization test with the given pair of vectors.
//
// \param src The source vector to be serialized.
// \param dst The destination vector to be reconstituted.
// \return void
// \exception std::runtime_error Error detected.
//
// This function tests the (de-)serialization process with the given pair of vectors. The
// source vector is serialized and the destination vector is reconstituted from the resulting
// archive. In case an error is detected, a \a std::runtime_error exception is thrown.
*/
template< typename VT1    // Type of the source vector
        , typename VT2 >  // Type of the destination vector
void ClassTest::runTest( const VT1& src, VT2& dst )
{
   BLAZE_CONSTRAINT_MUST_BE_VECTOR_TYPE( VT1 );
   BLAZE_CONSTRAINT_MUST_BE_VECTOR_TYPE( VT2 );

   blaze::Archive<std::stringstream> archive;

   testSerialization  ( archive, src );
   testDeserialization( archive, dst );
   compareVectors     ( src, dst );
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Testing the serialization process.
//
// \param archive The archive to be written.
// \param src The source vector to be serialized.
// \return void
// \exception std::runtime_error Error detected.
//
// This function test the serialization process with the given archive and vector. In case
// an error is detected, a \a std::runtime_error exception is thrown.
*/
template< typename Archive  // Type of the archive
        , typename VT >     // Type of the vector
void ClassTest::testSerialization( Archive& archive, const VT& src )
{
   BLAZE_CONSTRAINT_MUST_BE_VECTOR_TYPE( VT );

   using blaze::IsDenseVector;

   try {
      archive << src;
   }
   catch( std::runtime_error& ex ) {
      std::ostringstream oss;
      oss << " Test: " << test_ << "\n"
          << " Error: Serialization failed\n"
          << " Details:\n"
          << "   " << ( IsDenseVector<VT>::value ? ( "Dense" ) : ( "Sparse" ) ) << " vector type:\n"
          << "     " << typeid( VT ).name() << "\n"
          << "   Vector:\n" << src << "\n"
          << "   Error message: " << ex.what() << "\n";
      throw std::runtime_error( oss.str() );
   }
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Testing the deserialization process.
//
// \param archive The archive to be read.
// \param dst The source vector to be reconstituted.
// \return void
// \exception std::runtime_error Error detected.
//
// This function test the deserialization process with the given archive and vector. In case
// an error is detected, a \a std::runtime_error exception is thrown.
*/
template< typename Archive  // Type of the archive
        , typename VT >     // Type of the vector
void ClassTest::testDeserialization( Archive& archive, VT& dst )
{
   BLAZE_CONSTRAINT_MUST_BE_VECTOR_TYPE( VT );

   using blaze::IsDenseVector;

   try {
      archive >> dst;
   }
   catch( std::runtime_error& ex )
   {
      std::ostringstream oss;
      oss << " Test: " << test_ << "\n"
          << " Error: Deserialization failed\n"
          << " Details:\n"
          << "   " << ( IsDenseVector<VT>::value ? ( "Dense" ) : ( "Sparse" ) ) << " vector type:\n"
          << "     " << typeid( VT ).name() << "\n"
          << "   Vector:\n" << dst << "\n"
          << "   Error message: " << ex.what() << "\n";
      throw std::runtime_error( oss.str() );
   }
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Comparison of a source and destination vector.
//
// \param src The source vector.
// \param dst The destination vector.
// \return void
// \exception std::runtime_error Error detected.
//
// This function performs a comparison between the given source and destination vector. In
// case the vector are not equal, a \a std::runtime_error exception is thrown.
*/
template< typename VT1    // Type of the source vector
        , typename VT2 >  // Type of the destination vector
void ClassTest::compareVectors( const VT1& src, const VT2& dst )
{
   BLAZE_CONSTRAINT_MUST_BE_VECTOR_TYPE( VT1 );
   BLAZE_CONSTRAINT_MUST_BE_VECTOR_TYPE( VT2 );

   using blaze::IsDenseVector;

   if( src != dst ) {
      std::ostringstream oss;
      oss << " Test: " << test_ << "\n"
          << " Error: Vector comparison failed\n"
          << " Details:\n"
          << "   " << ( IsDenseVector<VT1>::value ? ( "Dense" ) : ( "Sparse" ) ) << " source vector type:\n"
          << "     " << typeid( VT1 ).name() << "\n"
          << "   " << ( IsDenseVector<VT2>::value ? ( "Dense" ) : ( "Sparse" ) ) << " destination vector type:\n"
          << "     " << typeid( VT2 ).name() << "\n"
          << "   Source:\n" << src << "\n"
          << "   Destination:\n" << dst << "\n";
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
/*!\brief Testing the functionality of the VectorSerializer class.
//
// \return void
*/
void runTest()
{
   ClassTest();
}
//*************************************************************************************************




//=================================================================================================
//
//  MACRO DEFINITIONS
//
//=================================================================================================

//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Macro for the execution of the VectorSerializer class test.
*/
#define RUN_VECTORSERIALIZER_CLASS_TEST \
   blazetest::mathtest::vectorserializer::runTest()
/*! \endcond */
//*************************************************************************************************

} // namespace vectorserializer

} // namespace mathtest

} // namespace blazetest

#endif
