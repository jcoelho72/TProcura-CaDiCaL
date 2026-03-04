#include "CNDamas.h"
#include <stdio.h>
#include <algorithm>
#include <random>
#include <iterator>

enum EParametrosNDamas {
	NDAMAS_METODO = PARAMETROS_CADICAL
};


void CNDamas::ResetParametros()
{
	CCaDiCaL::ResetParametros();

	CCaDiCaL::solver = "./../cadical/build/cadical";

	// adicionar o novo parâmetro para a conversão de N Damas para SAT
	parametro += {
		"Método", 0, 0, 2, "Método de conversão de N damas para CNF",
		{
			"Quadrática (pares negativos)",
			"Sequential Counter (Sinz)",
			"Unário por linha + SC nas diagonais"
		}
	};

	instancia = { "Instance", 8,4,1000000, "Instância é o número N de damas a colocar num tabuleiro de NxN" };
}


int CNDamas::ExecutaAlgoritmo()
{
	// converter para SAT
	TString ficheiro;
	ficheiro.printf("%s%d.cnf", *ficheiroInstancia, mpiID)
		.writeLines(SATConverter(Parametro(NDAMAS_METODO)));

	// executar cadical
	CCaDiCaL::ExecutaAlgoritmo();

	if (indicators[IND_RESULTADO] == 1) {
		// verificar solução
		if (VerificarSolucao(satSol, Parametro(NDAMAS_METODO))) {
			if (Parametro(NIVEL_DEBUG) > ATIVIDADE) {
				printf("\nSolução válida!");
				MostrarSolucao();
			}
		}
		else {
			printf("\nSolução inválida!");
			indicators[IND_RESULTADO] = -1; // código para indicar que houve problema
		}
	}

	if (Parametro(NIVEL_DEBUG) < DETALHE)
		remove(ficheiro); // apagar ficheiro CNF

	return 1;
}

TVector<TString> CNDamas::SATConverter(int metodo) {
	TVector<TString> cnf;
	int N = instancia.valor;
	ResetHashtable();
	// comentário indicando o método de conversão
	cnf += TString().printf("c conversão %d damas para CNF, método %d.", N, metodo);
	// número de variáveis e cláusulas, a atualizar no final 
	cnf += TString();
	// criação das variáveis q(i,j) para indicar se há uma dama na casa (i,j)
	for (int i = 0; i < N; i++) 
		for (int j = 0; j < N; j++) 
			Var(TString().printf("q %d %d", i, j));

	if (metodo <= 1) {
		// uma dama por linha / coluna
		for (int i = 0; i < N; i++) {
			TVector<int> varsLinha, varsColuna;
			for (int j = 0; j < N; j++) {
				varsLinha += Casa(i, j);
				varsColuna += Casa(j, i);
			}
			cnf += ExactlyOne(varsLinha, metodo > 0);
			cnf += ExactlyOne(varsColuna, metodo > 0);
		}
	}
	else if (metodo == 2) {
		// criar variáveis unárias para cada linha, indicando a coluna onde está a dama
		for (int i = 0; i < N; i++) {
			TVector<int> varsColuna;
			cnf += CreateUnaryVar(TString().printf("u %d", i), 0, N - 1);
			// ligação com variáveis q (não necessário, apenas para manter o teste igual para todas as codificações)
			// u(i,j) = 1 e u(i,j+1) = 0  <=>  q(i,j)
			// -q(i,j) v u(i,j), -q(i,j) v -u(i,j+1), -u(i,j) v u(i,j+1) v q(i,j) 
			for (int j = 0; j < N; j++) {
				int qij = Casa(i, j);
				int uij = Var(TString().printf("u %d %d", i, j));
				int uij1 = Var(TString().printf("u %d %d", i, j + 1));
				varsColuna += Casa(j, i);
				cnf += TString().printf("-%d %d 0", qij, uij); // -q(i,j) v u(i,j)
				cnf += TString().printf("-%d -%d 0", qij, uij1); // -q(i,j) v -u(i,j+1)
				cnf += TString().printf("-%d %d %d 0", uij, uij1, qij); // -u(i,j) v u(i,j+1) v q(i,j) 
			}
			// uma dama por coluna
			cnf += ExactlyOne(varsColuna, true);
		}
	}
	// uma dama por diagonal esq(\) e dir(/)
	for (int d = -N + 1; d < 2 * N - 1; d++) {
		TVector<int> varsDiagEsq, varsDiagDir;
		for (int i = 0; i < N; i++) {
			{	// diagonal esq(\)
				int j = i + d;
				if (j >= 0 && j < N)
					varsDiagEsq += Casa(i, j);
			}
			{	// diagonal dir(/)
				int j = d - i;
				if (j >= 0 && j < N)
					varsDiagDir += Casa(i, j);
			}
		}
		if (varsDiagEsq.Count() > 1)
			cnf += AtMostOne(varsDiagEsq, metodo > 0);
		if (varsDiagDir.Count() > 1)
			cnf += AtMostOne(varsDiagDir, metodo > 0);

	}
	// completar o início do ficheiro CNF, com o número de variáveis e clausulas
	cnf[1].printf("p cnf %d %d", variaveis.Count() - 1, cnf.Count() - 2); // apenas duas linhas não são clausulas
	indicators[IND_NUMVARS] = variaveis.Count() - 1;
	indicators[IND_NUMCLAUSES] = cnf.Count() - 2;

	return cnf;
}

bool CNDamas::VerificarSolucao(const TVector<int64_t>& satSol, int metodo) {
	int N = instancia.valor;
	TVector<TVector<bool>> tab;
	tab.Count(N);
	for (auto& linha : tab)
		linha.Count(N).Reset(false);
	for (int64_t var : satSol) {
		if (var > 0) {
			int linha, coluna;
			if (sscanf(Var(var), "q %d %d", &linha, &coluna) == 2)
				tab[linha][coluna] = true;
		}
	}
	// verificar linhas e colunas
	for (int i = 0; i < N; i++) {
		int countLinha = 0, countColuna = 0;
		for (int j = 0; j < N; j++) {
			if (tab[i][j])
				countLinha++;
			if (tab[j][i])
				countColuna++;
		}
		if (countLinha != 1 || countColuna != 1) {
			printf("\nErro na linha %d: %d damas, coluna %d: %d damas",
				i, countLinha, i, countColuna);
			//MostrarSolucao();
			return false;
		}
	}
	// verificar diagonais
	for (int d = -N + 1; d < 2 * N - 1; d++) {
		int countDiagEsq = 0, countDiagDir = 0;
		for (int i = 0; i < N; i++) {
			int jEsq = i + d;
			if (jEsq >= 0 && jEsq < N && tab[i][jEsq])
				countDiagEsq++;
			int jDir = d - i;
			if (jDir >= 0 && jDir < N && tab[i][jDir])
				countDiagDir++;
		}
		if (countDiagEsq > 1 || countDiagDir > 1) {
			printf("\nErro na diagonal esq(%d): %d damas, diag dir(%d): %d damas",
				d, countDiagEsq, d, countDiagDir);
			MostrarSolucao();
			return false;
		}
	}
	return true;
}

void CNDamas::MostrarSolucao() {
	int N = instancia.valor;
	TVector<TVector<bool>> tab;
	tab.Count(N);
	for (auto& linha : tab)
		linha.Count(N).Reset(false);
	for (int64_t var : satSol) {
		if (var > 0) {
			int linha, coluna;
			if (sscanf(Var(var), "q %d %d", &linha, &coluna) == 2)
				tab[linha][coluna] = true;
		}
	}
	// mostrar o tabuleiro
	for (auto& linha : tab) {
		printf("\n");
		for (auto& casa : linha)
			printf(casa ? "Q " : ". ");
	}
}
