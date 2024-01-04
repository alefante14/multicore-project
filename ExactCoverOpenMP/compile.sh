#!/bin/bash

nSudoku=1
echo "Inserire numero di thread che si vogliono utilizzare"
read nThread

cd ./
cd ./Sudoku-Generator
#bash ./setup.sh
echo "generando il sudoku..."
./sudokuGen "$nSudoku"
echo "sudoku generato!"
echo ""
mv ./puzzles.txt ../Exact-Cover
cd ../Exact-Cover
gcc -fopenmp -o ec_omp exact_cover_openmp.c
./ec_omp "$nThread"
