#pragma once
#include <armadillo>
#include "DFSAlign.hpp"
#include "pyarma.h"
#include <pybind11/stl.h>

#define BRANCHES 12

double
dist(pyarr_d a, pyarr_d b)
{
	arma::mat oa = py_to_mat(a);
	arma::mat ob = py_to_mat(b);
	return dist_dfs(oa, ob, BRANCHES);
}

pyarr_d
pdist(std::vector<pyarr_d> pyobjs)
{
	size_t N = pyobjs.size();
	arma::mat dists(N,N,arma::fill::zeros);
	std::vector<arma::mat> objs(N);
	for(size_t i = 0; i < N; ++i)
		objs.at(i) = py_to_mat(pyobjs.at(i));
//		mat_np_init(objs.at(i),pyobjs.at(i));
// load balances by computing subdiagonal elements from the bottom of the matrix
#if defined(ENABLE_OPENMP)
		size_t n_rows = (N-1)/2;
	#pragma omp parallel for shared(dists)
		for(size_t i = 0; i < n_rows; ++i) 
		{
			for(size_t j = 0; j < N; ++j)
			{
				size_t idx_i = i, idx_j = j;
				if(idx_i >= idx_j)
				{
					idx_i = N - 2 - idx_i;
					idx_j = N - 1 - idx_j;
				}
				double d = dist_dfs(objs.at(idx_i), objs.at(idx_j), BRANCHES);
				dists(idx_i, idx_j) = d;
				dists(idx_j, idx_i) = d;
			}
		}

		if(N % 2 == 0)
		{
		#pragma omp parallel for shared(dists)
			for(size_t j = n_rows+1; j < N; ++j)
			{
				double d = dist_dfs(objs.at(n_rows), objs.at(j), BRANCHES);
				dists(n_rows, j) = d;
				dists(j, n_rows) = d;
			}
		}
#else	
	for(size_t i = 0; i < N; ++i)
	{
		for(size_t j=i+1; j < N; ++j)
		{
			dists(i,j) = dist_dfs(objs.at(i), objs.at(j), BRANCHES);
			dists(j,i) = dists(i,j);
		}
	}
#endif
	return mat_to_py(dists);
}

pyarr_d
pdist2(std::vector<pyarr_d> p1, std::vector<pyarr_d> p2)
{
	size_t N1 = p1.size();
	size_t N2 = p2.size();

	arma::mat dists(N1,N2);
	std::vector<arma::mat> o1(N1);
	std::vector<arma::mat> o2(N2);

	for(size_t i = 0; i < N1; ++i)
		o1.at(i) = py_to_mat(p1.at(i));
	
	for(size_t i = 0; i < N2; ++i)
		o2.at(i) = py_to_mat(p2.at(i));

#if defined(ENABLE_OPENMP)
#pragma omp parallel for shared(dists)
#endif
	for(size_t i = 0; i < N1; ++i)
		for(size_t j = 0; j < N2; ++j)
			dists(i,j) = dist_dfs(o1.at(i), o2.at(j), BRANCHES);
	
	return mat_to_py(dists);
}

pyarr_d
pdist2(std::vector<pyarr_d> p1, pyarr_d p2)
{
	size_t N1 = p1.size();

	arma::vec dists(N1);
	std::vector<arma::mat> o1(N1);
	arma::mat o2;

	for(size_t i = 0; i < N1; ++i)
		o1.at(i) = py_to_mat(p1.at(i));
	o2 = py_to_mat(p2);	

#if defined(ENABLE_OPENMP)
#pragma omp parallel for shared(dists)
#endif
	for(size_t i = 0; i < N1; ++i)
			dists(i) = dist_dfs(o1.at(i), o2, BRANCHES);
	
	return vec_to_py(dists);
}

pyarr_d
pdist2(pyarr_d p1, std::vector<pyarr_d> p2)
{
	return pdist2(p2,p1);
}

