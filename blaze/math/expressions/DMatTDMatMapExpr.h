//=================================================================================================
/*!
//  \file blaze/math/expressions/DMatTDMatMapExpr.h
//  \brief Header file for the dense matrix/transpose dense matrix map expression
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

#ifndef _BLAZE_MATH_EXPRESSIONS_DMATTDMATMAPEXPR_H_
#define _BLAZE_MATH_EXPRESSIONS_DMATTDMATMAPEXPR_H_


//*************************************************************************************************
// Includes
//*************************************************************************************************

#include <utility>
#include <blaze/math/Aliases.h>
#include <blaze/math/constraints/ColumnMajorMatrix.h>
#include <blaze/math/constraints/DenseMatrix.h>
#include <blaze/math/constraints/RequiresEvaluation.h>
#include <blaze/math/constraints/RowMajorMatrix.h>
#include <blaze/math/constraints/StorageOrder.h>
#include <blaze/math/Exception.h>
#include <blaze/math/expressions/Computation.h>
#include <blaze/math/expressions/DenseMatrix.h>
#include <blaze/math/expressions/Forward.h>
#include <blaze/math/expressions/MatMatMapExpr.h>
#include <blaze/math/functors/Max.h>
#include <blaze/math/functors/Min.h>
#include <blaze/math/shims/Serial.h>
#include <blaze/math/SIMD.h>
#include <blaze/math/traits/BinaryMapExprTrait.h>
#include <blaze/math/traits/BinaryMapTrait.h>
#include <blaze/math/traits/ColumnExprTrait.h>
#include <blaze/math/traits/RowExprTrait.h>
#include <blaze/math/traits/SubmatrixExprTrait.h>
#include <blaze/math/typetraits/Columns.h>
#include <blaze/math/typetraits/IsAligned.h>
#include <blaze/math/typetraits/IsExpression.h>
#include <blaze/math/typetraits/IsHermitian.h>
#include <blaze/math/typetraits/IsLower.h>
#include <blaze/math/typetraits/IsPadded.h>
#include <blaze/math/typetraits/IsStrictlyLower.h>
#include <blaze/math/typetraits/IsStrictlyUpper.h>
#include <blaze/math/typetraits/IsSymmetric.h>
#include <blaze/math/typetraits/IsUniLower.h>
#include <blaze/math/typetraits/IsUniUpper.h>
#include <blaze/math/typetraits/IsUpper.h>
#include <blaze/math/typetraits/RequiresEvaluation.h>
#include <blaze/math/typetraits/Rows.h>
#include <blaze/system/Blocking.h>
#include <blaze/system/Inline.h>
#include <blaze/util/Assert.h>
#include <blaze/util/DisableIf.h>
#include <blaze/util/EnableIf.h>
#include <blaze/util/FunctionTrace.h>
#include <blaze/util/IntegralConstant.h>
#include <blaze/util/mpl/And.h>
#include <blaze/util/mpl/If.h>
#include <blaze/util/mpl/Maximum.h>
#include <blaze/util/Types.h>


namespace blaze {

//=================================================================================================
//
//  CLASS DMATTDMATMAPEXPR
//
//=================================================================================================

//*************************************************************************************************
/*!\brief Expression object for the dense matrix/tranpose dense matrix map() function.
// \ingroup dense_matrix_expression
//
// The DMatTDMatMapExpr class represents the compile time expression for the pairwise evaluation
// of a binary custom operation on the elements of a row-major dense matrix and a column-major
// matrix via the map() function.
*/
template< typename MT1   // Type of the left-hand side dense matrix
        , typename MT2   // Type of the right-hand side dense matrix
        , typename OP >  // Type of the custom operation
class DMatTDMatMapExpr : public DenseMatrix< DMatTDMatMapExpr<MT1,MT2,OP>, false >
                       , private MatMatMapExpr
                       , private Computation
{
 private:
   //**Type definitions****************************************************************************
   using RT1 = ResultType_<MT1>;     //!< Result type of the left-hand side dense matrix expression.
   using RT2 = ResultType_<MT2>;     //!< Result type of the right-hand side dense matrix expression.
   using ET1 = ElementType_<MT1>;    //!< Element type of the left-hand side dense matrix expression.
   using ET2 = ElementType_<MT2>;    //!< Element type of the right-hand side dense matrix expression.
   using RN1 = ReturnType_<MT1>;     //!< Return type of the left-hand side dense matrix expression.
   using RN2 = ReturnType_<MT2>;     //!< Return type of the right-hand side dense matrix expression.
   using CT1 = CompositeType_<MT1>;  //!< Composite type of the left-hand side dense matrix expression.
   using CT2 = CompositeType_<MT2>;  //!< Composite type of the right-hand side dense matrix expression.
   //**********************************************************************************************

   //**Serial evaluation strategy******************************************************************
   //! Compilation switch for the serial evaluation strategy of the map expression.
   /*! The \a useAssign compile time constant expression represents a compilation switch for
       the serial evaluation strategy of the map expression. In case either of the two dense
       matrix operands requires an intermediate evaluation, \a useAssign will be set to 1 and
       the addition expression will be evaluated via the \a assign function family. Otherwise
       \a useAssign will be set to 0 and the expression will be evaluated via the subscript
       operator. */
   enum : bool { useAssign = ( RequiresEvaluation<MT1>::value || RequiresEvaluation<MT2>::value ) };

   /*! \cond BLAZE_INTERNAL */
   //! Helper structure for the explicit application of the SFINAE principle.
   template< typename MT >
   struct UseAssign {
      enum : bool { value = useAssign };
   };
   /*! \endcond */
   //**********************************************************************************************

   //**Parallel evaluation strategy****************************************************************
   /*! \cond BLAZE_INTERNAL */
   //! Helper structure for the explicit application of the SFINAE principle.
   /*! The UseSMPAssign struct is a helper struct for the selection of the parallel evaluation
       strategy. In case at least one of the two dense matrix operands is not SMP assignable and
       at least one of the two operands requires an intermediate evaluation, \a value is set to 1
       and the expression specific evaluation strategy is selected. Otherwise \a value is set to
       0 and the default strategy is chosen. */
   template< typename MT >
   struct UseSMPAssign {
      enum : bool { value = ( !MT1::smpAssignable || !MT2::smpAssignable ) && useAssign };
   };
   /*! \endcond */
   //**********************************************************************************************

 public:
   //**Type definitions****************************************************************************
   using This          = DMatTDMatMapExpr<MT1,MT2,OP>;  //!< Type of this DMatTDMatMapExpr instance.
   using ResultType    = BinaryMapTrait_<RT1,RT2,OP>;   //!< Result type for expression template evaluations.
   using OppositeType  = OppositeType_<ResultType>;     //!< Result type with opposite storage order for expression template evaluations.
   using TransposeType = TransposeType_<ResultType>;    //!< Transpose type for expression template evaluations.
   using ElementType   = ElementType_<ResultType>;      //!< Resulting element type.

   //! Return type for expression template evaluations.
   using ReturnType = decltype( std::declval<OP>()( std::declval<RN1>(), std::declval<RN2>() ) );

   //! Data type for composite expression templates.
   using CompositeType = IfTrue_< useAssign, const ResultType, const DMatTDMatMapExpr& >;

   //! Composite type of the left-hand side dense matrix expression.
   using LeftOperand = If_< IsExpression<MT1>, const MT1, const MT1& >;

   //! Composite type of the right-hand side dense matrix expression.
   using RightOperand = If_< IsExpression<MT2>, const MT2, const MT2& >;

   //! Data type of the custom unary operation.
   using Operation = OP;

   //! Type for the assignment of the left-hand side dense matrix operand.
   using LT = If_< RequiresEvaluation<MT1>, const RT1, CT1 >;

   //! Type for the assignment of the right-hand side dense matrix operand.
   using RT = If_< RequiresEvaluation<MT2>, const RT2, CT2 >;
   //**********************************************************************************************

   //**Compilation flags***************************************************************************
   //! Compilation switch for the expression template evaluation strategy.
   enum : bool { simdEnabled = false };

   //! Compilation switch for the expression template assignment strategy.
   enum : bool { smpAssignable = MT1::smpAssignable && MT2::smpAssignable };
   //**********************************************************************************************

   //**Constructor*********************************************************************************
   /*!\brief Constructor for the DMatTDMatMapExpr class.
   //
   // \param lhs The left-hand side dense matrix operand of the map expression.
   // \param rhs The right-hand side dense matrix operand of the map expression.
   // \param op The custom unary operation.
   */
   explicit inline DMatTDMatMapExpr( const MT1& lhs, const MT2& rhs, OP op ) noexcept
      : lhs_( lhs )  // Left-hand side dense matrix of the map expression
      , rhs_( rhs )  // Right-hand side dense matrix of the map expression
      , op_ ( op  )  // The custom unary operation
   {}
   //**********************************************************************************************

   //**Access operator*****************************************************************************
   /*!\brief 2D-access to the matrix elements.
   //
   // \param i Access index for the row. The index has to be in the range \f$[0..M-1]\f$.
   // \param j Access index for the column. The index has to be in the range \f$[0..N-1]\f$.
   // \return The resulting value.
   */
   inline ReturnType operator()( size_t i, size_t j ) const {
      BLAZE_INTERNAL_ASSERT( i < lhs_.rows()   , "Invalid row access index"    );
      BLAZE_INTERNAL_ASSERT( j < lhs_.columns(), "Invalid column access index" );
      return op_( lhs_(i,j), rhs_(i,j) );
   }
   //**********************************************************************************************

   //**At function*********************************************************************************
   /*!\brief Checked access to the matrix elements.
   //
   // \param i Access index for the row. The index has to be in the range \f$[0..M-1]\f$.
   // \param j Access index for the column. The index has to be in the range \f$[0..N-1]\f$.
   // \return The resulting value.
   // \exception std::out_of_range Invalid matrix access index.
   */
   inline ReturnType at( size_t i, size_t j ) const {
      if( i >= lhs_.rows() ) {
         BLAZE_THROW_OUT_OF_RANGE( "Invalid row access index" );
      }
      if( j >= lhs_.columns() ) {
         BLAZE_THROW_OUT_OF_RANGE( "Invalid column access index" );
      }
      return (*this)(i,j);
   }
   //**********************************************************************************************

   //**Rows function*******************************************************************************
   /*!\brief Returns the current number of rows of the matrix.
   //
   // \return The number of rows of the matrix.
   */
   inline size_t rows() const noexcept {
      return lhs_.rows();
   }
   //**********************************************************************************************

   //**Columns function****************************************************************************
   /*!\brief Returns the current number of columns of the matrix.
   //
   // \return The number of columns of the matrix.
   */
   inline size_t columns() const noexcept {
      return lhs_.columns();
   }
   //**********************************************************************************************

   //**Left operand access*************************************************************************
   /*!\brief Returns the left-hand side dense matrix operand.
   //
   // \return The left-hand side dense matrix operand.
   */
   inline LeftOperand leftOperand() const noexcept {
      return lhs_;
   }
   //**********************************************************************************************

   //**Right operand access************************************************************************
   /*!\brief Returns the right-hand side dense matrix operand.
   //
   // \return The right-hand side dense matrix operand.
   */
   inline RightOperand rightOperand() const noexcept {
      return rhs_;
   }
   //**********************************************************************************************

   //**Operation access****************************************************************************
   /*!\brief Returns a copy of the custom operation.
   //
   // \return A copy of the custom operation.
   */
   inline Operation operation() const {
      return op_;
   }
   //**********************************************************************************************

   //**********************************************************************************************
   /*!\brief Returns whether the expression can alias with the given address \a alias.
   //
   // \param alias The alias to be checked.
   // \return \a true in case the expression can alias, \a false otherwise.
   */
   template< typename T >
   inline bool canAlias( const T* alias ) const noexcept {
      return ( IsExpression<MT1>::value && lhs_.canAlias( alias ) ) ||
             ( IsExpression<MT2>::value && rhs_.canAlias( alias ) );
   }
   //**********************************************************************************************

   //**********************************************************************************************
   /*!\brief Returns whether the expression is aliased with the given address \a alias.
   //
   // \param alias The alias to be checked.
   // \return \a true in case an alias effect is detected, \a false otherwise.
   */
   template< typename T >
   inline bool isAliased( const T* alias ) const noexcept {
      return ( lhs_.isAliased( alias ) || rhs_.isAliased( alias ) );
   }
   //**********************************************************************************************

   //**********************************************************************************************
   /*!\brief Returns whether the operands of the expression are properly aligned in memory.
   //
   // \return \a true in case the operands are aligned, \a false if not.
   */
   inline bool isAligned() const noexcept {
      return lhs_.isAligned() && rhs_.isAligned();
   }
   //**********************************************************************************************

   //**********************************************************************************************
   /*!\brief Returns whether the expression can be used in SMP assignments.
   //
   // \return \a true in case the expression can be used in SMP assignments, \a false if not.
   */
   inline bool canSMPAssign() const noexcept {
      return lhs_.canSMPAssign() && rhs_.canSMPAssign();
   }
   //**********************************************************************************************

 private:
   //**Member variables****************************************************************************
   LeftOperand  lhs_;  //!< Left-hand side dense matrix of the map expression.
   RightOperand rhs_;  //!< Right-hand side dense matrix of the map expression.
   Operation    op_;   //!< The custom unary operation.
   //**********************************************************************************************

   //**Assignment to dense matrices****************************************************************
   /*! \cond BLAZE_INTERNAL */
   /*!\brief Assignment of a dense matrix-transpose dense matrix map expression to a dense matrix.
   // \ingroup dense_matrix
   //
   // \param lhs The target left-hand side dense matrix.
   // \param rhs The right-hand side map expression to be assigned.
   // \return void
   //
   // This function implements the performance optimized assignment of a dense matrix-transpose
   // dense matrix map expression to a dense matrix. Due to the explicit application of the
   // SFINAE principle, this function can only be selected by the compiler in case either of
   // the two operands requires an intermediate evaluation.
   */
   template< typename MT  // Type of the target dense matrix
           , bool SO >    // Storage order of the target dense matrix
   friend inline DisableIf_< UseAssign<MT> >
      assign( DenseMatrix<MT,SO>& lhs, const DMatTDMatMapExpr& rhs )
   {
      BLAZE_FUNCTION_TRACE;

      BLAZE_INTERNAL_ASSERT( (~lhs).rows()    == rhs.rows()   , "Invalid number of rows"    );
      BLAZE_INTERNAL_ASSERT( (~lhs).columns() == rhs.columns(), "Invalid number of columns" );

      constexpr size_t block( BLOCK_SIZE );

      const size_t m( rhs.rows() );
      const size_t n( rhs.columns() );

      for( size_t ii=0UL; ii<m; ii+=block ) {
         const size_t iend( ( m < ii+block )?( m ):( ii+block ) );
         for( size_t jj=0UL; jj<n; jj+=block ) {
            const size_t jend( ( n < jj+block )?( n ):( jj+block ) );
            for( size_t i=ii; i<iend; ++i ) {
               for( size_t j=jj; j<jend; ++j ) {
                  (~lhs)(i,j) = rhs.op_( rhs.lhs_(i,j), rhs.rhs_(i,j) );
               }
            }
         }
      }
   }
   /*! \endcond */
   //**********************************************************************************************

   //**Assignment to dense matrices****************************************************************
   /*! \cond BLAZE_INTERNAL */
   /*!\brief Assignment of a dense matrix-transpose dense matrix map expression to a dense matrix.
   // \ingroup dense_matrix
   //
   // \param lhs The target left-hand side dense matrix.
   // \param rhs The right-hand side map expression to be assigned.
   // \return void
   //
   // This function implements the performance optimized assignment of a dense matrix-transpose
   // dense matrix map expression to a dense matrix. Due to the explicit application of the
   // SFINAE principle, this function can only be selected by the compiler in case either of
   // the two operands requires an intermediate evaluation.
   */
   template< typename MT  // Type of the target dense matrix
           , bool SO >    // Storage order of the target dense matrix
   friend inline EnableIf_< UseAssign<MT> >
      assign( DenseMatrix<MT,SO>& lhs, const DMatTDMatMapExpr& rhs )
   {
      BLAZE_FUNCTION_TRACE;

      BLAZE_INTERNAL_ASSERT( (~lhs).rows()    == rhs.rows()   , "Invalid number of rows"    );
      BLAZE_INTERNAL_ASSERT( (~lhs).columns() == rhs.columns(), "Invalid number of columns" );

      LT A( serial( rhs.lhs_ ) );  // Evaluation of the left-hand side dense matrix operand
      RT B( serial( rhs.rhs_ ) );  // Evaluation of the right-hand side dense matrix operand

      BLAZE_INTERNAL_ASSERT( A.rows()    == rhs.lhs_.rows()   , "Invalid number of rows"    );
      BLAZE_INTERNAL_ASSERT( A.columns() == rhs.lhs_.columns(), "Invalid number of columns" );
      BLAZE_INTERNAL_ASSERT( B.rows()    == rhs.rhs_.rows()   , "Invalid number of rows"    );
      BLAZE_INTERNAL_ASSERT( B.columns() == rhs.rhs_.columns(), "Invalid number of columns" );
      BLAZE_INTERNAL_ASSERT( A.rows()    == (~lhs).rows()     , "Invalid number of rows"    );
      BLAZE_INTERNAL_ASSERT( B.columns() == (~lhs).columns()  , "Invalid number of columns" );

      assign( ~lhs, map( A, B, rhs.op_ ) );
   }
   /*! \endcond */
   //**********************************************************************************************

   //**Assignment to sparse matrices***************************************************************
   /*! \cond BLAZE_INTERNAL */
   /*!\brief Assignment of a dense matrix-transpose dense matrix map expression to a sparse matrix.
   // \ingroup dense_matrix
   //
   // \param lhs The target left-hand side sparse matrix.
   // \param rhs The right-hand side map expression to be assigned.
   // \return void
   //
   // This function implements the performance optimized assignment of a dense matrix-transpose
   // dense matrix map expression to a sparse matrix. Due to the explicit application of the
   // SFINAE principle, this function can only be selected by the compiler in case either of
   // the two operands requires an intermediate evaluation.
   */
   template< typename MT  // Type of the target sparse matrix
           , bool SO >    // Storage order of the target sparse matrix
   friend inline EnableIf_< UseAssign<MT> >
      assign( SparseMatrix<MT,SO>& lhs, const DMatTDMatMapExpr& rhs )
   {
      BLAZE_FUNCTION_TRACE;

      using TmpType = IfTrue_< SO, OppositeType, ResultType >;

      BLAZE_CONSTRAINT_MUST_BE_DENSE_MATRIX_TYPE( ResultType );
      BLAZE_CONSTRAINT_MUST_BE_DENSE_MATRIX_TYPE( OppositeType );
      BLAZE_CONSTRAINT_MUST_BE_ROW_MAJOR_MATRIX_TYPE( ResultType );
      BLAZE_CONSTRAINT_MUST_BE_COLUMN_MAJOR_MATRIX_TYPE( OppositeType );
      BLAZE_CONSTRAINT_MATRICES_MUST_HAVE_SAME_STORAGE_ORDER( MT, TmpType );
      BLAZE_CONSTRAINT_MUST_NOT_REQUIRE_EVALUATION( TmpType );

      BLAZE_INTERNAL_ASSERT( (~lhs).rows()    == rhs.rows()   , "Invalid number of rows"    );
      BLAZE_INTERNAL_ASSERT( (~lhs).columns() == rhs.columns(), "Invalid number of columns" );

      const TmpType tmp( serial( rhs ) );
      assign( ~lhs, tmp );
   }
   /*! \endcond */
   //**********************************************************************************************

   //**Addition assignment to dense matrices*******************************************************
   /*! \cond BLAZE_INTERNAL */
   /*!\brief Addition assignment of a dense matrix/dense matrix map expression to a dense matrix.
   // \ingroup dense_matrix
   //
   // \param lhs The target left-hand side dense matrix.
   // \param rhs The right-hand side map expression to be added.
   // \return void
   //
   // This function implements the performance optimized addition assignment of a dense
   // matrix/dense matrix map expression to a dense matrix. Due to the explicit application
   // of the SFINAE principle, this function can only be selected by the compiler in case
   // either of the two operands requires an intermediate evaluation.
   */
   template< typename MT  // Type of the target dense matrix
           , bool SO >    // Storage order of the target dense matrix
   friend inline DisableIf_< UseAssign<MT> >
      addAssign( DenseMatrix<MT,SO>& lhs, const DMatTDMatMapExpr& rhs )
   {
      BLAZE_FUNCTION_TRACE;

      BLAZE_INTERNAL_ASSERT( (~lhs).rows()    == rhs.rows()   , "Invalid number of rows"    );
      BLAZE_INTERNAL_ASSERT( (~lhs).columns() == rhs.columns(), "Invalid number of columns" );

      constexpr size_t block( BLOCK_SIZE );

      const size_t m( rhs.rows() );
      const size_t n( rhs.columns() );

      for( size_t ii=0UL; ii<m; ii+=block ) {
         const size_t iend( ( m < ii+block )?( m ):( ii+block ) );
         for( size_t jj=0UL; jj<n; jj+=block ) {
            const size_t jend( ( n < jj+block )?( n ):( jj+block ) );
            for( size_t i=ii; i<iend; ++i ) {
               for( size_t j=jj; j<jend; ++j ) {
                  (~lhs)(i,j) += rhs.op_( rhs.lhs_(i,j), rhs.rhs_(i,j) );
               }
            }
         }
      }
   }
   /*! \endcond */
   //**********************************************************************************************

   //**Addition assignment to dense matrices*******************************************************
   /*! \cond BLAZE_INTERNAL */
   /*!\brief Addition assignment of a dense matrix/dense matrix map expression to a dense matrix.
   // \ingroup dense_matrix
   //
   // \param lhs The target left-hand side dense matrix.
   // \param rhs The right-hand side map expression to be added.
   // \return void
   //
   // This function implements the performance optimized addition assignment of a dense
   // matrix/dense matrix map expression to a dense matrix. Due to the explicit application
   // of the SFINAE principle, this function can only be selected by the compiler in case
   // either of the two operands requires an intermediate evaluation.
   */
   template< typename MT  // Type of the target dense matrix
           , bool SO >    // Storage order of the target dense matrix
   friend inline EnableIf_< UseAssign<MT> >
      addAssign( DenseMatrix<MT,SO>& lhs, const DMatTDMatMapExpr& rhs )
   {
      BLAZE_FUNCTION_TRACE;

      BLAZE_INTERNAL_ASSERT( (~lhs).rows()    == rhs.rows()   , "Invalid number of rows"    );
      BLAZE_INTERNAL_ASSERT( (~lhs).columns() == rhs.columns(), "Invalid number of columns" );

      LT A( serial( rhs.lhs_ ) );  // Evaluation of the left-hand side dense matrix operand
      RT B( serial( rhs.rhs_ ) );  // Evaluation of the right-hand side dense matrix operand

      BLAZE_INTERNAL_ASSERT( A.rows()    == rhs.lhs_.rows()   , "Invalid number of rows"    );
      BLAZE_INTERNAL_ASSERT( A.columns() == rhs.lhs_.columns(), "Invalid number of columns" );
      BLAZE_INTERNAL_ASSERT( B.rows()    == rhs.rhs_.rows()   , "Invalid number of rows"    );
      BLAZE_INTERNAL_ASSERT( B.columns() == rhs.rhs_.columns(), "Invalid number of columns" );
      BLAZE_INTERNAL_ASSERT( A.rows()    == (~lhs).rows()     , "Invalid number of rows"    );
      BLAZE_INTERNAL_ASSERT( B.columns() == (~lhs).columns()  , "Invalid number of columns" );

      addAssign( ~lhs, map( A, B, rhs.op_ ) );
   }
   /*! \endcond */
   //**********************************************************************************************

   //**Addition assignment to sparse matrices******************************************************
   // No special implementation for the addition assignment to sparse matrices.
   //**********************************************************************************************

   //**Subtraction assignment to dense matrices****************************************************
   /*! \cond BLAZE_INTERNAL */
   /*!\brief Subtraction assignment of a dense matrix/dense matrix map expression to a dense matrix.
   // \ingroup dense_matrix
   //
   // \param lhs The target left-hand side dense matrix.
   // \param rhs The right-hand side map expression to be subtracted.
   // \return void
   //
   // This function implements the performance optimized subtraction assignment of a dense
   // matrix/dense matrix map expression to a dense matrix. Due to the explicit application
   // of the SFINAE principle, this function can only be selected by the compiler in case
   // either of the two operands requires an intermediate evaluation.
   */
   template< typename MT  // Type of the target dense matrix
           , bool SO >    // Storage order of the target dense matrix
   friend inline DisableIf_< UseAssign<MT> >
      subAssign( DenseMatrix<MT,SO>& lhs, const DMatTDMatMapExpr& rhs )
   {
      BLAZE_FUNCTION_TRACE;

      BLAZE_INTERNAL_ASSERT( (~lhs).rows()    == rhs.rows()   , "Invalid number of rows"    );
      BLAZE_INTERNAL_ASSERT( (~lhs).columns() == rhs.columns(), "Invalid number of columns" );

      constexpr size_t block( BLOCK_SIZE );

      const size_t m( rhs.rows() );
      const size_t n( rhs.columns() );

      for( size_t ii=0UL; ii<m; ii+=block ) {
         const size_t iend( ( m < ii+block )?( m ):( ii+block ) );
         for( size_t jj=0UL; jj<n; jj+=block ) {
            const size_t jend( ( n < jj+block )?( n ):( jj+block ) );
            for( size_t i=ii; i<iend; ++i ) {
               for( size_t j=jj; j<jend; ++j ) {
                  (~lhs)(i,j) -= rhs.op_( rhs.lhs_(i,j), rhs.rhs_(i,j) );
               }
            }
         }
      }
   }
   /*! \endcond */
   //**********************************************************************************************

   //**Subtraction assignment to dense matrices****************************************************
   /*! \cond BLAZE_INTERNAL */
   /*!\brief Subtraction assignment of a dense matrix/dense matrix map expression to a dense matrix.
   // \ingroup dense_matrix
   //
   // \param lhs The target left-hand side dense matrix.
   // \param rhs The right-hand side map expression to be subtracted.
   // \return void
   //
   // This function implements the performance optimized subtraction assignment of a dense
   // matrix/dense matrix map expression to a dense matrix. Due to the explicit application
   // of the SFINAE principle, this function can only be selected by the compiler in case
   // either of the two operands requires an intermediate evaluation.
   */
   template< typename MT  // Type of the target dense matrix
           , bool SO >    // Storage order of the target dense matrix
   friend inline EnableIf_< UseAssign<MT> >
      subAssign( DenseMatrix<MT,SO>& lhs, const DMatTDMatMapExpr& rhs )
   {
      BLAZE_FUNCTION_TRACE;

      BLAZE_INTERNAL_ASSERT( (~lhs).rows()    == rhs.rows()   , "Invalid number of rows"    );
      BLAZE_INTERNAL_ASSERT( (~lhs).columns() == rhs.columns(), "Invalid number of columns" );

      LT A( serial( rhs.lhs_ ) );  // Evaluation of the left-hand side dense matrix operand
      RT B( serial( rhs.rhs_ ) );  // Evaluation of the right-hand side dense matrix operand

      BLAZE_INTERNAL_ASSERT( A.rows()    == rhs.lhs_.rows()   , "Invalid number of rows"    );
      BLAZE_INTERNAL_ASSERT( A.columns() == rhs.lhs_.columns(), "Invalid number of columns" );
      BLAZE_INTERNAL_ASSERT( B.rows()    == rhs.rhs_.rows()   , "Invalid number of rows"    );
      BLAZE_INTERNAL_ASSERT( B.columns() == rhs.rhs_.columns(), "Invalid number of columns" );
      BLAZE_INTERNAL_ASSERT( A.rows()    == (~lhs).rows()     , "Invalid number of rows"    );
      BLAZE_INTERNAL_ASSERT( B.columns() == (~lhs).columns()  , "Invalid number of columns" );

      subAssign( ~lhs, map( A, B, rhs.op_ ) );
   }
   /*! \endcond */
   //**********************************************************************************************

   //**Subtraction assignment to sparse matrices***************************************************
   // No special implementation for the subtraction assignment to sparse matrices.
   //**********************************************************************************************

   //**Schur product assignment to dense matrices**************************************************
   /*! \cond BLAZE_INTERNAL */
   /*!\brief Schur product assignment of a dense matrix/dense matrix map expression to a dense matrix.
   // \ingroup dense_matrix
   //
   // \param lhs The target left-hand side dense matrix.
   // \param rhs The right-hand side map expression for the Schur product.
   // \return void
   //
   // This function implements the performance optimized Schur product assignment of a dense
   // matrix/dense matrix map expression to a dense matrix. Due to the explicit application of
   // the SFINAE principle, this function can only be selected by the compiler in case either
   // of the two operands requires an intermediate evaluation.
   */
   template< typename MT  // Type of the target dense matrix
           , bool SO >    // Storage order of the target dense matrix
   friend inline DisableIf_< UseAssign<MT> >
      schurAssign( DenseMatrix<MT,SO>& lhs, const DMatTDMatMapExpr& rhs )
   {
      BLAZE_FUNCTION_TRACE;

      BLAZE_INTERNAL_ASSERT( (~lhs).rows()    == rhs.rows()   , "Invalid number of rows"    );
      BLAZE_INTERNAL_ASSERT( (~lhs).columns() == rhs.columns(), "Invalid number of columns" );

      constexpr size_t block( BLOCK_SIZE );

      const size_t m( rhs.rows() );
      const size_t n( rhs.columns() );

      for( size_t ii=0UL; ii<m; ii+=block ) {
         const size_t iend( ( m < ii+block )?( m ):( ii+block ) );
         for( size_t jj=0UL; jj<n; jj+=block ) {
            const size_t jend( ( n < jj+block )?( n ):( jj+block ) );
            for( size_t i=ii; i<iend; ++i ) {
               for( size_t j=jj; j<jend; ++j ) {
                  (~lhs)(i,j) *= rhs.op_( rhs.lhs_(i,j), rhs.rhs_(i,j) );
               }
            }
         }
      }
   }
   /*! \endcond */
   //**********************************************************************************************

   //**Schur product assignment to dense matrices**************************************************
   /*! \cond BLAZE_INTERNAL */
   /*!\brief Schur product assignment of a dense matrix/dense matrix map expression to a dense matrix.
   // \ingroup dense_matrix
   //
   // \param lhs The target left-hand side dense matrix.
   // \param rhs The right-hand side map expression for the Schur product.
   // \return void
   //
   // This function implements the performance optimized Schur product assignment of a dense
   // matrix/dense matrix map expression to a dense matrix. Due to the explicit application of
   // the SFINAE principle, this function can only be selected by the compiler in case either
   // of the two operands requires an intermediate evaluation.
   */
   template< typename MT  // Type of the target dense matrix
           , bool SO >    // Storage order of the target dense matrix
   friend inline EnableIf_< UseAssign<MT> >
      schurAssign( DenseMatrix<MT,SO>& lhs, const DMatTDMatMapExpr& rhs )
   {
      BLAZE_FUNCTION_TRACE;

      BLAZE_INTERNAL_ASSERT( (~lhs).rows()    == rhs.rows()   , "Invalid number of rows"    );
      BLAZE_INTERNAL_ASSERT( (~lhs).columns() == rhs.columns(), "Invalid number of columns" );

      LT A( serial( rhs.lhs_ ) );  // Evaluation of the left-hand side dense matrix operand
      RT B( serial( rhs.rhs_ ) );  // Evaluation of the right-hand side dense matrix operand

      BLAZE_INTERNAL_ASSERT( A.rows()    == rhs.lhs_.rows()   , "Invalid number of rows"    );
      BLAZE_INTERNAL_ASSERT( A.columns() == rhs.lhs_.columns(), "Invalid number of columns" );
      BLAZE_INTERNAL_ASSERT( B.rows()    == rhs.rhs_.rows()   , "Invalid number of rows"    );
      BLAZE_INTERNAL_ASSERT( B.columns() == rhs.rhs_.columns(), "Invalid number of columns" );
      BLAZE_INTERNAL_ASSERT( A.rows()    == (~lhs).rows()     , "Invalid number of rows"    );
      BLAZE_INTERNAL_ASSERT( B.columns() == (~lhs).columns()  , "Invalid number of columns" );

      schurAssign( ~lhs, map( A, B, rhs.op_ ) );
   }
   /*! \endcond */
   //**********************************************************************************************

   //**Schur product assignment to sparse matrices*************************************************
   // No special implementation for the Schur product assignment to sparse matrices.
   //**********************************************************************************************

   //**Multiplication assignment to dense matrices*************************************************
   // No special implementation for the multiplication assignment to dense matrices.
   //**********************************************************************************************

   //**Multiplication assignment to sparse matrices************************************************
   // No special implementation for the multiplication assignment to sparse matrices.
   //**********************************************************************************************

   //**SMP assignment to dense matrices************************************************************
   /*! \cond BLAZE_INTERNAL */
   /*!\brief SMP assignment of a dense matrix/dense matrix map expression to a dense matrix.
   // \ingroup dense_matrix
   //
   // \param lhs The target left-hand side dense matrix.
   // \param rhs The right-hand side map expression to be assigned.
   // \return void
   //
   // This function implements the performance optimized SMP assignment of a dense matrix/dense
   // matrix map expression to a dense matrix. Due to the explicit application of the SFINAE
   // principle, this function can only be selected by the compiler in case the expression
   // specific parallel evaluation strategy is selected.
   */
   template< typename MT  // Type of the target dense matrix
           , bool SO >    // Storage order of the target dense matrix
   friend inline EnableIf_< UseSMPAssign<MT> >
      smpAssign( DenseMatrix<MT,SO>& lhs, const DMatTDMatMapExpr& rhs )
   {
      BLAZE_FUNCTION_TRACE;

      BLAZE_INTERNAL_ASSERT( (~lhs).rows()    == rhs.rows()   , "Invalid number of rows"    );
      BLAZE_INTERNAL_ASSERT( (~lhs).columns() == rhs.columns(), "Invalid number of columns" );

      LT A( rhs.lhs_ );  // Evaluation of the left-hand side dense matrix operand
      RT B( rhs.rhs_ );  // Evaluation of the right-hand side dense matrix operand

      BLAZE_INTERNAL_ASSERT( A.rows()    == rhs.lhs_.rows()   , "Invalid number of rows"    );
      BLAZE_INTERNAL_ASSERT( A.columns() == rhs.lhs_.columns(), "Invalid number of columns" );
      BLAZE_INTERNAL_ASSERT( B.rows()    == rhs.rhs_.rows()   , "Invalid number of rows"    );
      BLAZE_INTERNAL_ASSERT( B.columns() == rhs.rhs_.columns(), "Invalid number of columns" );
      BLAZE_INTERNAL_ASSERT( A.rows()    == (~lhs).rows()     , "Invalid number of rows"    );
      BLAZE_INTERNAL_ASSERT( B.columns() == (~lhs).columns()  , "Invalid number of columns" );

      smpAssign( ~lhs, map( A, B, rhs.op_ ) );
   }
   /*! \endcond */
   //**********************************************************************************************

   //**SMP assignment to sparse matrices***********************************************************
   /*! \cond BLAZE_INTERNAL */
   /*!\brief SMP assignment of a dense matrix/dense matrix map expression to a sparse matrix.
   // \ingroup dense_matrix
   //
   // \param lhs The target left-hand side sparse matrix.
   // \param rhs The right-hand side map expression to be assigned.
   // \return void
   //
   // This function implements the performance optimized SMP assignment of a dense matrix/dense
   // matrix map expression to a sparse matrix. Due to the explicit application of the SFINAE
   // principle, this function can only be selected by the compiler in case the expression
   // specific parallel evaluation strategy is selected.
   */
   template< typename MT  // Type of the target sparse matrix
           , bool SO >    // Storage order of the target sparse matrix
   friend inline EnableIf_< UseSMPAssign<MT> >
      smpAssign( SparseMatrix<MT,SO>& lhs, const DMatTDMatMapExpr& rhs )
   {
      BLAZE_FUNCTION_TRACE;

      using TmpType = IfTrue_< SO, OppositeType, ResultType >;

      BLAZE_CONSTRAINT_MUST_BE_DENSE_MATRIX_TYPE( ResultType );
      BLAZE_CONSTRAINT_MUST_BE_DENSE_MATRIX_TYPE( OppositeType );
      BLAZE_CONSTRAINT_MUST_BE_ROW_MAJOR_MATRIX_TYPE( ResultType );
      BLAZE_CONSTRAINT_MUST_BE_COLUMN_MAJOR_MATRIX_TYPE( OppositeType );
      BLAZE_CONSTRAINT_MATRICES_MUST_HAVE_SAME_STORAGE_ORDER( MT, TmpType );
      BLAZE_CONSTRAINT_MUST_NOT_REQUIRE_EVALUATION( TmpType );

      BLAZE_INTERNAL_ASSERT( (~lhs).rows()    == rhs.rows()   , "Invalid number of rows"    );
      BLAZE_INTERNAL_ASSERT( (~lhs).columns() == rhs.columns(), "Invalid number of columns" );

      const TmpType tmp( rhs );
      smpAssign( ~lhs, tmp );
   }
   /*! \endcond */
   //**********************************************************************************************

   //**SMP addition assignment to dense matrices***************************************************
   /*! \cond BLAZE_INTERNAL */
   /*!\brief SMP addition assignment of a dense matrix/dense matrix map expression to a dense
   //        matrix.
   // \ingroup dense_matrix
   //
   // \param lhs The target left-hand side dense matrix.
   // \param rhs The right-hand side map expression to be added.
   // \return void
   //
   // This function implements the performance optimized SMP addition assignment of a dense
   // matrix/dense matrix map expression to a dense matrix. Due to the explicit application
   // of the SFINAE principle, this function can only be selected by the compiler in case the
   // expression specific parallel evaluation strategy is selected.
   */
   template< typename MT  // Type of the target dense matrix
           , bool SO >    // Storage order of the target dense matrix
   friend inline EnableIf_< UseSMPAssign<MT> >
      smpAddAssign( DenseMatrix<MT,SO>& lhs, const DMatTDMatMapExpr& rhs )
   {
      BLAZE_FUNCTION_TRACE;

      BLAZE_INTERNAL_ASSERT( (~lhs).rows()    == rhs.rows()   , "Invalid number of rows"    );
      BLAZE_INTERNAL_ASSERT( (~lhs).columns() == rhs.columns(), "Invalid number of columns" );

      LT A( rhs.lhs_ );  // Evaluation of the left-hand side dense matrix operand
      RT B( rhs.rhs_ );  // Evaluation of the right-hand side dense matrix operand

      BLAZE_INTERNAL_ASSERT( A.rows()    == rhs.lhs_.rows()   , "Invalid number of rows"    );
      BLAZE_INTERNAL_ASSERT( A.columns() == rhs.lhs_.columns(), "Invalid number of columns" );
      BLAZE_INTERNAL_ASSERT( B.rows()    == rhs.rhs_.rows()   , "Invalid number of rows"    );
      BLAZE_INTERNAL_ASSERT( B.columns() == rhs.rhs_.columns(), "Invalid number of columns" );
      BLAZE_INTERNAL_ASSERT( A.rows()    == (~lhs).rows()     , "Invalid number of rows"    );
      BLAZE_INTERNAL_ASSERT( B.columns() == (~lhs).columns()  , "Invalid number of columns" );

      smpAddAssign( ~lhs, map( A, B, rhs.op_ ) );
   }
   /*! \endcond */
   //**********************************************************************************************

   //**SMP addition assignment to sparse matrices**************************************************
   // No special implementation for the SMP addition assignment to sparse matrices.
   //**********************************************************************************************

   //**SMP subtraction assignment to dense matrices************************************************
   /*! \cond BLAZE_INTERNAL */
   /*!\brief SMP subtraction assignment of a dense matrix/dense matrix map expression to a dense
   //        matrix.
   // \ingroup dense_matrix
   //
   // \param lhs The target left-hand side dense matrix.
   // \param rhs The right-hand side map expression to be subtracted.
   // \return void
   //
   // This function implements the performance optimized SMP subtraction assignment of a dense
   // matrix/dense matrix map expression to a dense matrix. Due to the explicit application of
   // the SFINAE principle, this function can only be selected by the compiler in case the
   // expression specific parallel evaluation strategy is selected.
   */
   template< typename MT  // Type of the target dense matrix
           , bool SO >    // Storage order of the target dense matrix
   friend inline EnableIf_< UseSMPAssign<MT> >
      smpSubAssign( DenseMatrix<MT,SO>& lhs, const DMatTDMatMapExpr& rhs )
   {
      BLAZE_FUNCTION_TRACE;

      BLAZE_INTERNAL_ASSERT( (~lhs).rows()    == rhs.rows()   , "Invalid number of rows"    );
      BLAZE_INTERNAL_ASSERT( (~lhs).columns() == rhs.columns(), "Invalid number of columns" );

      LT A( rhs.lhs_ );  // Evaluation of the left-hand side dense matrix operand
      RT B( rhs.rhs_ );  // Evaluation of the right-hand side dense matrix operand

      BLAZE_INTERNAL_ASSERT( A.rows()    == rhs.lhs_.rows()   , "Invalid number of rows"    );
      BLAZE_INTERNAL_ASSERT( A.columns() == rhs.lhs_.columns(), "Invalid number of columns" );
      BLAZE_INTERNAL_ASSERT( B.rows()    == rhs.rhs_.rows()   , "Invalid number of rows"    );
      BLAZE_INTERNAL_ASSERT( B.columns() == rhs.rhs_.columns(), "Invalid number of columns" );
      BLAZE_INTERNAL_ASSERT( A.rows()    == (~lhs).rows()     , "Invalid number of rows"    );
      BLAZE_INTERNAL_ASSERT( B.columns() == (~lhs).columns()  , "Invalid number of columns" );

      smpSubAssign( ~lhs, map( A, B, rhs.op_ ) );
   }
   /*! \endcond */
   //**********************************************************************************************

   //**SMP subtraction assignment to sparse matrices***********************************************
   // No special implementation for the SMP subtraction assignment to sparse matrices.
   //**********************************************************************************************

   //**SMP Schur product assignment to dense matrices**********************************************
   /*! \cond BLAZE_INTERNAL */
   /*!\brief SMP Schur product assignment of a dense matrix/dense matrix map expression to a
   //        dense matrix.
   // \ingroup dense_matrix
   //
   // \param lhs The target left-hand side dense matrix.
   // \param rhs The right-hand side map expression for the Schur product.
   // \return void
   //
   // This function implements the performance optimized SMP Schur product assignment of a
   // dense matrix/dense matrix map expression to a dense matrix. Due to the explicit application
   // of the SFINAE principle, this function can only be selected by the compiler in case the
   // expression specific parallel evaluation strategy is selected.
   */
   template< typename MT  // Type of the target dense matrix
           , bool SO >    // Storage order of the target dense matrix
   friend inline EnableIf_< UseSMPAssign<MT> >
      smpSchurAssign( DenseMatrix<MT,SO>& lhs, const DMatTDMatMapExpr& rhs )
   {
      BLAZE_FUNCTION_TRACE;

      BLAZE_INTERNAL_ASSERT( (~lhs).rows()    == rhs.rows()   , "Invalid number of rows"    );
      BLAZE_INTERNAL_ASSERT( (~lhs).columns() == rhs.columns(), "Invalid number of columns" );

      LT A( rhs.lhs_ );  // Evaluation of the left-hand side dense matrix operand
      RT B( rhs.rhs_ );  // Evaluation of the right-hand side dense matrix operand

      BLAZE_INTERNAL_ASSERT( A.rows()    == rhs.lhs_.rows()   , "Invalid number of rows"    );
      BLAZE_INTERNAL_ASSERT( A.columns() == rhs.lhs_.columns(), "Invalid number of columns" );
      BLAZE_INTERNAL_ASSERT( B.rows()    == rhs.rhs_.rows()   , "Invalid number of rows"    );
      BLAZE_INTERNAL_ASSERT( B.columns() == rhs.rhs_.columns(), "Invalid number of columns" );
      BLAZE_INTERNAL_ASSERT( A.rows()    == (~lhs).rows()     , "Invalid number of rows"    );
      BLAZE_INTERNAL_ASSERT( B.columns() == (~lhs).columns()  , "Invalid number of columns" );

      smpSchurAssign( ~lhs, map( A, B, rhs.op_ ) );
   }
   /*! \endcond */
   //**********************************************************************************************

   //**SMP Schur product assignment to sparse matrices*********************************************
   // No special implementation for the SMP Schur product assignment to sparse matrices.
   //**********************************************************************************************

   //**SMP multiplication assignment to dense matrices*********************************************
   // No special implementation for the SMP multiplication assignment to dense matrices.
   //**********************************************************************************************

   //**SMP multiplication assignment to sparse matrices********************************************
   // No special implementation for the SMP multiplication assignment to sparse matrices.
   //**********************************************************************************************

   //**Compile time checks*************************************************************************
   /*! \cond BLAZE_INTERNAL */
   BLAZE_CONSTRAINT_MUST_BE_DENSE_MATRIX_TYPE( MT1 );
   BLAZE_CONSTRAINT_MUST_BE_DENSE_MATRIX_TYPE( MT2 );
   BLAZE_CONSTRAINT_MATRICES_MUST_HAVE_DIFFERENT_STORAGE_ORDER( MT1, MT2 );
   /*! \endcond */
   //**********************************************************************************************
};
//*************************************************************************************************




//=================================================================================================
//
//  GLOBAL FUNCTIONS
//
//=================================================================================================

//*************************************************************************************************
/*!\brief Evaluates the given binary operation on each single element of the row-major dense
//        matrix \a lhs and the column-major dense matrix \a rhs.
// \ingroup dense_matrix
//
// \param lhs The left-hand side dense matrix operand.
// \param rhs The right-hand side dense matrix operand.
// \param op The custom, binary operation.
// \return The binary operation applied to each single element of \a lhs and \a rhs.
// \exception std::invalid_argument Matrix sizes do not match.
//
// The \a map() function evaluates the given binary operation on each element of the input
// matrices \a lhs and \a rhs. The function returns an expression representing this operation.\n
// The following example demonstrates the use of the \a map() function:

   \code
   using blaze::rowMajor;
   using blaze::columnMajor;

   blaze::DynamicMatrix<double,rowMajor> A, C;
   blaze::DynamicMatrix<double,columnMajor> B;
   // ... Resizing and initialization
   C = map( A, B, []( double x, double y ){ return std::min( x, y ); } );
   \endcode
*/
template< typename MT1   // Type of the left-hand side dense matrix
        , typename MT2   // Type of the right-hand side dense matrix
        , typename OP >  // Type of the custom operation
inline EnableIf_< And< Not< IsSymmetric<MT1> >, Not< IsSymmetric<MT2> > >
                , const DMatTDMatMapExpr<MT1,MT2,OP> >
   map( const DenseMatrix<MT1,false>& lhs, const DenseMatrix<MT2,true>& rhs, OP op )
{
   BLAZE_FUNCTION_TRACE;

   if( (~lhs).rows() != (~rhs).rows() || (~lhs).columns() != (~rhs).columns() ) {
      BLAZE_THROW_INVALID_ARGUMENT( "Matrix sizes do not match" );
   }

   return DMatTDMatMapExpr<MT1,MT2,OP>( ~lhs, ~rhs, op );
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Evaluates the given binary operation on each single element of the column-major dense
//        matrix \a lhs and the row-major dense matrix \a rhs.
// \ingroup dense_matrix
//
// \param lhs The left-hand side dense matrix operand.
// \param rhs The right-hand side dense matrix operand.
// \param op The custom, binary operation.
// \return The binary operation applied to each single element of \a lhs and \a rhs.
// \exception std::invalid_argument Matrix sizes do not match.
//
// The \a map() function evaluates the given binary operation on each element of the input
// matrices \a lhs and \a rhs. The function returns an expression representing this operation.\n
// The following example demonstrates the use of the \a map() function:

   \code
   using blaze::rowMajor;
   using blaze::columnMajor;

   blaze::DynamicMatrix<double,columnMajor> A;
   blaze::DynamicMatrix<double,rowMajor> B, C;
   // ... Resizing and initialization
   C = map( A, B, []( double x, double y ){ return std::min( x, y ); } );
   \endcode
*/
template< typename MT1   // Type of the left-hand side dense matrix
        , typename MT2   // Type of the right-hand side dense matrix
        , typename OP >  // Type of the custom operation
inline EnableIf_< And< Not< IsSymmetric<MT1> >, Not< IsSymmetric<MT2> > >
                , const DMatTDMatMapExpr<MT1,MT2,OP> >
   map( const DenseMatrix<MT1,true>& lhs, const DenseMatrix<MT2,false>& rhs, OP op )
{
   BLAZE_FUNCTION_TRACE;

   if( (~lhs).rows() != (~rhs).rows() || (~lhs).columns() != (~rhs).columns() ) {
      BLAZE_THROW_INVALID_ARGUMENT( "Matrix sizes do not match" );
   }

   return DMatTDMatMapExpr<MT1,MT2,OP>( ~lhs, ~rhs, op );
}
//*************************************************************************************************




//=================================================================================================
//
//  GLOBAL RESTRUCTURING BINARY ARITHMETIC OPERATORS
//
//=================================================================================================

//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Evaluates the given binary operation on each single element of the symmetric row-major
//        dense matrix \a lhs and the column-major dense matrix \a rhs.
// \ingroup dense_matrix
//
// \param lhs The left-hand side dense matrix operand.
// \param rhs The right-hand side dense matrix operand.
// \param op The custom, binary operation.
// \return The binary operation applied to each single element of \a lhs and \a rhs.
// \exception std::invalid_argument Matrix sizes do not match.
//
// This \a map() implements a performance optimized treatment for applying the given binary
// operation to a symmetric row-major dense matrix and a column-major dense matrix.
*/
template< typename MT1   // Type of the left-hand side dense matrix
        , typename MT2   // Type of the right-hand side dense matrix
        , typename OP >  // Type of the custom operation
inline EnableIf_< And< IsSymmetric<MT1>, Not< IsSymmetric<MT2> > >
                , const BinaryMapExprTrait_<MT1,MT2,OP> >
   map( const DenseMatrix<MT1,false>& lhs, const DenseMatrix<MT2,true>& rhs, OP op )
{
   BLAZE_FUNCTION_TRACE;

   if( (~lhs).rows() != (~rhs).rows() || (~lhs).columns() != (~rhs).columns() ) {
      BLAZE_THROW_INVALID_ARGUMENT( "Matrix sizes do not match" );
   }

   return map( trans( ~lhs ), ~rhs, op );
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Evaluates the given binary operation on each single element of the row-major dense
//        matrix \a lhs and the symmetric column-major dense matrix \a rhs.
// \ingroup dense_matrix
//
// \param lhs The left-hand side dense matrix operand.
// \param rhs The right-hand side dense matrix operand.
// \param op The custom, binary operation.
// \return The binary operation applied to each single element of \a lhs and \a rhs.
// \exception std::invalid_argument Matrix sizes do not match.
//
// This \a map() implements a performance optimized treatment for applying the given binary
// operation to a (potentially symmetric) row-major dense matrix and a symmetric column-major
// dense matrix.
*/
template< typename MT1   // Type of the left-hand side dense matrix
        , typename MT2   // Type of the right-hand side dense matrix
        , typename OP >  // Type of the custom operation
inline EnableIf_< IsSymmetric<MT2>, const BinaryMapExprTrait_<MT1,MT2,OP> >
   map( const DenseMatrix<MT1,false>& lhs, const DenseMatrix<MT2,true>& rhs, OP op )
{
   BLAZE_FUNCTION_TRACE;

   if( (~lhs).rows() != (~rhs).rows() || (~lhs).columns() != (~rhs).columns() ) {
      BLAZE_THROW_INVALID_ARGUMENT( "Matrix sizes do not match" );
   }

   return map( ~lhs, trans( ~rhs ), op );
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Evaluates the given binary operation on each single element of the column-major dense
//        matrix \a lhs and the symmetric row-major dense matrix \a rhs.
// \ingroup dense_matrix
//
// \param lhs The left-hand side dense matrix operand.
// \param rhs The right-hand side dense matrix operand.
// \param op The custom, binary operation.
// \return The binary operation applied to each single element of \a lhs and \a rhs.
// \exception std::invalid_argument Matrix sizes do not match.
//
// This \a map() implements a performance optimized treatment for applying the given binary
// operation to a column-major dense matrix and a symmetric row-major dense matrix.
*/
template< typename MT1   // Type of the left-hand side dense matrix
        , typename MT2   // Type of the right-hand side dense matrix
        , typename OP >  // Type of the custom operation
inline EnableIf_< And< Not< IsSymmetric<MT1> >, IsSymmetric<MT2> >
                , const BinaryMapExprTrait_<MT1,MT2,OP> >
   map( const DenseMatrix<MT1,true>& lhs, const DenseMatrix<MT2,false>& rhs, OP op )
{
   BLAZE_FUNCTION_TRACE;

   if( (~lhs).rows() != (~rhs).rows() || (~lhs).columns() != (~rhs).columns() ) {
      BLAZE_THROW_INVALID_ARGUMENT( "Matrix sizes do not match" );
   }

   return map( ~lhs, trans( ~rhs ), op );
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Evaluates the given binary operation on each single element of the symmetric
//        column-major dense matrix \a lhs and the row-major dense matrix \a rhs.
// \ingroup dense_matrix
//
// \param lhs The left-hand side dense matrix operand.
// \param rhs The right-hand side dense matrix operand.
// \param op The custom, binary operation.
// \return The binary operation applied to each single element of \a lhs and \a rhs.
// \exception std::invalid_argument Matrix sizes do not match.
//
// This \a map() implements a performance optimized treatment for applying the given binary
// operation to a symmetric column-major dense matrix and a (potentially symmetric) row-major
// dense matrix.
*/
template< typename MT1   // Type of the left-hand side dense matrix
        , typename MT2   // Type of the right-hand side dense matrix
        , typename OP >  // Type of the custom operation
inline EnableIf_< IsSymmetric<MT1>, const BinaryMapExprTrait_<MT1,MT2,OP> >
   map( const DenseMatrix<MT1,true>& lhs, const DenseMatrix<MT2,false>& rhs, OP op )
{
   BLAZE_FUNCTION_TRACE;

   if( (~lhs).rows() != (~rhs).rows() || (~lhs).columns() != (~rhs).columns() ) {
      BLAZE_THROW_INVALID_ARGUMENT( "Matrix sizes do not match" );
   }

   return map( trans( ~lhs ), ~rhs, op );
}
/*! \endcond */
//*************************************************************************************************




//=================================================================================================
//
//  ROWS SPECIALIZATIONS
//
//=================================================================================================

//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
template< typename MT1, typename MT2, typename OP >
struct Rows< DMatTDMatMapExpr<MT1,MT2,OP> >
   : public Maximum< Rows<MT1>, Rows<MT2> >
{};
/*! \endcond */
//*************************************************************************************************




//=================================================================================================
//
//  COLUMNS SPECIALIZATIONS
//
//=================================================================================================

//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
template< typename MT1, typename MT2, typename OP >
struct Columns< DMatTDMatMapExpr<MT1,MT2,OP> >
   : public Maximum< Columns<MT1>, Columns<MT2> >
{};
/*! \endcond */
//*************************************************************************************************




//=================================================================================================
//
//  ISALIGNED SPECIALIZATIONS
//
//=================================================================================================

//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
template< typename MT1, typename MT2, typename OP >
struct IsAligned< DMatTDMatMapExpr<MT1,MT2,OP> >
   : public BoolConstant< And< IsAligned<MT1>, IsAligned<MT2> >::value  >
{};
/*! \endcond */
//*************************************************************************************************




//=================================================================================================
//
//  ISPADDED SPECIALIZATIONS
//
//=================================================================================================

//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
template< typename MT1, typename MT2, typename OP >
struct IsPadded< DMatTDMatMapExpr<MT1,MT2,OP> >
   : public BoolConstant< And< IsPadded<MT1>, IsPadded<MT2> >::value  >
{};
/*! \endcond */
//*************************************************************************************************




//=================================================================================================
//
//  ISSYMMETRIC SPECIALIZATIONS
//
//=================================================================================================

//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
template< typename MT1, typename MT2 >
struct IsSymmetric< DMatTDMatMapExpr<MT1,MT2,Min> >
   : public BoolConstant< And< IsSymmetric<MT1>, IsSymmetric<MT2> >::value >
{};

template< typename MT1, typename MT2 >
struct IsSymmetric< DMatTDMatMapExpr<MT1,MT2,Max> >
   : public BoolConstant< And< IsSymmetric<MT1>, IsSymmetric<MT2> >::value >
{};
/*! \endcond */
//*************************************************************************************************




//=================================================================================================
//
//  ISHERMITIAN SPECIALIZATIONS
//
//=================================================================================================

//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
template< typename MT1, typename MT2 >
struct IsHermitian< DMatTDMatMapExpr<MT1,MT2,Min> >
   : public BoolConstant< And< IsHermitian<MT1>, IsHermitian<MT2> >::value >
{};

template< typename MT1, typename MT2 >
struct IsHermitian< DMatTDMatMapExpr<MT1,MT2,Max> >
   : public BoolConstant< And< IsHermitian<MT1>, IsHermitian<MT2> >::value >
{};
/*! \endcond */
//*************************************************************************************************




//=================================================================================================
//
//  ISLOWER SPECIALIZATIONS
//
//=================================================================================================

//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
template< typename MT1, typename MT2 >
struct IsLower< DMatTDMatMapExpr<MT1,MT2,Min> >
   : public BoolConstant< And< IsLower<MT1>, IsLower<MT2> >::value >
{};

template< typename MT1, typename MT2 >
struct IsLower< DMatTDMatMapExpr<MT1,MT2,Max> >
   : public BoolConstant< And< IsLower<MT1>, IsLower<MT2> >::value >
{};
/*! \endcond */
//*************************************************************************************************




//=================================================================================================
//
//  ISUNILOWER SPECIALIZATIONS
//
//=================================================================================================

//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
template< typename MT1, typename MT2 >
struct IsUniLower< DMatTDMatMapExpr<MT1,MT2,Min> >
   : public BoolConstant< And< IsUniLower<MT1>, IsUniLower<MT2> >::value >
{};

template< typename MT1, typename MT2 >
struct IsUniLower< DMatTDMatMapExpr<MT1,MT2,Max> >
   : public BoolConstant< And< IsUniLower<MT1>, IsUniLower<MT2> >::value >
{};
/*! \endcond */
//*************************************************************************************************




//=================================================================================================
//
//  ISSTRICTLYLOWER SPECIALIZATIONS
//
//=================================================================================================

//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
template< typename MT1, typename MT2 >
struct IsStrictlyLower< DMatTDMatMapExpr<MT1,MT2,Min> >
   : public BoolConstant< And< IsStrictlyLower<MT1>, IsStrictlyLower<MT2> >::value >
{};

template< typename MT1, typename MT2 >
struct IsStrictlyLower< DMatTDMatMapExpr<MT1,MT2,Max> >
   : public BoolConstant< And< IsStrictlyLower<MT1>, IsStrictlyLower<MT2> >::value >
{};
/*! \endcond */
//*************************************************************************************************




//=================================================================================================
//
//  ISUPPER SPECIALIZATIONS
//
//=================================================================================================

//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
template< typename MT1, typename MT2 >
struct IsUpper< DMatTDMatMapExpr<MT1,MT2,Min> >
   : public BoolConstant< And< IsUpper<MT1>, IsUpper<MT2> >::value >
{};

template< typename MT1, typename MT2 >
struct IsUpper< DMatTDMatMapExpr<MT1,MT2,Max> >
   : public BoolConstant< And< IsUpper<MT1>, IsUpper<MT2> >::value >
{};
/*! \endcond */
//*************************************************************************************************




//=================================================================================================
//
//  ISUNIUPPER SPECIALIZATIONS
//
//=================================================================================================

//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
template< typename MT1, typename MT2 >
struct IsUniUpper< DMatTDMatMapExpr<MT1,MT2,Min> >
   : public BoolConstant< And< IsUniUpper<MT1>, IsUniUpper<MT2> >::value >
{};

template< typename MT1, typename MT2 >
struct IsUniUpper< DMatTDMatMapExpr<MT1,MT2,Max> >
   : public BoolConstant< And< IsUniUpper<MT1>, IsUniUpper<MT2> >::value >
{};
/*! \endcond */
//*************************************************************************************************




//=================================================================================================
//
//  ISSTRICTLYUPPER SPECIALIZATIONS
//
//=================================================================================================

//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
template< typename MT1, typename MT2 >
struct IsStrictlyUpper< DMatTDMatMapExpr<MT1,MT2,Min> >
   : public BoolConstant< And< IsStrictlyUpper<MT1>, IsStrictlyUpper<MT2> >::value >
{};

template< typename MT1, typename MT2 >
struct IsStrictlyUpper< DMatTDMatMapExpr<MT1,MT2,Max> >
   : public BoolConstant< And< IsStrictlyUpper<MT1>, IsStrictlyUpper<MT2> >::value >
{};
/*! \endcond */
//*************************************************************************************************




//=================================================================================================
//
//  EXPRESSION TRAIT SPECIALIZATIONS
//
//=================================================================================================

//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
template< typename MT1, typename MT2, typename OP, bool AF >
struct SubmatrixExprTrait< DMatTDMatMapExpr<MT1,MT2,OP>, AF >
{
 public:
   //**********************************************************************************************
   using Type = BinaryMapExprTrait_< SubmatrixExprTrait_<const MT1,AF>
                                   , SubmatrixExprTrait_<const MT2,AF>
                                   , OP >;
   //**********************************************************************************************
};
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
template< typename MT1, typename MT2, typename OP >
struct RowExprTrait< DMatTDMatMapExpr<MT1,MT2,OP> >
{
 public:
   //**********************************************************************************************
   using Type = BinaryMapExprTrait_< RowExprTrait_<const MT1>
                                   , RowExprTrait_<const MT2>
                                   , OP >;
   //**********************************************************************************************
};
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
template< typename MT1, typename MT2, typename OP >
struct ColumnExprTrait< DMatTDMatMapExpr<MT1,MT2,OP> >
{
 public:
   //**********************************************************************************************
   using Type = BinaryMapExprTrait_< ColumnExprTrait_<const MT1>
                                   , ColumnExprTrait_<const MT2>
                                   , OP >;
   //**********************************************************************************************
};
/*! \endcond */
//*************************************************************************************************

} // namespace blaze

#endif
