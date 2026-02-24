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
	parametro += { "ndamas-metodo", 0, 0, 1, "Método de conversão de N damas para CNF",
		{ "Uma variável por cada casa" }};

	instancia = { "Instance", 8,4,1000000, "Instância é o número N de damas a colocar num tabuleiro de NxN" };
}


int CNDamas::ExecutaAlgoritmo()
{
	// converter para SAT
	TString ficheiro;
	ficheiro.printf("%s%d.cnf", *ficheiroInstancia, instancia.valor)
		.writeLines(SATConverter(Parametro(NDAMAS_METODO)));

	// executar cadical
	CCaDiCaL::ExecutaAlgoritmo();

	// verificar solução
	if (VerificarSolucao(satSol, Parametro(NDAMAS_METODO))) {
		if (Parametro(NIVEL_DEBUG) > ATIVIDADE) {
			printf("\nSolução válida!");
			MostrarSolucao();
		}
	}
	else {
		printf("\nSolução inválida!");
		MostrarSolucao();
	}

	remove(ficheiro); // apagar ficheiro CNF

	return 1;
}

TVector<TString> CNDamas::SATConverter(int metodo) {
	TVector<TString> cnf;
	int N = instancia.valor;
	if (metodo == 0) {
		// comentário indicando o método de conversão
		cnf += TString().printf("c uma variável por casa");
		// número de variáveis e cláusulas, a atualizar no final 
		cnf += TString();

		// uma dama por linha / coluna
		for (int i = 0; i < N; i++) {
			TVector<int> varsLinha, varsColuna;
			for (int j = 0; j < N; j++) {
				varsLinha += Casa(i, j);
				varsColuna += Casa(j, i);
			}
			cnf += ExactlyOne(varsLinha);
			cnf += ExactlyOne(varsColuna);
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
				cnf += AtMostOne(varsDiagEsq);
			if (varsDiagDir.Count() > 1)
				cnf += AtMostOne(varsDiagDir);
		}

		// completar o início do ficheiro CNF, com o número de variáveis e clausulas
		cnf[1].printf("p cnf %d %d", N * N, cnf.Count() -2); // apenas duas linhas não são clausulas
		indicators[IND_NUMVARS] = N * N;
		indicators[IND_NUMCLAUSES] = cnf.Count() - 2;
	}

	return cnf;
}

bool CNDamas::VerificarSolucao(const TVector<int64_t>& satSol, int metodo) {
	if (metodo == 0) {
		int N = instancia.valor;
		TVector<TVector<bool>> tab;
		tab.Count(N);
		for (auto& linha : tab)
			linha.Count(N).Reset(false);
		for (int64_t var : satSol) {
			if (var > 0) {
				int id = var - 1;
				int linha = id / N;
				int coluna = id % N;
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
				return false;
			}
		}
		return true;
	}
	return false;
}

void CNDamas::MostrarSolucao() {
	int N = instancia.valor;
	TVector<TVector<bool>> tab;
	tab.Count(N);
	for (auto& linha : tab)
		linha.Count(N).Reset(false);
	for (int64_t var : satSol) {
		if (var > 0) {
			int id = var - 1;
			int linha = id / N;
			int coluna = id % N;
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
