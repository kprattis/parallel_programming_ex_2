#ifndef KNN
#define KNN

#include <stdio.h>

// Definition of the kNN result struct
typedef struct knnresult{
  int    * nidx;    //!< Indices (0-based) of nearest neighbors [m-by-k]
  double * ndist;   //!< Distance of nearest neighbors          [m-by-k]
  int      m;       //!< Number of query points                 [scalar]
  int      k;       //!< Number of nearest neighbors            [scalar]
} knnresult;

//! Compute k nearest neighbors of each point in X [m-by-d]
/*!

  \param  X      Query data points              [m-by-d]
  \param  Y      Corpus data points               [n-by-d]
  \param  m      Number of query points          [scalar]
  \param  n      Number of corpus points         [scalar]
  \param  d      Number of dimensions            [scalar]
  \param  k      Number of neighbors             [scalar]

  \return  The kNN result
*/
knnresult kNN(double *X, double *Y, int n, int m, int d, int k);

//! Compute distributed all-kNN of points in X
/*!

  \param  X      Data points                     [n-by-d]
  \param  n      Number of data points           [scalar]
  \param  d      Number of dimensions            [scalar]
  \param  k      Number of neighbors             [scalar]

  \param N       Number of total data points

  \return  The kNN result
*/
knnresult distrAllkNN(double * X, int n, int d, int k, int N);


//functions for initialiization / dealocation of knnresult struct
knnresult init_knnresult(int k, int m);
void free_knnresult(knnresult knn);

/*! function to select the k nearest neighbours from array D between positions left - right.
    isknnempty is a flag showing if the arrays ndist, nidx have already some points as knn from previous iteration.
    shift specifies the proper index shift show that the function stores the global indices of the points
*/
void kselect(double *D, int left, int right, int k, double *ndist, int *nidx, int isknnempty, int shift);

//helpers
double euclidean_norm(double *vec, int d);
long min(long a, long b);
void print_arrd(double *arr, int r, int c);
void print_arri(int *arr, int r, int c);
void fprint_arrd(FILE *f, double *arr, int n, int k);
void fprint_arri(FILE *f, int *arr, int n, int k);

#endif