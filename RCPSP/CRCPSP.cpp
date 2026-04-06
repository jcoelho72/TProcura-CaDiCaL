#include "CRCPSP.h"
#include <stdio.h>
#include <algorithm>
#include <random>
#include <iterator>

enum EParametrosRCPSP {
	RCPSP_METODO = PARAMETROS_CADICAL,
	RCPSP_MAXVARS,
	RCPSP_MAXCLAUSES,
	RCPSP_UB_LB, // upper and lower bounds to use in conversion (LST/CPM or best of 13 PRs and LBs)
	RCPSP_HORIZON, // time horizon to use in conversion to add to current LB 
};

TVector<TAtivity> CRCPSP::act = {}; // activities (including dummy start and end)
TVector<int> CRCPSP::capacity = {}; // resource capacities
TVector<TBits> CRCPSP::predBits; // bitset of predecessors (direct and indirect)
double CRCPSP::indCNC, CRCPSP::indOS, CRCPSP::indSP,
CRCPSP::indAD, CRCPSP::indLA, CRCPSP::indI5, CRCPSP::indTF;
bool CRCPSP::mophologicOK = false;
int CRCPSP::bestUB = 0, CRCPSP::bestLB = 0; // best upper and lower bounds, to use in conversion
TVector<int> CRCPSP::es, CRCPSP::ls; // time windows of SAT conversion
int CRCPSP::calls; // number of calls to SAT solver

void CRCPSP::ResetParametros()
{
	CCaDiCaL::ResetParametros();

	CCaDiCaL::solver = "./../cadical/build/cadical";

	if (ficheiroInstancia.Empty())
		ficheiroInstancia = "Instancias/DC1/mv";

	// adicionar o novo parâmetro para a conversão de N Damas para SAT
	parametro += {
		"Resources method", 0, 0, 3, "Converting resources optinos",
		{
			"COV",
			"MFS",
			"BDD",
			"ADD"
		}
	};

	parametro += {
		"MaxVars", 1000000, 1000, 1000000000,
			"Limite máximo de variáveis permitidas na conversão para CNF"
	};

	parametro += {
		"MaxClauses", 10000000, 1000, 2000000000,
			"Limite máximo de cláusulas permitidas na conversão para CNF"
	};

	parametro += {
		"RCPSP_UB_LB", 1, 0, 1, "upper and lower bounds to use in conversion",
		{
			"LST/CPM",
			"13 PRs/LBs"
		}
	};

	parametro += {
		"RCPSP_HORIZON", -3, -3, 1000, "Time horizon to use in conversion to add to current LB. If -1 runs LBS, -2 UBS, -3 DBS."
	};

	instancia = { "Instance", 1,1,1000000, "Instance loaded in rcp format" };

	// project indicators (not active in beginning, since are too many)
	indicador += {
		{ "UB", "best upper bound found", IND_UB },
		{ "LB", "best lower bound found", IND_LB },
		{ "CALLS", "number of SAT calls", IND_CALLS },
		{ "N", "Number of activities" },
		{ "CNC", "Complexity indicator" },
		{ "OS", "Order strenght" },
		{ "SP", "Serial parallel" },
		{ "AD", "Activity distribution" },
		{ "LA", "length arcs" },
		{ "I5", "Long arcs" },
		{ "TF", "Total float" },
		{ "WPREC", "Number of activities" },
		{ "WALL", "Number of activities" },
		{ "F21", "Number of activities" },
		{ "F22", "Number of activities" },
		{ "F31", "Number of activities" },
		{ "F32", "Number of activities" },
		{ "RF", "Resource factor" },
		{ "RU", "Resource usage" },
		{ "RS", "Resource strength" },
		{ "RC", "Resource constraintness" },
		{ "LBcp", "Lower bound critical path" },
		{ "LBc", "Lower bound resource capacity" },
		{ "LBcpc", "Lower bound critical path and capacity" },
		{ "LBcs", "Lower bound critical sequence" },
		{ "LBip", "Lower bound incompatible pairs" },
		{ "LBit", "Lower bound incompatible triplets" },
		{ "LBnp0", "Lower bound node packing mode 0" },
		{ "LBnp1", "Lower bound node packing mode 1" },
		{ "LBnp2", "Lower bound node packing mode 2" },
		{ "LBpm0", "Lower bound parallel machine mode 0" },
		{ "LBpm1", "Lower bound parallel machine mode 1" },
		{ "LBpm2", "Lower bound parallel machine mode 2" },
		{ "LBpm3", "Lower bound parallel machine mode 3" },
		{ "LBpr", "Lower bound reduction precedences" },
		{ "LBct", "Lower bound reduction core times" },
		{ "LBtp", "Lower bound reduction time period" },
		{ "LBall", "Number of activities" },

	};

	indAtivo += { IND_UB, IND_LB, IND_CALLS };

	// para poder correr em MPI e gerar a mesma instância com diferentes parâmetros
	// é preciso que o prefixo da instância seja único
	//ficheiroInstancia += TString(mpiID);

}


// load instance from file, and initialize variables
void CRCPSP::Inicializar(void) {
	int id = 0;
	CCaDiCaL::Inicializar();

	// clean datastructure
	act = {};
	capacity = {};
	predBits = {};
	mophologicOK = false;
	bestLB = bestUB = 0;
	es = ls = {};
	calls = 0;

	// load and convert to ints
	TVector<int> nums;
	for (auto& line : TString().printf("%s%d.rcp", *ficheiroInstancia, instancia.valor).readLines())
		for (auto& num : line.tok())
			nums += atoi(num);

	// process numnbers:
	// N, R, <capacity> (all resources), <pt res1 res2 ... #sucs suc1 suc2 ...>  (for all activities)
	if (nums.Count() >= 2) {
		act.Count(nums[id++]); // N
		capacity.Count(nums[id++]); // R
	}
	// load capacity
	if (nums.Count() >= id + R()) {
		for (int i = 0; i < R(); i++)
			capacity[i] = nums[id++];
	}
	// reset of precedence relations
	for (auto& activity : act) {
		activity.pred.Count(0);
		activity.predBits.Count((N(true) - 63) / 64).Reset(0);
	}
	// load activities
	for (int i = 0; i < N(true) && id < nums.Count(); i++) {
		act[i].pt = nums[id++]; // processing time
		act[i].res.Count(R()); // resource usage
		for (int j = 0; j < act[i].res.Count() && id < nums.Count(); j++)
			act[i].res[j] = nums[id++]; // resource usage
		int numSucs = nums[id++]; // number of successors
		for (int s = 0; s < numSucs && id < nums.Count(); s++) {
			act[i].suc += nums[id] - 1; // successors (0-indexed)
			act[nums[id] - 1].pred += i; // predecessors (0-indexed)
			act[nums[id] - 1].predBits.SetBit(i, true); // bitset of predecessors for quick checking
			id++;
		}
	}

	// update predBits (direct and indirect)
	predBits.Count(N(true)).Reset(0);
	for (int i = 0; i < N(true); i++) {
		predBits[i] = act[i].predBits;
		for (auto& pred : act[i].pred)
			predBits[i] |= predBits[pred];
	}

	// carregar valores sem debug
	ENivelDebug backupDebug = (ENivelDebug)Parametro(NIVEL_DEBUG);
	Parametro(NIVEL_DEBUG) = NADA;
	if (Parametro(RCPSP_UB_LB) == 0) {
		bestLB = LBCPM(); // CPM
		PriorityRule(9); // LST
	}
	else { // use all LBs and PRs, and take the best of them
		bestLB = LBAll();
		BestPR();
	}
	bestUB = st.Last();
	Parametro(NIVEL_DEBUG) = backupDebug;
}



int CRCPSP::ExecutaAlgoritmo()
{
	calls = 0;
	// search strategy
	if (Parametro(RCPSP_HORIZON) < 0) {
		int totalTime = 0; // sum the time from all runs
		int backup = Parametro(RCPSP_HORIZON);
		TVector<int> bestSolution;
		bestSolution = st;

		if (backup == -1) { // LBS 
			Parametro(RCPSP_HORIZON) = 0;
			while (bestLB < bestUB) {
				ExecutaAlgoritmo();
				calls++;
				totalTime += indicators[IND_TEMPO];
				if (indicators[IND_RESULTADO] == 1) { // resolvido, solução
					bestUB = bestLB;
					bestSolution = st; // save best solution found
				}
				else if (indicators[IND_RESULTADO] == 2) { // impossível
					bestLB++;
					Parametro(RCPSP_HORIZON)++;
				}
				else
					break; // not solved, stop
			}
		}
		else if (Parametro(RCPSP_HORIZON) == -2) { // UBS
			Parametro(RCPSP_HORIZON) = bestUB - bestLB - 1;
			while (bestLB < bestUB) {
				ExecutaAlgoritmo();
				calls++;
				totalTime += indicators[IND_TEMPO];
				if (indicators[IND_RESULTADO] == 1) { // resolvido, solução
					bestUB = bestLB + Parametro(RCPSP_HORIZON);
					Parametro(RCPSP_HORIZON)--;
					bestSolution = st; // save best solution found
				}
				else if (indicators[IND_RESULTADO] == 2) // impossible
					bestLB = bestUB;
				else
					break; // not solved, stop
			}
		}
		else if (Parametro(RCPSP_HORIZON) == -3) { // DBS
			Parametro(RCPSP_HORIZON) = (bestUB - bestLB) / 2;
			while (bestLB < bestUB) {
				ExecutaAlgoritmo();
				calls++;
				totalTime += indicators[IND_TEMPO];
				if (indicators[IND_RESULTADO] == 1) { // resolvido, solução
					bestUB = bestLB + Parametro(RCPSP_HORIZON);
					Parametro(RCPSP_HORIZON) = (bestUB - bestLB) / 2;
					bestSolution = st; // save best solution found
				}
				else if (indicators[IND_RESULTADO] == 2) { // impossível
					bestLB = bestLB + Parametro(RCPSP_HORIZON) + 1;
					Parametro(RCPSP_HORIZON) = (bestUB - bestLB) / 2;
				}
				else
					break; // not solved, stop
			}
		}

		indicators[IND_TEMPO] = totalTime;
		st = bestSolution; // restore best solution found
		if (bestLB == bestUB)
			indicators[IND_RESULTADO] = 1; // mark as solved, since we know the optimal makespan
		Parametro(RCPSP_HORIZON) = backup;
	}
	else {
		// single run with given horizon
		// converter para SAT
		TString ficheiro;
		ficheiro.printf("%s%d.cnf", *ficheiroInstancia, instancia.valor)
			.writeLines(SATConverter(Parametro(RCPSP_METODO)));

		if (conversaoOK) {
			// executar cadical
			CCaDiCaL::ExecutaAlgoritmo();
			calls = 1;

			if (indicators[IND_RESULTADO] == 1) {
				// verificar solução
				if (VerificarSolucao(satSol, Parametro(RCPSP_METODO))) {
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
		}
		else {
			if (TempoExcedido())
				indicators[IND_RESULTADO] = -2; // código para indicar que houve problema no tempo na conversão
			else
				indicators[IND_RESULTADO] = -3; // código para indicar que houve problema no tamanho da conversão
		}

		if (Parametro(NIVEL_DEBUG) < DETALHE)
			remove(ficheiro); // apagar ficheiro CNF
	}

	return 1;
}

bool CRCPSP::TamanhoOK(TVector<TString>& cnf) {
	return variaveis.Count() - 1 <= Parametro(RCPSP_MAXVARS) &&
		cnf.Count() - 2 <= Parametro(RCPSP_MAXCLAUSES);
}


TVector<TString> CRCPSP::SATConverter(int metodo) {
	TVector<TString> cnf;
	conversaoOK = false;
	LimparEstatisticas();
	ResetHashtable();
	CPM(es, &ls, bestLB + Parametro(RCPSP_HORIZON)); // calculate es and ls for the given makespan (bestLB + horizon)
	// starting comment
	cnf += TString().printf("c (%s%d) RCPSP to CNF, method %d.", *ficheiroInstancia, instancia.valor, metodo);
	cnf += TString(); // fill in the end, after processing all data

	// access to variables to create IDs
	// add l(j,t) variables for each activity j and time t, indicating if activity j starts at or less instant t
	for (int i = 0; i < N(true); i++)
		for (int t = es[i]; t <= ls[i]; t++)
			VarL(i, t);
	// add u(j,t), in the time window - activity j running at instant t
	for (int i = 0; i < N(true); i++)
		for (int t = es[i]; t < ls[i] + act[i].pt; t++)
			VarU(i, t);

	// add clauses for CPM constraints
	for (int i = 0; i < N(true) && TamanhoOK(cnf) && !TempoExcedido(); i++) // Range limits
		cnf += TString().printf("%d 0", VarL(i, ls[i])); // l(j,ls[j]) = 1

	// Consistency: l(i,t) => l(i,t+1)   -l(j,t) + l(j,t+1)
	for (int i = 0; i < N(true) && TamanhoOK(cnf) && !TempoExcedido(); i++)
		for (int t = es[i]; t < ls[i]; t++)
			cnf += TString().printf("-%d %d 0", VarL(i, t), VarL(i, t + 1));

	// Link: u(i,t) <=> (l(i,t) & ~l(i,t-di))
	// -u(j,t) + l(j,t)  | -u(j,t) + -l(j,t-dj) | -l(j,t) + l(j,t-dj) + u(j,t)
	for (int i = 0; i < N(true) && TamanhoOK(cnf) && !TempoExcedido(); i++)
		for (int t = es[i]; t < ls[i] + act[i].pt; t++) {
			if (t <= ls[i])
				cnf += TString().printf("-%d %d 0", VarU(i, t), VarL(i, t)); // -u(j,t) + l(j,t)
			if (t - act[i].pt >= es[i])
				cnf += TString().printf("-%d -%d 0", VarU(i, t), VarL(i, t - act[i].pt)); // -u(j,t) + -l(j,t-dj)
			cnf += TString().printf("%d ", VarU(i, t)); // u(j,t)
			if (t < ls[i])
				cnf.Last().printf("-%d ", VarL(i, t)); // -l(j,t)
			if (t - act[i].pt >= es[i])
				cnf.Last().printf("%d ", VarL(i, t - act[i].pt)); // l(j,t-dj) 
			cnf.Last().printf("0");
		}

	// Precedence(i, i'): l(i', t) => l(i, t - di)
	// -l(i,t) + l(j,t-dj)
	for (int i = 0; i < N(true) && TamanhoOK(cnf) && !TempoExcedido(); i++)
		for (auto& suc : act[i].suc)
			for (int t = es[suc]; t <= ls[suc]; t++)
				if (es[i] <= t - act[i].pt && t - act[i].pt <= ls[i]) // only add clause if t-di is in the time window of i
					cnf += TString().printf("-%d %d 0", VarL(suc, t), VarL(i, t - act[i].pt));


	// add resource restrictions for all time instants
	for (int t = 0; t <= bestLB + Parametro(RCPSP_HORIZON) && TamanhoOK(cnf) && !TempoExcedido(); t++) {
		TVector<int> inTW, inFS; // activities in time window (can be running at time t)
		for (int i = 1; i <= N(); i++)
			if (es[i] <= t && t < ls[i] + act[i].pt) // if activity i can be running at time t
				inTW += i;
		if (metodo == 0) // COV - covers
			cnf += AddCOV(inTW, inFS, t);
		else if (metodo == 1) // MFS - Minimal Forbiddent Sets
			cnf += AddMFS(inTW, t);
		else if (metodo == 2) // BDD - Binary Decision Diagrams
			cnf += AddBDD(inTW, t);
		else if (metodo == 3) // ADD - Adders
			cnf += AddADD(inTW, t);
	}


	// number of variables and clauses
	cnf[1].printf("p cnf %d %d", variaveis.Count() - 1, cnf.Count() - 2);
	indicators[IND_NUMVARS] = variaveis.Count() - 1;
	indicators[IND_NUMCLAUSES] = cnf.Count() - 2;

	conversaoOK = TamanhoOK(cnf) && !TempoExcedido();

	return cnf;
}

TVector<TString> CRCPSP::AddCOV(TVector<int>& inTW, TVector<int>& inFS, int t) {
	static TVector<int> resUsage;
	TVector<TString> cnf;
	int j;

	if (inTW.Empty())
		return cnf;

	if (inFS.Empty())
		resUsage.Count(R()).Reset(0);

	// try with top activity
	inFS.Push(j = inTW.Pop());
	for (int k = 0; k < R(); k++)
		resUsage[k] += act[j].res[k];

	// check if resources are violated
	if (ResourcesViolated(resUsage)) {
		bool minimal = true;
		// check if is minimal cover (test if removing any activity from inFS makes it feasible)
		for (int i = 0; minimal && i < inFS.Count() - 1; i++) {
			TVector<int> resTest;
			resTest = resUsage;
			for (int k = 0; k < R(); k++)
				resTest[k] -= act[inFS[i]].res[k];
			minimal = !ResourcesViolated(resTest);
		}
		if (minimal) {
			// add clause to forbid this combination of activities in time t
			cnf += TString();
			for (auto& activity : inFS)
				cnf.Last().printf("-%d ", VarU(activity, t));
			cnf.Last().printf("0");
		}
	}
	else
		cnf += AddCOV(inTW, inFS, t);


	inFS.Pop();
	for (int k = 0; k < R(); k++)
		resUsage[k] -= act[j].res[k];

	// try without top activity
	cnf += AddCOV(inTW, inFS, t);

	// left the set identical
	inTW.Push(j);

	return cnf;
}

bool CRCPSP::ResourcesViolated(TVector<int>& resUsage) {
	for (int k = 0; k < R(); k++)
		if (resUsage[k] > capacity[k])
			return true;
	return false;
}


TVector<TString> CRCPSP::AddMFS(TVector<int> activities, int t) {
	TVector<TString> cnf;
	return cnf;
}

TVector<TString> CRCPSP::AddBDD(TVector<int> activities, int t) {
	TVector<TString> cnf;
	return cnf;
}

TVector<TString> CRCPSP::AddADD(TVector<int> activities, int t) {
	TVector<TString> cnf;
	return cnf;
}

bool CRCPSP::VerificarSolucao(const TVector<int64_t>& satSol, int metodo) {
	TVector<int> instants, onProgress, resUsed;
	resUsed.Count(R()).Reset(0);

	// extract the starting times
	st = es; // initialize with earliest start times, and update with the solution
	for (int64_t var : satSol) {
		int i, t;
		// can exist only variables true, if time windows is null, but in that case will be equal to es
		if (var < 0) {
			if (sscanf(Var(-var), "l %d %d", &i, &t) == 2) {
				// if l(i,t) = 1, st[i] <= t
				// if l(i,t) = 0, st[i] > t
				if (st[i] <= t)
					st[i] = t + 1;
				//				printf("l(%d,%d) = 0 => st[%d] > %d\n", i, t, i, t);
			}
		}
	}

	// check precedence constraints
	for (int i = 1; i <= N(); i++)
		for (auto& suc : act[i].suc)
			if (st[i] + act[i].pt > st[suc]) {
				printf("\nPrecedence violation: activity %d finishes at %d, but successor %d starts at %d", i, st[i] + act[i].pt, suc, st[suc]);
				MostrarSolucao();
				return false;
			}

	// add instants when activities start and end
	instants = st;
	for (int i = 1; i <= N(); i++)
		instants += st[i] + act[i].pt;
	instants.BeASet();
	bestUB = instants.Last();

	for (auto& t : instants) {
		for (int i = 1; i <= N(); i++)
			if (st[i] == t) {
				onProgress += i;
				for (int k = 0; k < R(); k++)
					resUsed[k] += act[i].res[k];
			}
			else if (st[i] + act[i].pt == t) {
				onProgress -= i;
				for (int k = 0; k < R(); k++)
					resUsed[k] -= act[i].res[k];
			}
		for (int k = 0; k < R(); k++)
			if (resUsed[k] > capacity[k]) {
				printf("\nResource violation in %d: resource %d use %d, capacity %d", t, k, resUsed[k], capacity[k]);
				MostrarSolucao();
				return false;
			}
	}

	return true;
}

void CRCPSP::MostrarSolucao() {
	TVector<TString> lines;
	TVector<int> instants, onProgress, resUsed;
	resUsed.Count(R()).Reset(0);

	lines += TString().printf("Solution to %s%d.rcp (%s%d %s%d)",
		*ficheiroInstancia, instancia.valor, Icon(EIcon::LB), bestLB, Icon(EIcon::UB), bestUB);

	// add instants when activities start and end
	instants = st;
	for (int i = 1; i <= N(); i++)
		instants += st[i] + act[i].pt;
	instants.BeASet();

	lines += TString().printf("  t%s", Icon(EIcon::TEMPO));
	for (int k = 0; k < R(); k++)
		lines.Last() += Res2Str(capacity[k], k);
	lines.Last().printf("Activities%s", Icon(EIcon::ID));

	for (auto& t : instants) {
		lines += TString().printf("%3d%s", t, Icon(EIcon::TEMPO));
		for (int i = 1; i <= N(); i++)
			if (st[i] == t) {
				onProgress += i;
				for (int k = 0; k < R(); k++)
					resUsed[k] += act[i].res[k];
			}
			else if (st[i] + act[i].pt == t) {
				onProgress -= i;
				for (int k = 0; k < R(); k++)
					resUsed[k] -= act[i].res[k];
			}
		for (int k = 0; k < R(); k++)
			lines.Last() += Res2Str(resUsed[k], k);
		for (auto& activity : onProgress)
			lines.Last() += Act2Str(activity, 0, 1);
	}
	MostraCaixa(lines);
}

// scheduling scheme methods (to implement priority rules, needed in conversion)
void CRCPSP::Serial(TVector<int> ids) {
	// schedule each activity by id order in the first available time slot
	int horizon = 0;
	TVector<TVector<int>> resAvailable;
	resAvailable.Count(R());
	st.Count(N(true)).Reset(0);
	for (auto& activity : act)
		horizon += activity.pt;
	for (int k = 0; k < R(); k++)
		resAvailable[k].Count(horizon).Reset(capacity[k]);
	for (auto& id : ids) {
		int startTime = 0;
		// check predecessors of this activity, when finish
		for (auto& pred : act[id].pred)
			if (startTime < st[pred] + act[pred].pt)
				startTime = st[pred] + act[pred].pt;
		// find the first time slot where resources are available for the duration of the activity
		while (true) {
			bool canSchedule = true;
			for (int t = startTime + act[id].pt - 1; t >= startTime && canSchedule; t--)
				for (int r = 0; r < R() && canSchedule; r++)
					if (resAvailable[r][t] < act[id].res[r]) {
						startTime = t + 1; // move to the next time slot after this one
						canSchedule = false;
					}
			if (canSchedule) {
				st[id] = startTime;
				for (int t = startTime; t < startTime + act[id].pt; t++)
					for (int r = 0; r < R(); r++)
						resAvailable[r][t] -= act[id].res[r];
				break;
			}
		}
	}
}

void CRCPSP::Parallel(TVector<int> ids) {

}

// calculate es and ls of activities for a given makespan
void CRCPSP::CPM(TVector<int>& es, TVector<int>* ls, int makespan) {
	es.Count(N(true)).Reset(0);
	for (int i = 0; i < N(true); i++) {
		es[i] = 0;
		for (auto& pred : act[i].pred)
			if (es[i] < es[pred] + act[pred].pt)
				es[i] = es[pred] + act[pred].pt;
	}
	if (makespan < 0)
		makespan = es.Last();
	if (ls != NULL) {
		for (int i = N(true) - 1; i >= 0; i--) {
			(*ls)[i] = makespan - act[i].pt;
			for (auto& suc : act[i].suc)
				if ((*ls)[i] > (*ls)[suc] - act[i].pt)
					(*ls)[i] = (*ls)[suc] - act[i].pt;
		}
	}
}


// correct id order according to network of predecessors and successors (for scheduling scheme methods)
void CRCPSP::TopologicalSort(TVector<int>& ids) {
	TBits notProcessed;
	notProcessed.Count((N(true) + 63) / 64).Reset(0);
	notProcessed = ~notProcessed;
	for (int i = 0; i < N(true); i++) {
		// all predecessors need to be processed
		if (act[ids[i]].predBits & notProcessed) {
			// find the next activity that can be processed
			for (int j = i + 1; j < N(true); j++) {
				if (!(act[ids[j]].predBits & notProcessed)) {
					// swap ids[i] and ids[j]
					int temp = ids[i];
					ids[i] = ids[j];
					ids[j] = temp;
					break;
				}
			}
		}
		notProcessed.SetBit(ids[i], false); // mark this activity as processed
	}
}

// show the instance data (for debugging)
void CRCPSP::Debug(bool completo) {
	TVector<TString> lines;
	lines += TString().printf("Instância: %s%d.rcp (%s%d %s%d)",
		*ficheiroInstancia, instancia.valor, Icon(EIcon::LB), bestLB, Icon(EIcon::UB), bestUB);
	lines += TString().printf(" ID%sPT%s", Icon(EIcon::ID), Icon(EIcon::TEMPO));
	for (int k = 0; k < R(); k++)
		lines.Last() += Res2Str(capacity[k], k);
	lines.Last().printf("Pred%s", Icon(EIcon::ID));
	for (int i = 0; i < N(true); i++) {
		lines += Act2Str(i, 3).printf("%2d%s", act[i].pt, Icon(EIcon::TEMPO));
		for (int k = 0; k < R(); k++)
			lines.Last() += Res2Str(act[i].res[k], k);
		for (auto& pred : act[i].pred)
			lines.Last() += Act2Str(pred);
	}
	MostraCaixa(lines);
}

// show resource usage with colors depending on usage
TString CRCPSP::Res2Str(int usage, int resource, bool compact) {
	if (usage <= 0)
		return TString().printf("%*s%s", (compact ? 0 : 2), "", (compact ? "" : Icon(EIcon::ELEMENTO)));
	if (usage <= capacity[resource])
		return TString().printf("%s%*d%s%s",
			*HSL(resource * 360 / R(), .75 * usage / capacity[resource] + 0.25,
				0.1 * usage / capacity[resource] + 0.2, true),
			(compact ? 0 : 2), usage, *HSL(), (compact ? "" : Icon(EIcon::ELEMENTO)));
	// if usage exceeds capacity, show in full color and with black font
	return TString().printf("%s%s%*d%s%s",
		*HSL(resource * 360 / R(), 1, 0.5, true),
		*HSL(resource * 360 / R(), 1, 0, false),
		(compact ? 0 : 2), usage, *HSL(), (compact ? "" : Icon(EIcon::ELEMENTO)));
}

// show ID activity in color
// extra: 0 - only activity ID, 1 - resources and predecessors, 2 - processing time and predecessors
TString CRCPSP::Act2Str(int id, int space, int extra) {
	TString resText, predText;
	if (extra == 2 || extra == 3)
		resText.printf(":%d%s", act[id].pt, extra == 3 ? ":" : "");
	for (int k = 0; (extra == 1 || extra == 3) && k < R(); k++)
		if (act[id].res[k] > 0) {
			if (resText.Empty())
				resText = ":";
			resText += Res2Str(act[id].res[k], k, true);
		}
	for (int j = 0; extra > 0 && j < act[id].pred.Count(); j++)
		if (act[id].pred[j] > 0) { // do not show dummy activity 
			if (predText.Empty())
				predText = "(";
			predText += Act2Str(act[id].pred[j]);
		}
	if (!predText.Empty()) // last space became a parenthesis
		predText[predText.Count() - 2] = ')';
	if (space == 0)
		return TString().printf("%s%d%s%s%s ", *HSL(id * 360.0 / N(true), 1, 0.5, false), id, *HSL(), *resText, *predText);
	return TString().printf("%s%*d%s%s%s%s", *HSL(id * 360.0 / N(true), 1, 0.5, false), space, id, *HSL(), *resText, *predText, Icon(EIcon::ID));
}



int64_t CRCPSP::Indicador(int id) {
	if (id < IND_CADICAL)
		return CCaDiCaL::Indicador(id);

	if (id == IND_N)
		return N();
	else if (id == IND_UB)
		return bestUB;
	else if (id == IND_LB)
		return bestLB;
	else if (id == IND_CALLS)
		return calls;

	if (id >= IND_CNC && id <= IND_TF) {
		MorphologicalIndicators();
		switch (id) {
		case IND_CNC:
			return (int64_t)(indCNC * 1000);
		case IND_OS:
			return (int64_t)(indOS * 1000);
		case IND_SP:
			return (int64_t)(indSP * 1000);
		case IND_AD:
			return	(int64_t)(indAD * 1000);
		case IND_LA:
			return (int64_t)(indLA * 1000);
		case IND_I5:
			return (int64_t)(indI5 * 1000);
		case IND_TF:
			return	(int64_t)(indTF * 1000);
		}
	}

	switch (id) {
	case IND_RF:
		return RF();
	case IND_RU:
		return RU();
	case IND_RS:
		return RS();
	case IND_RC:
		return RC();
	case IND_WPREC:
		return W();
	case IND_WALL:
		return W(false);
	case IND_FS21:
		return FS(2, 1);
	case IND_FS22:
		return FS(2, 2);
	case IND_FS31:
		return FS(3, 1);
	case IND_FS32:
		return FS(3, 2);
	case IND_LB_CPM:
		return LBCPM();
	case IND_LB_C:
		return LBC();
	case IND_LB_CPC:
		return LBCPC();
	case IND_LB_CS:
		return LBCS();
	case IND_LB_CT:
		return LBCT();
	case IND_LB_IP:
		return LBIP();
	case IND_LB_IT:
		return LBIT();
	case IND_LB_NP0:
		return LBNodePacking(0);
	case IND_LB_NP1:
		return LBNodePacking(1);
	case IND_LB_NP2:
		return LBNodePacking(2);
	case IND_LB_PM0:
		return LBParallelMachine(0);
	case IND_LB_PM1:
		return LBParallelMachine(1);
	case IND_LB_PM2:
		return LBParallelMachine(2);
	case IND_LB_PM3:
		return LBParallelMachine(3);
	case IND_LB_PR:
		return LBPR();
	case IND_LB_TP:
		return LBTP();
	case IND_LB_ALL:
		return LBAll();
	default:
		return 0;
	}
}

void CRCPSP::MorphologicalIndicators() {
	TVector<int> PL, RL, WA, n;

	if (mophologicOK)
		return;

	PL.Count(N(true)).Reset(0);
	RL.Count(N(true)).Reset(0);

	// update PL and RL
	for (int i = 0; i < N(true); i++) {
		PL[i] = 0;
		for (auto pred : act[i].pred)
			if (PL[pred] + 1 > PL[i])
				PL[i] = PL[pred] + 1;
	}
	int M = PL.Last() - 1; // number of levels (without dummy activities)
	for (int i = N(true) - 1; i >= 0; i--) {
		RL[i] = M + 1;
		for (auto suc : act[i].suc)
			if (RL[suc] - 1 < RL[i])
				RL[i] = RL[suc] - 1;
	}
	if (N() > 0)
		indSP = (double)(M - 1) / (N() - 1); // (M-1)/(N-1)
	else
		indSP = 0;

	WA.Count(M).Reset(0);
	for (int i = 1; i <= N(); i++) // ignore dummy activities
		WA[PL[i] - 1]++;
	if (M == 1 || M == N())
		indAD = 0;
	else {
		// average absolute deviation
		double av = (double)N() / M; // average number of activities per level
		double avgDev = 0;
		for (auto w : WA)
			avgDev += abs(w - av);

		indAD = (double)M * avgDev / (2 * (N() - M) * (M - 1));
	}
	// calculation of total number of short precedence relations D
	double D = 0;
	for (int k = 0; k < M - 1; k++)
		D += WA[k] * WA[k + 1];
	// maximal size precedence relation size V
	int V = 0;
	for (int i = 1; i <= N(); i++)
		for (auto& suc : act[i].suc)
			if (suc <= N() && V < PL[suc] - PL[i])
				V = PL[suc] - PL[i];
	n.Count(V).Reset(0);
	for (int i = 1; i <= N(); i++)
		for (auto& suc : act[i].suc)
			if (suc <= N())
				n[PL[suc] - PL[i]]++;
	if (D == 0 || D == N() - WA[0])
		indLA = 1;
	else
		indLA = (double)(n[0] - (N() - WA[0])) / (D - (N() - WA[0]));

	// total direct precedence relations
	uint64_t TDP = 0;
	for (auto& precs : n)
		TDP += precs;
	indCNC = (double)TDP / N();

	if (TDP == 0 || TDP == N() - WA[0])
		indI5 = 1;
	else {
		double count = n[0];
		for (int k = 1; k < V; k++) // O(N)
			count += 1.0 * n[k] * (k + 1 - (M - 1)) / (1 - (M - 1));

		indI5 = (double)(count - (N() - WA[0])) / (TDP - (N() - WA[0]));
	}

	if (N() == M || M == 1 || N() == 0)
		indTF = 0;
	else {
		int count = 0;
		for (int i = 1; i <= N(); i++)
			count += RL[i] - PL[i];
		indTF = (double)count / ((N() - M) * (M - 1));
	}

	// OS --- need direct and indirect precedence relations
	int OSCount = 0;
	for (int i = 1; i <= N(); i++)
		for (int j = 1; j < i; j++)
			if (predBits[i].GetBit(j)) // if j is a predecessor of i
				OSCount++;
	indOS = (double)(OSCount * 2) / (N() * (N() - 1)); // 2*OS/(N*(N-1))
	mophologicOK = true;
}

// project indicators:
double CRCPSP::RU() {
	int totalUse = 0;
	if (R() == 0)
		return 0;
	for (auto& activity : act)
		for (auto& res : activity.res)
			if (res > 0)
				totalUse++;
	return (double)totalUse / N();
}

double CRCPSP::RF() {
	if (R() == 0)
		return 0;
	return (double)RU() / R();
}

double CRCPSP::RC() { // average of utilization, when a resource is used
	double avgUse = 0;
	int totalUse = 0;
	for (auto& activity : act)
		for (int k = 0; k < R(); k++)
			if (activity.res[k] > 0) {
				avgUse += (capacity[k] > 0 ? activity.res[k] / capacity[k] : 0);
				totalUse++;
			}
	if (totalUse == 0)
		return 0;
	return avgUse / totalUse;
}

double CRCPSP::RS() {
	return 0;
}
double CRCPSP::W(bool prec) { // WPREC, WALL
	return 0;
}
double CRCPSP::FS(int x, int u) { // FS21, FS22, FS31, FS32
	int64_t FS = 0, ref = 0;

	// x=2 all feasible pairs
	// x=3 all feasible triples
	// u=1 all possible pairs/triplets,
	// u=2 all possible pairs/triplets that are not precedence-related 


	return (ref > 0 ? (double)FS / ref : 0);
}

// generate a critical path from es/ls data
TVector<int> CRCPSP::CriticalPath(TVector<int>& es, TVector<int>& ls) {
	TVector<int> criticalPath;
	criticalPath += 0; // dummy start activity
	while (criticalPath.Last() != N(true) - 1) {
		int current = criticalPath.Last();
		for (auto& suc : act[current].suc)
			if (es[current] + act[current].pt == es[suc] && es[suc] == ls[suc]) {
				criticalPath += suc;
				break;
			}
	}
	return criticalPath;
}

void CRCPSP::MostraCP(TVector<int> criticalPath) {
	int t = 0, count = 0;
	for (auto& activity : criticalPath) {
		TProcura::Debug(DETALHE, true, count++ % 6 != 0 ? "%s" : "%s\n", *Act2Str(activity));
		TProcura::Debug(COMPLETO, false, "\n%2d%s%s", t, Icon(EIcon::TEMPO), *Act2Str(activity, 0, 3));
		t += act[activity].pt;
	}
}

int CRCPSP::LBCPM() {
	TVector<int> es, ls;
	CPM(es, Parametro(NIVEL_DEBUG) >= DETALHE ? &ls : NULL);

	if (Parametro(NIVEL_DEBUG) >= DETALHE) {
		printf("\nCritical path: ");
		MostraCP(CriticalPath(es, ls));
	}
	return es.Last();
}

int CRCPSP::LBC() {
	int bound = 0;
	TProcura::Debug(DETALHE, false, "\nLBcapacity: ");
	for (int k = 0; k < R(); k++)
		if (capacity[k] > 0) {
			int workContent = 0;
			for (auto& activity : act)
				workContent += activity.res[k] * activity.pt;
			if (workContent > 0 && capacity[k] > 0 && bound < (workContent - 1) / capacity[k] + 1)
				bound = (workContent - 1) / capacity[k] + 1;
			TProcura::Debug(DETALHE, false, "%s/%s=%d ",
				*Res2Str(workContent, k, true), *Res2Str(capacity[k], k, true), (workContent - 1) / capacity[k] + 1);
		}
	return bound;
}

int CRCPSP::LBCPC() {
	TVector<int> es, activities, wc;
	int bound = 0, t = 0;
	CPM(es);
	bound = es.Last();
	es.Sort(&activities);
	wc.Count(R()).Reset(0);
	TProcura::Debug(DETALHE, false, "\nLBcpc: ");
	for (auto& activity : activities) {
		if (es[activity] != t) {
			if (Parametro(NIVEL_DEBUG) >= DETALHE) {
				TProcura::Debug(DETALHE, false, "%d%s ", t, Icon(EIcon::TEMPO));
				for (int k = 0; k < R(); k++)
					if (wc[k] > 0)
						TProcura::Debug(DETALHE, false, "%s/%s ",
							*Res2Str(wc[k], k, true),
							*Res2Str(capacity[k], k, true));
				TProcura::Debug(DETALHE, false, "\n");
			}
			// remove work content done 
			for (int k = 0; k < R(); k++) {
				wc[k] -= capacity[k] * (es[activity] - t);
				if (wc[k] < 0)
					wc[k] = 0;
			}
			t = es[activity];
		}
		TProcura::Debug(COMPLETO, false, "%s", *Act2Str(activity, 0, 3));
		// add work content of this activity
		for (int k = 0; k < R(); k++)
			wc[k] += act[activity].res[k] * act[activity].pt;
	}
	if (Parametro(NIVEL_DEBUG) >= DETALHE) {
		TProcura::Debug(DETALHE, false, "%d%s ", t, Icon(EIcon::TEMPO));
		for (int k = 0; k < R(); k++)
			if (wc[k] > 0)
				TProcura::Debug(DETALHE, false, "%s/%s=%d ",
					*Res2Str(wc[k], k, true),
					*Res2Str(capacity[k], k, true),
					t + (wc[k] - 1) / capacity[k] + 1);
		TProcura::Debug(DETALHE, false, "\n");
	}
	// calculate bound to the end of the schedule
	for (int k = 0; k < R(); k++)
		if (wc[k] > 0 && capacity[k] > 0 && bound < t + (wc[k] - 1) / capacity[k] + 1)
			bound = t + (wc[k] - 1) / capacity[k] + 1;
	return bound;
}

int CRCPSP::LBCS() {
	int bound = 0, ePT = 0;
	TVector<int> es, ls, criticalPath, pt, idPT;
	TVector<TVector<int>> resUsage;
	TBits inCP;
	CPM(es, &ls);
	bound = es.Last();
	resUsage.Count(R());
	for (auto& usage : resUsage)
		usage.Count(bound).Reset(0);

	TProcura::Debug(DETALHE, false, "\nLBcs: ");

	// build a critical path
	// note: other CP provide different LBs
	criticalPath += CriticalPath(es, ls);
	for (auto& critic : criticalPath) {
		inCP.SetBit(critic, true);
		for (int t = es[critic]; t < es[critic] + act[critic].pt; t++)
			for (int k = 0; k < R(); k++)
				resUsage[k][t] += act[critic].res[k];
	}

	if (Parametro(NIVEL_DEBUG) >= DETALHE) {
		printf("\nCritical path: ");
		MostraCP(criticalPath);
	}

	// process activities by decreasing PT, to maximiaze cuts
	for (int i = 0; i < N(true); i++)
		pt += act[i].pt;
	pt.Sort(&idPT);

	// check for other activities, if they cannot be scheduled in the same time as critical activities
	ePT = 0; // maximal increment found
	for (auto& i : idPT.Invert())
		// if activity is not in this critical path, and large enough to increase the LB
		if (act[i].pt <= ePT)
			break; // can stop since PT is in decreasing order
		else if (!inCP.GetBit(i)) {
			int maxZ = 0, z = 0;
			// go throw all time window, and find the maximal slot compatible (maxZ)
			// ePT can be set to pt-maxS
			TProcura::Debug(DETALHE, false, "\n%s[%d;%d] ",
				*Act2Str(i, 0, Parametro(NIVEL_DEBUG) >= COMPLETO ? 3 : 0),
				es[i], ls[i] + act[i].pt - 1);

			for (int t = es[i]; t < ls[i] + act[i].pt; t++) {
				// check if is possible in instant t
				bool possible = true;
				for (int k = 0; possible && k < R(); k++)
					possible = (resUsage[k][t] + act[i].res[k] <= capacity[k]);
				if (possible) {
					TProcura::Debug(COMPLETO, false, "%d%s", t, Icon(EIcon::SEL));
					if (++z > maxZ)
						maxZ = z;
					if (z == act[i].pt || ePT >= act[i].pt - maxZ)
						break; // no more gain is possible
				}
				else { // violation
					z = 0;
				}
			}
			if (ePT < act[i].pt - maxZ) {
				ePT = act[i].pt - maxZ;
				TProcura::Debug(DETALHE, false, " %s%d ", Icon(EIcon::LB), bound + ePT);
			}
		}

	bound += ePT;

	return bound;
}

// check if activities are incompatible (pairs, triplets, and so on)
bool CRCPSP::ResourceIncompatible(TVector<int> activities) {
	TVector<int> resUsage;
	resUsage.Count(R()).Reset(0);
	for (auto& activity : activities)
		for (int k = 0; k < R(); k++)
			resUsage[k] += act[activity].res[k];
	return ResourcesViolated(resUsage);
}


int CRCPSP::LBIP() {
	TVector<int> es, ls;
	int bound, cpm;
	CPM(es, &ls);
	bound = cpm = es.Last();

	TProcura::Debug(DETALHE, false, "\nLBip: ");

	// process all precedence OK pairs i>j
	for (int i = 1; i <= N(); i++)
		for (int j = i + 1; j <= N(); j++)
			if (!predBits[j].GetBit(i)) {
				bool resOK = true;
				for (int k = 0; resOK && k < R(); k++)
					resOK = (act[i].res[k] + act[j].res[k] <= capacity[k]);
				if (!resOK) { // incompatible pair i||j
					int lowestBound = BoundIP(i, j, es, ls);
					if (bound < lowestBound) {
						bound = lowestBound;
						TProcura::Debug(DETALHE, false, " %s%d ", Icon(EIcon::LB), bound);
					}
				}
			}

	return bound;
}

int CRCPSP::BoundIP(int i, int j, TVector<int>& es, TVector<int>& ls) {
	int cpm = es.Last();
	int bound_ij = es[i] + act[i].pt + (cpm - ls[j]);
	int bound_ji = es[j] + act[j].pt + (cpm - ls[i]);
	int bound = (bound_ij < bound_ji ? bound_ij : bound_ji);

	TProcura::Debug(COMPLETO, false, "\n%s[%d;%d] ",
		*Act2Str(i, 0, Parametro(NIVEL_DEBUG) >= COMPLETO ? 3 : 0),
		es[i], ls[i] + act[i].pt - 1);
	TProcura::Debug(COMPLETO, false, "%s[%d;%d] ",
		*Act2Str(j, 0, Parametro(NIVEL_DEBUG) >= COMPLETO ? 3 : 0),
		es[j], ls[j] + act[j].pt - 1);
	TProcura::Debug(DETALHE, false, "[%s> %s %d | %s> %s %d] ",
		*Act2Str(i), *Act2Str(j), bound_ij,
		*Act2Str(j), *Act2Str(i), bound_ji);

	return bound;
}

int CRCPSP::BoundIT(int i, int j, int l, TVector<int>& es, TVector<int>& ls, int bound) {
	int cpm = es.Last();
	int localBound = 0, boundPair;
	TVector<int> prec = { i,j,i,l,j,l };
	TVector<int> suc = { j,i,l,i,l,j };

	TProcura::Debug(COMPLETO, false, "\n%s[%d;%d] ",
		*Act2Str(i, 0, Parametro(NIVEL_DEBUG) >= COMPLETO ? 3 : 0),
		es[i], ls[i] + act[i].pt - 1);
	TProcura::Debug(COMPLETO, false, "%s[%d;%d] ",
		*Act2Str(j, 0, Parametro(NIVEL_DEBUG) >= COMPLETO ? 3 : 0),
		es[j], ls[j] + act[j].pt - 1);
	TProcura::Debug(COMPLETO, false, "%s[%d;%d] ",
		*Act2Str(l, 0, Parametro(NIVEL_DEBUG) >= COMPLETO ? 3 : 0),
		es[l], ls[l] + act[l].pt - 1);


	for (int k = 0; k < 6; k++) {
		boundPair = es[prec[k]] + act[prec[k]].pt + (cpm - ls[suc[k]]);

		TProcura::Debug(DETALHE, false, " %s> %s %d |",
			*Act2Str(prec[k]), *Act2Str(suc[k]), boundPair);

		if (k == 0 || localBound > boundPair)
			localBound = boundPair;
		if (localBound <= bound)
			return localBound;
	}
	return localBound;
}

int CRCPSP::LBIT() {
	TVector<int> es, ls, resUsage;
	int bound, cpm;
	CPM(es, &ls);
	bound = cpm = es.Last();
	resUsage.Count(R());

	TProcura::Debug(DETALHE, false, "\nLBit: ");

	// process all precedence OK pairs i>j
	for (int i = 1; i <= N(); i++)
		for (int j = i + 1; j <= N(); j++)
			if (!predBits[j].GetBit(i)) {
				bool resOK = true;
				for (int k = 0; resOK && k < R(); k++)
					resOK = ((resUsage[k] = act[i].res[k] + act[j].res[k]) <= capacity[k]);
				if (!resOK) { // incompatible pair i||j
					int lowestBound = BoundIP(i, j, es, ls);
					if (bound < lowestBound) {
						bound = lowestBound;
						TProcura::Debug(DETALHE, false, " %s%d ", Icon(EIcon::LB), bound);
					}
				}
				else
					for (int l = j + 1; l <= N(); l++)
						if (!predBits[l].GetBit(j) && !predBits[l].GetBit(i)) {
							bool resOK = true;
							for (int k = 0; resOK && k < R(); k++)
								resOK = (resUsage[k] + act[l].res[k] <= capacity[k]);
							if (!resOK) { // incompatible triplet i||j||l
								int lowestBound = BoundIT(i, j, l, es, ls, bound);
								if (bound < lowestBound) {
									bound = lowestBound;
									TProcura::Debug(DETALHE, false, " %s%d ", Icon(EIcon::LB), bound);
								}
							}
						}
			}
	return bound;
}

// return the number of compatible activities to each activity
TVector<TVector<int>> CRCPSP::CompatibleActivities() {
	TVector<TVector<int>> compatible;
	compatible.Count(N(true));

	for (int i = 1; i <= N(); i++)
		for (int j = i + 1; j <= N(); j++)
			// precedence compatible && resource compatible
			if (!predBits[j].GetBit(i) && !ResourceIncompatible({ i,j })) {
				compatible[i] += j;
				compatible[j] += i;
			}

	return compatible;
}


// Node Packing: 
// select an activityh (most incompatible one), and schedule in serial
// remove all compatible activities with the selected activity from the list
// process until is empty
int CRCPSP::LBNodePacking(int n) {
	TVector<int> weight, activities; // process by increasing order of this weight vector, depending on mode n
	TVector<TVector<int>> compatible; // number of compatible activities (precedences and resources)
	TVector<int> es, ls, pt;
	int cpm, bound = 0, currentPT = 0, minHead, minTail;
	compatible = CompatibleActivities();
	CPM(es, &ls);
	minHead = minTail = bound = cpm = es.Last();

	TProcura::Debug(DETALHE, false, "\nLBnp(%d): ", n);

	for (int i = 0; i < N(true); i++)
		switch (n) {
		case 0:
			// first the critical activities, 
			// then the number of compatible (lowest first) 
			// then PT, longer first
			weight += (es[i] == ls[i] ? 0 : 10000) + compatible[i].Count() * 100 - act[i].pt;
			break;
		case 1:
			// first the number of compatible(lowest first)
			// then PT, longer first
			weight += compatible[i].Count() * 100 - act[i].pt;
			break;
		case 2:
			// start first by activities with the center of slack in the middle (head*tail)
			// second criteria is the number of compatible activities (lowest first)
			// third criteria processing time, longer first
			weight += -100 * es[i] * (cpm - ls[i] - act[i].pt) + compatible[i].Count() * 100 - act[i].pt;
			break;
		default:
			weight += TRand::rand() % 10000; // random order
		}
	weight.Sort(&activities);
	activities.Invert(); // process the top first

	// local pt (to reduce pt instead of removing an activity)
	for (int i = 0; i < N(true); i++)
		pt += act[i].pt;

	while (!activities.Empty()) {
		int i = activities.Pop();

		if (pt[i] == 0) // do not process dummy activities
			continue;

		currentPT += pt[i]; // activity done in serial

		TProcura::Debug(DETALHE, false, "\n%s: (%d) ",
			*Act2Str(i, 0, (Parametro(NIVEL_DEBUG) >= COMPLETO ? 3 : 1)), currentPT);

		for (auto& j : compatible[i]) {
			int index = activities.Find(j);
			if (index >= 0) {
				if (pt[j] > pt[i])
					pt[j] -= pt[i];
				else
					activities[index] = -1;
				TProcura::Debug(COMPLETO, false, "%s%s", *Act2Str(j), Icon(EIcon::NSEL));
			}
		}
		activities.Remove(-1);
		// update head/tails
		if (minHead > es[i]) {
			minHead = es[i];
			TProcura::Debug(COMPLETO, false, " head %d ", minHead);
		}
		if (minTail > cpm - ls[i] - act[i].pt) {
			minTail = cpm - ls[i] - act[i].pt;
			TProcura::Debug(COMPLETO, false, " tail %d ", minTail);
		}

		// base bound
		if (bound < minHead + currentPT + minTail) {
			bound = minHead + currentPT + minTail;
			TProcura::Debug(DETALHE, false, "%s%d ", Icon(EIcon::LB), bound);
		}
	}

	return bound;
}

// parallel machine bound
// mode:
// 0 - normal run, all pairs (u_min,machines), with the less number of machines
// 1 - use only u_min=1, with the minimal number of machines
// 2 - use u_min={1,2}, machines={1,2}, four configurations of (u_min,machines)
// 3 - use work content instead of machine usage
// - use es and the LS of the instance
int CRCPSP::LBParallelMachine(int m) {
	TVector<int> es, ls, pt, heads, tails, tHeads, tTails;
	//TVector<TVector<TVector<int>>> actMatrix;
	//TVector<TVector<int>> sumMatrix;
	int cpm, bound;
	CPM(es, &ls);
	cpm = bound = es.Last();
	heads = es;
	tails = ls;
	for (int i = 0; i < N(true); i++)
		tails[i] = cpm - ls[i] - act[i].pt;

	TProcura::Debug(DETALHE, false, "\nLBpm(%d): ", m);

	// process different instants of heads/tails
	(tHeads = heads).BeASet();
	(tTails = tails).BeASet();

	/*
	// setup actMatrix[idxHead][idxTail][idAct] --- activities with idxHead and idxTail
	actMatrix.Count(tHeads.Count());
	sumMatrix.Count(tHeads.Count());
	for (int i = 0; i < tHeads.Count(); i++) {
		actMatrix[i].Count(tTails.Count());
		sumMatrix[i].Count(tTails.Count()).Reset(0);
	}

	for (int i = 1; i <= N(); i++)
		actMatrix[tHeads.Find(heads[i], true)][tTails.Find(tails[i], true)] += i;
	*/

	// process each resource
	for (int k = 0; k < R(); k++) {
		int machines = capacity[k];
		int stage = 0, u_min;
		TProcura::Debug(DETALHE, false, "\n%s:", *Res2Str(capacity[k], k));

		do {
			stage++;
			u_min = capacity[k] / (machines + 1) + 1;

			TProcura::Debug(COMPLETO, false, " %d/%d ", u_min, machines);

			for (auto& tHead : tHeads)
				for (auto& tTail : tTails) {
					// for all activities with head>=tHead and tail>=tTail, sum pt 
					int sum = 0;
					for (int i = 1; i <= N(); i++)
						if (heads[i] >= tHead && tails[i] >= tTail &&
							act[i].pt > 0 && act[i].res[k] >= u_min)
						{
							// considering work content, assuming a machine require u_min resources
							sum += (act[i].res[k] / u_min) * act[i].pt;
						}
					if (sum > 0 && bound < (sum = (sum - 1) / machines + 1 + tHead + tTail)) {
						bound = sum;
						TProcura::Debug(DETALHE, false, " head:%d tail:%d %s%d ", tHead, tTail, Icon(EIcon::LB), bound);
					}
					else if (sum > 0)
						TProcura::Debug(COMPLETO, false, "%d ", sum);

				}
			u_min++;

			if (m == 1 || u_min >= capacity[k])
				break;
			if (m == 2)
				switch (stage) {
				case 1: u_min = 2; break;
				case 2: u_min = capacity[k] / 3 + 1; break;
				case 3: u_min = capacity[k] / 2 + 1; break;
				}

			// increment the number of machines, until u_min
			machines = 1;
			while (u_min < capacity[k] / (machines + 1) + 1)
				machines++;

			if (u_min > capacity[k] / (machines + 1) + 1)
				machines--;

		} while (u_min <= capacity[k] && machines > 0);
	}
	return bound;
}

int CRCPSP::LBPR() {
	TVector<int> es, ls, headPT, tailPT;
	int bound, cpm, tw = 0;
	CPM(es, &ls);
	bound = cpm = es.Last();

	TProcura::Debug(DETALHE, false, "\nLBpr: ");

	// headPT: head+PT, tailPT; tail+PT
	headPT = es;
	tailPT = ls;
	for (int i = 0; i < N(true); i++) {
		headPT[i] += act[i].pt;
		tailPT[i] = cpm - tailPT[i];
	}

	// process all precedence OK pairs i>j
	for (int i = 1; i <= N(); i++) {
		TProcura::Debug(COMPLETO, false, "\n%s ", *Act2Str(i));
		for (int j = i + 1; j <= N(); j++)
			if (!predBits[j].GetBit(i)) {
				bool resOK = true;
				for (int k = 0; resOK && k < R(); k++)
					resOK = (act[i].res[k] + act[j].res[k] <= capacity[k]);
				if (!resOK) { // incompatible pair i||j
					int bound_ij = headPT[i] + tailPT[j];
					int bound_ji = headPT[j] + tailPT[i];
					int pairBound = (bound_ij < bound_ji ? bound_ij : bound_ji);
					TProcura::Debug(COMPLETO, false, "%s %d,%d ", *Act2Str(j), bound_ij, bound_ji);
					if (bound < pairBound) {
						if (tw == 0)
							bound = pairBound;
						else
							bound++;
						TProcura::Debug(DETALHE, false, " %s%s%s%d ", *Act2Str(i), *Act2Str(j), Icon(EIcon::LB), bound);
						tw = -1; // recalculate all
						i = 0; // restart
						break;
					}
					if (bound < bound_ij && bound >= bound_ji) {
						bool updated = false;
						// fix ji, the only compatible option with current bound
						if (headPT[j] + act[i].pt > headPT[i]) {
							headPT[i] = headPT[j] + act[i].pt;
							updated = true;
						}
						if (tailPT[i] + act[j].pt > tailPT[j]) {
							tailPT[j] = tailPT[i] + act[j].pt;
							updated = true;
						}
						if (updated) {
							PropagateTW(headPT, tailPT, j, i);
							TProcura::Debug(COMPLETO, false, " (%s>%s) ", *Act2Str(j), *Act2Str(i));
							tw = 1;
							i = 0;
							break;
						}
					}
					if (bound < bound_ji && bound >= bound_ij) {
						bool updated = false;
						// fix ij
						if (headPT[i] + act[j].pt > headPT[j]) {
							headPT[j] = headPT[i] + act[j].pt;
							updated = true;
						}
						if (tailPT[j] + act[i].pt > tailPT[i]) {
							tailPT[i] = tailPT[j] + act[i].pt;
							updated = true;
						}
						if (updated) {
							PropagateTW(headPT, tailPT, i, j);
							TProcura::Debug(COMPLETO, false, " (%s>%s) ", *Act2Str(i), *Act2Str(j));
							tw = 1;
							i = 0;
							break;
						}
					}
				}
			}
		if (i == 0 && tw == -1) {
			// bound changed, previous time windows invalid
			TProcura::Debug(DETALHE, false, " restart %s%d. ", Icon(EIcon::LB), bound);
			tw = 0;
			headPT = es;
			tailPT = ls;
			for (int i = 0; i < N(true); i++) {
				headPT[i] += act[i].pt;
				tailPT[i] = cpm - tailPT[i];
			}
		}
	}
	return bound;
}

void CRCPSP::PropagateTW(TVector<int>& headPT, TVector<int>& tailPT, int i, int j) {
	TVector<int> updateB = { i };
	TVector<int> updateF = { j };
	// tailPT[i] updated backward
	while (!updateB.Empty()) {
		int i = updateB.Pop();
		for (auto& pred : act[i].pred)
			if (tailPT[pred] < tailPT[i] + act[pred].pt) {
				tailPT[pred] = tailPT[i] + act[pred].pt;
				updateB += pred;
			}
	}
	// headPT[j] updated backward
	while (!updateF.Empty()) {
		int i = updateF.Pop();
		for (auto& suc : act[i].suc)
			if (headPT[suc] < headPT[i] + act[suc].pt) {
				headPT[suc] = headPT[i] + act[suc].pt;
				updateF += suc;
			}
	}
}

bool CRCPSP::PropagateEFLS(TVector<int>& ef, TVector<int>& ls, int i, int bound) {
	TVector<int> updateB = { i };
	TVector<int> updateF = { i };
	// ls[i] updated backward
	while (!updateB.Empty()) {
		int i = updateB.Pop();
		for (auto& pred : act[i].pred)
			if (ls[pred] > ls[i] - act[pred].pt) {
				ls[pred] = ls[i] - act[pred].pt;
				if (ls[pred] < 0)
					return true;
				updateB += pred;
			}
	}
	// ef[i] updated forward
	while (!updateF.Empty()) {
		int i = updateF.Pop();
		for (auto& suc : act[i].suc)
			if (ef[suc] < ef[i] + act[suc].pt) {
				ef[suc] = ef[i] + act[suc].pt;
				if (ef[suc] > bound)
					return true;
				updateF += suc;
			}
	}
	return false;
}

int CRCPSP::LBCT() {
	TVector<int> es, ls, ef;
	TVector<TVector<int>> resUsage;
	int bound, cpm, tw = 0;
	int realES, realLS, alternatives, ok;
	CPM(es, &ls);
	bound = cpm = es.Last();
	resUsage.Count(R());
	ef = es;
	for (int i = 0; i < N(true); i++)
		ef[i] += act[i].pt;

	// core times from ls to ef if intersect

	TProcura::Debug(DETALHE, false, "\nLBct: ");

	do {
		tw = 0;
		// reset on resource usage (in core times)
		for (int k = 0; k < R(); k++)
			resUsage[k].Count(bound).Reset(0);

		// fill the resource in core times
		for (int i = 1; i <= N() && tw == 0; i++)
			for (int t = ls[i]; t < ef[i]; t++) {
				TVector<int> res;
				if (t < 0) {
					bound++;
					TProcura::Debug(DETALHE, false, "\n %s%s%d ", *Act2Str(i), Icon(EIcon::LB), bound);
					tw = -1;
					break;
				}
				for (int k = 0; k < R(); k++)
					res += (resUsage[k][t] += act[i].res[k]);
				if (ResourcesViolated(res)) {
					bound++;
					TProcura::Debug(DETALHE, false, "\n %s%s%d ", *Act2Str(i), Icon(EIcon::LB), bound);
					tw = -1;
					break;
				}
			}

		if (Parametro(NIVEL_DEBUG) >= COMPLETO) {
			TVector<int> res, resAnt;
			for (int i = 1; i <= N(); i++)
				TProcura::Debug(COMPLETO, false, "\n%s [%d;%d]", *Act2Str(i, 0, 3), ls[i], ef[i] - 1);
			TProcura::Debug(COMPLETO, false, tw == 0 ? "\n Core times: " : " Core times: ");
			for (int t = 0; t < bound + tw; t++) {
				res = {};
				for (int k = 0; k < R(); k++)
					res += resUsage[k][t];
				if (!res.Equal(resAnt)) {
					TProcura::Debug(COMPLETO, false, "\n%2d%s", t, Icon(EIcon::TEMPO));
					for (int k = 0; k < R(); k++)
						TProcura::Debug(COMPLETO, false, "%s", *Res2Str(res[k], k));
				}
				resAnt = res;
			}
		}

		// try to find a slot for job i. If is the only one, add to the core times
		for (int i = 1; i <= N() && tw >= 0; i++) {
			realES = realLS = -1;
			alternatives = ok = 0;
			for (int t = ef[i] - act[i].pt; t <= ls[i]; t++) {
				ok = 1;
				for (int l = t; l >= 0 && l < t + act[i].pt; l++)
					if (l < ls[i] || l >= ef[i]) { // outside core time, check
						TVector<int> res;
						res = {};
						for (int k = 0; k < R(); k++)
							res += resUsage[k][l] + act[i].res[k];
						if (ResourcesViolated(res)) {
							t = l;
							l = -2;
							ok = 0;
						}
					}
				if (ok) {
					if (realES == -1)
						realES = t;
					alternatives++;
					realLS = t;
					break; // identify the realLS in inverse order
				}
			}
			for (int t = ls[i]; realLS >= 0 && t > realLS; t--) {
				ok = 1;
				for (int l = t + act[i].pt - 1; l >= 0 && l >= t; l--)
					if (l < ls[i] || l >= ef[i]) { // outside core time, check
						TVector<int> res;
						res = {};
						for (int k = 0; k < R(); k++)
							res += resUsage[k][l] + act[i].res[k];
						if (ResourcesViolated(res)) {
							t = l - act[i].pt + 1;
							l = -2;
							ok = 0;
						}
					}
				if (ok) {
					alternatives++;
					realLS = t;
					break; // is identified the realLS (the first in reverse order)
				}
			}
			if (realLS < 0) { // no valid spot found
				bound++;
				TProcura::Debug(DETALHE, false, " %s%s%d ", *Act2Str(i), Icon(EIcon::LB), bound);
				tw = -1;
				break;
			}
			else if (ef[i] - act[i].pt != realES || ls[i] != realLS) {
				ef[i] = realES + act[i].pt;
				ls[i] = realLS;
				if (PropagateEFLS(ef, ls, i, bound)) {
					bound++;
					tw = -1;
					break;
				}
				tw = 1;
			}
		}

		// reset time windows for the new bound
		if (tw < 0) {
			CPM(es, &ls, bound);
			ef = es;
			for (int i = 0; i < N(true); i++)
				ef[i] += act[i].pt;
			continue;
		}

	} while (tw != 0);

	return bound;
}

int CRCPSP::LBTP() {
	TVector<int> es, ls, ef, ts, tt, workContent;
	int bound, cpm, z;
	CPM(es, &ls);
	bound = cpm = es.Last();
	ef = es;
	for (int i = 0; i < N(true); i++)
		ef[i] += act[i].pt;

	(ts = ef).BeASet();
	(tt = ls).BeASet();

	do {
		z = 0;
		// go throw all intervals
		for (auto& t : ts) {
			for (auto& q : tt)
				if (t < q) {
					workContent.Count(R()).Reset(0);
					for (int i = 1; i <= N(); i++)
						if (ef[i] > t && ls[i] < q) {
							int di = act[i].pt;
							if (di > q - t)
								di = q - t;
							if (ef[i] - t < di)
								di = ef[i] - t;
							if (q - ls[i] < di)
								di = q - ls[i];
							// at least dj must be executed in this period
							for (int k = 0; k < R(); k++)
								workContent[k] += act[i].res[k] * di;
						}
					for (int k = 0; k < R(); k++)
						if (workContent[k] > 0 &&
							(workContent[k] - 1) / capacity[k] + 1 > q - t) {
							bound++;
							TProcura::Debug(DETALHE, false, " [%d;%d]%s%s%d ",
								t, q, Icon(EIcon::TEMPO), Icon(EIcon::LB), bound);
							// update tt for the new bound
							CPM(es, &ls, bound);
							(tt = ls).BeASet();
							z = 1;
							break;
						}
					if (z == 1)
						break;
				}
			if (z == 1)
				break;
		}
	} while (z > 0);

	return bound;
}

int CRCPSP::LBAll() {
	int best = LBCPM();
	for (int i = IND_LB_CPM + 1; i < IND_LB_ALL; i++) {
		int bound = Indicador(i);
		if (bound > best)
			best = bound;
	}
	return best;
}

TString CRCPSP::HSL(float h, float s, float l, bool fundo) {
	if (h < 0 || h > 360) // reset de cores
		return TString().printf("%s", COR_RESET);

	float f = (2 * l - 1);
	float c = (1 - (f < 0 ? -f : f)) * s;

	float h60 = h / 60.0f;
	float hmod2 = h60 - 2 * int(h60 / 2);
	float x = c * (1 - ((hmod2 - 1) < 0 ? -(hmod2 - 1) : (hmod2 - 1)));
	float m = l - c / 2;

	float r, g, b;
	if (h < 60) { r = c; g = x; b = 0; }
	else if (h < 120) { r = x; g = c; b = 0; }
	else if (h < 180) { r = 0; g = c; b = x; }
	else if (h < 240) { r = 0; g = x; b = c; }
	else if (h < 300) { r = x; g = 0; b = c; }
	else { r = c; g = 0; b = x; }

	return TString().printf("\x1b[%d;2;%d;%d;%dm", (fundo ? 48 : 38),
		(int)((r + m) * 255), (int)((g + m) * 255), (int)((b + m) * 255));
}

// set current solution to a priority rule
void CRCPSP::PriorityRule(int rule, bool serial) {
	TVector<int> ids, value, valueAux;
	int invI;
	ids.Count(N(true));
	value.Count(N(true));
	valueAux.Count(N(true));
	for (int i = 0; i < N(true); i++) {
		switch (rule) {
		case 1: // SPT
			value[i] = act[i].pt;
			break;
		case 2: // LPT
			value[i] = -act[i].pt;
			break;
		case 3: // MIS
			value[i] = -act[i].suc.Count();
		case 4: // MTS
			value[i] = 0;
			for (int j = i + 1; j <= N(); j++)
				if (act[j].predBits.GetBit(i))
					value[i]--;
		case 5: // LNJ
			value[i] = 0;
			for (int j = 1; j <= N(); j++)
				if (j != i && !act[j < i ? i : j].predBits.GetBit(j < i ? j : i))
					value[i]++;
			break;
		case 6: // GRPW
			value[i] = -act[i].pt;
			for (auto& suc : act[i].suc)
				value[i] -= act[suc].pt;
			break;
		case 7: // EST
			value[i] = 0;
			for (auto& pred : act[i].pred)
				if (value[pred] + act[pred].pt > value[i])
					value[i] = value[pred] + act[pred].pt;
			break;
		case 8: // EFT
			value[i] = act[i].pt;
			for (auto& pred : act[i].pred)
				if (value[pred] + act[i].pt > value[i])
					value[i] = value[pred] + act[i].pt;
		case 9: // LST
			invI = N(true) - i - 1;
			value[invI] = -act[invI].pt; // 0 is the finish of project
			for (auto& suc : act[invI].suc)
				if (value[suc] - act[invI].pt < value[invI])
					value[invI] = value[suc] - act[invI].pt;
			break;
		case 10: // LFT
			invI = N(true) - i - 1;
			value[invI] = 0;
			for (auto& suc : act[invI].suc)
				if (value[suc] - act[suc].pt < value[invI])
					value[invI] = value[suc] - act[suc].pt;
			break;
		case 11: // MSLK
			value[i] = 0; // EST
			for (auto& pred : act[i].pred)
				if (value[pred] + act[pred].pt > value[i])
					value[i] = value[pred] + act[pred].pt;
			invI = N(true) - i - 1;
			valueAux[invI] = 0; // LST
			for (auto& suc : act[invI].suc)
				if (valueAux[suc] - act[suc].pt < valueAux[invI])
					valueAux[invI] = valueAux[suc] - act[suc].pt;
			break;
		case 12: // GRWC
			value[i] = 0;
			for (int k = 0; k < R(); k++)
				value[i] -= act[i].res[k] * act[i].pt;
			break;
		case 13: // GCRWC
			value[i] = 0;
			for (int k = 0; k < R(); k++)
				value[i] -= act[i].res[k] * act[i].pt;
			for (auto& suc : act[i].suc)
				for (int k = 0; k < R(); k++)
					value[i] -= act[suc].res[k] * act[suc].pt;
			break;

		default: // ID
			value[i] = i;
			break;
		}
	}
	if (rule == 11) // MSLK, need to combine EST and LST (LST-EST)
		for (int i = 0; i < N(true); i++)
			value[i] = valueAux[i] - value[i];

	value.Sort(&ids);
	if (rule < 7 || rule > 10) // for EST, EFT, LST and LFT, the order is already correct, no need to sort again
		TopologicalSort(ids);

	if (serial)
		Serial(ids);
	else
		Parallel(ids);
}

void CRCPSP::BestPR() {
	int bestRule = 0;
	int bestValue = -1;
	int bestMode = 0;
	for (int rule = 0; rule <= 13; rule++)
		for (int mode = 0; mode < 2; mode++) {
			PriorityRule(rule, mode == 0);
			if (bestValue < 0 || st.Last() < bestValue) {
				bestValue = st.Last();
				bestRule = rule;
				bestMode = mode;
			}
		}
	PriorityRule(bestRule, bestMode == 0);
}
