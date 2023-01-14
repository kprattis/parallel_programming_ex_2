# Parallel Programming Exercise 2 - MPI brute force KNN 

This project implements a brute force knn algorithm using OpenCilk and MPI.

This was developed and tested for Ubuntu 22.04

### Requirements:
* [OpenCilk clang compiler](https://www.opencilk.org/)
* [CBLAS Library for the function dgemm (I used OpenBlas)](https://www.openblas.net/)

You can use in terminal the command 
```````````
sudo apt-get install libopenblas-dev
``````````````

* [OpenMPI](https://www.open-mpi.org/)

### Instructions:

##### To run Localy:

* Open a terminal and with the following commands clone the repository and cd inside:

`````````````
git clone https://github.com/kprattis/parallel_programming_ex_2

cd parallel_programming_ex_2

`````````````
* then I suggest the following command

`````````````
chmod +x scripts/*

`````````````
* After that, if the path to opencilk compiler is "pathtoOpenCilk/clang", then use:
`````````````
exoprt OMPI_CC="pathtoOpenCilk/clang"

make CC="pathtoOpenCilk/clang"

`````````````
* There should be created a bin folder containing the binary files:
    * **reg_grid**:
        Use this to produce a regular grid of **d** dimensions and **N** values per dimension and save it at a file. k will be $ 3^d $. You can either use:    

        ``````````````
            make grid IFILE="inputs/filename.txt" N=10 d=3

        ````````````````
        or
        ``````````````
            bin/reg_grid "inputs/filename.txt" 10 3
        
        ````````````````
        You can also specify if you want a specific number of points instead of the whole $ N^d $ points. So if you want for example only 100 points totally you can use in the above example. In this case k has been set to 5:
        
        ``````````````
            bin/reg_grid "inputs/filename.txt" 10 3 100

        ````````````````

        If you want to use your own files for testing, make sure the format is like this:
        ``````````````
        10000 3 27
        0.000 1.000 2.000
        1.000 2.000 2.000
        .
        .
        .
        ``````````````
        where the first line specifies the number of points in total (10000), the number of dimensions (3) and the k value (27).


    * **knn**:
        With this run the V0 version of the code. Take as input a file formatted as described above. Write the result to a file specified as an argument. Use:
        ``````````````
            make seqrun IFILE="inputs/infile.txt" OFILE="results/ofile.txt"

        ````````````````
        or
        ``````````````
            bin/knn "inputs/infile.txt" "results/V0_ofile.txt"

        ````````````````

    * **knn_mpi**:
    With this run the MPI (V1) version of the code. Arguments similar with the knn. Use:
        ``````````````
            make mpirun IFILE="inputs/infile.txt" OFILE="results/V1_ofile.txt"

        ````````````````
        or
        ``````````````
            CILK_NWORKERS=1 mpiexec -n 6 bin/knn_mpi inputs/infile.txt results/ofile.txt 

        ````````````````

    * **test**:
        Use this to test the results of a 2d or 3d regular grid with values from 1-N at each dimension. You can use:
        ``````````````
            make test IFILE="inputs/infile.txt" OFILE="results/V1_ofile.txt"
        ````````````````
        or
        ``````````````
            bin/test inputs/infile.txt results/ofile.txt

        ````````````````
##### To run at HPC

After the login, from terminal use:

`````````````
git clone https://github.com/kprattis/parallel_programming_ex_2

cd parallel_programming_ex_2
`````````````
then I suggest to copy the following commands to compile reg_grid, test and knn_mpi (V0 better to be used locally).

`````````````
chmod +x scripts/*

module load gcc/9.4.0

make bin/reg_grid CC=gcc

module load gcc/9.4.0 openmpi/4.1.2

module load openblas
module load OpenCilk

make bin/knn CC=clang HPCBLASINC="$OPENBLAS_ROOT/include" LBLAS=$OPENBLAS_ROOT/lib

export OMPI_CC=clang
make mpi CC=clang HPCBLASINC="$OPENBLAS_ROOT/include" LBLAS=$OPENBLAS_ROOT/lib

`````````````
The binary files can be used as before.

To submit a job use the script (in the scripts folder):

mpi_batch.sh, e.g. :



`````````````
sbatch scripts/mpi_batch.sh inputs/ifile.txt results/ofile.txt
````````````` 

* For convienience, there are more bash scripts in the folder _scripts_. You can open and modify them or run them with the default parameters:

Use :
`````````````
scripts/make_reg_grids.sh
````````````` 
to make regular grids in the folder inputs (local or HPC).

Use:
`````````````
scripts/make_large_datasets.sh
````````````` 
to make high dimensional input files with a specified amount of points (local or HPC).

Finally, you can use:
`````````````
scripts/local_run_grids.sh
`````````````
To run all the files in the folder inputs with the V0 code (locally)

or 

`````````````
scripts/mpirun_grids_batch.sh
`````````````
To submit a job at HPC for every input file in _inputs_ with the V1 code.
