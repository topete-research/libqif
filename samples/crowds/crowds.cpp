#include "types.h"
#include "aux.h"
#include "Metric.h"
#include "GLeakage.h"
#include "MinEntropy.h"
#include "Mechanism.h"
#include "Chan.h"

#include <iostream>
#include <fstream>
#include <stdlib.h>

using namespace std;

const uint honest = 5;
const uint corrupted = 1;

typedef double T;


// Matrix of Crowds protocol with _n honest users, _c corrupted
// and pf probability of forwarding
//
Chan<T> crowds_matrix(uint _n, uint _c, T pf) {

	T n(_n);
	T c(_c);
	T m(n + c);

	Chan<T> C(_n, _n+1);

	T alpha = equal(pf, T(1))
		? T(0)
		: (n - n * pf) / (m - n * pf);
	T beta = (c * (m - pf * (n - 1))) / (m * (m - pf * n));
	T gamma = (c * pf) / (m * (m - pf * n));

	for(uint i = 0; i < n; i++) {
		for(uint j = 0; j < n+1; j++) {
			C.at(i, j) = j == n ? alpha : i == j ? beta : gamma;
		}
	}
	check_proper(C);

	return C;
}

Mat<T> tiger_g(uint n) {
	Mat<T> G(n+1, n);
	G.submat(0, 0, n-1, n-1).eye();			// first n guesses are the identity
	G.row(n).fill(T(1)/2);
	return G;
}

// biased prior, n users, one has probability p, the others share the remaining 1-p
//
Prob<T> biased_prior(uint n, T p) {
	Prob<T> pi(n);
	pi.fill(equal(p, T(1)) ? T(0) : (1-p)/(n-1));
	pi.at(0) = p;
	check_proper(pi);
	return pi;
}


void meleakage_by_p() {
	MinEntropy<T> me1, me2, me3;
	me1.C = crowds_matrix(honest, corrupted, 0);
	me2.C = crowds_matrix(honest, corrupted, 0.5);
	me3.C = crowds_matrix(honest, corrupted, 1);

	ofstream file;
	file.open("crowds_data/data-min-p.txt");

	for(T p(0); less_than_or_eq(p, T(1)); p += T(1)/1000) {
		Prob<T> pi = biased_prior(honest, p);

		file << p << "   " 
			<< (me1.leakage(pi)) << "   "
			<< (me2.leakage(pi)) << "   "
			<< (me3.leakage(pi)) << "\n";
	}
}

void meleakage_by_pf() {
	Prob<T> pi1 = biased_prior(honest, T(4)/10);
	Prob<T> pi2 = biased_prior(honest, T(3)/10);
	Prob<T> pi3 = biased_prior(honest, T(2)/10);

	ofstream file;
	file.open("crowds_data/data-min-pf.txt");

	for(T pf(0); less_than_or_eq(pf, T(1)); pf += T(1)/1000) {
		MinEntropy<T> me;
		me.C = crowds_matrix(honest, corrupted, pf);

		file << pf << "   " 
			<< (me.leakage(pi1)) << "   "
			<< (me.leakage(pi2)) << "   "
			<< (me.leakage(pi3)) << "\n";
	}
}

void gleakage_by_p() {
	GLeakage<T> gl1, gl2, gl3;
	gl1.C = crowds_matrix(honest, corrupted, 0);
	gl2.C = crowds_matrix(honest, corrupted, 0.5);
	gl3.C = crowds_matrix(honest, corrupted, 1);

	// tiger
	gl1.G = gl2.G = gl3.G = tiger_g(honest);

	ofstream file;
	file.open("crowds_data/data-tiger-p.txt");

	for(T p(0); less_than_or_eq(p, T(1)); p += T(1)/1000) {
		Prob<T> pi = biased_prior(honest, p);

		file << p << "   " 
			<< (gl1.leakage(pi)) << "   "
			<< (gl2.leakage(pi)) << "   "
			<< (gl3.leakage(pi)) << "\n";
	}
}

void gleakage_by_pf() {
	Prob<T> pi1 = biased_prior(honest, T(4)/10);
	Prob<T> pi2 = biased_prior(honest, T(3)/10);
	Prob<T> pi3 = biased_prior(honest, T(2)/10);

	// tiger
	ofstream file;
	file.open("crowds_data/data-tiger-pf.txt");

	for(T pf(0); less_than_or_eq(pf, T(1)); pf += T(1)/1000) {
		GLeakage<T> gl;
		gl.C = crowds_matrix(honest, corrupted, pf);
		gl.G = tiger_g(honest);

		file << pf << "   " 
			<< (gl.leakage(pi1)) << "   "
			<< (gl.leakage(pi2)) << "   "
			<< (gl.leakage(pi3)) << "\n";
	}
}

int main() {
	arma::arma_rng::set_seed_random();

	if(system("mkdir -p crowds_data")) {}

	// Create 4 plots:
	// - Min-entropy leakage as a function of p for various values of pf
	// - Min-entropy leakage as a function of pf for various values of p
	// - same for g-leakage

	meleakage_by_p();
	meleakage_by_pf();
	gleakage_by_p();
	gleakage_by_pf();

	if(system("cd crowds_data && gnuplot ../../../samples/crowds/crowds.plt")) {}	// ignore return value without warning
}

