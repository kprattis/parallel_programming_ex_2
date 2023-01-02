typedef enum bool {false = 0, true = 1} bool;

typedef enum type {INT, DOUBLE} type;

//  Keeps the block size
extern int BLOCKSIZE;

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
knnresult distrAllkNN(double * X, int n, int d, int k);

//functions for initialiization / dealocation of knnresult struct
knnresult init_knnresult(int k, int m);
void free_knnresult(knnresult knn);

//helpers
void print_results(knnresult knn, bool print_dist);
void print_arr(void *arr, int a, int b, type t);
double euclidean_norm(double *vec, int d);
int min(int a, int b);
void save_image(double *Image, int rows, int cols, char *filename);

//functions to select k smallest numbers
void shift_select(int *nidx, double *ndist, int n_found ,double *dist, int n, int k, int global_id_offset);
int shift(void *arr, int size, int start, int end, type t);

//functions to initialize query and corpus
double * read_MNIST_images(char * filename, int *num_images, int *rows, int *cols);
double * regular_grid(int dim, double start[], double end[], double step[], int *n_points);
double * randarr(int size, double max, double min);