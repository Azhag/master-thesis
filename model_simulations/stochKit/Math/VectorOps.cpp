//*****************************************************************************|
//*  FILE:    VectorOps.cpp
//*
//*  AUTHOR:  Andrew Hall
//*
//*  CREATED: January 20, 2003
//*
//*  LAST MODIFIED: Aug 11, 2004 
//*             BY: Yang Cao
//*             TO: Improve the efficiency of Norm() and remove Sum() 
//*  LAST MODIFIED: Nov. 4, 2004
//*             BY: Yang Cao
//*             TO: to help implement the GillespieStepsizeSelectiona, the Min 
//* 		    Max functions are added. 
//*
//*  SUMMARY:
//*
//*
//*  NOTES:
//*
//*
//*  TO DO:
//*
//*
//*****************************************************************************|
//       1         2         3         4         5         6         7         8
//345678901234567890123456789012345678901234567890123456789012345678901234567890
#include "VectorOps.h"
#include "IEEE.h"
#include "Errors.h"
#include <math.h>
#include <float.h>
#include <algorithm>


namespace CSE {
  namespace Math {

    double Norm(const Vector& v, double pNorm)
    {
      double * data = v.addr(); 
      if (pNorm > DBL_MAX) {
        // Infinity Norm
        double maxVal = 0.0;
	int tmpSize = v.Size();
		
        for (int i = 0; i < tmpSize; ++i) {
          maxVal = std::max(maxVal, fabs(data[i]));
        }
        return maxVal;
      }
      else if (pNorm < -DBL_MAX) {
        // Minus Infinity Norm
        double minVal = Inf();
	int tmpSize = v.Size();
		
        for (int i = 0; i < tmpSize; ++i) {
          minVal = std::min(minVal, fabs(data[i]));
        }
        return minVal;
      }
      else {
        // Regular p-norm
        double norm = 0.0;
	    int tmpSize = v.Size();
        for (int i = 0; i < tmpSize; ++i) {
          norm += pow(fabs(data[i]), pNorm);
        }
        return pow(norm, 1.0/pNorm);
      }
    }

    
    Vector Round(const Vector& v)
    {
      Vector rv(v);
	  int tmpSize = v.Size();
      for (int i = 0; i < tmpSize; ++i) {
        rv(i) = Round(rv(i));
      }
      return rv;
    }

    double Min(const Vector& v)
    { 
	int tmpSize = v.Size(); 
	double * data = v.addr(); 
	if ( tmpSize == 0) return NaN(); 
	else { 
	  double minvalue = data[0]; 
	  for (int i = 1; i < tmpSize; ++i) { 
	    if (minvalue > data[i]) { 
		minvalue = data[i]; 
	    } 
	  } 
	  return minvalue; 
	} 
    } 

    double Max(const Vector& v)
    {
        int tmpSize = v.Size();
        double * data = v.addr();
        if ( tmpSize == 0) return NaN();
        else { 
          double maxvalue = data[0];
          for (int i = 1; i < tmpSize; ++i) {
            if (maxvalue < data[i]) { 
                maxvalue = data[i]; 
            } 
          }
          return maxvalue; 
        } 
    }

    double InnerProduct(const Vector& lhs, const Vector& rhs)
    {
      if (lhs.Size() != rhs.Size()) {
        throw NonconformableShapesError();
      }

      return UncheckedInnerProduct(lhs, rhs);
    }


    double InnerProduct(const Vector& lhs, const RowProxy& rhs)
    {
      if (lhs.Size() != rhs.Size()) {
        throw NonconformableShapesError();
      }

      return UncheckedInnerProduct(lhs, rhs);
    }


    double InnerProduct(const Vector& lhs, const ColProxy& rhs)
    {
      if (lhs.Size() != rhs.Size()) {
        throw NonconformableShapesError();
      }

      return UncheckedInnerProduct(lhs, rhs);
    }


    double InnerProduct(const RowProxy& lhs, const RowProxy& rhs)
    {
      if (lhs.Size() != rhs.Size()) {
        throw NonconformableShapesError();
      }

      return UncheckedInnerProduct(lhs, rhs);
    }


    double InnerProduct(const RowProxy& lhs, const Vector& rhs)
    {
      if (lhs.Size() != rhs.Size()) {
        throw NonconformableShapesError();
      }

      return UncheckedInnerProduct(lhs, rhs);
    }


    double InnerProduct(const RowProxy& lhs, const ColProxy& rhs)
    {
      if (lhs.Size() != rhs.Size()) {
        throw NonconformableShapesError();
      }

      return UncheckedInnerProduct(lhs, rhs);
    }


    double InnerProduct(const ColProxy& lhs, const ColProxy& rhs)
    {
      if (lhs.Size() != rhs.Size()) {
        throw NonconformableShapesError();
      }

      return UncheckedInnerProduct(lhs, rhs);
    }


    double InnerProduct(const ColProxy& lhs, const Vector& rhs)
    {
      if (lhs.Size() != rhs.Size()) {
        throw NonconformableShapesError();
      }

      return UncheckedInnerProduct(lhs, rhs);
    }


    double InnerProduct(const ColProxy& lhs, const RowProxy& rhs)
    {
      if (lhs.Size() != rhs.Size()) {
        throw NonconformableShapesError();
      }

      return UncheckedInnerProduct(lhs, rhs);
    }
    

    double UncheckedInnerProduct(const Vector& lhs, const Vector& rhs)
    {
      const int size = lhs.Size();
      double prod = 0.0;
      for (int i = 0; i < size; ++i) {
        prod += lhs(i) * rhs(i);
      }
      return prod;
    }


    double UncheckedInnerProduct(const Vector& lhs, const RowProxy& rhs)
    {
      const int size = lhs.Size();
      double prod = 0.0;
      for (int i = 0; i < size; ++i) {
        prod += lhs(i) * rhs(i);
      }
      return prod;
    }


    double UncheckedInnerProduct(const Vector& lhs, const ColProxy& rhs)
    {
      const int size = lhs.Size();
      double prod = 0.0;
      for (int i = 0; i < size; ++i) {
        prod += lhs(i) * rhs(i);
      }
      return prod;
    }


    double UncheckedInnerProduct(const RowProxy& lhs, const RowProxy& rhs)
    {
      const int size = lhs.Size();
      double prod = 0.0;
      for (int i = 0; i < size; ++i) {
        prod += lhs(i) * rhs(i);
      }
      return prod;
    }


    double UncheckedInnerProduct(const RowProxy& lhs, const Vector& rhs)
    {
      const int size = rhs.Size();
      double prod = 0.0;
      for (int i = 0; i < size; ++i) {
        prod += lhs(i) * rhs(i);
      }
      return prod;
    }


    double UncheckedInnerProduct(const RowProxy& lhs, const ColProxy& rhs)
    {
      const int size = lhs.Size();
      double prod = 0.0;
      for (int i = 0; i < size; ++i) {
        prod += lhs(i) * rhs(i);
      }
      return prod;
    }


    double UncheckedInnerProduct(const ColProxy& lhs, const ColProxy& rhs)
    {
      const int size = lhs.Size();
      double prod = 0.0;
      for (int i = 0; i < size; ++i) {
        prod += lhs(i) * rhs(i);
      }
      return prod;
    }


    double UncheckedInnerProduct(const ColProxy& lhs, const Vector& rhs)
    {
      const int size = rhs.Size();
      double prod = 0.0;
      for (int i = 0; i < size; ++i) {
        prod += lhs(i) * rhs(i);
      }
      return prod;
    }


    double UncheckedInnerProduct(const ColProxy& lhs, const RowProxy& rhs)
    {
      const int size = lhs.Size();
      double prod = 0.0;
      for (int i = 0; i < size; ++i) {
        prod += lhs(i) * rhs(i);
      }
      return prod;
    }

    
  } // Close CSE::Math namespace
} // Close CSE namespace
