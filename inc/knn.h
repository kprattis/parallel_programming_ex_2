typedef enum bool {false = 0, true = 1} bool;
typedef enum type {INT, DOUBLE} type;

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

//functions for initialiization / dealocation of knnresult struct
knnresult init_knnresult(int k, int m);
void free_knnresult(knnresult knn);


//helper to print array
void print_arr(void *arr, int a, int b, type t);


//functions to select k smallest numbers
void k_select(knnresult *knn, double *dist, int m, int n, int k);
int shift(void *arr, int size, int start, int end, type t);

void quickselect();
void partition();
void merge();