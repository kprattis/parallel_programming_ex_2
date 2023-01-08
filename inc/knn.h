
#ifndef KNN
#define KNN

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

  \return  The kNN result
*/
knnresult distrAllkNN(double * X, int n, int d, int k, int N);


//functions for initialiization / dealocation of knnresult struct
knnresult init_knnresult(int k, int m);
void free_knnresult(knnresult knn);

//select function
void kselect(double *D, int left, int right, int k, double *ndist, int *nidx, int isknnempty, int shift);
void swap(double *a, double *b);

//helpers
double euclidean_norm(double *vec, int d);
long min(long a, long b);
void print_arrd(double *arr, int r, int c);
void print_arri(int *arr, int r, int c);


//init query
void randarr(double *X, int size, double max, double min);
double * read_MNIST_images(char *filename, int *num_images, int *rows, int *cols);

#endif