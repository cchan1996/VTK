/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkMath.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) 1993-2002 Ken Martin, Will Schroeder, Bill Lorensen 
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#include "vtkMath.h"
#include "vtkObjectFactory.h"

vtkCxxRevisionMacro(vtkMath, "1.74");
vtkStandardNewMacro(vtkMath);

long vtkMath::Seed = 1177; // One authors home address

//
// some constants we need
//
#define VTK_K_A 16807
#define VTK_K_M 2147483647                      /* Mersenne prime 2^31 -1 */
#define VTK_K_Q 127773                  /* VTK_K_M div VTK_K_A */
#define VTK_K_R 2836                    /* VTK_K_M mod VTK_K_A */
//
// Some useful macros
//
#define VTK_SIGN(x)              (( (x) < 0 )?( -1 ):( 1 ))
// avoid dll boundary problems

// Generate random numbers between 0.0 and 1.0.
// This is used to provide portability across different systems.
float vtkMath::Random()
{
  long hi, lo;
    
  // Based on code in "Random Number Generators: Good Ones are Hard to Find,"
  // by Stephen K. Park and Keith W. Miller in Communications of the ACM,
  // 31, 10 (Oct. 1988) pp. 1192-1201.
  // Borrowed from: Fuat C. Baran, Columbia University, 1988.
  hi = vtkMath::Seed / VTK_K_Q;
  lo = vtkMath::Seed % VTK_K_Q;
  if ((vtkMath::Seed = VTK_K_A * lo - VTK_K_R * hi) <= 0)
    {
    Seed += VTK_K_M;
    }
  return ((float) vtkMath::Seed / VTK_K_M);
}

// Initialize seed value. NOTE: Random() has the bad property that 
// the first random number returned after RandomSeed() is called 
// is proportional to the seed value! To help solve this, call 
// RandomSeed() a few times inside seed. This doesn't ruin the 
// repeatability of Random().
//
void vtkMath::RandomSeed(long s)
{
  vtkMath::Seed = s;

  vtkMath::Random();
  vtkMath::Random();
  vtkMath::Random();
}

// Find unit vectors which is perpendicular to this on and to
// each other.
void vtkMath::Perpendiculars(const double x[3], double y[3], double z[3],
                             double theta)
{
  int dx,dy,dz;
  double x2 = x[0]*x[0];
  double y2 = x[1]*x[1];
  double z2 = x[2]*x[2];
  double r = sqrt(x2 + y2 + z2);

  // transpose the vector to avoid divide-by-zero error
  if (x2 > y2 && x2 > z2)
  {
    dx = 0; dy = 1; dz = 2;
  }
  else if (y2 > z2) 
  {
    dx = 1; dy = 2; dz = 0;
  }
  else 
  {
    dx = 2; dy = 0; dz = 1;
  }

  double a = x[dx]/r;
  double b = x[dy]/r;
  double c = x[dz]/r;

  double tmp = sqrt(a*a+c*c);

  if (theta != 0)
    {
    double sintheta = sin(theta);
    double costheta = cos(theta);

    if (y)
    {
      y[dx] = (c*costheta - a*b*sintheta)/tmp;
      y[dy] = sintheta*tmp;
      y[dz] = (-a*costheta - b*c*sintheta)/tmp;
    }

    if (z)
      {
      z[dx] = (-c*sintheta - a*b*costheta)/tmp;
      z[dy] = costheta*tmp;
      z[dz] = (a*sintheta - b*c*costheta)/tmp;
      }
    }
  else
    {
    if (y)
    {
      y[dx] = c/tmp;
      y[dy] = 0;
      y[dz] = -a/tmp;
    }

    if (z)
      {
      z[dx] = -a*b/tmp;
      z[dy] = tmp;
      z[dz] = -b*c/tmp;
      }
    }      
}

// Find unit vectors which are perpendicular to this one and to
// each other.
void vtkMath::Perpendiculars(const float x[3], float y[3], float z[3],
                             double theta)
{
  int dx,dy,dz;
  double x2 = x[0]*x[0];
  double y2 = x[1]*x[1];
  double z2 = x[2]*x[2];
  double r = sqrt(x2 + y2 + z2);

  // transpose the vector to avoid divide-by-zero error
  if (x2 > y2 && x2 > z2)
  {
    dx = 0; dy = 1; dz = 2;
  }
  else if (y2 > z2) 
  {
    dx = 1; dy = 2; dz = 0;
  }
  else 
  {
    dx = 2; dy = 0; dz = 1;
  }

  double a = x[dx]/r;
  double b = x[dy]/r;
  double c = x[dz]/r;

  double tmp = sqrt(a*a+c*c);

  if (theta != 0)
    {
    double sintheta = sin(theta);
    double costheta = cos(theta);

    if (y)
    {
      y[dx] = (c*costheta - a*b*sintheta)/tmp;
      y[dy] = sintheta*tmp;
      y[dz] = (-a*costheta - b*c*sintheta)/tmp;
    }

    if (z)
      {
      z[dx] = (-c*sintheta - a*b*costheta)/tmp;
      z[dy] = costheta*tmp;
      z[dz] = (a*sintheta - b*c*costheta)/tmp;
      }
    }
  else
    {
    if (y)
    {
      y[dx] = c/tmp;
      y[dy] = 0;
      y[dz] = -a/tmp;
    }

    if (z)
      {
      z[dx] = -a*b/tmp;
      z[dy] = tmp;
      z[dz] = -b*c/tmp;
      }
    }      
}

#define VTK_SMALL_NUMBER 1.0e-12

// Solve linear equations Ax = b using Crout's method. Input is square matrix A
// and load vector x. Solution x is written over load vector. The dimension of
// the matrix is specified in size. If error is found, method returns a 0.
int vtkMath::SolveLinearSystem(double **A, double *x, int size)
{
  static int *index = NULL, maxSize=0;

  // if we solving something simple, just solve it
  //
  if (size == 2)
    {
    double det, y[2];

    det = vtkMath::Determinant2x2(A[0][0], A[0][1], A[1][0], A[1][1]);

    if (det == 0.0)
      {
      return 0;
      }

    y[0] = (A[1][1]*x[0] - A[0][1]*x[1]) / det;
    y[1] = (-A[1][0]*x[0] + A[0][0]*x[1]) / det;

    x[0] = y[0];
    x[1] = y[1];
    return 1;
    }
  else if (size == 1)
    {
    if (A[0][0] == 0.0)
      {
      return 0;
      }
    
    x[0] /= A[0][0];
    return 1;
    }

  //
  // System of equations is not trivial, use Crout's method
  //
  
  
  //
  // Check on allocation of working vectors
  //
  if ( index == NULL ) 
    {
    index = new int[size];
    maxSize = size;
    } 
  else if ( size > maxSize ) 
    {
    delete [] index;
    index = new int[size];
    maxSize = size;
    }
  //
  // Factor and solve matrix
  //
  if ( vtkMath::LUFactorLinearSystem(A, index, size) == 0 )
    {
    return 0;
    }
  vtkMath::LUSolveLinearSystem(A,index,x,size);

  return 1;
}


// Invert input square matrix A into matrix AI. Note that A is modified during
// the inversion. The size variable is the dimension of the matrix. Returns 0
// if inverse not computed.
int vtkMath::InvertMatrix(double **A, double **AI, int size)
{
  static int *index = NULL, maxSize=0;
  static double *column = NULL;

  //
  // Check on allocation of working vectors
  //
  if ( index == NULL ) 
    {
    index = new int[size];
    column = new double[size];
    maxSize = size;
    } 
  else if ( size > maxSize ) 
    {
    delete [] index; delete [] column;
    index = new int[size];
    column = new double[size];
    maxSize = size;
    }
  return vtkMath::InvertMatrix(A, AI, size, index, column);
}

// Factor linear equations Ax = b using LU decompostion A = LU where L is
// lower triangular matrix and U is upper triangular matrix. Input is 
// square matrix A, integer array of pivot indices index[0->n-1], and size
// of square matrix n. Output factorization LU is in matrix A. If error is 
// found, method returns 0. 
int vtkMath::LUFactorLinearSystem(double **A, int *index, int size)
{
  double *scale = new double[size];
  int i, j, k;
  int maxI = 0;
  double largest, temp1, temp2, sum;

  //
  // Loop over rows to get implicit scaling information
  //
  for ( i = 0; i < size; i++ ) 
    {
    for ( largest = 0.0, j = 0; j < size; j++ ) 
      {
      if ( (temp2 = fabs(A[i][j])) > largest )
        {
        largest = temp2;
        }
      }

    if ( largest == 0.0 )
      {
      return 0;
      }
      scale[i] = 1.0 / largest;
    }
  //
  // Loop over all columns using Crout's method
  //
  for ( j = 0; j < size; j++ ) 
    {
    for (i = 0; i < j; i++) 
      {
      sum = A[i][j];
      for ( k = 0; k < i; k++ )
        {
        sum -= A[i][k] * A[k][j];
        }
      A[i][j] = sum;
      }
    //
    // Begin search for largest pivot element
    //
    for ( largest = 0.0, i = j; i < size; i++ ) 
      {
      sum = A[i][j];
      for ( k = 0; k < j; k++ )
        {
        sum -= A[i][k] * A[k][j];
        }
      A[i][j] = sum;

      if ( (temp1 = scale[i]*fabs(sum)) >= largest ) 
        {
        largest = temp1;
        maxI = i;
        }
      }
    //
    // Check for row interchange
    //
    if ( j != maxI ) 
      {
      for ( k = 0; k < size; k++ ) 
        {
        temp1 = A[maxI][k];
        A[maxI][k] = A[j][k];
        A[j][k] = temp1;
        }
      scale[maxI] = scale[j];
      }
    //
    // Divide by pivot element and perform elimination
    //
    index[j] = maxI;

    if ( fabs(A[j][j]) <= VTK_SMALL_NUMBER )
      {
      return 0;
      }

    if ( j != (size-1) ) 
      {
      temp1 = 1.0 / A[j][j];
      for ( i = j + 1; i < size; i++ )
        {
        A[i][j] *= temp1;
        }
      }
    }

  delete [] scale;

  return 1;
}


// Solve linear equations Ax = b using LU decompostion A = LU where L is
// lower triangular matrix and U is upper triangular matrix. Input is 
// factored matrix A=LU, integer array of pivot indices index[0->n-1],
// load vector x[0->n-1], and size of square matrix n. Note that A=LU and
// index[] are generated from method LUFactorLinearSystem). Also, solution
// vector is written directly over input load vector.
void vtkMath::LUSolveLinearSystem(double **A, int *index, 
                                  double *x, int size)
{
  int i, j, ii, idx;
  double sum;
//
// Proceed with forward and backsubstitution for L and U
// matrices.  First, forward substitution.
//
  for ( ii = -1, i = 0; i < size; i++ ) 
    {
    idx = index[i];
    sum = x[idx];
    x[idx] = x[i];

    if ( ii >= 0 )
      {
      for ( j = ii; j <= (i-1); j++ )
        {
        sum -= A[i][j]*x[j];
        }
      }
    else if (sum)
      {
      ii = i;
      }

    x[i] = sum;
  }
//
// Now, back substitution
//
  for ( i = size-1; i >= 0; i-- ) 
    {
    sum = x[i];
    for ( j = i + 1; j < size; j++ )
      {
      sum -= A[i][j]*x[j];
      }
    x[i] = sum / A[i][i];
    }
}

#undef VTK_SMALL_NUMBER

#define VTK_ROTATE(a,i,j,k,l) g=a[i][j];h=a[k][l];a[i][j]=g-s*(h+g*tau);\
        a[k][l]=h+s*(g-h*tau);

#define VTK_MAX_ROTATIONS 20

//#undef VTK_MAX_ROTATIONS

//#define VTK_MAX_ROTATIONS 50

// Jacobi iteration for the solution of eigenvectors/eigenvalues of a nxn
// real symmetric matrix. Square nxn matrix a; size of matrix in n;
// output eigenvalues in w; and output eigenvectors in v. Resulting
// eigenvalues/vectors are sorted in decreasing order; eigenvectors are
// normalized.
template<class T>
static inline int vtkJacobiN(T **a, int n, T *w, T **v)
{
  int i, j, k, iq, ip, numPos;
  T tresh, theta, tau, t, sm, s, h, g, c, tmp;
  T bspace[4], zspace[4];
  T *b = bspace;
  T *z = zspace;

  // only allocate memory if the matrix is large
  if (n > 4)
    {
    b = new T[n];
    z = new T[n]; 
    }

  // initialize
  for (ip=0; ip<n; ip++) 
    {
    for (iq=0; iq<n; iq++)
      {
      v[ip][iq] = 0.0;
      }
    v[ip][ip] = 1.0;
    }
  for (ip=0; ip<n; ip++) 
    {
    b[ip] = w[ip] = a[ip][ip];
    z[ip] = 0.0;
    }

  // begin rotation sequence
  for (i=0; i<VTK_MAX_ROTATIONS; i++) 
    {
    sm = 0.0;
    for (ip=0; ip<n-1; ip++) 
      {
      for (iq=ip+1; iq<n; iq++)
        {
        sm += fabs(a[ip][iq]);
        }
      }
    if (sm == 0.0)
      {
      break;
      }

    if (i < 3)                                // first 3 sweeps
      {
      tresh = 0.2*sm/(n*n);
      }
    else
      {
      tresh = 0.0;
      }

    for (ip=0; ip<n-1; ip++) 
      {
      for (iq=ip+1; iq<n; iq++) 
        {
        g = 100.0*fabs(a[ip][iq]);

        // after 4 sweeps
        if (i > 3 && (fabs(w[ip])+g) == fabs(w[ip])
        && (fabs(w[iq])+g) == fabs(w[iq]))
          {
          a[ip][iq] = 0.0;
          }
        else if (fabs(a[ip][iq]) > tresh) 
          {
          h = w[iq] - w[ip];
          if ( (fabs(h)+g) == fabs(h))
            {
            t = (a[ip][iq]) / h;
            }
          else 
            {
            theta = 0.5*h / (a[ip][iq]);
            t = 1.0 / (fabs(theta)+sqrt(1.0+theta*theta));
            if (theta < 0.0)
              {
              t = -t;
              }
            }
          c = 1.0 / sqrt(1+t*t);
          s = t*c;
          tau = s/(1.0+c);
          h = t*a[ip][iq];
          z[ip] -= h;
          z[iq] += h;
          w[ip] -= h;
          w[iq] += h;
          a[ip][iq]=0.0;

          // ip already shifted left by 1 unit
          for (j = 0;j <= ip-1;j++) 
            {
            VTK_ROTATE(a,j,ip,j,iq)
              }
          // ip and iq already shifted left by 1 unit
          for (j = ip+1;j <= iq-1;j++) 
            {
            VTK_ROTATE(a,ip,j,j,iq)
              }
          // iq already shifted left by 1 unit
          for (j=iq+1; j<n; j++) 
            {
            VTK_ROTATE(a,ip,j,iq,j)
              }
          for (j=0; j<n; j++) 
            {
            VTK_ROTATE(v,j,ip,j,iq)
            }
          }
        }
      }

    for (ip=0; ip<n; ip++) 
      {
      b[ip] += z[ip];
      w[ip] = b[ip];
      z[ip] = 0.0;
      }
    }

  //// this is NEVER called
  if ( i >= VTK_MAX_ROTATIONS )
    {
    vtkGenericWarningMacro(
       "vtkMath::Jacobi: Error extracting eigenfunctions");
    return 0;
    }

  // sort eigenfunctions                 these changes do not affect accuracy 
  for (j=0; j<n-1; j++)                  // boundary incorrect
    {
    k = j;
    tmp = w[k];
    for (i=j+1; i<n; i++)                // boundary incorrect, shifted already
      {
      if (w[i] >= tmp)                   // why exchage if same?
        {
        k = i;
        tmp = w[k];
        }
      }
    if (k != j) 
      {
      w[k] = w[j];
      w[j] = tmp;
      for (i=0; i<n; i++) 
        {
        tmp = v[i][j];
        v[i][j] = v[i][k];
        v[i][k] = tmp;
        }
      }
    }
  // insure eigenvector consistency (i.e., Jacobi can compute vectors that
  // are negative of one another (.707,.707,0) and (-.707,-.707,0). This can
  // reek havoc in hyperstreamline/other stuff. We will select the most
  // positive eigenvector.
  for (j=0; j<n; j++)
    {
    for (numPos=0, i=0; i<n; i++)
      {
      if ( v[i][j] >= 0.0 )
        {
        numPos++;
        }
      }
    if ( numPos < ceil(double(n)/double(2.0)) )
      {
      for(i=0; i<n; i++)
        {
        v[i][j] *= -1.0;
        }
      }
    }

  if (n > 4)
    {
    delete [] b;
    delete [] z;
    }
  return 1;
}

#undef VTK_ROTATE
#undef VTK_MAX_ROTATIONS

int vtkMath::JacobiN(float **a, int n, float *w, float **v)
{
  return vtkJacobiN(a,n,w,v);
}

int vtkMath::JacobiN(double **a, int n, double *w, double **v)
{
  return vtkJacobiN(a,n,w,v);
}


// Jacobi iteration for the solution of eigenvectors/eigenvalues of a 3x3
// real symmetric matrix. Square 3x3 matrix a; output eigenvalues in w;
// and output eigenvectors in v. Resulting eigenvalues/vectors are sorted
// in decreasing order; eigenvectors are normalized.
int vtkMath::Jacobi(float **a, float *w, float **v)
{
  return vtkMath::JacobiN(a, 3, w, v);
}

int vtkMath::Jacobi(double **a, double *w, double **v)
{
  return vtkMath::JacobiN(a, 3, w, v);
}

// Estimate the condition number of a LU factored matrix. Used to judge the
// accuracy of the solution. The matrix A must have been previously factored
// using the method LUFactorLinearSystem. The condition number is the ratio
// of the infinity matrix norm (i.e., maximum value of matrix component)
// divided by the minimum diagonal value. (This works for triangular matrices
// only: see Conte and de Boor, Elementary Numerical Analysis.)
double vtkMath::EstimateMatrixCondition(double **A, int size)
{
  int i;
  int j = 0;
  double min=VTK_LARGE_FLOAT, max=(-VTK_LARGE_FLOAT);

  // find the maximum value
  for (i=0; i < size; i++)
    {
    for (j=i; j < size; j++)
      {
      if ( fabs(A[i][j]) > max )
        {
        max = fabs(A[i][j]);
        }
      }
    }

  // find the minimum diagonal value
  for (i=0; i < size; i++)
    {
    if ( fabs(A[i][i]) < min )
      {
      min = fabs(A[i][i]);
      }
    }

  if ( min == 0.0 )
    {
    return VTK_LARGE_FLOAT;
    }
  else
    {
    return (max/min);
    }
}

// Solves a cubic equation c0*t^3  + c1*t^2  + c2*t + c3 = 0 when
// c0, c1, c2, and c3 are REAL.
// Solution is motivated by Numerical Recipes In C 2nd Ed.
// Return array contains number of (real) roots (counting multiple roots as one)
// followed by roots themselves. The value in roots[4] is a integer giving
// further information about the roots (see return codes for int SolveCubic()).
double* vtkMath::SolveCubic( double c0, double c1, double c2, double c3 ) 
{
  static double roots[5];
  roots[1] = 0.0;
  roots[2] = 0.0;
  roots[3] = 0.0;
  int num_roots;

  roots[4] = vtkMath::SolveCubic(c0, c1, c2, c3, 
                                 &roots[1], &roots[2], &roots[3], &num_roots );
  roots[0] = num_roots;
  return roots;
}

// Solves a cubic equation when c0, c1, c2, And c3 Are REAL.  Solution
// is motivated by Numerical Recipes In C 2nd Ed.  Roots and number of
// real roots are stored in user provided variables r1, r2, r3, and
// num_roots. Note that the function can return the following integer
// values describing the roots: (0)-no solution; (-1)-infinite number
// of solutions; (1)-one distinct real root of multiplicity 3 (stored
// in r1); (2)-two distinct real roots, one of multiplicity 2 (stored
// in r1 & r2); (3)-three distinct real roots; (-2)-quadratic equation
// with complex conjugate solution (real part of root returned in r1,
// imaginary in r2); (-3)-one real root and a complex conjugate pair
// (real root in r1 and real part of pair in r2 and imaginary in r3).
int vtkMath::SolveCubic( double c0, double c1, double c2, double c3, 
                         double *r1, double *r2, double *r3, int *num_roots )
{
  double        Q, R;
  double        R_squared;      /* R*R */
  double        Q_cubed;        /* Q*Q*Q */
  double        theta;
  double        A, B;

  // Cubic equation: c0*t^3  + c1*t^2  + c2*t + c3 = 0 
  //                                               
  //   r1, r2, r3 are roots and num_roots is the number
  //   of real roots                               

  // Make Sure This Is A Bonafide Cubic Equation 
  if( c0 != 0.0 )
    {
    //Put Coefficients In Right Form 
    c1 = c1/c0;
    c2 = c2/c0;
    c3 = c3/c0;

    Q = ((c1*c1) - 3*c2)/9.0;

    R = (2.0*(c1*c1*c1) - 9.0*(c1*c2) + 27.0*c3)/54.0;

    R_squared = R*R;
    Q_cubed   = Q*Q*Q;

    if( R_squared <= Q_cubed )
      {
      if( Q_cubed == 0.0 )
        {
        *r1 = -c1/3.0;
        *r2 = *r1;
        *r3 = *r1;
        *num_roots = 1;
        return 1;
        } 
      else
        {
        theta = acos( R / (sqrt(Q_cubed) ) );

        *r1 = -2.0*sqrt(Q)*cos( theta/3.0 ) - c1/3.0;
        *r2 = -2.0*sqrt(Q)*cos( (theta + 2.0*3.141592653589)/3.0) - c1/3.0;
        *r3 = -2.0*sqrt(Q)*cos( (theta - 2.0*3.141592653589)/3.0) - c1/3.0;

        *num_roots = 3;

        // Reduce Number Of Roots To Two 
        if( *r1 == *r2 )
          {
          *num_roots = 2;
          *r2 = *r3;
          }
        else if( *r1 == *r3 )
          {
          *num_roots = 2;
          }

        if( (*r2 == *r3) && (*num_roots == 3) )
          {
          *num_roots = 2;
          }

        // Reduce Number Of Roots To One 
        if( (*r1 == *r2) )
          {
          *num_roots = 1;
          }
        }
      return *num_roots;
      }
    else //single real and complex conjugate pair
      {
      A = -VTK_SIGN(R) * pow(fabs(R) + sqrt(R_squared - Q_cubed),0.33333333);

      if( A == 0.0 )
        {
        B = 0.0;
        }
      else
        {
        B = Q/A;
        }

      *r1 =  (A + B) - c1/3.0;
      *r2 = -0.5*(A + B) - c1/3.0;
      *r3 = sqrt(3.0)/2.0*(A - B);

      *num_roots = 1;
      return (-3);
      }
    } //if cubic equation

  else // Quadratic Equation: c1*t  + c2*t + c3 = 0 
    {
    // Okay this was not a cubic - lets try quadratic
    return vtkMath::SolveQuadratic( c1, c2, c3, r1, r2, num_roots );
    }
}

// Solves a quadratic equation c1*t^2 + c2*t + c3 = 0 when c1, c2, and
// c3 are REAL.  Solution is motivated by Numerical Recipes In C 2nd
// Ed.  Return array contains number of (real) roots (counting
// multiple roots as one) followed by roots themselves. Note that 
// roots[3] contains a return code further describing solution - see
// documentation for SolveCubic() for meaining of return codes.
double* vtkMath::SolveQuadratic( double c1, double c2, double c3) 
{
  static double roots[4];
  roots[0] = 0.0;
  roots[1] = 0.0;
  roots[2] = 0.0;
  int num_roots;

  roots[3] = vtkMath::SolveQuadratic( c1, c2, c3, &roots[1], &roots[2], 
                                      &num_roots );
  roots[0] = num_roots;
  return roots;
}

// Solves A Quadratic Equation c1*t^2  + c2*t  + c3 = 0 when 
// c1, c2, and c3 are REAL.
// Solution is motivated by Numerical Recipes In C 2nd Ed.
// Roots and number of roots are stored in user provided variables
// r1, r2, num_roots
int vtkMath::SolveQuadratic( double c1, double c2, double c3, 
                             double *r1, double *r2, int *num_roots )
{
  double        Q;
  double        determinant;

  // Quadratic equation: c1*t^2 + c2*t + c3 = 0 

  // Make sure this is a quadratic equation
  if( c1 != 0.0 )
    {
    determinant = c2*c2 - 4*c1*c3;

    if( determinant >= 0.0 )
      {
      Q = -0.5 * (c2 + VTK_SIGN(c2)*sqrt(determinant));

      *r1 = Q / c1;

      if( Q == 0.0 )
        {
        *r2 = 0.0;
        }
      else
        {
        *r2 = c3 / Q;
        }

      *num_roots = 2;

      // Reduce Number Of Roots To One 
      if( *r1 == *r2 )
        {
        *num_roots = 1;
        }
      return *num_roots;
      }
    else        // Equation Does Not Have Real Roots 
      {
      *num_roots = 0;
      return (-2);
      }
    }

  else // Linear Equation: c2*t + c3 = 0 
    {
    // Okay this was not quadratic - lets try linear
    return vtkMath::SolveLinear( c2, c3, r1, num_roots );
    }
}

// Solves a linear equation c2*t  + c3 = 0 when c2 and c3 are REAL.
// Solution is motivated by Numerical Recipes In C 2nd Ed.
// Return array contains number of roots followed by roots themselves.
double* vtkMath::SolveLinear( double c2, double c3) 
{
  static double roots[3];
  int num_roots;
  roots[1] = 0.0;
  roots[2] = vtkMath::SolveLinear( c2, c3, &roots[1], &num_roots );
  roots[0] = num_roots;
  return roots;
}

// Solves a linear equation c2*t + c3 = 0 when c2 and c3 are REAL.
// Solution is motivated by Numerical Recipes In C 2nd Ed.
// Root and number of (real) roots are stored in user provided variables
// r2 and num_roots.
int vtkMath::SolveLinear( double c2, double c3, double *r1, int *num_roots )
{
  // Linear equation: c2*t + c3 = 0 
  // Now this had better be linear 
  if( c2 != 0.0 )
    {
    *r1 = -c3 / c2;
    *num_roots = 1;
    return *num_roots;
    }
  else
    {
    *num_roots = 0;
    if ( c3 == 0.0 )
      {
      return (-1);
      }
    }

  return *num_roots;
}

// Solves for the least squares best fit matrix for the equation X'M' = Y'.
// Uses pseudoinverse to get the ordinary least squares. 
// The inputs and output are transposed matrices.
//    Dimensions: X' is numberOfSamples by xOrder,
//                Y' is numberOfSamples by yOrder,
//                M' dimension is xOrder by yOrder.
// M' should be pre-allocated. All matrices are row major. The resultant
// matrix M' should be pre-multiplied to X' to get Y', or transposed and
// then post multiplied to X to get Y
int vtkMath::SolveLeastSquares(int numberOfSamples, double **xt, int xOrder,
                               double **yt, int yOrder, double **mt)
{
  // check dimensional consistency
  if ((numberOfSamples < xOrder) || (numberOfSamples < yOrder))
    {
    vtkGenericWarningMacro("Insufficient number of samples. Underdetermined.");
    return 0;
    }

  int i, j, k;

  // set up intermediate variables
  double **XXt = new double *[xOrder];     // size x by x
  double **XXtI = new double *[xOrder];    // size x by x
  double **XYt = new double *[xOrder];     // size x by y
  for (i = 0; i < xOrder; i++)
    {
    XXt[i] = new double[xOrder];
    XXtI[i] = new double[xOrder];

    for (j = 0; j < xOrder; j++)
      {
      XXt[i][j] = 0.0;
      XXtI[i][j] = 0.0;
      }

    XYt[i] = new double[yOrder];
    for (j = 0; j < yOrder; j++)
      {
      XYt[i][j] = 0.0;
      }
    }

  // first find the pseudoinverse matrix
  for (k = 0; k < numberOfSamples; k++)
    {
    for (i = 0; i < xOrder; i++)
      {
      // first calculate the XXt matrix, only do the upper half (symmetrical)
      for (j = i; j < xOrder; j++)
        {
        XXt[i][j] += xt[k][i] * xt[k][j];
        }

      // now calculate the XYt matrix
      for (j = 0; j < yOrder; j++)
        {
        XYt[i][j] += xt[k][i] * yt[k][j];
        }
      }
    }
  
  // now fill in the lower half of the XXt matrix
  for (i = 0; i < xOrder; i++)
    {
    for (j = 0; j < i; j++)
      {
      XXt[i][j] = XXt[j][i];
      }
    }
  
  // next get the inverse of XXt
  if (!(vtkMath::InvertMatrix(XXt, XXtI, xOrder)))
    {
    return 0;
    }
  
  // next get m
  for (i = 0; i < xOrder; i++)
    {
    for (j = 0; j < yOrder; j++)
      {
      mt[i][j] = 0.0;
      for (k = 0; k < xOrder; k++)
        {
        mt[i][j] += XXtI[i][k] * XYt[k][j];
        }
      }
    }

  // clean up:
  // set up intermediate variables
  for (i = 0; i < xOrder; i++)
    {
    delete [] XXt[i];
    delete [] XXtI[i];

    delete [] XYt[i];
    }
  delete [] XXt;
  delete [] XXtI;
  delete [] XYt;
  
  return 1;
}

//=============================================================================
// Thread safe versions of math methods.
//=============================================================================


// Invert input square matrix A into matrix AI. Note that A is modified during
// the inversion. The size variable is the dimension of the matrix. Returns 0
// if inverse not computed.
// -----------------------
// For thread safe behavior, temporary arrays tmp1SIze and tmp2Size
// of length size must be passsed in.
int vtkMath::InvertMatrix(double **A, double **AI, int size,
                          int *tmp1Size, double *tmp2Size)
{
  int i, j;

  //
  // Factor matrix; then begin solving for inverse one column at a time.
  // Note: tmp1Size returned value is used later, tmp2Size is just working
  // memory whose values are not used in LUSolveLinearSystem
  //
  if ( vtkMath::LUFactorLinearSystem(A, tmp1Size, size, tmp2Size) == 0 )
    {
    return 0;
    }
  
  for ( j=0; j < size; j++ )
    {
    for ( i=0; i < size; i++ )
      {
      tmp2Size[i] = 0.0;
      }
    tmp2Size[j] = 1.0;

    vtkMath::LUSolveLinearSystem(A,tmp1Size,tmp2Size,size);

    for ( i=0; i < size; i++ )
      {
      AI[i][j] = tmp2Size[i];
      }
    }

  return 1;
}



#define VTK_SMALL_NUMBER 1.0e-12

// Factor linear equations Ax = b using LU decompostion A = LU where L is
// lower triangular matrix and U is upper triangular matrix. Input is 
// square matrix A, integer array of pivot indices index[0->n-1], and size
// of square matrix n. Output factorization LU is in matrix A. If error is 
// found, method returns 0.
//------------------------------------------------------------------
// For thread safe, temporary memory array tmpSize of length size
// must be passed in.
int vtkMath::LUFactorLinearSystem(double **A, int *index, int size,
                                  double *tmpSize)
{
  int i, j, k;
  int maxI = 0;
  double largest, temp1, temp2, sum;

  //
  // Loop over rows to get implicit scaling information
  //
  for ( i = 0; i < size; i++ ) 
    {
    for ( largest = 0.0, j = 0; j < size; j++ ) 
      {
      if ( (temp2 = fabs(A[i][j])) > largest )
        {
        largest = temp2;
        }
      }

    if ( largest == 0.0 )
      {
      return 0;
      }
      tmpSize[i] = 1.0 / largest;
    }
  //
  // Loop over all columns using Crout's method
  //
  for ( j = 0; j < size; j++ ) 
    {
    for (i = 0; i < j; i++) 
      {
      sum = A[i][j];
      for ( k = 0; k < i; k++ )
        {
        sum -= A[i][k] * A[k][j];
        }
      A[i][j] = sum;
      }
    //
    // Begin search for largest pivot element
    //
    for ( largest = 0.0, i = j; i < size; i++ ) 
      {
      sum = A[i][j];
      for ( k = 0; k < j; k++ )
        {
        sum -= A[i][k] * A[k][j];
        }
      A[i][j] = sum;

      if ( (temp1 = tmpSize[i]*fabs(sum)) >= largest ) 
        {
        largest = temp1;
        maxI = i;
        }
      }
    //
    // Check for row interchange
    //
    if ( j != maxI ) 
      {
      for ( k = 0; k < size; k++ ) 
        {
        temp1 = A[maxI][k];
        A[maxI][k] = A[j][k];
        A[j][k] = temp1;
        }
      tmpSize[maxI] = tmpSize[j];
      }
    //
    // Divide by pivot element and perform elimination
    //
    index[j] = maxI;

    if ( fabs(A[j][j]) <= VTK_SMALL_NUMBER )
      {
      return 0;
      }

    if ( j != (size-1) ) 
      {
      temp1 = 1.0 / A[j][j];
      for ( i = j + 1; i < size; i++ )
        {
        A[i][j] *= temp1;
        }
      }
    }

  return 1;
}

#undef VTK_SMALL_NUMBER

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
// All of the following methods are for dealing with 3x3 matrices
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// helper function, swap two 3-vectors
template<class T>
static inline void vtkSwapVectors3(T v1[3], T v2[3])
{
  for (int i = 0; i < 3; i++)
    {
    T tmp = v1[i];
    v1[i] = v2[i];
    v2[i] = tmp;
    }
}

//----------------------------------------------------------------------------
// Unrolled LU factorization of a 3x3 matrix with pivoting.
// This decomposition is non-standard in that the diagonal
// elements are inverted, to convert a division to a multiplication
// in the backsubstitution.
template<class T>
static inline void vtkLUFactor3x3(T A[3][3], int index[3])
{
  int i,maxI;
  T tmp,largest;
  T scale[3];

  // Loop over rows to get implicit scaling information

  for ( i = 0; i < 3; i++ ) 
    {
    largest =  fabs(A[i][0]);
    if ((tmp = fabs(A[i][1])) > largest)
      {
      largest = tmp;
      }
    if ((tmp = fabs(A[i][2])) > largest)
      {
      largest = tmp;
      }
    scale[i] = T(1.0)/largest;
    }
  
  // Loop over all columns using Crout's method

  // first column
  largest = scale[0]*fabs(A[0][0]);
  maxI = 0;
  if ((tmp = scale[1]*fabs(A[1][0])) >= largest) 
    {
    largest = tmp;
    maxI = 1;
    }
  if ((tmp = scale[2]*fabs(A[2][0])) >= largest) 
    {
    maxI = 2;
    }
  if (maxI != 0) 
    {
    vtkSwapVectors3(A[maxI],A[0]);
    scale[maxI] = scale[0];
    }
  index[0] = maxI;

  A[0][0] = T(1.0)/A[0][0];
  A[1][0] *= A[0][0];
  A[2][0] *= A[0][0];
    
  // second column
  A[1][1] -= A[1][0]*A[0][1];
  A[2][1] -= A[2][0]*A[0][1];
  largest = scale[1]*fabs(A[1][1]);
  maxI = 1;
  if ((tmp = scale[2]*fabs(A[2][1])) >= largest) 
    {
    maxI = 2;
    vtkSwapVectors3(A[2],A[1]);
    scale[2] = scale[1];
    }
  index[1] = maxI;
  A[1][1] = T(1.0)/A[1][1];
  A[2][1] *= A[1][1];

  // third column
  A[1][2] -= A[1][0]*A[0][2];
  A[2][2] -= A[2][0]*A[0][2] + A[2][1]*A[1][2];
  largest = scale[2]*fabs(A[2][2]);
  index[2] = 2;
  A[2][2] = T(1.0)/A[2][2];
}

void vtkMath::LUFactor3x3(float A[3][3], int index[3])
{
  vtkLUFactor3x3(A,index);
}

void vtkMath::LUFactor3x3(double A[3][3], int index[3])
{
  vtkLUFactor3x3(A,index);
}

//----------------------------------------------------------------------------
// Backsubsitution with an LU-decomposed matrix.  This is the standard
// LU decomposition, except that the diagonals elements have been inverted.
template<class T1, class T2>
static inline void vtkLUSolve3x3(const T1 A[3][3], const int index[3], T2 x[3])
{
  T2 sum;

  // forward substitution
  
  sum = x[index[0]];
  x[index[0]] = x[0];
  x[0] = sum;

  sum = x[index[1]];
  x[index[1]] = x[1];
  x[1] = sum - A[1][0]*x[0];

  sum = x[index[2]];
  x[index[2]] = x[2];
  x[2] = sum - A[2][0]*x[0] - A[2][1]*x[1];

  // back substitution
  
  x[2] = x[2]*A[2][2];
  x[1] = (x[1] - A[1][2]*x[2])*A[1][1];
  x[0] = (x[0] - A[0][1]*x[1] - A[0][2]*x[2])*A[0][0];
}  

void vtkMath::LUSolve3x3(const float A[3][3], 
                         const int index[3], float x[3])
{
  vtkLUSolve3x3(A,index,x);
}

void vtkMath::LUSolve3x3(const double A[3][3], 
                         const int index[3], double x[3])
{
  vtkLUSolve3x3(A,index,x);
}

//----------------------------------------------------------------------------
// this method solves Ay = x for y
template<class T1, class T2, class T3>
static inline void vtkLinearSolve3x3(const T1 A[3][3], const T2 x[3], T3 y[3])
{
  int index[3];
  T3 B[3][3];
  for (int i = 0; i < 3; i++)
    {
    B[i][0] = A[i][0];
    B[i][1] = A[i][1];
    B[i][2] = A[i][2];
    y[i] = x[i];
    }

  vtkMath::LUFactor3x3(B,index);
  vtkMath::LUSolve3x3(B,index,y);
}

void vtkMath::LinearSolve3x3(const float A[3][3], 
                             const float x[3], float y[3])
{
  vtkLinearSolve3x3(A,x,y);
}

void vtkMath::LinearSolve3x3(const double A[3][3], 
                             const double x[3], double y[3])
{
  vtkLinearSolve3x3(A,x,y);
}

//----------------------------------------------------------------------------
template<class T1, class T2, class T3>
static inline void vtkMultiply3x3(const T1 A[3][3], const T2 v[3], T3 u[3])
{
  T3 x = A[0][0]*v[0] + A[0][1]*v[1] + A[0][2]*v[2];
  T3 y = A[1][0]*v[0] + A[1][1]*v[1] + A[1][2]*v[2];
  T3 z = A[2][0]*v[0] + A[2][1]*v[1] + A[2][2]*v[2];

  u[0] = x;
  u[1] = y;
  u[2] = z;
}

void vtkMath::Multiply3x3(const float A[3][3], const float v[3], float u[3])
{
  vtkMultiply3x3(A,v,u);
}

void vtkMath::Multiply3x3(const double A[3][3], const double v[3], double u[3])
{
  vtkMultiply3x3(A,v,u);
}

//----------------------------------------------------------------------------
template<class T, class T2, class T3>
static inline void vtkMultiplyMatrix3x3(const T A[3][3], const T2 B[3][3],
                                        T3 C[3][3])
{
  T3 D[3][3];

  for (int i = 0; i < 3; i++)
    {
    D[0][i] = A[0][0]*B[0][i] + A[0][1]*B[1][i] + A[0][2]*B[2][i];
    D[1][i] = A[1][0]*B[0][i] + A[1][1]*B[1][i] + A[1][2]*B[2][i];
    D[2][i] = A[2][0]*B[0][i] + A[2][1]*B[1][i] + A[2][2]*B[2][i];
    }

  for (int j = 0; j < 3; j++)
    {
    C[j][0] = D[j][0];
    C[j][1] = D[j][1];
    C[j][2] = D[j][2];
    }
}

void vtkMath::Multiply3x3(const float A[3][3], 
                          const float B[3][3], float C[3][3])
{
  vtkMultiplyMatrix3x3(A,B,C);
}

void vtkMath::Multiply3x3(const double A[3][3], 
                          const double B[3][3], double C[3][3])
{
  vtkMultiplyMatrix3x3(A,B,C);
}

//----------------------------------------------------------------------------
template<class T1, class T2>
static inline void vtkTranspose3x3(const T1 A[3][3], T2 AT[3][3])
{
  T2 tmp;
  tmp = A[1][0];
  AT[1][0] = A[0][1];
  AT[0][1] = tmp;
  tmp = A[2][0];
  AT[2][0] = A[0][2];
  AT[0][2] = tmp;
  tmp = A[2][1];
  AT[2][1] = A[1][2];
  AT[1][2] = tmp;

  AT[0][0] = A[0][0];
  AT[1][1] = A[1][1];
  AT[2][2] = A[2][2];
}

void vtkMath::Transpose3x3(const float A[3][3], float AT[3][3])
{
  vtkTranspose3x3(A,AT);
}

void vtkMath::Transpose3x3(const double A[3][3], double AT[3][3])
{
  vtkTranspose3x3(A,AT);
}

//----------------------------------------------------------------------------
template<class T1, class T2>
static inline void vtkInvert3x3(const T1 A[3][3], T2 AI[3][3])
{
  int index[3];
  T2 tmp[3][3];

  for (int k = 0; k < 3; k++)
    {
    AI[k][0] = A[k][0];
    AI[k][1] = A[k][1];
    AI[k][2] = A[k][2];
    }
  // invert one column at a time
  vtkMath::LUFactor3x3(AI,index);
  for (int i = 0; i < 3; i++)
    {
    T2 *x = tmp[i];
    x[0] = x[1] = x[2] = 0.0;
    x[i] = 1.0;
    vtkMath::LUSolve3x3(AI,index,x);
    }
  for (int j = 0; j < 3; j++)
    {
    T2 *x = tmp[j];
    AI[0][j] = x[0];
    AI[1][j] = x[1];
    AI[2][j] = x[2];      
    }
}

void vtkMath::Invert3x3(const float A[3][3], float AI[3][3])
{
  vtkInvert3x3(A,AI);
}

void vtkMath::Invert3x3(const double A[3][3], double AI[3][3])
{
  vtkInvert3x3(A,AI);
}

//----------------------------------------------------------------------------
template<class T>
static inline void vtkIdentity3x3(T A[3][3])
{
  for (int i = 0; i < 3; i++)
    {
    A[i][0] = A[i][1] = A[i][2] = T(0.0);
    A[i][i] = 1.0;
    }
}

void vtkMath::Identity3x3(float A[3][3])
{
  vtkIdentity3x3(A);
}

void vtkMath::Identity3x3(double A[3][3])
{
  vtkIdentity3x3(A);
}

//----------------------------------------------------------------------------
template<class T1, class T2>
static inline void vtkQuaternionToMatrix3x3(T1 quat[4], T2 A[3][3])
{
  T2 ww = quat[0]*quat[0];
  T2 wx = quat[0]*quat[1];
  T2 wy = quat[0]*quat[2];
  T2 wz = quat[0]*quat[3];

  T2 xx = quat[1]*quat[1];
  T2 yy = quat[2]*quat[2];
  T2 zz = quat[3]*quat[3];

  T2 xy = quat[1]*quat[2];
  T2 xz = quat[1]*quat[3];
  T2 yz = quat[2]*quat[3];

  T2 rr = xx + yy + zz;
  // normalization factor, just in case quaternion was not normalized
  T2 f = T2(1)/T2(sqrt(ww + rr));
  T2 s = (ww - rr)*f;
  f *= 2;

  A[0][0] = xx*f + s;
  A[1][0] = (xy + wz)*f;
  A[2][0] = (xz - wy)*f;

  A[0][1] = (xy - wz)*f;
  A[1][1] = yy*f + s;
  A[2][1] = (yz + wx)*f;

  A[0][2] = (xz + wy)*f;
  A[1][2] = (yz - wx)*f;
  A[2][2] = zz*f + s;
}

void vtkMath::QuaternionToMatrix3x3(const float quat[4], float A[3][3])
{
  vtkQuaternionToMatrix3x3(quat,A);
}

void vtkMath::QuaternionToMatrix3x3(const double quat[4], double A[3][3])
{
  vtkQuaternionToMatrix3x3(quat,A);
}

//----------------------------------------------------------------------------
//  The solution is based on
//  Berthold K. P. Horn (1987),
//  "Closed-form solution of absolute orientation using unit quaternions,"
//  Journal of the Optical Society of America A, 4:629-642
template<class T1, class T2>
static inline void vtkMatrix3x3ToQuaternion(const T1 A[3][3], T2 quat[4])
{
  T2 N[4][4];

  // on-diagonal elements
  N[0][0] =  A[0][0]+A[1][1]+A[2][2];
  N[1][1] =  A[0][0]-A[1][1]-A[2][2];
  N[2][2] = -A[0][0]+A[1][1]-A[2][2];
  N[3][3] = -A[0][0]-A[1][1]+A[2][2];

  // off-diagonal elements
  N[0][1] = N[1][0] = A[2][1]-A[1][2];
  N[0][2] = N[2][0] = A[0][2]-A[2][0];
  N[0][3] = N[3][0] = A[1][0]-A[0][1];

  N[1][2] = N[2][1] = A[1][0]+A[0][1];
  N[1][3] = N[3][1] = A[0][2]+A[2][0];
  N[2][3] = N[3][2] = A[2][1]+A[1][2];

  T2 eigenvectors[4][4],eigenvalues[4];
  
  // convert into format that JacobiN can use,
  // then use Jacobi to find eigenvalues and eigenvectors
  T2 *NTemp[4],*eigenvectorsTemp[4];
  for (int i = 0; i < 4; i++)
    {
    NTemp[i] = N[i];
    eigenvectorsTemp[i] = eigenvectors[i];
    }
  vtkMath::JacobiN(NTemp,4,eigenvalues,eigenvectorsTemp);
  
  // the first eigenvector is the one we want
  quat[0] = eigenvectors[0][0];
  quat[1] = eigenvectors[1][0];
  quat[2] = eigenvectors[2][0];
  quat[3] = eigenvectors[3][0];
}
  
void vtkMath::Matrix3x3ToQuaternion(const float A[3][3], float quat[4])
{
  vtkMatrix3x3ToQuaternion(A,quat);
}

void vtkMath::Matrix3x3ToQuaternion(const double A[3][3], double quat[4])
{
  vtkMatrix3x3ToQuaternion(A,quat);
}
  
//----------------------------------------------------------------------------
//  The orthogonalization is done via quaternions in order to avoid
//  having to use a singular value decomposition algorithm.  
template <class T1, class T2>
static inline void vtkOrthogonalize3x3(const T1 A[3][3], T2 B[3][3])
{
  int i;

  // copy the matrix
  for (i = 0; i < 3; i++)
    {
    B[0][i] = A[0][i];
    B[1][i] = A[1][i];
    B[2][i] = A[2][i];
    }

  // Pivot the matrix to improve accuracy
  T2 scale[3];
  int index[3];
  T2 tmp, largest;

  // Loop over rows to get implicit scaling information
  for (i = 0; i < 3; i++)
    {
    largest = fabs(B[i][0]);
    if ((tmp = fabs(B[i][1])) > largest)
      {
      largest = tmp;
      }
    if ((tmp = fabs(B[i][2])) > largest)
      {
      largest = tmp;
      }
    scale[i] = T2(1.0)/largest;
    }

  // first column
  index[0] = 0;
  largest = scale[0]*fabs(B[0][0]);
  if ((tmp = scale[1]*fabs(B[1][0])) >= largest) 
    {
    largest = tmp;
    index[0] = 1;
    }
  if ((tmp = scale[2]*fabs(B[2][0])) >= largest) 
    {
    index[0] = 2;
    }
  if (index[0] != 0) 
    {
    vtkSwapVectors3(B[index[0]],B[0]);
    scale[index[0]] = scale[0];
    }

  // second column
  index[1] = 1;
  largest = scale[1]*fabs(B[1][1]);
  if ((tmp = scale[2]*fabs(B[2][1])) >= largest) 
    {
    index[1] = 2;
    vtkSwapVectors3(B[2],B[1]);
    }

  // third column
  index[2] = 2;

  // A quaternian can only describe a pure rotation, not
  // a rotation with a flip, therefore the flip must be
  // removed before the matrix is converted to a quaternion.
  T2 d = vtkDeterminant3x3(B);
  if (d < 0)
    {
    for (i = 0; i < 3; i++)
      {
      B[0][i] = -B[0][i];
      B[1][i] = -B[1][i];
      B[2][i] = -B[2][i];
      }
    }

  // Do orthogonalization using a quaternion intermediate
  // (this, essentially, does the orthogonalization via
  // diagonalization of an appropriately constructed symmetric
  // 4x4 matrix rather than by doing SVD of the 3x3 matrix)
  T2 quat[4];
  vtkMath::Matrix3x3ToQuaternion(B,quat);
  vtkMath::QuaternionToMatrix3x3(quat,B);

  // Put the flip back into the orthogonalized matrix.
  if (d < 0)
    {
    for (i = 0; i < 3; i++)
      {
      B[0][i] = -B[0][i];
      B[1][i] = -B[1][i];
      B[2][i] = -B[2][i];
      }
    }

  // Undo the pivoting
  if (index[1] != 1)
    {
    vtkSwapVectors3(B[index[1]],B[1]);
    }
  if (index[0] != 0)
    {
    vtkSwapVectors3(B[index[0]],B[0]);
    }
}

void vtkMath::Orthogonalize3x3(const float A[3][3], float B[3][3])
{
  vtkOrthogonalize3x3(A,B);
}

void vtkMath::Orthogonalize3x3(const double A[3][3], double B[3][3])
{
  vtkOrthogonalize3x3(A,B);
}

float vtkMath::Norm(const float* x, int n)
{
  double sum=0;
  for (int i=0; i<n; i++)
    {
    sum += x[i]*x[i];
    }

  return sqrt(sum);
}

//----------------------------------------------------------------------------
// Extract the eigenvalues and eigenvectors from a 3x3 matrix.
// The eigenvectors (the columns of V) will be normalized. 
// The eigenvectors are aligned optimally with the x, y, and z
// axes respectively.
template <class T1, class T2>
static inline void vtkDiagonalize3x3(const T1 A[3][3], T2 w[3], T2 V[3][3])
{
  int i,j,k,maxI;
  T2 tmp, maxVal;

  // do the matrix[3][3] to **matrix conversion for Jacobi
  T2 C[3][3];
  T2 *ATemp[3],*VTemp[3];
  for (i = 0; i < 3; i++)
    {
    C[i][0] = A[i][0];
    C[i][1] = A[i][1];
    C[i][2] = A[i][2];
    ATemp[i] = C[i];
    VTemp[i] = V[i];
    }

  // diagonalize using Jacobi
  vtkMath::JacobiN(ATemp,3,w,VTemp);
  
  // if all the eigenvalues are the same, return identity matrix
  if (w[0] == w[1] && w[0] == w[2])
    {
    vtkMath::Identity3x3(V);
    return;
    }

  // transpose temporarily, it makes it easier to sort the eigenvectors
  vtkMath::Transpose3x3(V,V);

  // if two eigenvalues are the same, re-orthogonalize to optimally line
  // up the eigenvectors with the x, y, and z axes
  for (i = 0; i < 3; i++)
    {
    if (w[(i+1)%3] == w[(i+2)%3]) // two eigenvalues are the same
      {
      // find maximum element of the independant eigenvector
      maxVal = fabs(V[i][0]);
      maxI = 0;
      for (j = 1; j < 3; j++)
        {
        if (maxVal < (tmp = fabs(V[i][j])))
          {
          maxVal = tmp;
          maxI = j;
          }
        }
      // swap the eigenvector into its proper position
      if (maxI != i)
        {
        tmp = w[maxI];
        w[maxI] = w[i];
        w[i] = tmp;
        vtkSwapVectors3(V[i],V[maxI]);
        }
      // maximum element of eigenvector should be positive
      if (V[maxI][maxI] < 0)
        {
        V[maxI][0] = -V[maxI][0];
        V[maxI][1] = -V[maxI][1];
        V[maxI][2] = -V[maxI][2];
        }
      
      // re-orthogonalize the other two eigenvectors
      j = (maxI+1)%3;
      k = (maxI+2)%3;

      V[j][0] = 0.0; 
      V[j][1] = 0.0; 
      V[j][2] = 0.0;
      V[j][j] = 1.0;
      vtkMath::Cross(V[maxI],V[j],V[k]);
      vtkMath::Normalize(V[k]);
      vtkMath::Cross(V[k],V[maxI],V[j]);

      // transpose vectors back to columns
      vtkMath::Transpose3x3(V,V);
      return;
      }
    }

  // the three eigenvalues are different, just sort the eigenvectors
  // to align them with the x, y, and z axes

  // find the vector with the largest x element, make that vector
  // the first vector
  maxVal = fabs(V[0][0]);
  maxI = 0;
  for (i = 1; i < 3; i++)
    {
    if (maxVal < (tmp = fabs(V[i][0])))
      {
      maxVal = tmp;
      maxI = i;
      }
    }
  // swap eigenvalue and eigenvector
  if (maxI != 0)
    {
    tmp = w[maxI];
    w[maxI] = w[0];
    w[0] = tmp;
    vtkSwapVectors3(V[maxI],V[0]);
    }
  // do the same for the y element
  if (fabs(V[1][1]) < fabs(V[2][1]))
    {
    tmp = w[2];
    w[2] = w[1];
    w[1] = tmp;
    vtkSwapVectors3(V[2],V[1]);
    }

  // ensure that the sign of the eigenvectors is correct
  for (i = 0; i < 2; i++)
    {
    if (V[i][i] < 0)
      {
      V[i][0] = -V[i][0];
      V[i][1] = -V[i][1];
      V[i][2] = -V[i][2];
      }
    }
  // set sign of final eigenvector to ensure that determinant is positive
  if (vtkMath::Determinant3x3(V) < 0)
    {
    V[2][0] = -V[2][0];
    V[2][1] = -V[2][1];
    V[2][2] = -V[2][2];
    }

  // transpose the eigenvectors back again
  vtkMath::Transpose3x3(V,V);
}

void vtkMath::Diagonalize3x3(const float A[3][3], float w[3], float V[3][3])
{
  vtkDiagonalize3x3(A,w,V);
}

void vtkMath::Diagonalize3x3(const double A[3][3],double w[3],double V[3][3])
{
  vtkDiagonalize3x3(A,w,V);
}

//----------------------------------------------------------------------------
// Perform singular value decomposition on the matrix A:
//    A = U * W * VT
// where U and VT are orthogonal W is diagonal (the diagonal elements
// are returned in vector w).
// The matrices U and VT will both have positive determinants.
// The scale factors w are ordered according to how well the
// corresponding eigenvectors (in VT) match the x, y and z axes
// respectively.
//
// The singular value decomposition is used to decompose a linear
// transformation into a rotation, followed by a scale, followed
// by a second rotation.  The scale factors w will be negative if
// the determinant of matrix A is negative.
//
// Contributed by David Gobbi (dgobbi@irus.rri.on.ca)
template <class T1, class T2>
static inline void vtkSingularValueDecomposition3x3(const T1 A[3][3], 
                                                    T2 U[3][3], T2 w[3],
                                                    T2 VT[3][3])
{
  int i;
  T2 B[3][3];

  // copy so that A can be used for U or VT without risk
  for (i = 0; i < 3; i++)
    {
    B[0][i] = A[0][i];
    B[1][i] = A[1][i];
    B[2][i] = A[2][i];
    }

  // temporarily flip if determinant is negative
  T2 d = vtkMath::Determinant3x3(B);
  if (d < 0)
    {
    for (i = 0; i < 3; i++)
      {
      B[0][i] = -B[0][i];
      B[1][i] = -B[1][i];
      B[2][i] = -B[2][i];
      }
    }

  // orthogonalize, diagonalize, etc.
  vtkMath::Orthogonalize3x3(B, U);
  vtkMath::Transpose3x3(B, B);
  vtkMath::Multiply3x3(B, U, VT);
  vtkMath::Diagonalize3x3(VT, w, VT);
  vtkMath::Multiply3x3(U, VT, U);
  vtkMath::Transpose3x3(VT, VT);

  // re-create the flip
  if (d < 0)
    {
    w[0] = -w[0];
    w[1] = -w[1];
    w[2] = -w[2];
    }

  /* paranoia check: recombine to ensure that the SVD is correct
  vtkMath::Transpose3x3(B, B);

  if (d < 0)
    {
    for (i = 0; i < 3; i++)
      {
      B[0][i] = -B[0][i];
      B[1][i] = -B[1][i];
      B[2][i] = -B[2][i];
      }
    }

  int j;
  T2 maxerr = 0;
  T2 tmp;
  T2 M[3][3];
  T2 W[3][3];
  vtkMath::Identity3x3(W);
  W[0][0] = w[0]; W[1][1] = w[1]; W[2][2] = w[2];
  vtkMath::Identity3x3(M);
  vtkMath::Multiply3x3(M, U, M);
  vtkMath::Multiply3x3(M, W, M);
  vtkMath::Multiply3x3(M, VT, M);

  for (i = 0; i < 3; i++)
    {
    for (j = 0; j < 3; j++)
      {
      if ((tmp = fabs(B[i][j] - M[i][j])) > maxerr)
        {
        maxerr = tmp;
        }
      }
    }

  vtkGenericWarningMacro("SingularValueDecomposition max error = " << maxerr);
  */
}

void vtkMath::SingularValueDecomposition3x3(const float A[3][3], 
                                            float U[3][3], float w[3],
                                            float VT[3][3])
{
  vtkSingularValueDecomposition3x3(A,U,w,VT);
}

void vtkMath::SingularValueDecomposition3x3(const double A[3][3], 
                                            double U[3][3], double w[3],
                                            double VT[3][3])
{
  vtkSingularValueDecomposition3x3(A,U,w,VT);
}

//----------------------------------------------------------------------------
void vtkMath::RGBToHSV(float r, float g, float b, float *h, float *s, float *v)
{
  float hue = 0;
  float sat = 0;
  float val = 0;
  float lx, ly, lz;

  if (r <= 0.0 && g <= 0.0 && b <= 0.0)
    {
    *h = 0.0;
    *s = 0.0;
    *v = 0.0;
    return;
    }
  if (r >= g && g >= b)
    { // case 0
    val = r;
    lz = g;
    lx = b;
    sat = 1.0 - (lx/val);
    hue = (0.0 + (1.0 - ((1.0 - (lz/val))/sat)))/6.0;
    }
  else if (g >= r && r >= b)
    { // case 1
    ly = r;
    val = g;
    lx = b;
    sat = 1.0 - (lx/val);
    hue = (1.0 + ((1.0 - (ly/val))/sat))/6.0;
    }
  else if (g >= b && b >= r)
    { // case 2
    lx = r;
    val = g;
    lz = b;
    sat = 1.0 - (lx/val);
    hue = (2.0 + (1.0 - ((1.0 - (lz/val))/sat)))/6.0;
    }
  else if (b >= g && g >= r)
    { // case 3
    lx = r;
    ly = g;
    val = b;
    sat = 1.0 - (lx/val);
    hue = (3.0 + ((1.0 - (ly/val))/sat))/6.0;
    }
  else if (b >= r && r >= g)
    { // case 4
    lz = r;
    lx = g;
    val = b;
    sat = 1.0 - (lx/val);
    hue = (4.0 + (1.0 - ((1.0 - (lz/val))/sat)))/6.0;
    }
  else if (r >= b && b >= g)
    { // case 5
    val = r;
    lx = g;
    ly = b;
    sat = 1.0 - (lx/val);
    hue = (5.0 + ((1.0 - (ly/val))/sat))/6.0;
    }
  *h = hue;
  *s = sat;
  *v = val;
}

//----------------------------------------------------------------------------
void vtkMath::HSVToRGB(float h, float s, float v, float *r, float *g, float *b)
{
  float R, G, B;
  float max = 1.0;
  float third = max / 3.0;
  float temp;

  // compute rgb assuming S = 1.0;
  if (h >= 0.0 && h <= third) // red -> green
    {
    G = h/third;
    R = 1.0 - G;
    B = 0.0;
    }
  else if (h >= third && h <= 2.0*third) // green -> blue
    {
    B = (h - third)/third;
    G = 1.0 - B;
    R = 0.0;
    }
  else // blue -> red
    {
    R = (h - 2.0 * third)/third;
    B = 1.0 - R;
    G = 0.0;
    }
        
  // add Saturation to the equation.
  s = s / max;
  //R = S + (1.0 - S)*R;
  //G = S + (1.0 - S)*G;
  //B = S + (1.0 - S)*B;
  // what happend to this?
  R = s*R + (1.0 - s);
  G = s*G + (1.0 - s);
  B = s*B + (1.0 - s);
      
  // Use value to get actual RGB 
  // normalize RGB first then apply value
  temp = R + G + B; 
  //V = 3 * V / (temp * max);
  // and what happend to this?
  v = 3 * v / (temp);
  R = R * v;
  G = G * v;
  B = B * v;
      
  // clip below 255
  //if (R > 255.0) R = max;
  //if (G > 255.0) G = max;
  //if (B > 255.0) B = max;
  // mixed constant 255 and max ?????
  if (R > max)
    {
    R = max;
    }
  if (G > max)
    {
    G = max;
    }
  if (B > max)
    {
    B = max;
    }
  *r = R;
  *g = G;
  *b = B;
}
