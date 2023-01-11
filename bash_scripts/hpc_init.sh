#!/bin/bash

module load gcc/9.4.0 openmpi/4.1.2

export OMPI_CC=gcc

make bin/reg_grid CC=gcc

module load openblas
module load OpenCilk

export OMPI_CC=clang

make mpi CC=clang HPCBLASINC="$OPENBLAS_ROOT/include" LBLAS=$OPENBLAS_ROOT/lib
