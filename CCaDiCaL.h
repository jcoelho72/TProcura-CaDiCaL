#pragma once
#include "TProcura/TProcura.h"

/**
 * @class CCaDiCaL
 * @brief Class for running CaDiCaL, in the framework TProcura.
 *
 * @see TProcura
 */
class CCaDiCaL : public TProcura
{
public:
    CCaDiCaL() {}
    ~CCaDiCaL() {}

	TVector<int64_t> indicators; ///< Vector to store indicators extracted from the last run.
	TVector<int> defaultCadical; ///< default parameters for CaDiCaL
	// SAT solution, a set of integers
	TVector<int64_t> satSol;

	/// Reset existing parameters of CaDiCaL.
    void ResetParametros();
	/// Execute the CaDiCaL with current parameters.
    int ExecutaAlgoritmo();
	/// return the indicator value after execution.
    int64_t Indicador(int id);
	/// @brief  retorna um vetor de inteiros com a codificação da solução (esta codificação será adicionada aos indicadores, no ficheiro CSV de resultados)
	TVector<int64_t> CodificarSolucao() { return satSol; }

};