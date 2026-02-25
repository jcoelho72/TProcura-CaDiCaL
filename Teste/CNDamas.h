#pragma once
#include "../CCaDiCaL.h"

/**
 * @class N Damas
 * @brief Class for converting N Damas to SAT and solving it with CaDiCaL.
 */
class CNDamas : public CCaDiCaL
{
public:
	CNDamas() {}
    ~CNDamas() {}


	/// Reset existing parameters of NDamas.
    void ResetParametros();
	/// Convert to SAT, execute CaDiCaL, extract solution, check.
    int ExecutaAlgoritmo();

	/// mostrar a solução satSol
	void MostrarSolucao();


protected:
	// Convert to SAT
	TVector<TString> SATConverter(int metodo=0);

	// verifica uma solução convertida por um dado método
	bool VerificarSolucao(const TVector<int64_t> &satSol, int metodo=0);

private:
	// id de uma casa
	int Casa(int linha, int coluna) { return Var(TString().printf("q %d %d",linha,coluna)); }
};