#include "CRCPSP.h"
#include <stdio.h>
#include <algorithm>
#include <random>
#include <iterator>

TVector<TVector<TSubNets>> CRCPSP::subNets;

// Indicadores RCPSP
int CRCPSP::IND_UB;
int CRCPSP::IND_LB; // final LB
int CRCPSP::IND_CALLS;
int CRCPSP::IND_BASE_LB; // Base LBs - set initial LB
int CRCPSP::IND_PR; // PRs - set initial UB
int CRCPSP::IND_N;
// project indicators
int CRCPSP::IND_CNC;
int CRCPSP::IND_OS;
int CRCPSP::IND_SP;
int CRCPSP::IND_AD;
int CRCPSP::IND_LA;
int CRCPSP::IND_I5;
int CRCPSP::IND_TF;
int CRCPSP::IND_WPREC;
int CRCPSP::IND_WALL;
int CRCPSP::IND_FS21;
int CRCPSP::IND_FS22;
int CRCPSP::IND_FS31;
int CRCPSP::IND_FS32;
int CRCPSP::IND_RF;
int CRCPSP::IND_RU;
int CRCPSP::IND_RS;
int CRCPSP::IND_RC;

// Parâmetros RCPSP
int CRCPSP::RES_METHOD;
int CRCPSP::MAX_VARS;
int CRCPSP::MAX_CLAUSES;
int CRCPSP::HORIZON; // time horizon to use in conversion to add to current LB 
int CRCPSP::BASE_LB;
int CRCPSP::LB_MODE;
int CRCPSP::BASE_PR;
int CRCPSP::PR_MODE;
int CRCPSP::HEADS_TAILS; // mode to calculate heads and tails
int CRCPSP::IND_MODE;
int CRCPSP::LOAD_FORMAT; // rcp or any sub-RCPSP problem implemented
int CRCPSP::SUB_INST_ID; // instance ID in case a file have more than one instance
int CRCPSP::CLEAN_RES;
int CRCPSP::RES_WASTE; // mode to optimize resources, adding resource wasted to activities


void CRCPSP::ResetParametros()
{
	CCaDiCaL::ResetParametros();

	CCaDiCaL::solver = "./../cadical/build/cadical";

	if (ficheiroInstancia.Empty())
		ficheiroInstancia = "Instancias/DC1/mv";

	int id = parametro.Count();
#define INICIALIZA(X) X = id++;
	INICIALIZA(RES_METHOD);
	INICIALIZA(RES_METHOD);
	INICIALIZA(MAX_VARS);
	INICIALIZA(MAX_CLAUSES);
	INICIALIZA(HORIZON); 
	INICIALIZA(BASE_LB);
	INICIALIZA(LB_MODE);
	INICIALIZA(BASE_PR);
	INICIALIZA(PR_MODE);
	INICIALIZA(HEADS_TAILS); 
	INICIALIZA(IND_MODE);
	INICIALIZA(LOAD_FORMAT); 
	INICIALIZA(SUB_INST_ID); 
	INICIALIZA(CLEAN_RES);
	INICIALIZA(RES_WASTE); 
	id = indicador.Count();
	INICIALIZA(IND_UB);
	INICIALIZA(IND_LB); 
	INICIALIZA(IND_CALLS);
	INICIALIZA(IND_BASE_LB); 
	INICIALIZA(IND_PR); 
	INICIALIZA(IND_N);
	INICIALIZA(IND_CNC);
	INICIALIZA(IND_OS);
	INICIALIZA(IND_SP);
	INICIALIZA(IND_AD);
	INICIALIZA(IND_LA);
	INICIALIZA(IND_I5);
	INICIALIZA(IND_TF);
	INICIALIZA(IND_WPREC);
	INICIALIZA(IND_WALL);
	INICIALIZA(IND_FS21);
	INICIALIZA(IND_FS22);
	INICIALIZA(IND_FS31);
	INICIALIZA(IND_FS32);
	INICIALIZA(IND_RF);
	INICIALIZA(IND_RU);
	INICIALIZA(IND_RS);
	INICIALIZA(IND_RC);
#undef DEFINE_PARAM


	// adicionar o novo parâmetro para a conversão de N Damas para SAT
	parametro += {
		"RES_METHOD", 0, 0, 3, "Converting resources optinos",
		{
			"COV",
			"MFS",
			"BDD",
			"ADD"
		}
	};

	parametro += {
		"MAX_VARS", 1000000, 1000, 1000000000,
			"Limit to maximal number of variables in conversion to CNF"
	};

	parametro += {
		"MAX_CLAUSES", 10000000, 1000, 2000000000,
			"Limit to maximal number of clauses in conversion to CNF"
	};

	parametro += {
		"HORIZON", -3, -3, 1000, "Time horizon to use in conversion to add to current LB. If -1 runs LBS, -2 UBS, -3 DBS."
	};

	parametro += {
		"BASE_LB", 12, 1, 12, "Base LB: set the initial lower bound to use in conversion",
		{
			"CPM","C","CPC","CS","IP","IT","NP",
			"PM","PR","CT","TP","ALL"
		}
	};

	parametro += { "LB_MODE", 0, 0, 1000, "LB mode. In LBs in Node Packing and Parallel Machine. " };

	parametro += {
		"BASE_PR", 9, 0, 14, "Base PR - Priority rule: set the initial upper bound to use in conversion",
		{
			"ID", "SPT", "LPT","MIS","MTS","LNJ","GRPW",
			"EST","EFT","LST","LFT","MSLK","GRWC","GCRWC","ALL"
		}
	};

	parametro += { "PR_MODE", 0, 0, 1, "PR mode, currently scheduling generation scheme.",
		{ "SGS", "PGS" } };

	parametro += {
		"HEADS_TAILS", 2, 0, 2, "Calculate heads and tails",
		{
			"cpm", "resource capacity", "base LB"
		}
	};

	parametro += { "IND_MODE", 0, 0, 1, "IND mode: 0 - pure topological indicators; 1 - duration topological indicators." };

	parametro += {
		"LOAD_FORMAT", 0, 0, 1, "File format to load an RCPSP instance, or any sub-RCPSP",
		{
			"rcp",
			"Binpacking"
		}
	};

	parametro += { "SUB_INST_ID", 0, 0, 10000, "instance ID in case a file have more than one instance" };

	parametro += { "CLEAN_RES", 0, 0, 2, "Clean resources when loading a new instance" };

	parametro += {
		"RES_WASTE", 0, 0, 3, "Mode to optimize resources, adding resource wasted to activities",
		{
			"No resource waste",
			"Add resource waste (effort of 10 * pt subsets of compatible activities)",
			"Add resource waste (effort of 100 * pt subsets of compatible activities)",
			"Add resource waste (effort of 1000 * pt subsets of compatible activities)",
		}
	};


	instancia = { "Instance", 1,1,1000000, "Instance loaded in rcp format" };

	// project indicators (not active in beginning, since are too many)
	indicador += {
		{ "UB", "best upper bound found", IND_UB },
		{ "LB", "best lower bound found", IND_LB },
		{ "CALLS", "number of SAT calls", IND_CALLS },
		{ "BASE_LB", "Base lower bound - initial LB", IND_BASE_LB },
		{ "PR", "Priority rule - initial UB", IND_PR },
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

	};

	indAtivo += { IND_UB, IND_LB, IND_CALLS, IND_BASE_LB, IND_PR };

	// para poder correr em MPI e gerar a mesma instância com diferentes parâmetros
	// é preciso que o prefixo da instância seja único
	//ficheiroInstancia += TString(mpiID);

}


// load instance from file, and initialize variables
void CRCPSP::Inicializar(void) {
	CCaDiCaL::Inicializar();

	// clean datastructure
	act = {};
	capacity = {};
	mophologicOK = false;
	bestLB = bestUB = 0;
	calls = 0;
	insertedPred = {};

	switch (Parametro(LOAD_FORMAT)) {
	case 0: // rcp
		LoadRCPSP(TString().printf("%s%d.rcp", *ficheiroInstancia, instancia.valor).readLines());
		break;
	case 1: // Binpacking
		LoadBinpacking(
			TString().printf("%s%d.txt", *ficheiroInstancia, instancia.valor).readLines(),
			Parametro(SUB_INST_ID));
		break;
	}

	ResetSubNets();
	InitialSetup();
}

void CRCPSP::LoadBinpacking(TVector<TString> lines, int instanceID) {
	int i = 0, itens; //, bks;
	int instances = atoi(lines[i++]);
	TVector<int> data;

	printf("\nload binpack"); fflush(stdout);
	{
		TString name = lines[i++]; // identifyer
		data = {};
		for (auto& token : lines[i++].tok()) // capacity, number of itens, bks
			data += atoi(token);
		(capacity = {}) += data[0]; // first is capacity of the single resource
		itens = data[1];
		act.Count(itens + 2);
		//bks = data[2];
		act[0].id = 0;
		act[0].pt = 0;
		act[0].res.Count(1);
		act[0].res.Reset(0);
		for (int j = 0; j < itens; j++) {
			act[0].suc += (j + 1);
			act[j + 1].id = j + 1;
			act[j + 1].pt = 1; // processing time
			act[j + 1].res.Count(1); // resource usage
			act[j + 1].res[0] = atoi(lines[i++]);
			act[j + 1].pred += 0;
			act[j + 1].suc += (itens + 1);
			act[itens + 1].pred += (j + 1);
		}
		act[itens + 1].id = itens + 1;
		act[itens + 1].pt = 0;
		act[itens + 1].res.Count(1);
		act[itens + 1].res.Reset(0);
	}
	printf("\nfinish load binpack"); fflush(stdout);
}


void CRCPSP::LoadRCPSP(TVector<TString> lines) {
	TVector<int> nums;
	for (auto& line : lines)
		for (auto& num : line.tok())
			nums += atoi(num);
	nums.Invert(); // invert to extract from the top
	TVector<int>::erro = 0; // if file ends, the rest of the data is set to 0

	// process numnbers:
	// N, R, <capacity> (all resources), <pt res1 res2 ... #sucs suc1 suc2 ...>  (for all activities)
	act.Count(nums.Pop()); // N
	capacity.Count(nums.Pop()); // R

	// load capacity
	for (auto& available : capacity)
		available = nums.Pop();
	// reset of precedence relations, resources, and activity ID
	int i = 0;
	for (auto& activity : act) {
		activity.id = i++; // original activity id
		activity.res.Count(R()); // resource usage
		activity.pred = {};
		activity.suc = {};
	}
	// load activities
	for (auto& activity : act) {
		activity.pt = nums.Pop(); // processing time
		for (int k = 0; k< R(); k++) {
			activity.res[k] = nums.Pop(); // resource usage
			if (activity.res[k] < 0 || activity.res[k]>capacity[k]) 
				printf("\nLoadRCPSP: error, resource usage %d over capacity %d in activity %d.", 
					activity.res[k], capacity[k], activity.id);
		}
		int numSucs = nums.Pop(); // number of successors
		for (int s = 0; s < numSucs; s++) {
			activity.suc += nums.Pop() - 1; // successors (0-indexed)
			if (activity.suc.Last() < 0 || activity.suc.Last() >= N(true))
				printf("\nLoadRCPSP: error, sucessor %d does not exist.", activity.suc.Pop() + 1);
		}
	}

	// setup pred based on suc
	for (auto& activity : act)
		for (auto& suc : activity.suc)
			act[act[suc].id].pred += activity.id;

	// correct missing dummy links from dummy start
	for (auto& activity : act)
		if (activity.id > 0 && activity.pred.Empty()) {
			activity.pred.Add(0);
			act.First().suc += activity.id;
		}
	// correct missing dummy links from dummy end
	for (auto& activity : act)
		if (activity.id <= N() && activity.suc.Empty()) {
			activity.suc.Add(N() + 1);
			act.Last().pred += activity.id;
		}

	// Dummy start/end have 0 PT and resources
	act.Last().pt = act.First().pt = 0;
	act.First().res.Reset(0);
	act.Last().res.Reset(0);
}


// clone the project with just the selected activties
CRCPSP* CRCPSP::Clone(TBits activities) {
	CRCPSP* p = new CRCPSP();

	activities.SetBit(0, true).SetBit(N(true) - 1, true);

	// clean datastructure
	p->act = {};
	p->capacity = capacity;
	p->mophologicOK = false;
	p->bestLB = p->bestUB = 0;
	p->calls = 0;
	p->insertedPred = {};
	p->parametro = parametro;

	// copy only activities set to true
	//printf("%s", *activities.String(2));
	p->act += act.First();
	for (int i = 1; i <= N(); i++)
		if (activities.GetBit(i))
			p->act += act[i];
	p->act += act.Last();
	// set sucessors/predecessors as indirect that are selected
	for (int i = 0; i < p->N(true); i++) {
		TBits sucs = p->act[i].sucBits & activities;
		TBits preds = p->act[i].predBits & activities;
		p->act[i].suc = {};
		p->act[i].pred = {};
		for (int j = 0; j < N(true); j++) {
			if (sucs.GetBit(j))
				// add sucessor
				p->act[i].suc += act[j].id;
			if (preds.GetBit(j))
				// add predecessor
				p->act[i].pred += act[j].id;
		}
	}
	p->InitialSetup();

	// make an hashtable on unique, saving bestLB and bestUB, 
	// to guaranty one calculation for each network

	// only 2N networks are needed, calculate them by order of size, and reuse heads/tails inside each subnetwork

	/*
	printf("\nSubset {");
	for(auto item : unique)
		printf("%d ", item);
	printf("} LB%d UB%d",p->bestLB,p->bestUB);
	p->Debug(true);*/
	return p;
}

void CRCPSP::UpdateIndex() {
	// check first if all ID are from 0 to N(true)-1
	index.Count(act.Last().id + 1).Reset(-1);
	for (int i = 0; i < N(true); i++)
		index[act[i].id] = i;

	if (index.Count() == act.Count()) { // no need an extra index
		for (int i = 0; i < N(true); i++)
			act[i].index = index[i];
		index = {};
	}
}

// change the order of activities to PL and by ID, to get all predecessors index before sucessor index
void CRCPSP::ActivityOrder() {
	TVector<int> weight, id;
	TVector<TActivity> newOrder;
	int j = 0, maxID = act.Last().id+1;
	if (act.Count() < 2)
		return;
	UpdateIndex();
	//  initiate pLevel
	for (auto& activity : act)
		activity.pLevel = -1;
	id += 0;
	// calculate pLevel
	while (j < id.Count()) {
		int i = id[j++];
		act[i].pLevel = 0;
		for (auto& pred : act[i].pred)
			if (act[i].pLevel <= act[Index(pred)].pLevel)
				act[i].pLevel = act[Index(pred)].pLevel + 1;
		// add sucessors that have all predecessors calculated
		for (auto& suc : act[i].suc) {
			bool allDone = true;
			for (auto& pred : act[Index(suc, N(true) - 1)].pred)
				if (act[Index(pred)].pLevel < 0) {
					allDone = false;
					break;
				}
			if (allDone)
				id += Index(suc, N(true) - 1);
		}
	}
	for (auto& activity : act) {
		if (activity.pLevel < 0) {
			// big error, report always
			printf("\nError: Cycle detected in activity %d!", activity.id);
			for (auto& activity : act) {
				printf("\nid %d pLevel %d sucs: ",
					activity.id, activity.pLevel);
				for (auto suc : activity.suc)
					printf("%d ", suc);
				printf("pred: ");
					for (auto pred : activity.pred)
						printf("%d ", pred);
			}


			// setup parallel network
			for (int i = 0; i < N(true); i++) {
				act[Index(i, 0)].pred = act[Index(i)].suc = {};
				if (i < N(true) - 1)
					act[Index(i)].suc += i + 1;
				if (i > 0)
					act[Index(i)].pred += i - 1;
			}
			exit(1);
			ActivityOrder();
			return;
		}
		weight += activity.pLevel * maxID + activity.id;
	}
	// sort index
	weight.Sort(&id);
	// set activities in the correct order
	for (auto& i : id)
		newOrder += act[i];
	act = newOrder;
	UpdateIndex();
}

void CRCPSP::CleanResources() {
	if (Parametro(CLEAN_RES) == 0)
		return;
	// delete resources with sumRU <= capacity
	for (int k = 0; k < R(); k++) {
		int sumRU = 0;
		for (auto& activity : act)
			if ((sumRU += activity.res[k]) > capacity[k])
				break;
		if (sumRU <= capacity[k]) {
			// remove resource k
			capacity.Delete(k);
			for (auto& activity : act)
				activity.res.Delete(k);
			k--;
		}
	}
	if (Parametro(CLEAN_RES) == 1)
		return;
	// delete the resource if for any activity i, the sumRU of all compatible activities plus RU(i) <= capacity
	for (int k = 0; k < R(); k++) {
		bool noRestriction = true;
		for (auto& activity : act) {
			int sumRU = 0;
			TBits compatible = ~(activity.predBits | activity.sucBits);
			for (int j = 0; j < N(true) && noRestriction; j++)
				if (compatible.GetBit(j))
					noRestriction = ((sumRU += act[j].res[k]) <= capacity[k]);
			if (!noRestriction)
				break;
		}
		if (noRestriction) {
			// remove resource k
			capacity.Delete(k);
			for (auto& activity : act)
				activity.res.Delete(k);
			k--;
		}
	}
}


// update auxiliar datastructure
void CRCPSP::InitialSetup() {
	int max;
	ActivityOrder();

	// setup the binary precedence relations
	for (auto& activity : act) {
		activity.predBits.Count((N(true) - 63) / 64).Reset(0);
		activity.sucBits.Count((N(true) - 63) / 64).Reset(0);
		// setup direct precedence relations, and progressive level
		for (auto& pred : activity.pred) 
			activity.predBits.SetBit(Index(pred), true);
		for (auto& suc : activity.suc) 
			activity.sucBits.SetBit(Index(suc, N(true) - 1), true);
	}

	// use bits direct/indirect 
	for (int i = 0; i < N(true); i++)
		for (auto& pred : act[i].pred)
			act[i].predBits |= act[Index(pred)].predBits;
	for (int i = N(true) - 1; i >= 0; i--)
		for (auto& suc : act[i].suc)
			act[i].sucBits |= act[Index(suc)].sucBits;

	CleanResources();
	// resource waste procedure
	if(Parametro(RES_WASTE) > 0)
		FeasibleSets(3, max);

	// initial calculations
	ENivelDebug backupDebug = (ENivelDebug)Parametro(NIVEL_DEBUG);
	Parametro(NIVEL_DEBUG) = NADA;
	// calculate cpm heads/tails 
	TVector<int> es, ls;
	CPM(es, &ls);
	cpm = bestLB = es.Last();
	for (int i = 0; i < N(true); i++) {
		act[i].head = es[i];
		act[i].tail = bestLB - ls[i] - act[i].pt;
	}
	bestLB = es.Last(); // 1:CPM
	PriorityRule(9); // 9:LST
	bestUB = st.Last();
	if (bestLB < bestUB) {
		Parametro(NIVEL_DEBUG) = backupDebug;
		// improve heads/tails
		if (Parametro(HEADS_TAILS) >= 1) { // calculate LBc from all predecessors
			for (int i = 0; i < N(true); i++) {
				int bound;
				Parametro(NIVEL_DEBUG) = NADA;
				if (act[i].head < (bound = LBC(&(act[i].predBits)))) {
					act[i].head = bound;
					Parametro(NIVEL_DEBUG) = backupDebug;
					TProcura::Debug(COMPLETO, false, "\n%s h%d=%d %s%d", *Act2Str(act[i].id), i, bound,
						Icon(EIcon::LB), act[i].head + act[i].pt + act[i].tail);
				}
				Parametro(NIVEL_DEBUG) = NADA;
				if (act[i].tail < (bound = LBC(&(act[i].sucBits)))) {
					act[i].tail = bound;
					Parametro(NIVEL_DEBUG) = backupDebug;
					TProcura::Debug(COMPLETO, false, "\n%s t%d=%d %s%d", *Act2Str(act[i].id), i, bound,
						Icon(EIcon::LB), act[i].head + act[i].pt + act[i].tail);
				}
				Parametro(NIVEL_DEBUG) = backupDebug;
			}
		}
		if (Parametro(HEADS_TAILS) == 2) { // use baseLB for all heads/tails
			for (int i = 1; i <= N(); i++) {
				if (LBHead(act[i].predBits)) {
					TBits originalPredBits = OriginalBits(act[i].predBits);
					TSubNets& headNet = GetSubNet(originalPredBits);
					if (headNet.bestLB == -1) { // not calculated yet
						CRCPSP* head = Clone(act[i].predBits);
						// headNet pode ter movido de local na hashtable
						TSubNets& headNet = GetSubNet(originalPredBits);
						if (head->bestLB > act[i].head) {
							headNet.bestLB = head->bestLB;
							Debug(headNet, act[i].id, act[i].head);
							act[i].head = head->bestLB;
						}
						delete head;
						headNet.bestLB = act[i].head;
					}
					else {
						if (act[i].head < headNet.bestLB) {
							Debug(headNet, act[i].id, act[i].head, true);
							act[i].head = headNet.bestLB;
						}
					}
				}
				if (LBTail(act[i].sucBits)) {
					TBits originalSucBits = OriginalBits(act[i].sucBits);
					TSubNets& tailNet = GetSubNet(originalSucBits);
					if (tailNet.bestLB == -1) { // not calculated yet
						CRCPSP* tail = Clone(act[i].sucBits);
						// tailNet pode ter movido de local na hashtable
						TSubNets& tailNet = GetSubNet(originalSucBits);
						if (tail->bestLB > act[i].tail) {
							tailNet.bestLB = tail->bestLB;
							Debug(tailNet, act[i].id, act[i].tail);
							act[i].tail = tail->bestLB;
						}
						delete tail;
						tailNet.bestLB = act[i].tail;
					}
					else {
						if (act[i].tail < tailNet.bestLB) {
							Debug(tailNet, act[i].id, act[i].tail, true);
							act[i].tail = tailNet.bestLB;
						}
					}
				}
			}
		}
		// update bestLB and heads/tails of dummy activities
		for (auto& activity : act)
			if (bestLB < activity.head + activity.pt + activity.tail)
				bestLB = activity.head + activity.pt + activity.tail;
		if (cpm < bestLB)
			TProcura::Debug(COMPLETO, false, "\nCPM bound %d, after heads/tails %d", cpm, bestLB);
		if (Parametro(BASE_LB) != 1) { // not the CPM
			int baseLB = BaseLB(Parametro(BASE_LB), Parametro(LB_MODE));
			if (bestLB < baseLB)
				bestLB = baseLB;
		}
		if (bestLB < bestUB && (Parametro(BASE_PR) != 9 || Parametro(PR_MODE) != 0)) // not the LST
			bestUB = BaseUB(Parametro(BASE_PR), Parametro(PR_MODE));
		// tail/head of dummy start and end activities are the bestLB
		act.First().tail = bestLB;
		act.Last().head = bestLB;
	}
	Parametro(NIVEL_DEBUG) = backupDebug;
}

void CRCPSP::Debug(TSubNets& subNet, int id, int prevBound, bool reused) {
	TProcura::Debug(DETALHE, false, "\nSubset {", subNet.activities.Data());
	for (auto& activity : act)
		if (subNet.activities.GetBit(activity.id))
			TProcura::Debug(DETALHE, false, "%s ", *Act2Str(activity.id));
	TProcura::Debug(DETALHE, false, "} LB%d - %s (prev %d)%s",
		subNet.bestLB, *Act2Str(id), prevBound, reused ? " reused" : "");
}


// convert bits in orignal ID bits
TBits CRCPSP::OriginalBits(TBits& bits) {
	TBits original;
	for (int i = 1; i <= N(); i++)
		if (bits.GetBit(i))
			original.SetBit(act[i].id, true);
	return original;
}

int CRCPSP::ExecutaAlgoritmo()
{
	calls = 0;
	// check if there is no procedure or no instance
	if (Parametro(ALGORITMO) == 0 || N() == 0)
		return 1;

	// search strategy
	if (Parametro(HORIZON) < 0) {
		int totalTime = 0; // sum the time from all runs
		int backup = Parametro(HORIZON);
		TVector<int> bestSolution;
		bestSolution = st;

		if (backup == -1) { // LBS 
			Parametro(HORIZON) = 0;
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
					Parametro(HORIZON)++;
				}
				else
					break; // not solved, stop
			}
		}
		else if (Parametro(HORIZON) == -2) { // UBS
			Parametro(HORIZON) = bestUB - bestLB - 1;
			while (bestLB < bestUB) {
				ExecutaAlgoritmo();
				calls++;
				totalTime += indicators[IND_TEMPO];
				if (indicators[IND_RESULTADO] == 1) { // resolvido, solução
					bestUB = bestLB + Parametro(HORIZON);
					Parametro(HORIZON)--;
					bestSolution = st; // save best solution found
				}
				else if (indicators[IND_RESULTADO] == 2) // impossible
					bestLB = bestUB;
				else
					break; // not solved, stop
			}
		}
		else if (Parametro(HORIZON) == -3) { // DBS
			Parametro(HORIZON) = (bestUB - bestLB) / 2;
			while (bestLB < bestUB) {
				ExecutaAlgoritmo();
				calls++;
				totalTime += indicators[IND_TEMPO];
				if (indicators[IND_RESULTADO] == 1) { // resolvido, solução
					bestUB = bestLB + Parametro(HORIZON);
					Parametro(HORIZON) = (bestUB - bestLB) / 2;
					bestSolution = st; // save best solution found
				}
				else if (indicators[IND_RESULTADO] == 2) { // impossível
					bestLB = bestLB + Parametro(HORIZON) + 1;
					Parametro(HORIZON) = (bestUB - bestLB) / 2;
				}
				else
					break; // not solved, stop
			}
		}

		indicators[IND_TEMPO] = totalTime;
		st = bestSolution; // restore best solution found
		if (bestLB == bestUB)
			indicators[IND_RESULTADO] = 1; // mark as solved, since we know the optimal makespan
		Parametro(HORIZON) = backup;
	}
	else {
		// single run with given horizon
		// converter para SAT
		TString ficheiro;
		ficheiro.printf("%s%d.cnf", *ficheiroInstancia, instancia.valor)
			.writeLines(SATConverter(Parametro(RES_METHOD)));

		if (conversaoOK) {
			// executar cadical
			CCaDiCaL::ExecutaAlgoritmo();
			calls = 1;

			if (indicators[IND_RESULTADO] == 1) {
				ExtrairSolucao(satSol, Parametro(RES_METHOD));
				// verificar solução
				if (VerificarSolucao()) {
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
	return variaveis.Count() - 1 <= Parametro(MAX_VARS) &&
		cnf.Count() - 2 <= Parametro(MAX_CLAUSES);
}

TVector<TString> CRCPSP::SATConverter(int metodo) {
	TVector<TString> cnf;
	int horizon = bestLB + Parametro(HORIZON);
	conversaoOK = false;
	LimparEstatisticas();
	ResetHashtable();
	// starting comment
	cnf += TString().printf("c (%s%d) RCPSP to CNF, method %d.", *ficheiroInstancia, instancia.valor, metodo);
	cnf += TString(); // fill in the end, after processing all data

	// access to variables to create IDs
	// add l(j,t) variables for each activity j and time t, indicating if activity j starts at or less instant t
	for (auto& activity : act)
		for (int t = ES(activity); t <= LS(activity, horizon); t++)
			VarL(activity.id, t);
	// add u(j,t), in the time window - activity j running at instant t
	for (auto& activity : act)
		for (int t = ES(activity); t < LF(activity, horizon); t++)
			VarU(activity.id, t);

	// add clauses for CPM constraints
	for (auto& activity : act)
		cnf += TString().printf("%d 0", VarL(activity.id, LS(activity, horizon))); // l(j,ls[j]) = 1

	// Consistency: l(i,t) => l(i,t+1)   -l(j,t) + l(j,t+1)
	if (TamanhoOK(cnf) && !TempoExcedido())
		for (auto& activity : act)
			for (int t = ES(activity); t < LS(activity, horizon); t++)
				cnf += TString().printf("%d %d 0", -VarL(activity.id, t), VarL(activity.id, t + 1));

	// Link: u(i,t) <=> (l(i,t) & ~l(i,t-di))
	// -u(j,t) + l(j,t)  | -u(j,t) + -l(j,t-dj) | -l(j,t) + l(j,t-dj) + u(j,t)
	if (TamanhoOK(cnf) && !TempoExcedido())
		for (auto& activity : act)
			for (int t = ES(activity); t < LF(activity, horizon); t++) {
				if (t <= LS(activity, horizon))
					cnf += TString().printf("%d %d 0",
						-VarU(activity.id, t),
						VarL(activity.id, t)); // -u(j,t) + l(j,t)
				if (t - activity.pt >= ES(activity))
					cnf += TString().printf("%d %d 0",
						-VarU(activity.id, t),
						-VarL(activity.id, t - activity.pt)); // -u(j,t) + -l(j,t-dj)
				cnf += TString().printf("%d ", VarU(activity.id, t)); // u(j,t)
				if (t < LS(activity, horizon))
					cnf.Last().printf("%d ", -VarL(activity.id, t)); // -l(j,t)
				if (t - activity.pt >= ES(activity))
					cnf.Last().printf("%d ", VarL(activity.id, t - activity.pt)); // l(j,t-dj) 
				cnf.Last().printf("0");
			}


	// Precedence(i, i'): l(i', t) => l(i, t - di)
	// -l(i,t) + l(j,t-dj)
	if (TamanhoOK(cnf) && !TempoExcedido())
		for (auto& activity : act)
			for (auto& suc : activity.suc) {
				TActivity& sucessor = act[Index(suc, N(true) - 1)];
				for (int t = ES(sucessor); t <= LS(sucessor, horizon); t++)
					if (ES(activity) <= t - activity.pt &&
						t - activity.pt <= LS(activity, horizon)) // only add clause if t-di is in the time window of i
						cnf += TString().printf("%d %d 0", -VarL(sucessor.id, t), VarL(activity.id, t - activity.pt));
			}

	// add resource restrictions for all time instants
	for (int t = 0; t <= bestLB + Parametro(HORIZON) && TamanhoOK(cnf) && !TempoExcedido(); t++) {
		TVector<int> inTW, inFS; // activities in time window (can be running at time t)
		TBits inFSb;
		for (auto& activity : act) // add activitis that can be running at time t
			if (activity.pt > 0 && ES(activity) <= t && t < LF(activity, horizon))
				inTW += activity.id;
		if (metodo == 0) // COV - covers
			cnf += AddCOV(inTW, inFS, t);
		else if (metodo == 1) // MFS - Minimal Forbiddent Sets
			cnf += AddMFS(inTW, inFS, inFSb, t);
		else if (metodo == 2) // BDD - Binary Decision Diagrams
			for (int k = 0; k < R(); k++)
				cnf += AddBDD(inTW, inFS, t, k);
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
		resUsage[k] += act[Index(j)].res[k];

	// check if resources are violated
	if (ResourcesViolated(resUsage)) {
		bool minimal = true;
		// check if is minimal cover (test if removing any activity from inFS makes it feasible)
		for (int i = 0; minimal && i < inFS.Count() - 1; i++) {
			TVector<int> resTest;
			resTest = resUsage;
			for (int k = 0; k < R(); k++)
				resTest[k] -= act[Index(inFS[i])].res[k];
			minimal = !ResourcesViolated(resTest);
		}
		if (minimal) {
			// add clause to forbid this combination of activities in time t
			cnf += TString();
			for (auto& activity : inFS)
				cnf.Last().printf("%d ", -VarU(activity, t));
			cnf.Last().printf("0");
		}
	}
	else
		cnf += AddCOV(inTW, inFS, t);


	inFS.Pop();
	for (int k = 0; k < R(); k++)
		resUsage[k] -= act[Index(j)].res[k];

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


TVector<TString> CRCPSP::AddMFS(TVector<int>& inTW, TVector<int>& inFS, TBits& inFSb, int t) {
	static TVector<int> resUsage;
	TVector<TString> cnf;
	int j;

	if (inTW.Empty())
		return cnf;

	if (inFS.Empty())
		resUsage.Count(R()).Reset(0);

	// try with top activity
	j = inTW.Pop();

	// check if a current activity is predecessor of j or successor
	if (!(inFSb & act[Index(j)].predBits || inFSb & act[Index(j)].sucBits)) {
		// all compatible in precedence relations
		inFS.Push(j);
		inFSb.SetBit(Index(j), true);
		// check if j is compatible with other activities inFS
		for (int k = 0; k < R(); k++)
			resUsage[k] += act[Index(j)].res[k];

		// check if resources are violated
		if (ResourcesViolated(resUsage)) {
			bool minimal = true;
			// check if is minimal cover (test if removing any activity from inFS makes it feasible)
			for (int i = 0; minimal && i < inFS.Count() - 1; i++) {
				TVector<int> resTest;
				resTest = resUsage;
				for (int k = 0; k < R(); k++)
					resTest[k] -= act[Index(inFS[i])].res[k];
				minimal = !ResourcesViolated(resTest);
			}
			if (minimal) {
				// add clause to forbid this combination of activities in time t
				cnf += TString();
				for (auto& activity : inFS)
					cnf.Last().printf("%d ", -VarU(activity, t));
				cnf.Last().printf("0");
			}
		}
		else
			cnf += AddMFS(inTW, inFS, inFSb, t);

		inFS.Pop();
		inFSb.SetBit(j, false);
		for (int k = 0; k < R(); k++)
			resUsage[k] -= act[Index(j)].res[k];
	}

	// try without top activity
	cnf += AddMFS(inTW, inFS, inFSb, t);

	// left the set identical
	inTW.Push(j);

	return cnf;
}

// one Binary Decision Diagram for each resource type and time instant t
// - nodes are ITE gates
// - top level the gate should be set to 1
// - each gate have a label "IFE(level,resource,t,available)"
// - in gate=ITE(decision,true,false)
//   decision is a fixed variable in each level (inTW)
//   true is the gate with label "IFE(level+1,resource,t,available-act[j].res[resource])" -- resource is consumed
//   false is the gate with label "IFE(level+1,resource,t,available)" --- resource is not consumed
//   if resources are not enouth, true must be set to 0
//   if resources are more than needed, false must be set to 1

TVector<TString> CRCPSP::AddBDD(TVector<int>& inTW, TVector<int>& inFS, int t, int resource) {
	TVector<TString> cnf;
	static int available;
	static TVector<int> maxRes;
	int j;
	int vars, gate, varTrue, varFalse;

	if (inTW.Empty())
		return cnf;

	if (inFS.Empty()) {
		available = capacity[resource];
		maxRes.Count(inTW.Count());
		for (int i = 0; i < inTW.Count(); i++)
			maxRes[i] = act[Index(inTW[i])].res[resource] + (i > 0 ? maxRes[i - 1] : 0);
	}

	// activity j in this level
	inFS.Push(j = inTW.Pop());

	vars = variaveis.Count();
	gate = VarITE(inTW.Count(), resource, t, available);
	// if variable is selected and there is not enouth resources, then link to false
	if (available < act[Index(j)].res[resource])
		varTrue = 0;
	else
		varTrue = VarITE(inTW.Count() - 1, resource, t, available - act[Index(j)].res[resource]);
	// if varialbe is not selected and exist resources for the rest of activities, then link to true
	if (inTW.Count() == 0 || maxRes[inTW.Count() - 1] >= available)
		varFalse = 0;
	else
		varFalse = VarITE(inTW.Count() - 1, resource, t, available);

	cnf += GateITE(gate, VarU(j, t), varTrue, varFalse);

	// variable varTrue created here, generate the gate recursive
	// if was not created, already existed and created in other node
	if (varTrue > 0 && varTrue >= vars) {
		available -= act[j].res[resource];
		cnf += AddBDD(inTW, inFS, t, resource);
		available += act[Index(j)].res[resource];
	}

	if (varFalse > 0 && varFalse >= vars)
		cnf += AddBDD(inTW, inFS, t, resource);

	inTW.Push(inFS.Pop());

	return cnf;
}

TVector<TString> CRCPSP::AddADD(TVector<int> activities, int t) {
	TVector<TString> cnf;
	return cnf;
}

void CRCPSP::ExtrairSolucao(const TVector<int64_t>& satSol, int metodo) {
	if (Parametro(ALGORITMO) > 0) {
		// extract the starting times
		// initialize with earliest start times, and update with the solution
		st.Count(N(true));
		for (int i = 0; i < N(true); i++)
			st[i] = ES(act[i]);
		for (int64_t var : satSol) {
			int i, t;
			// can exist only variables true, if time windows is null, but in that case will be equal to es
			if (var < 0) {
				if (sscanf(Var(-var), "l %d %d", &i, &t) == 2) {
					// if l(i,t) = 1, st[i] <= t
					// if l(i,t) = 0, st[i] > t
					if (st[Index(i)] <= t)
						st[Index(i)] = t + 1;
					//				printf("l(%d,%d) = 0 => st[%d] > %d\n", i, t, i, t);
				}
			}
		}
	}
}

bool CRCPSP::VerificarSolucao() {
	TVector<int> instants, onProgress, resUsed;
	resUsed.Count(R()).Reset(0);
	// check precedence constraints
	for (int i = 0; i < N(true); i++)
		for (auto& suc : act[i].suc)
			if (st[i] + act[i].pt > st[Index(suc, N(true) - 1)]) {
				printf("\nPrecedence violation: activity %d finishes at %d, but successor %d starts at %d",
					act[i].id, st[i] + act[i].pt, suc, st[Index(suc, N(true) - 1)]);
				MostrarSolucao();
				return false;
			}

	// add instants when activities start and end
	instants = st;
	for (int i = 0; i < N(true); i++)
		instants += st[i] + act[i].pt;
	instants.BeASet();
	bestUB = instants.Last();

	for (auto& t : instants) {
		for (int i = 0; i < N(true); i++) {
			if (st[i] == t) {
				onProgress += act[i].id;
				for (int k = 0; k < R(); k++)
					resUsed[k] += act[i].res[k];
			}
			if (st[i] + act[i].pt == t) {
				onProgress -= act[i].id;
				for (int k = 0; k < R(); k++)
					resUsed[k] -= act[i].res[k];
			}
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
	for (int i = 0; i < N(true); i++)
		if (st[i] >= 0)
			instants += st[i] + act[i].pt;
	instants += 0;
	instants.BeASet();
	instants -= -1; // remove the code for activity not scheduled

	lines += TString().printf("  t%s", Icon(EIcon::TEMPO));
	for (int k = 0; k < R(); k++)
		lines.Last() += Res2Str(capacity[k], k);
	lines.Last().printf("Activities%s", Icon(EIcon::ID));

	for (auto& t : instants) {
		lines += TString().printf("%3d%s", t, Icon(EIcon::TEMPO));
		for (int i = 0; i < N(true); i++) {
			if (st[i] == t) {
				onProgress += act[i].id;
				for (int k = 0; k < R(); k++)
					resUsed[k] += act[i].res[k];
			}
			if (st[i] >= 0 && st[i] + act[i].pt == t) {
				onProgress -= act[i].id;
				for (int k = 0; k < R(); k++)
					resUsed[k] -= act[i].res[k];
			}
		}
		for (int k = 0; k < R(); k++)
			lines.Last() += Res2Str(resUsed[k], k);
		int count = 0;
		for (auto& activity : onProgress) {
			if (count++ >= 10) {
				count = 0;
				lines += TString().printf("   %s", Icon(EIcon::TEMPO));
				for (int k = 0; k < R(); k++)
					lines.Last() += Res2Str(0, k);
			}
			lines.Last() += Act2Str(activity, 0, 1);
		}
	}
	MostraCaixa(lines);
}

// scheduling scheme methods (to implement priority rules, needed in conversion)
void CRCPSP::Serial(TVector<int> ids) {
	// schedule each activity by id order in the first available time slot
	int horizon = 0;
	TVector<TVector<int>> resAvailable;
	resAvailable.Count(R());
	st.Count(N(true)).Reset(-1);
	for (auto& activity : act)
		horizon += activity.pt;
	for (int k = 0; k < R(); k++)
		resAvailable[k].Count(horizon).Reset(capacity[k]);

	if (Parametro(NIVEL_DEBUG) >= COMPLETO) {
		printf("\nSerial: ");
		for (auto& i : ids)
			printf("%s", *Act2Str(act[i].id));
	}

	for (auto& id : ids) {
		int startTime = ES(act[id]);
		// check predecessors of this activity, when finish
		for (auto& pred : act[id].pred)
			if (startTime < st[Index(pred)] + act[Index(pred)].pt)
				startTime = st[Index(pred)] + act[Index(pred)].pt;
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
		if (Parametro(NIVEL_DEBUG) >= DETALHE) {
			printf("\ns[ %s] = %d ", *Act2Str(act[id].id, 0, 3), st[id]);
			if (Parametro(NIVEL_DEBUG) >= COMPLETO)
				MostrarSolucao();
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
		for (auto& pred : act[i].pred) {
			int j = Index(pred);
			if (es[i] < es[j] + act[j].pt)
				es[i] = es[j] + act[j].pt;
		}
	}
	if (makespan < 0) {
		makespan = es.Last();
		if (ls != NULL)
			for (int i = N(true) - 1; i >= 0; i--) {
				(*ls)[i] = makespan - act[i].pt;
				for (auto& suc : act[i].suc) {
					int j = Index(suc, N(true) - 1);
					if ((*ls)[i] > (*ls)[j] - act[i].pt)
						(*ls)[i] = (*ls)[j] - act[i].pt;
				}
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
					// swap ids[j]  ids[i] 
					int temp = ids[j];
					ids[j] = ids[i];
					ids[i] = temp;
					break;
				}
			}
		}
		if (!(act[ids[i]].predBits & notProcessed))
			notProcessed.SetBit(ids[i], false); // mark this activity as processed
	}
}

// show the instance data (for debugging)
void CRCPSP::Debug(bool completo) {
	int level = -1;
	TVector<TString> lines;
	lines += TString().printf("Instância: %s%d.rcp (%s%d %s%d)",
		*ficheiroInstancia, instancia.valor, Icon(EIcon::LB), bestLB, Icon(EIcon::UB), bestUB);
	lines += TString().printf(" ID%s h ↤PT↦ t %sPL%s", Icon(EIcon::ID), Icon(EIcon::TEMPO), Icon(EIcon::LIMITE));
	for (int k = 0; k < R(); k++)
		lines.Last() += Res2Str(capacity[k], k);
	lines.Last().printf("Pred%sSuc", Icon(EIcon::ID));
	for (auto& activity : act) {
		lines += Act2Str(activity.id, 3).printf("%s↤%s↦%s%s",
			*PT2Str(activity.head, true),
			*PT2Str(activity.pt),
			*PT2Str(activity.tail, true),
			Icon(EIcon::TEMPO));
		if (activity.pLevel != level)
			lines.Last().printf("%2d%s", level = activity.pLevel, Icon(EIcon::LIMITE));
		else
			lines.Last().printf("  %s", Icon(EIcon::LIMITE));
		for (int k = 0; k < R(); k++)
			lines.Last() += Res2Str(activity.res[k], k);
		for (auto& pred : activity.pred)
			if (pred > 0) // do not show dummy activity
				lines.Last() += Act2Str(pred);
		lines.Last().printf("%s", Icon(EIcon::ID));
		for (auto& suc : activity.suc)
			if (suc <= N())
				lines.Last() += Act2Str(suc);
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
	TActivity& activity = act[Index(id)];
	if (extra == 2 || extra == 3)
		resText.printf(":%d%s", activity.pt, extra == 3 ? ":" : "");
	for (int k = 0; (extra == 1 || extra == 3) && k < R(); k++)
		if (activity.res[k] > 0) {
			if (resText.Empty())
				resText = ":";
			resText += Res2Str(activity.res[k], k, true);
		}
	for (int j = 0; extra > 0 && j < activity.pred.Count(); j++)
		if (activity.pred[j] > 0) { // do not show dummy activity 
			if (predText.Empty())
				predText = "(";
			predText += Act2Str(activity.pred[j]);
		}
	if (!predText.Empty()) // last space became a parenthesis
		predText[predText.Count() - 2] = ')';
	if (space == 0)
		return TString().printf("%s%d%s%s%s ", *HSL(id * 360.0 / N(true), 1, 0.5, false), id, *HSL(), *resText, *predText);
	return TString().printf("%s%*d%s%s%s%s", *HSL(id * 360.0 / N(true), 1, 0.5, false), space, id, *HSL(), *resText, *predText, Icon(EIcon::ID));
}

TString CRCPSP::PT2Str(int pt, bool headTail) {
	if (headTail) {
		if (pt == 0)
			return TString("   ");
		return TString().printf("%s%3d%s", *HSL(0, 0, 0.5, false), pt, *HSL());
	}
	return TString().printf("%2d", pt);
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
	else if (id == IND_BASE_LB)
		return BaseLB(Parametro(BASE_LB), Parametro(LB_MODE));
	else if (id == IND_PR)
		return BaseUB(Parametro(BASE_PR), Parametro(PR_MODE));

	if (id >= IND_CNC && id <= IND_TF) {
		MorphologicalIndicators(Parametro(IND_MODE) == 1);
		if(id == IND_CNC)
			return (int64_t)(indCNC * 1000 + 0.5);
		if (id == IND_OS)
			return (int64_t)(indOS * 1000 + 0.5);
		if (id == IND_SP)
			return (int64_t)(indSP * 1000 + 0.5);
		if (id == IND_AD)
			return	(int64_t)(indAD * 1000 + 0.5);
		if (id == IND_LA)
			return (int64_t)(indLA * 1000 + 0.5);
		if (id == IND_I5)
			return (int64_t)(indI5 * 1000 + 0.5);
		if (id == IND_TF)
			return (int64_t)(indTF * 1000 + 0.5);
	}

	if (id == IND_RF)
		return (int64_t)(1000 * RF() + 0.5);
	if (id == IND_RU)
		return (int64_t)(1000 * RU() + 0.5);
	if (id == IND_RS)
		return (int64_t)(1000 * RS() + 0.5);
	if (id == IND_RC)
		return (int64_t)(1000 * RC() + 0.5);
	if (id == IND_WPREC)
		return (int64_t)(1000 * W() + 0.5);
	if (id == IND_WALL)
		return (int64_t)(1000 * W(false) + 0.5);
	if (id == IND_FS21)
		return (int64_t)(1000 * FS(2, 1) + 0.5);
	if (id == IND_FS22)
		return (int64_t)(1000 * FS(2, 2) + 0.5);
	if (id == IND_FS31)
		return (int64_t)(1000 * FS(3, 1) + 0.5);
	if (id == IND_FS32)
		return (int64_t)(1000 * FS(3, 2) + 0.5);
	return 0;
}

void CRCPSP::MorphologicalIndicators(bool duration) {
	TVector<int> PL, RL, WA, n;
	int maxPT = 1, sumPT = 0;

	if (mophologicOK)
		return;

	PL.Count(N(true)).Reset(0);
	RL.Count(N(true)).Reset(0);

	// update PL and RL (with duration just use ES and LS when appropriate)
	for (int i = 0; i < N(true); i++) {
		PL[i] = 0;
		for (auto pred : act[i].pred)
			if (PL[Index(pred)] + 1 > PL[i])
				PL[i] = PL[Index(pred)] + 1;
	}
	// number of levels (without dummy activities) or CPM in case of duration
	int M = PL.Last() - 1;

	if (Parametro(NIVEL_DEBUG) >= COMPLETO) {
		printf("\nPL: ");
		for (auto& pl : PL)
			printf("%d ", pl);
		printf("M=%d.", M);
	}

	for (int i = N(true) - 1; i >= 0; i--) {
		RL[i] = M + 1;
		for (auto suc : act[i].suc)
			if (RL[Index(suc, N(true) - 1)] - 1 < RL[i])
				RL[i] = RL[Index(suc, N(true) - 1)] - 1;
	}
	if (Parametro(NIVEL_DEBUG) >= COMPLETO) {
		printf("\nRL: ");
		for (auto& rl : RL)
			printf("%d ", rl);
	}

	if (duration) {
		// calculate maxPT, otherwise is 1
		for (auto& activity : act) {
			if (maxPT < activity.pt)
				maxPT = activity.pt;
			sumPT += activity.pt;
		}
	}
	if (N() > 0) {
		if (duration)
			indSP = (double)(ES(act.Last()) - maxPT) / (sumPT - maxPT);
		else
			indSP = (double)(M - 1) / (N() - 1); // (M-1)/(N-1)
	}
	else
		indSP = 0;

	WA.Count(M).Reset(0);

	for (int i = 1; i <= N(); i++)
		// ignore dummy activities
		WA[PL[i] - 1]++;

	if (Parametro(NIVEL_DEBUG) >= COMPLETO) {
		printf("\nWA:");
		for (auto& w : WA)
			printf("%d ", w);
	}
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

	//printf("\nD: %f.", D); fflush(stdout);
	// maximal size precedence relation size V
	int V = 0;
	for (int i = 1; i <= N(); i++)
		for (auto& suc : act[i].suc)
			if (Index(suc, N(true) - 1) <= N() && V < PL[Index(suc, N(true) - 1)] - PL[i])
				V = PL[Index(suc, N(true) - 1)] - PL[i];
	//printf("\nV: %d.", V); fflush(stdout);
	n.Count(V).Reset(0);
	for (int i = 1; i <= N(); i++)
		for (auto& suc : act[i].suc)
			if (Index(suc, N(true) - 1) <= N())
				n[PL[Index(suc, N(true) - 1)] - PL[i] - 1]++;

	if (Parametro(NIVEL_DEBUG) >= COMPLETO) {
		printf("\nn:");
		for (auto& w : n)
			printf("%d ", w);
		fflush(stdout);
	}

	double WA0 = WA[0];
	if (D == 0 || D == N() - WA0)
		indLA = 1;
	else
		indLA = (double)(n[0] - (N() - WA0)) / (D - (N() - WA0));

	//printf("\nLA: %f.", indLA); fflush(stdout);

	// total direct precedence relations
	uint64_t TDP = 0;
	for (auto& precs : n)
		TDP += precs;
	indCNC = (double)TDP / N();
	//printf("\nCNC: %f.", indCNC); fflush(stdout);

	if (TDP == 0 || TDP == (uint64_t)(N() - WA0))
		indI5 = 1;
	else {
		double count = n[0];
		for (int k = 1; k < V; k++) // O(N)
			count += 1.0 * n[k] * (k + 1 - (M - 1)) / (1 - (M - 1));

		indI5 = (double)(count - (N() - WA0)) / (TDP - (N() - WA0));
	}
	//printf("\nI5: %f.", indI5); fflush(stdout);

	if (duration ? M == sumPT || M == 0 : N() == M || M == 1 || N() == 0)
		indTF = 0;
	else {
		int count = 0;
		for (int i = 1; i <= N(); i++)
			count += (duration ? LS(act[i]) - ES(act[i]) : RL[i] - PL[i]);
		indTF = (double)count / (duration ? (N() - M) * ES(act.Last()) : (N() - M) * (M - 1));
	}
	//printf("\nTF: %f.", indTF); fflush(stdout);

	// OS --- need direct and indirect precedence relations
	int OSCount = 0;
	for (int i = 1; i <= N(); i++)
		for (int j = 1; j < i; j++)
			if (act[i].predBits.GetBit(j)) // if j is a predecessor of i
				(duration ? OSCount += act[i].pt * act[j].pt : OSCount++);
	indOS = (double)(OSCount * 2) / (duration ? sumPT * (sumPT - 1) : N() * (N() - 1)); // 2*OS/(N*(N-1))
	//printf("\nOS: %f.", indOS); fflush(stdout);
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
	TProcura::Debug(DETALHE, false, "\nTotal use: %d.", totalUse);
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
				avgUse += (capacity[k] > 0 ? (double)activity.res[k] / capacity[k] : 0);
				totalUse++;
			}
	TProcura::Debug(DETALHE, false, "\nAvg. use: %.3f, Total use: %d.", avgUse, totalUse);
	if (totalUse == 0)
		return 0;
	return avgUse / totalUse;
}

double CRCPSP::RS() {
	double sumRS = 0;
	for (int k = 0; k < R(); k++) {
		TVector<int> resUse;
		resUse.Count(ES(act.Last())).Reset(0);
		for (auto& activity : act)
			for (int t = ES(activity); t < EF(activity); t++)
				resUse[t] += activity.res[k];
		int maxRes = resUse.First();
		for (auto& res : resUse)
			if (maxRes < res)
				maxRes = res;
		int minRes = act[1].res[k];
		for (int i = 2; i <= N(); i++)
			if (minRes < act[i].res[k])
				minRes = act[i].res[k];
		double iRS = 1;
		if (!(maxRes == minRes || capacity[k] >= maxRes))
			iRS = (double)(capacity[k] - minRes) / (maxRes - minRes);
		TProcura::Debug(DETALHE, false, "\nMin %d, max res %d, cap %d, iRS %.3f",
			minRes, maxRes, capacity[k], iRS);
		sumRS += iRS;
	}
	return sumRS / R();
}


// feasible sets 
// mode - 0 - only precedence relations, 1 - only resources, 2 - precedence relations and resources
//       -1 - count only combinations
//        3 - resource waste procedure (identify maximal resource usage of an activity in all FS, 
//            if maximal resource usage is less than capacity, add this to the activity
// max - size of the greatest compatible set (if limit is not set)
// limit - if set, is the limit on the size of the feasible sets, counting only these ones
// return value is the number of calls (feasible sets of any size)
int64_t CRCPSP::FeasibleSets(int mode, int& max, int limit, TVector<int>* i, TBits incompatible, TVector<int> resUse) {
	static TVector<TBits> actIncomp;
	static int64_t calls, maxEffort;
	static TVector<int> minResUse;
	static int activityLimit;
	static TVector<int> id;
	bool firstLevel = (i == NULL);
	if (mode < 0) { // count combinations up to limit 
		int64_t result = 0, combinations = 1;
		if (limit <= 0 || limit > N())
			limit = N();
		for (int i = N(), j = 1; i > 1 && j <= limit; i--, j++) {
			combinations *= i;
			combinations /= j;
			if (j > 1)
				result += combinations;
			TProcura::Debug(COMPLETO, false, "\n%d: %lld (%lld)", j, combinations, result);
		}
		TProcura::Debug(DETALHE, false, "\nComb(%d,%d): %lld ", limit, N(), result);
		return result;
	}

	if (i == NULL) { // first call + preprocessing
		TVector<int>::erro = 0; // first value to iterate is 1, if i.Empty()
		i = new TVector<int>();
		actIncomp.Count(act.Count());
		for (int j = 0; mode != 1 && j < N(true); j++)
			(actIncomp[j] = act[j].predBits | act[j].sucBits).SetBit(j, true);
		resUse.Count(R()).Reset(0);
		calls = 0;
		maxEffort = 1000000;
		if (mode == 3) {
			TVector<int> weight;
			for (auto& activity : act)
				weight += activity.pt;
			weight.Sort(&id); // process top first, with higher weight 
			//id.Remove(0).Remove(N(true) - 1); // remove dummy activities
			limit = -1; // do not limit the size of the feasible sets, to identify maximal resource usage of each activity
			if (Parametro(RES_WASTE) == 0)
				return calls;
		}
	}
	else if (limit < 0) // if limit>=0 the calls are called when i->Count()==limit 
		calls++;

	if (--maxEffort < 0) {
		TProcura::Debug(DETALHE, false, "\nFeasible sets: maximal effort limit.");
		return calls;
	}
	if (mode == 3) {
		int itWR;
		// need to process all subsets valid in all activities
		// if i is empty, process all activities, from last to the end
		// if i have one element, start from the last also, but skip the first element
		// all the rest start from the last element -1
		if (i->Count() <= 1)
			itWR = id.Count() - 1;
		else
			itWR = id.Find(i->Last()) - 1;
		if (!i->Empty() && id[itWR] == i->First())
			itWR--;
		for (i->Push(id[itWR--]);
			(itWR >= 0) && maxEffort >= 0;
			i->Last() = id[itWR--]) {
			if (!incompatible.GetBit(i->Last()))
			{ // resources and precedence relations
				bool OK = true;
				TVector<int> newResUse(R());

				if (act[i->Last()].pt == 0)
					continue;

				if (firstLevel) {
					switch (Parametro(RES_WASTE)) {
					case 1: activityLimit = 10 * act[i->Last()].pt; break;
					case 2: activityLimit = 100 * act[i->Last()].pt; break;
					case 3: activityLimit = 1000 * act[i->Last()].pt; break;
					default: activityLimit = 0; break;
					}
					minResUse.Count(R()).Reset(0); // reset minimal resource usage for each resource
				}

				for (int k = 0; OK && k < R(); k++)
					OK = ((newResUse[k] = resUse[k] + act[i->Last()].res[k]) <= capacity[k]);
				if (OK) {
					if (mode == 3) { // update minimal resource usage for each resource
						bool existWaste = false;
						for (int k = 0; k < R(); k++) {
							if (minResUse[k] < newResUse[k])
								minResUse[k] = newResUse[k];
							existWaste |= (minResUse[k] < capacity[k]);

						}
						if (false) {
							printf("\nFeasible set:");
							for (auto& item : *i)
								printf("%s ", *Act2Str(act[item].id));
							printf(" - ");
							for (int k = 0; k < R(); k++)
								printf("%s", *Res2Str(newResUse[k], k));
							printf(" - ");
							for (int k = 0; k < R(); k++) {
								printf("%s", *Res2Str(minResUse[k], k));
								if (minResUse[k] < capacity[k])
									printf("%d ", capacity[k] - minResUse[k]);
							}
							printf(" (%d) %s", activityLimit,
								existWaste ? "Resource Waste" : "");
						}
						if (!existWaste || --activityLimit <= 0) {
							if (false && !existWaste)
								printf(" - No more waste for this activity.");
							else if (false && activityLimit <= 0)
								printf(" - Activity limit reached for this activity.");
							if (firstLevel)
								continue; // next activity
							i->Pop();
							return calls; // no waste on activity, continue to the next activity
						}
					}
					if (limit < 0 || i->Count() < limit)
						FeasibleSets(mode, max, limit, i, incompatible | actIncomp[i->Last()], newResUse);
					if (limit >= 0 && i->Count() >= 2 && i->Count() <= limit) {
						calls++;
						DebugFeasibleSet(mode, *i, calls);
					}
					// update maximal feasible set found
					if (limit < 0 && max < i->Count()) {
						max = i->Count();
						DebugFeasibleSet(mode, *i, max);
					}
					// check if exist waste
					if (firstLevel && activityLimit > 0) {
						for (int k = 0; k < R(); k++)
							if (minResUse[k] < capacity[k]) {
								act[i->Last()].res[k] += capacity[k] - minResUse[k];
								TProcura::Debug(COMPLETO, false, "\nActivity %s: res %d on resource %d added to avoid waste (limit %d).",
									*Act2Str(act[i->Last()].id), capacity[k] - minResUse[k], k, activityLimit);
							}
					}
					else if (!firstLevel) {
						// check if exist waste
						bool existWaste = false;
						for (int k = 0; k < R(); k++)
							existWaste |= (minResUse[k] < capacity[k]);
						if (!existWaste) {
							i->Pop();
							return calls; // no waste on activity, continue to the next activity
						}
					}
				}
			}
		}
	}
	else {
		for (i->Push(i->Last() + 1);
			i->Last() <= N() && maxEffort >= 0;
			i->Last()++)
			// check if is parallel with all others
			if (mode == 1) { // check only resources
				bool OK = true;
				TVector<int> newResUse(R());
				for (int k = 0; OK && k < R(); k++)
					OK = ((newResUse[k] = resUse[k] + act[i->Last()].res[k]) <= capacity[k]);
				if (OK) {
					if (limit < 0 || i->Count() < limit)
						FeasibleSets(mode, max, limit, i, {}, newResUse);
					if (limit >= 0 && i->Count() >= 2 && i->Count() <= limit) {
						calls++;
						DebugFeasibleSet(mode, *i, calls);
					}
					// update maximal feasible set found
					if (limit < 0 && max < i->Count()) {
						max = i->Count();
						DebugFeasibleSet(mode, *i, max);
					}
				}
			}
			else if (!incompatible.GetBit(i->Last())) {
				if (mode == 0) { // only precedence relations
					if (limit < 0 || i->Count() < limit)
						FeasibleSets(mode, max, limit, i, incompatible | actIncomp[i->Last()]);
					if (limit >= 0 && i->Count() >= 2 && i->Count() <= limit) {
						calls++;
						DebugFeasibleSet(mode, *i, calls);
					}
					// update maximal feasible set found
					if (limit < 0 && max < i->Count()) {
						max = i->Count();
						DebugFeasibleSet(mode, *i, max);
					}
				}
				else { // resources and precedence relations
					bool OK = true;
					TVector<int> newResUse(R());

					for (int k = 0; OK && k < R(); k++)
						OK = ((newResUse[k] = resUse[k] + act[i->Last()].res[k]) <= capacity[k]);
					if (OK) {
						if (limit < 0 || i->Count() < limit)
							FeasibleSets(mode, max, limit, i, incompatible | actIncomp[i->Last()], newResUse);
						if (limit >= 0 && i->Count() >= 2 && i->Count() <= limit) {
							calls++;
							DebugFeasibleSet(mode, *i, calls);
						}
						// update maximal feasible set found
						if (limit < 0 && max < i->Count()) {
							max = i->Count();
							DebugFeasibleSet(mode, *i, max);
						}
					}
				}
			}
	}
	i->Pop();
	if (i->Empty())
		delete i;
	return calls;
}

void CRCPSP::DebugFeasibleSet(int mode, TVector<int>& i, int value) {
	if (ShowValue(value)) {
		TProcura::Debug(DETALHE, false, "\n%d: ", value);
		if (Parametro(NIVEL_DEBUG) >= COMPLETO)
			for (auto& item : i)
				printf("%s ", *Act2Str(act[item].id, 0, mode == 0 ? 0 : 1));
	}
}

bool CRCPSP::ShowValue(int value) {
	// first 10, and then only 10, 20, 50, 100, 200, 500, ...
	if (value < 10)
		return true;
	while (value % 10 == 0)
		value /= 10;
	return value == 1 || value == 2 || value == 5;
}


double CRCPSP::W(bool prec) { // WPREC, WALL
	int W = 1; // calculate the maximal compatible set (precedence only, or resource and precedence)

	FeasibleSets(prec ? 0 : 2, W);

	return (double)(W - 1) / (N() - 1);
}
double CRCPSP::FS(int x, int u) { // FS21, FS22, FS31, FS32
	int64_t FS = 0, ref = 0;
	int max;

	// x=2 all feasible pairs
	// x=3 all feasible triples
	// u=1 all possible pairs/triplets,
	// u=2 all possible pairs/triplets that are not precedence-related 
	FS = FeasibleSets(2, max, x);

	// calculate reference
	// reference is only precedence relations counting if u==2, otherwise is just combinations
	ref = FeasibleSets(u == 1 ? -1 : 0, max, x);

	TProcura::Debug(DETALHE, false, "\nFS: %lld ref: %lld", FS, ref);

	return (ref > 0 ? (double)FS / ref : 0);
}

// generate a critical path from es/ls data
TVector<int> CRCPSP::CriticalPath(TVector<int>& es, TVector<int>& ls, int& holes) {
	TVector<int> criticalPath;
	int current = 0;
	holes = 0;
	criticalPath += 0; // dummy start activity
	do {
		int minCost = 0, minSuc = -1;
		current = Index(criticalPath.Last(), N(true) - 1);
		for (auto& suc : act[current].suc) {
			int sucIndex = Index(suc, N(true) - 1);
			if (es[current] + act[current].pt == es[sucIndex] &&
				es[sucIndex] == ls[sucIndex]) {
				criticalPath += suc;
				minSuc = -1;
				break;
			}
			else if (minSuc < 0 || minCost > es[sucIndex] - (es[current] + act[current].pt)) {
				minSuc = suc;
				minCost = es[sucIndex] - (es[current] + act[current].pt);
			}
		}
		if (minSuc >= 0) {
			criticalPath += minSuc;
			holes += minCost;
		}
	} while (!act[current].suc.Empty());
	return criticalPath;
}

void CRCPSP::MostraCP(TVector<int> criticalPath) {
	int t = 0, count = 0;
	for (auto& activity : criticalPath) {
		TProcura::Debug(DETALHE, true, count++ % 6 != 0 ? "%s" : "%s\n", *Act2Str(activity));
		TProcura::Debug(COMPLETO, false, "\n%2d%s%s", t, Icon(EIcon::TEMPO), *Act2Str(activity, 0, 3));
		t += act[Index(activity)].pt;
	}
}

int CRCPSP::LBCPM() {
	// already calculated in InitialSetup
	if (Parametro(NIVEL_DEBUG) >= DETALHE) {
		TVector<int> es, ls;
		int holes;
		CPM(es, &ls); // not use the oficial ES/LS since my contain LBs
		printf("\nCritical path: ");
		MostraCP(CriticalPath(es, ls, holes));
	}
	return cpm;
}

int CRCPSP::LBC(TBits* sel) {
	int bound = 0;
	TProcura::Debug(DETALHE, false, "\nLBcapacity: ");
	for (int k = 0; k < R(); k++)
		if (capacity[k] > 0) {
			int workContent = 0;
			for (int i = 1; i <= N(); i++)
				if (sel == NULL || sel->GetBit(i))
					workContent += act[i].res[k] * act[i].pt;
			int wcBound = (workContent > 0 && capacity[k] > 0 ? (workContent - 1) / capacity[k] + 1 : 0);
			if (bound < wcBound)
				bound = wcBound;
			TProcura::Debug(DETALHE, false, "%s/%s=%d ",
				*Res2Str(workContent, k, true), *Res2Str(capacity[k], k, true), wcBound);
		}
	return bound;
}

int CRCPSP::LBCPC() {
	TVector<int> es, lf, activities, wc;
	int bound = 0, t = 0, fBound;
	es.Count(N(true));
	for (int i = 0; i < N(true); i++)
		es[i] = ES(act[i]);
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

	// repeat the calculation in inverse order
	lf.Count(N(true));
	for (int i = 0; i < N(true); i++)
		lf[i] = LF(act[i], bound);
	lf.Sort(&activities);
	wc.Count(R()).Reset(0);
	fBound = t = bound;
	TProcura::Debug(DETALHE, false, "Inv: ");
	for (auto& activity : activities.Invert()) {
		if (lf[activity] != t) {
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
				wc[k] -= capacity[k] * (t - lf[activity]);
				if (wc[k] < 0)
					wc[k] = 0;
			}
			t = lf[activity];
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
					fBound - t + (wc[k] - 1) / capacity[k] + 1);
		TProcura::Debug(DETALHE, false, "\n");
	}
	// calculate bound to the end of the schedule
	for (int k = 0; k < R(); k++)
		if (wc[k] > 0 && capacity[k] > 0 && bound < fBound - t + (wc[k] - 1) / capacity[k] + 1)
			bound = fBound - t + (wc[k] - 1) / capacity[k] + 1;

	return bound;
}

int CRCPSP::LBCS() {
	int bound = 0, ePT = 0, holes = 0;
	TVector<int> es, ls, criticalPath, pt, idPT;
	TVector<TVector<int>> resUsage;
	TBits inCP;

	bound = ES(act.Last());
	es.Count(N(true));
	ls.Count(N(true));
	for (int i = 0; i < N(true); i++) {
		es[i] = ES(act[i]);
		ls[i] = LS(act[i], bound);
	}
	resUsage.Count(R());
	for (auto& usage : resUsage)
		usage.Count(bound).Reset(0);

	TProcura::Debug(DETALHE, false, "\nLBcs: ");

	// build a critical path
	// note: other CP provide different LBs
	criticalPath = CriticalPath(es, ls, holes);
	for (auto critic : criticalPath) {
		inCP.SetBit(critic = Index(critic), true);
		for (int t = es[critic]; t < es[critic] + act[critic].pt; t++)
			for (int k = 0; k < R(); k++)
				resUsage[k][t] += act[critic].res[k];
	}

	if (Parametro(NIVEL_DEBUG) >= DETALHE) {
		printf("\nCritical path (%d): ", holes);
		MostraCP(criticalPath);
	}

	// only apply if the critical path is pure, 
	// otherwise is pointless, since can increase only maxPT
	if (holes > 0)
		return bound;

	// process activities by decreasing PT, to maximiaze cuts
	for (auto& activity : act)
		pt += activity.pt;
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
				*Act2Str(act[i].id, 0, Parametro(NIVEL_DEBUG) >= COMPLETO ? 3 : 0),
				es[i], ls[i] + act[i].pt - 1); fflush(stdout);

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
			if ((resUsage[k] += act[activity].res[k]) > capacity[k])
				return true;
	return false;
}


int CRCPSP::LBIP() {
	TVector<int> headPT, tailPT;
	int bound = ES(act.Last());
	for (auto& activity : act) {
		headPT += activity.head + activity.pt;
		tailPT += activity.tail + activity.pt;
	}

	TProcura::Debug(DETALHE, false, "\nLBip: ");

	// process all precedence OK pairs i>j
	for (int i = 1; i <= N(); i++)
		for (int j = i + 1; j <= N(); j++)
			if (!act[j].predBits.GetBit(i)) {
				int bound_ij, bound_ji;
				if ((bound_ij = headPT[i] + tailPT[j]) > bound &&
					(bound_ji = headPT[j] + tailPT[i]) > bound) {
					// potential new bound if resources are incompatible
					bool resOK = true;
					for (int k = 0; resOK && k < R(); k++)
						resOK = (act[i].res[k] + act[j].res[k] <= capacity[k]);

					if (!resOK) { // incompatible pair i||j
						bound = (bound_ij < bound_ji ? bound_ij : bound_ji);
						TProcura::Debug(DETALHE, false, "\n%s%d ", Icon(EIcon::LB), bound);
						TProcura::Debug(COMPLETO, false, "[%s> %s %d | %s> %s %d] ",
							*Act2Str(act[i].id), *Act2Str(act[j].id), bound_ij,
							*Act2Str(act[j].id), *Act2Str(act[i].id), bound_ji);
					}
				}
			}

	return bound;
}

int CRCPSP::LBIT() {
	return LBItuples();
}

// lower bound that detect any size resource incompatible tuples (2, 3, ... N)
int CRCPSP::LBItuples() {
	TVector<int> headPT, tailPT;
	TVector<int> resUsage;
	TVector<int> i; // vetor of iterators
	int bound = ES(act.Last());
	for (auto& activity : act) {
		headPT += activity.head + activity.pt;
		tailPT += activity.tail + activity.pt;
	}
	resUsage.Count(R()).Reset(0);
	TProcura::Debug(DETALHE, false, "\nLBituples (%d): ", bound);

	for (i.Push(1); i.Last() <= N(); i.Last()++) {
		// first level, impossible a incompatible tuple of 1
		for (int k = 0; k < R(); k++)
			resUsage[k] = act[i.Last()].res[k];
		BoundTuple(i,
			(act[i.Last()].predBits | act[i.Last()].sucBits).SetBit(i.Last(), true),
			resUsage, headPT, tailPT, bound);
	}
	return bound;
}

void CRCPSP::BoundTuple(TVector<int>& i, TBits predOK,
	TVector<int> resUsage, TVector<int>& headPT, TVector<int>& tailPT, int& bound, int bestBound)
{
	for (i.Push(i.Last() + 1);
		i.Last() <= N() && (bestBound < 0 || bestBound > bound);
		i.Last()++)
		// check if is parallel with all others
		if (!predOK.GetBit(i.Last())) {
			bool OK = true;
			int bound_ij, bound_ji;
			int localBest = bestBound;
			// check bound with all others
			for (int j = 0; OK && j < i.Count() - 1; j++)
				if ((bound_ij = headPT[i[j]] + tailPT[i.Last()]) > bound &&
					(bound_ji = headPT[i.Last()] + tailPT[i[j]]) > bound) {
					if (localBest < 0 || localBest > bound_ij)
						localBest = bound_ij;
					if (localBest > bound_ji)
						localBest = bound_ji;
				}
				else
					OK = false;
			// do not process this item if is not OK
			if (OK) {
				// verify resources
				bool resOK = true;
				TVector<int> backupRes = resUsage;
				for (int k = 0; resOK && k < R(); k++)
					resOK = ((resUsage[k] += act[i.Last()].res[k]) <= capacity[k]);
				if (!resOK) { // incompatible tuplet
					bound = localBest;
					TProcura::Debug(DETALHE, false, "\n%s%d #%d: ", Icon(EIcon::LB), bound, i.Count());
					for (auto& j : i)
						TProcura::Debug(COMPLETO, false, "%sh%d t%d ", *Act2Str(act[j].id, 0, 1), headPT[j], tailPT[j]);
				}
				else { // tuple can be increased
					BoundTuple(i,
						(predOK | act[i.Last()].predBits | act[i.Last()].sucBits).SetBit(i.Last(), true),
						resUsage, headPT, tailPT, bound, localBest);
				}
				resUsage = backupRes;
			}
		}
	i.Pop();
}


// return the number of compatible activities to each activity
TVector<TVector<int>> CRCPSP::CompatibleActivities() {
	TVector<TVector<int>> compatible;
	compatible.Count(N(true));

	for (int i = 1; i <= N(); i++)
		for (int j = i + 1; j <= N(); j++)
			// precedence compatible && resource compatible
			if (!act[j].predBits.GetBit(i) && !ResourceIncompatible({ i,j })) {
				compatible[i] += j;
				compatible[j] += i;
			}

	return compatible;
}


// Node Packing: 
// select an activity (most incompatible one), and schedule in serial
// remove all compatible activities with the selected activity from the list
// process until is empty
int CRCPSP::LBNodePacking(int n) {
	TVector<int> weight, activities; // process by increasing order of this weight vector, depending on mode n
	TVector<TVector<int>> compatible; // number of compatible activities (precedences and resources)
	TVector<int> es, ls, pt;
	int currentPT = 0, minHead, minTail;
	compatible = CompatibleActivities();
	int cpm, bound = ES(act.Last());
	es.Count(N(true));
	ls.Count(N(true));
	for (int i = 0; i < N(true); i++) {
		es[i] = ES(act[i]);
		ls[i] = LS(act[i], bound);
	}

	minHead = minTail = bound = cpm = es.Last();

	TProcura::Debug(DETALHE, false, "\nLBnp(%d,%d): ", n, bound);

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
			weight += -100 * act[i].head * act[i].tail + compatible[i].Count() * 100 - act[i].pt;
			break;
		case 3:
			// more restricted activities first (head+pt+tail), then number of compatible (lowest first)
			weight += -1000 * (act[i].head + act[i].pt + act[i].tail) + compatible[i].Count() * 100 - act[i].pt;
			break;
		default:
			weight += TRand::rand() % 10000; // random order
		}
	weight.Sort(&activities);
	activities.Invert(); // process the top first

	// local pt (to reduce pt instead of removing an activity)
	for (auto& activity : act)
		pt += activity.pt;

	while (!activities.Empty()) {
		int i = activities.Pop();

		if (pt[i] == 0) // do not process dummy activities
			continue;

		currentPT += pt[i]; // activity done in serial

		TProcura::Debug(DETALHE, false, "\n%s: %d%s ",
			*Act2Str(act[i].id, 0, (Parametro(NIVEL_DEBUG) >= COMPLETO ? 3 : 1)),
			currentPT, Icon(EIcon::TEMPO));

		for (auto& j : compatible[i]) {
			int index = activities.Find(j);
			if (index >= 0) {
				if (pt[j] > pt[i])
					pt[j] -= pt[i];
				else {
					activities[index] = -1;
					pt[j] = 0;
				}
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
		// test CPM and resource capacity bound with the rest of activities and current PT
		pt[i] = 0;
		int restLB = LBcpmCapacity(pt);
		if (restLB + currentPT > bound) {
			bound = restLB + currentPT;
			TProcura::Debug(DETALHE, false, "%s%d (%d) ", Icon(EIcon::LB), bound, restLB);
		}
	}

	return bound;
}

int CRCPSP::LBcpmCapacity(TVector<int>& pt) {
	TVector<int> es;
	int bound = 0;

	// calculate the CPM with the pt
	es.Count(N(true)).Reset(0);
	for (int i = 0; i < N(true); i++) {
		es[i] = 0;
		for (auto& pred : act[i].pred) {
			int j = Index(pred);
			if (es[i] < es[j] + pt[j])
				es[i] = es[j] + pt[j];
		}
	}
	bound = es.Last();
	// calculate the resource capacity bound with the pt
	for (int k = 0; k < R(); k++) {
		int workContent = 0;
		for (int i = 1; i <= N(); i++)
			workContent += act[i].res[k] * pt[i];
		int wcBound = (workContent > 0 && capacity[k] > 0 ? (workContent - 1) / capacity[k] + 1 : 0);
		if (bound < wcBound)
			bound = wcBound;
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
	TVector<int> pt, heads, tails, tHeads, tTails;
	TVector<TVector<TVector<int>>> actMatrix;
	TVector<TVector<int>> sumMatrix;
	int bound = ES(act.Last());
	heads.Count(N(true));
	tails.Count(N(true));
	for (int i = 0; i < N(true); i++) {
		heads[i] = act[i].head;
		tails[i] = act[i].tail;
	}

	TProcura::Debug(DETALHE, false, "\nLBpm(%d): ", m);

	// process different instants of heads/tails
	(tHeads = heads).BeASet();
	(tTails = tails).BeASet();

	// setup actMatrix[idxHead][idxTail][idAct] --- activities with idxHead and idxTail
	actMatrix.Count(tHeads.Count());
	sumMatrix.Count(tHeads.Count());
	for (int i = 0; i < tHeads.Count(); i++) {
		actMatrix[i].Count(tTails.Count());
		sumMatrix[i].Count(tTails.Count()).Reset(0);
	}

	for (int i = 1; i <= N(); i++)
		actMatrix[tHeads.Find(heads[i], true)][tTails.Find(tails[i], true)] += i;


	// process each resource
	for (int k = 0; k < R(); k++) {
		int machines = capacity[k];
		int stage = 0, u_min;
		TProcura::Debug(DETALHE, false, "\n%s:", *Res2Str(capacity[k], k));

		do {
			stage++;
			u_min = capacity[k] / (machines + 1) + 1;

			TProcura::Debug(COMPLETO, false, " %d/%d ", u_min, machines);

			for (int head = tHeads.Count() - 1; head >= 0; head--)
				for (int tail = tTails.Count() - 1; tail >= 0; tail--) {
					int sum = 0;
					for (auto& i : actMatrix[head][tail])
						if (act[i].pt > 0 && act[i].res[k] >= u_min)
							sum += (act[i].res[k] / u_min) * act[i].pt;
					// acumulate with the other activities, with greater head/tail
					if (head < tHeads.Count() - 2)
						sum += sumMatrix[head + 1][tail];
					if (tail < tTails.Count() - 2)
						sum += sumMatrix[head][tail + 1];
					if (head < tHeads.Count() - 2 && tail < tTails.Count() - 2)
						sum -= sumMatrix[head + 1][tail + 1];

					sumMatrix[head][tail] = sum;

					if (sum > 0 && bound < (sum = (sum - 1) / machines + 1 + tHeads[head] + tTails[tail])) {
						bound = sum;
						TProcura::Debug(DETALHE, false, " head:%d tail:%d %s%d ",
							tHeads[head], tTails[tail], Icon(EIcon::LB), bound);
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

int CRCPSP::LBPRtuples() {
	TVector<int> headPT, tailPT;
	TVector<int> resUsage;
	TVector<int> i; // vetor of iterators
	int bound = ES(act.Last());
	int tw = 0;
	for (auto& activity : act) {
		headPT += activity.head + activity.pt;
		tailPT += activity.tail + activity.pt;
	}
	resUsage.Count(R()).Reset(0);
	TProcura::Debug(DETALHE, false, "\nLBPRtuples (%d): ", bound);

	for (i.Push(1); i.Last() <= N(); i.Last()++) {
		// first level, impossible a incompatible tuple of 1
		for (int k = 0; k < R(); k++)
			resUsage[k] = act[i.Last()].res[k];
		if (BoundPRTuple(i,
			(act[i.Last()].predBits |
				act[i.Last()].sucBits).SetBit(i.Last(), true),
			resUsage, headPT, tailPT, bound, tw))
		{
			// restart
			i.Count(1).First() = 0;
			if (tw == -1) {
				tailPT = headPT = {};
				for (auto& activity : act) {
					headPT += activity.head + activity.pt;
					tailPT += activity.tail + activity.pt;
				}
				tw = 0;
			}
		}
	}
	return bound;
}

bool CRCPSP::BoundPRTuple(TVector<int>& i, TBits predOK,
	TVector<int> resUsage, TVector<int>& headPT, TVector<int>& tailPT, int& bound, int& tw,
	int weakPairs, int bestBound)
{
	for (i.Push(i.Last() + 1);
		i.Last() <= N() && (bestBound < 0 || bestBound > bound);
		i.Last()++)
		// check if is parallel with all others
		if (predOK.GetBit(i.Last())) {
			bool OK = true;
			int weakBounds = weakPairs;
			int bound_ij, bound_ji, localBound = bestBound;
			// check bound with all others
			for (int j = 0; OK && j < i.Count() - 1; j++) {
				bound_ij = headPT[i[j]] + tailPT[i.Last()];
				bound_ji = headPT[i.Last()] + tailPT[i[j]];
				if (bound_ij <= bound)
					weakBounds++;
				if (bound_ji <= bound)
					weakBounds++;
				if (localBound < 0 || localBound > bound_ij)
					localBound = bound_ij;
				if (localBound > bound_ji)
					localBound = bound_ji;
				// if two or more pairs are less or equal to current bound, then is pointless the tuple
				OK = (weakBounds <= 1);
			}
			// do not process this item if is not OK
			if (OK) {
				// verify resources
				bool resOK = true;
				TVector<int> backupRes = resUsage;
				for (int k = 0; resOK && k < R(); k++)
					resOK = ((resUsage[k] += act[i.Last()].res[k]) <= capacity[k]);
				if (!resOK) { // incompatible tuplet
					if (weakBounds == 0) {
						if (tw == 0)
							bound = bestBound;
						else
							bound++;
						tw = -1; // recalculate all
						return true; // restart
					}
					else {
						// add the only relation compatible with current bound
						// increase headPT and tailPT and update time windows of other activities

						// NOT DONE:
						// - identify the weak pair, to add time windows
						// - allow bestBound to be lower than bound if exist one weak pair
						// - this may not compensate

						tw = 1;
						return true; // restart with tbe new head/tails
					}
				}
				else // tuple can be increased
					if (BoundPRTuple(i,
						(predOK | act[i.Last()].predBits |
							act[i.Last()].sucBits).SetBit(i.Last(), true),
						resUsage, headPT, tailPT, bound, tw, weakBounds))
						return true;
				resUsage = backupRes;
			}
		}
	i.Pop();
	return false;
}


int CRCPSP::LBPR() {
	TVector<int> es, ls, headPT, tailPT;
	int tw = 0;
	int bound = ES(act.Last());
	headPT.Count(N(true));
	tailPT.Count(N(true));
	for (int i = 0; i < N(true); i++) {
		headPT[i] = act[i].head + act[i].pt;
		tailPT[i] = act[i].tail + act[i].pt;
	}

	TProcura::Debug(DETALHE, false, "\nLBpr (%d): ", bound);

	// process all precedence OK pairs i>j
	for (int i = 1; i <= N(); i++) {
		TProcura::Debug(COMPLETO, false, "\n%s ", *Act2Str(act[i].id));
		for (int j = i + 1; j <= N(); j++)
			if (!act[j].predBits.GetBit(i)) {
				bool resOK = true;
				for (int k = 0; resOK && k < R(); k++)
					resOK = (act[i].res[k] + act[j].res[k] <= capacity[k]);
				if (!resOK) { // incompatible pair i||j
					int bound_ij = headPT[i] + tailPT[j];
					int bound_ji = headPT[j] + tailPT[i];
					int pairBound = (bound_ij < bound_ji ? bound_ij : bound_ji);
					TProcura::Debug(COMPLETO, false, "%s %d,%d ", *Act2Str(act[j].id), bound_ij, bound_ji);
					if (bound < pairBound) {
						if (tw == 0)
							bound = pairBound;
						else
							bound++;
						TProcura::Debug(DETALHE, false, " %s%s%s%d ",
							*Act2Str(act[i].id), *Act2Str(act[j].id), Icon(EIcon::LB), bound);
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
							TProcura::Debug(COMPLETO, false, " (%s>%s) ",
								*Act2Str(act[j].id), *Act2Str(act[i].id));
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
							TProcura::Debug(COMPLETO, false, " (%s>%s) ",
								*Act2Str(act[i].id), *Act2Str(act[j].id));
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
			for (int i = 0; i < N(true); i++) {
				headPT[i] = act[i].head + act[i].pt;
				tailPT[i] = act[i].tail + act[i].pt;
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
			if (tailPT[Index(pred)] < tailPT[i] + act[Index(pred)].pt) {
				tailPT[Index(pred)] = tailPT[i] + act[Index(pred)].pt;
				updateB += Index(pred);
			}
	}
	// headPT[j] updated backward
	while (!updateF.Empty()) {
		int i = updateF.Pop();
		for (auto& suc : act[i].suc)
			if (headPT[Index(suc, N(true) - 1)] < headPT[i] + act[Index(suc, N(true) - 1)].pt) {
				headPT[Index(suc, N(true) - 1)] = headPT[i] + act[Index(suc, N(true) - 1)].pt;
				updateF += Index(suc, N(true) - 1);
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
			if (ls[Index(pred)] > ls[i] - act[Index(pred)].pt) {
				ls[Index(pred)] = ls[i] - act[Index(pred)].pt;
				if (ls[Index(pred)] < 0)
					return true;
				updateB += Index(pred);
			}
	}
	// ef[i] updated forward
	while (!updateF.Empty()) {
		int i = updateF.Pop();
		for (auto& suc : act[i].suc)
			if (ef[Index(suc, N(true) - 1)] < ef[i] + act[Index(suc, N(true) - 1)].pt) {
				ef[Index(suc, N(true) - 1)] = ef[i] + act[Index(suc, N(true) - 1)].pt;
				if (ef[Index(suc, N(true) - 1)] > bound)
					return true;
				updateF += Index(suc, N(true) - 1);
			}
	}
	return false;
}

int CRCPSP::LBCT() {
	TVector<int> ls, ef;
	TVector<TVector<int>> resUsage;
	int bound, tw = 0;
	int realES, realLS, alternatives, ok;
	bound = ES(act.Last());
	resUsage.Count(R());
	ls.Count(N(true));
	ef.Count(N(true));
	for (int i = 0; i < N(true); i++) {
		ls[i] = LS(act[i], bound);
		ef[i] = EF(act[i]);
	}

	// core times from ls to ef if intersect

	TProcura::Debug(DETALHE, false, "\nLBct (%d): ", bound);

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
					TProcura::Debug(DETALHE, false, "\n %s%s%d %d%s",
						*Act2Str(act[i].id), Icon(EIcon::LB), bound, t, Icon(EIcon::TEMPO));
					tw = -1;
					break;
				}
				for (int k = 0; k < R(); k++)
					res += (resUsage[k][t] += act[i].res[k]);
				if (ResourcesViolated(res)) {
					bound++;
					TProcura::Debug(DETALHE, false, "\n %s%s%d %d%s", *Act2Str(act[i].id), Icon(EIcon::LB), bound,
						t, Icon(EIcon::TEMPO));
					tw = -1;
					break;
				}
			}

		if (Parametro(NIVEL_DEBUG) >= COMPLETO) {
			TVector<int> res, resAnt;
			for (int i = 1; tw >= 0 && i <= N(); i++) {
				TProcura::Debug(COMPLETO, false, "\n%s ", *Act2Str(act[i].id, 0, 3));
				TProcura::Debug(COMPLETO, false,
					(ls[i] >= ef[i] ?
						COR_LEVE "[%d;%d]" COR_RESET : "[%d;%d]"),
					ls[i], ef[i]);
			}
			TProcura::Debug(COMPLETO, false, tw == 0 ? "\n Core times (%d): " : " Core times (%d): ",
				bound);
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
				TProcura::Debug(DETALHE, false, " %s%s%d " COR_INATIVO "[%d %d;%d %d]" COR_RESET,
					*Act2Str(act[i].id, 0, 3),
					Icon(EIcon::LB), bound, ef[i] - act[i].pt, ls[i], ef[i], ls[i] + act[i].pt);
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
				TProcura::Debug(DETALHE, false, "\n %s " COR_ATIVO "[%d %d;%d %d]" COR_RESET,
					*Act2Str(act[i].id, 0, 3),
					ef[i] - act[i].pt, ls[i], ef[i], ls[i] + act[i].pt);
			}
		}

		// reset time windows for the new bound
		if (tw < 0) {
			for (int i = 0; i < N(true); i++) {
				ls[i] = LS(act[i], bound);
				ef[i] = EF(act[i]);
			}
			continue;
		}

	} while (tw != 0);

	return bound;
}

int CRCPSP::LBTP() {
	TVector<int> ls, ef, ts, tt, workContent;
	int bound, z;
	bound = ES(act.Last());
	ls.Count(N(true));
	ef.Count(N(true));
	// this bound is based on LBcapacity
	if ((z = LBC()) > bound)
		bound = z;
	for (int i = 0; i < N(true); i++) {
		ls[i] = LS(act[i], bound);
		ef[i] = EF(act[i]);
	}

	TProcura::Debug(DETALHE, false, "\nLBtp (%d): ", bound);

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
							for (int i = 0; i < N(true); i++)
								ls[i] = LS(act[i], bound);
							(tt = ls).BeASet();
							z = 1;
							break;
						} //else
						//	TProcura::Debug(DETALHE, false, "\n [%d;%d]%s%s%d %d - %d ",
						//		t, q, Icon(EIcon::TEMPO), Icon(EIcon::LB), bound, k, workContent[k]);
					if (z == 1)
						break;
				}
			if (z == 1)
				break;
		}
	} while (z > 0);

	return bound;
}

int CRCPSP::LBAll(int mode) {
	int best, bound;
	TVector<int> baseLBs = { 2,3,4,5,6,7,8,9,10,11 };
	TVector<int> nModes = { 1,1,1,1,1,4,3,1,1 ,1 };
	ENivelDebug backupDebug = (ENivelDebug)Parametro(NIVEL_DEBUG);
	Parametro(NIVEL_DEBUG) = NADA;
	best = LBCPM();

	for (int i = 0; i < baseLBs.Count() && best < bestUB; i++)
		for (int m = 0; m < nModes[i] && best < bestUB; m++)
			if ((bound = BaseLB(baseLBs[i], m)) > best) {
				if (backupDebug >= COMPLETO) {
					// repeat the LB to show debug data
					Parametro(NIVEL_DEBUG) = backupDebug;
					BaseLB(baseLBs[i], m);
					Parametro(NIVEL_DEBUG) = NADA;
				}
				best = bound;
			}

	Parametro(NIVEL_DEBUG) = backupDebug;
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
				value[i] -= act[Index(suc, N(true) - 1)].pt;
			break;
		case 7: // EST
			value[i] = ES(act[i]);
			/*
			value[i] = 0;
			for (auto& pred : act[i].pred)
				if (value[Index(pred)] + act[Index(pred)].pt > value[i])
					value[i] = value[Index(pred)] + act[Index(pred)].pt;*/
			break;
		case 8: // EFT
			value[i] = EF(act[i]);
			/*
			value[i] = act[i].pt;
			for (auto& pred : act[i].pred)
				if (value[Index(pred)] + act[i].pt > value[i])
					value[i] = value[Index(pred)] + act[i].pt;*/
		case 9: // LST
			value[i] = LS(act[i]);
			/*
			invI = N(true) - i - 1;
			value[invI] = -act[invI].pt; // 0 is the finish of project
			for (auto& suc : act[invI].suc)
				if (value[Index(suc, N(true) - 1)] - act[invI].pt < value[invI])
					value[invI] = value[Index(suc, N(true) - 1)] - act[invI].pt;*/
			break;
		case 10: // LFT
			value[i] = LF(act[i]);
			/*
			invI = N(true) - i - 1;
			value[invI] = 0;
			for (auto& suc : act[invI].suc)
				if (value[Index(suc, N(true) - 1)] - act[Index(suc, N(true) - 1)].pt < value[invI])
					value[invI] = value[Index(suc, N(true) - 1)] - act[Index(suc, N(true) - 1)].pt;*/
			break;
		case 11: // MSLK
			value[i] = LS(act[i]) - ES(act[i]);
			/*
			value[i] = 0; // EST
			for (auto& pred : act[i].pred)
				if (value[Index(pred)] + act[Index(pred)].pt > value[i])
					value[i] = value[Index(pred)] + act[Index(pred)].pt;
			invI = N(true) - i - 1;
			valueAux[invI] = 0; // LST
			for (auto& suc : act[invI].suc)
				if (valueAux[Index(suc, N(true) - 1)] - act[Index(suc, N(true) - 1)].pt < valueAux[invI])
					valueAux[invI] = valueAux[Index(suc, N(true) - 1)] - act[Index(suc, N(true) - 1)].pt;*/
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
					value[i] -= act[Index(suc, N(true) - 1)].res[k] * act[Index(suc, N(true) - 1)].pt;
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



// explorar manualmente os dados:
// - mostrar instância e solução
// - permitir editar instância e solução
void CRCPSP::Explorar() {
	int option = 0;
	do {
		Debug(true);
		if (!insertedPred.Empty()) {
			printf("\nExtra: ");
			for (auto& extra : insertedPred)
				printf("(%d,%d) ", extra.pred, extra.suc);
		}
		if (VerificarSolucao())
			MostrarSolucao();
		TVector<TString> action = NovoTexto("\nAction: ").tok(" \t\r\n,();:");
		option = action.Count();
		if (action.First() == TString("s")) { // change  solution start times
			int j = atoi(action[1]);
			for (int i = 2; j + i - 2 < N(true) && i < action.Count(); i++)
				st[j + i - 2] = atoi(action[i]);
		}
		else if (action.First() == TString("d")) { // change  activity duration
			int j = atoi(action[1]);
			for (int i = 2; j + i - 2 < N(true) && i < action.Count(); i++)
				act[j + i - 2].pt = atoi(action[i]);
		}
		else if (action.First() == TString("r")) { // change resource usage
			int j = atoi(action[1]);
			for (int i = 1; i <= R() && i + 1 < action.Count(); i++)
				act[j].res[i - 1] = atoi(action[i + 1]);
		}
		else if (action.First() == TString("a")) { // change resource availability
			for (int i = 1; i <= R() && i < action.Count(); i++)
				capacity[i - 1] = atoi(action[i]);
		}
		else if (action.First() == TString("A")) { // change precedence relations
			for (int i = 1; i + 1 < action.Count(); i += 2)
				ChangePred(atoi(action[i]), atoi(action[i + 1]));
			InitialSetup();
		}
		else if (action.First() == TString("Istep")) { // one step of inverse method (testing)
			TVector<TPredecessor> arcs;
			int bound = atoi(action[1]); // reference for this method
			// all alternatives are provided 
			// check for each one, the two instances (UB and LB)
			// make one choice with a rule
			for (int i = 2; i + 1 < action.Count(); i += 2)
				arcs += {atoi(action[i]), atoi(action[i + 1])};

			for (auto& arc : arcs) {
				TVector<int> resUse;
				bool resOK = true, arcOK = false;
				printf("\n%s %s: ", *Act2Str(arc.pred, 0, 1), *Act2Str(arc.suc, 0, 1));
				ChangePred(arc.pred, arc.suc);
				InitialSetup();
				printf("\t%s%d %s%d ", Icon(EIcon::LB), bestLB, Icon(EIcon::UB), bestUB);
				ChangePred(arc.pred, arc.suc);
				// if are incompatible pairs, use base LB in the inverse relation
				for (int k = 0; resOK && k < R(); k++)
					resOK = ((resUse[k] = act[Index(arc.pred)].res[k] + act[Index(arc.suc, N(true) - 1)].res[k]) <= capacity[k]);
				if (!resOK) {
					ChangePred(arc.suc, arc.pred);
					InitialSetup();
					printf("\tIP[%s%d %s%d]", Icon(EIcon::LB), bestLB, Icon(EIcon::UB), bestUB);
					ChangePred(arc.suc, arc.pred);
					arcOK = (bestLB >= bound);
				}
				else {
					printf("\tIset[");
					// otherwise: 
					// - run the incompatible sets, starting with these activities!
					//   - verify using only the incompatible set LB, since is the faster one
					InitialSetup();
					// check if inverted bound is ok
					int iPrec = Index(arc.pred), iSuc = Index(arc.suc, N(true) - 1), localBound;
					if ((localBound = act[iSuc].head + act[iSuc].pt + act[iPrec].pt + act[iPrec].tail) < bound) {
						printf("%s %s %s%d.", *Act2Str(arc.suc), *Act2Str(arc.pred), Icon(EIcon::LB), localBound);
						arcOK = false;
					}
					else {
						TVector<int> i, headPT, tailPT;
						int auxBound = bound - 1;
						// setup data for calling BoundTuple() with these two activities, and needed bound
						(i += Index(arc.suc, N(true) - 1)) += Index(arc.pred);
						headPT.Count(N(true)).Reset(0);
						tailPT.Count(N(true)).Reset(0);
						for (auto& activity : act) {
							headPT[Index(activity.id)] += activity.head + activity.pt;
							tailPT[Index(activity.id)] += activity.tail + activity.pt;
						}

						BoundTuple(i,
							(act[i[0]].predBits | act[i[1]].predBits |
								act[i[0]].sucBits | act[i[1]].sucBits)
							.SetBit(i[0], true).SetBit(i[1], true),
							resUse, headPT, tailPT, auxBound);
						arcOK = (auxBound >= bound);
					}

					// collect data
					printf("]");
				}
				// give result on using this arc 
				printf(" %s", Icon(arcOK ? EIcon::SOL : EIcon::IMP));
			}
			InitialSetup();
		}
		else if (action.First() == TString("SAT")) { // run SAT with a fixed horizon
			Parametro(HORIZON) = atoi(action[1]) - bestLB;
			ExecutaAlgoritmo();
			if (indicators[IND_RESULTADO] == 2) {
				printf("Horizon %d: %s", atoi(action[1]), Icon(EIcon::IMP));
				bestLB = atoi(action[1]) + 1;
			}
			else if (indicators[IND_RESULTADO] == 1)
				printf("Solution found: %d %s", atoi(action[1]), Icon(EIcon::SUCESSO));
			else
				printf("Time over: %s", Icon(EIcon::INSUC));
		}
		else if (action.First() == TString("Asol")) { // fix A to the current solution 
			// not work since cannot use pred > suc
			for (int pred = 1; pred <= N(); pred++)
				for (int suc = 1; suc <= N(); suc++)
					if (pred != suc && st[pred] + act[pred].pt == st[suc] && !act[pred].sucBits.GetBit(suc))
						ChangePred(act[pred].id, act[suc].id);
			InitialSetup();
		}
		else if (action.First() == TString("init")) { // initial setup, recalculate PRs and LBs
			InitialSetup();
		}
		else
			printf("Available actions:\n"
				"s j s(j) [s(j+1) [s(j+2) ...]]\n"
				"d j d(j) [d(j+1) [d(j+2) ...]]\n"
				"r j r(j,1) [r(j,2) [ ... r(j,R)]]]\n"
				"a a(1) [a(2) [ ... a(R)]]]\n"
				"A i j [k w [ ... ]]\n"
				"Istep bound i j [k w [ ... ]]\n"
				"SAT horizon\n"
				"Asol\n"
				"init\n"
			);
	} while (option != 0);
}

// add/remove extra precedence relations (need to call InitialSetup() after all changes
void CRCPSP::ChangePred(int pred, int suc) {
	// use index
	pred = Index(pred);
	suc = Index(suc, N(true) - 1);
	if (pred > 0 && suc >= 0) {
		if (act[suc].predBits.GetBit(pred)) {
			for (int j = 0; j < insertedPred.Count(); j++)
				if (insertedPred[j].pred == act[pred].id &&
					insertedPred[j].suc == act[suc].id) {
					insertedPred.Delete(j);
					break;
				}
			// are sucessors, remove
			if ((act[suc].pred -= act[pred].id).Empty())
				act[suc].pred += 0;
			if ((act[pred].suc -= act[suc].id).Empty())
				act[pred].suc += N(true) - 1;
		}
		else {
			insertedPred += {act[pred].id, act[suc].id};
			// are not sucessors, add
			act[suc].pred += act[pred].id;
			act[pred].suc += act[suc].id;
		}
	}
}


int CRCPSP::BaseLB(int baseLB, int mode) {
	switch (baseLB) {
	case 1:
		return LBCPM();
	case 2:
		return LBC();
	case 3:
		return LBCPC();
	case 4:
		return LBCS();
	case 5:
		return LBIP();
	case 6:
		return LBIT();
	case 7:
		return LBNodePacking(mode);
	case 8:
		return LBParallelMachine(mode);
	case 9:
		return LBPR();
	case 10:
		return LBCT();
	case 11:
		return LBTP();
	case 12:
		return LBAll(mode);
	}
	return 0;
}

int CRCPSP::BestPR(int mode) {
	TVector<int> best;
	for (int rule = 0; rule <= 13 && (best.Empty() || best.Last() > bestLB); rule++) {
		PriorityRule(rule, mode == 0);
		if (best.Empty() || st.Last() < best.Last())
			best = st;
	}
	st = best;
	return st.Last();
}

int CRCPSP::BaseUB(int basePR, int mode) {
	// only replace schedule if is better
	TVector<int> backup = st;
	if (basePR == 14)
		BestPR(mode);
	else
		PriorityRule(basePR, mode == 0);
	int ub = st.Last();
	if (backup.Last() < ub)
		st = backup;
	return ub;
}

