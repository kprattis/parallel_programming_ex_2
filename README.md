# parallel_programming_ex_2

`````````````
git clone https://github.com/kprattis/parallel_programming_ex_2

cd parallel_programming_ex_2

`````````````

`````````````
chmod +x bash_scripts/*

module load gcc/9.4.0

make bin/reg_grid CC=gcc
make bin/test CC=gcc

module load gcc/9.4.0 openmpi/4.1.2

module load openblas
module load OpenCilk

export OMPI_CC=clang
make mpi CC=clang HPCBLASINC="$OPENBLAS_ROOT/include" LBLAS=$OPENBLAS_ROOT/lib

`````````````
