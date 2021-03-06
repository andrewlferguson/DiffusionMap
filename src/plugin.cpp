/****
*
* Plugin binding
*
*
****/

#include "pyarma.h"
#include "DFSAlign.hpp"
#include "Distance.hpp"
#include "DMap.h"
#include "Nystrom.hpp"
#include "KMedoids.hpp"

#if defined(ENABLE_OPENMP)
#include <omp.h>
uint get_omp_threads() { 
	uint ret=0;
	#pragma omp parallel
	{ 
		#pragma omp single
		{ ret = omp_get_num_threads(); }	
	}
	return ret; 
}
void set_omp_threads(uint n) { omp_set_num_threads(n); }
#else
void set_omp_threads(uint n) { }
uint get_omp_threads() { return 1; }
#endif


PYBIND11_PLUGIN(PyDMap) {
	
	py::module m("PyDMap", "C++ implementation of DMap using armadillo");
	m.def("dist",&dist);
	m.def("pdist",&pdist);
	m.def("pdist2",(pyarr_d(*)(std::vector<pyarr_d>,std::vector<pyarr_d>))&pdist2);
	m.def("pdist2",(pyarr_d(*)(pyarr_d,std::vector<pyarr_d>))&pdist2);
	m.def("pdist2",(pyarr_d(*)(std::vector<pyarr_d>,pyarr_d))&pdist2);

	m.def("kmedoids",&kmedoids,py::arg("dists"),py::arg("k"),py::arg("seeds")=pyarr_u(0,nullptr));

	// convert arma::mat and np.array
	/*
	py::class_<arma::mat>(m,"Mat")
		.def(py::init<const arma::uword, const arma::uword>())
		.def("__init__", &mat_np_init)
		.def_property_readonly("shape",
				[](const arma::mat &a){return std::make_tuple(a.n_rows,a.n_cols);})
		.def_property_readonly("n_elem",
				[](const arma::mat &a){return a.n_elem;})
		.def("print", 
				[](const arma::mat &a){a.print();})
		.def("print",
				[](const arma::mat &a, std::string arg){a.print(arg);})
		.def_buffer(&mat_buffer);

	// convert arma::vec and np.array
	py::class_<arma::vec>(m,"Vec")
		.def(py::init<const arma::uword>())
		.def("__init__",&vec_np_init)
		.def_property_readonly("shape",
				[](const arma::vec &a){return std::make_tuple(a.n_rows);})
		.def_property_readonly("n_elem",
				[](const arma::vec &a){return a.n_elem;})
		.def("print", 
				[](const arma::vec &a){a.print();})
		.def("print",
				[](const arma::vec &a, std::string arg){a.print(arg);})
		.def_buffer(&vec_buffer);
	*/

	py::class_<DMap>(m,"DMap")
		.def(py::init<>())
		.def("compute",&DMap::compute)
		.def("set_epsilon",&DMap::set_epsilon)
		.def("get_epsilon",&DMap::get_epsilon)
		.def("set_num_evec",&DMap::set_num_evec)
		.def("get_num_evec",&DMap::get_num_evec)
		.def("set_dists",&DMap::set_dists)
		.def("get_evec",&DMap::get_evec_py)
		.def("set_evec",&DMap::set_evec)
		.def("get_eval",&DMap::get_eval_py)
		.def("set_eval",&DMap::set_eval)
		.def("get_num_samples",&DMap::get_num_samples);

	m.def("nystrom",(pyarr_d(*)(DMap&,pyarr_d))&nystrom);

	m.def("set_omp_threads",&set_omp_threads);
	m.def("get_omp_threads",&get_omp_threads);
	return m.ptr();
}

