#pragma once
#define _CRT_SECURE_NO_WARNINGS
#include "../CCaDiCaL.h"


typedef struct SActivity {
	int id; // id - original activity id (0 based)
	int pt = 0; // processing time
	TVector<int> res = {}; // resource usage
	TVector<int> pred = {}; // predecessors (direct), original id
	TVector<int> suc = {}; // sucessors (direct), original id
	// auxiliar data
	int index; // current index of activity in this position
	TBits predBits; // bitset of predecessors, index with direct/indirect
	TBits sucBits; // bitset of sucessors, index with direct/indirect
	int head, tail; // head and tail of activity, from beginning and until the end
	int pLevel; // progressive level of activity
} TActivity;

typedef struct { int pred, suc; } TPredecessor;
typedef struct { TBits activities; int bestLB; } TSubNets;

enum EIndicadoresRCPSP {
	IND_UB = IND_CADICAL, // final UB
	IND_LB, // final LB
	IND_CALLS,
	IND_BASE_LB, // Base LBs - set initial LB
	IND_PR, // PRs - set initial UB
	IND_N,
	// project indicators
	IND_CNC,
	IND_OS,
	IND_SP,
	IND_AD,
	IND_LA,
	IND_I5,
	IND_TF,
	IND_WPREC,
	IND_WALL,
	IND_FS21,
	IND_FS22,
	IND_FS31,
	IND_FS32,
	IND_RF,
	IND_RU,
	IND_RS,
	IND_RC
};


/**
 * @class CRCPSP
 * @brief Class for converting RCPSP to SAT and solving it with CaDiCaL.
 */
class CRCPSP : public CCaDiCaL
{
public:
	CRCPSP() {}
	~CRCPSP() {}

	// data for the instance
	TVector<TActivity> act; // activities (including dummy start and end)
	TVector<int> capacity; // resource capacities
	TVector<TPredecessor> insertedPred; // predecessors inserted
	TVector<int> index = {}; // if not empty, should be used to access IDs


	// solution representation: start time of each activity
	TVector<int> st;

	bool conversaoOK = false;

	/// Reset existing parameters of RCPSP.
	void ResetParametros();
	/// Convert to SAT, execute CaDiCaL, extract solution, check.
	int ExecutaAlgoritmo();

	/// show solution in a human-readable format (for debugging)
	void MostrarSolucao();

	// show the instance data (for debugging)
	void Debug(bool completo);

	// load instance from file, and initialize variables
	void Inicializar(void);

	// scheduling scheme methods (to implement priority rules, needed in conversion)
	void Serial(TVector<int> ids);
	void Parallel(TVector<int> ids);
	// calculate es and ls of activities for a given makespan
	void CPM(TVector<int>& es, TVector<int>* ls = NULL, int makespan = -1);
	// set current solution to a priority rule
	void PriorityRule(int rule, bool serial = true);
	int BestPR(int mode);
	int BaseLB(int baseLB, int mode);
	int BaseUB(int basePR, int mode);

	// correct id order according to network of predecessors and successors (for scheduling scheme methods)
	void TopologicalSort(TVector<int>& ids);

	/// retorna indicadores após execução
	int64_t Indicador(int id);

	// explorar manualmente os dados:
	// - mostrar instância e solução
	// - permitir editar instância e solução
	void Explorar();

	// update auxiliar datastructure
	void InitialSetup();

	// access to an ID, or dummy if ID does not exit
	int Index(int originalID, int dummy = 0) {
		if (index.Empty() && originalID >= 0 && originalID < N(true))
			return act[originalID].index;
		else if (originalID >= 0 && originalID < index.Count())
			return index[originalID] >= 0 ? index[originalID] : dummy;
		return dummy;
	}
	int ES(TActivity& activity) { return act[Index(activity.id)].head; }
	int LF(TActivity& activity, int horizon = 0) { return (horizon == 0 ? bestLB : horizon) - act[Index(activity.id)].tail; }
	int EF(TActivity& activity) { return ES(activity) + act[Index(activity.id)].pt; }
	int LS(TActivity& activity, int horizon = 0) { return LF(activity, horizon) - act[Index(activity.id)].pt; }

	// clone the project with just the selected activties
	CRCPSP* Clone(TBits activities);

protected:
	// Convert to SAT
	TVector<TString> SATConverter(int metodo = 0);

	// extract solution
	void ExtrairSolucao(const TVector<int64_t>& satSol, int metodo = 0);

	// verifica uma solução convertida por um dado método
	bool VerificarSolucao();

	bool TamanhoOK(TVector<TString>& cnf);

private:
	// auxiliar variables, to share calculations between methods
	bool mophologicOK;
	double indCNC, indOS, indSP, indAD, indLA, indI5, indTF;
	int bestUB, bestLB; // best upper and lower bounds, to use in conversion
	int cpm; // critical path lower bound
	int calls; // number of calls to SAT solver

	void MorphologicalIndicators(bool duration = false);

	// activity count
	int N(bool dummy = false) { return act.Count() - (dummy ? 0 : 2); }
	// resource count
	int R() { return capacity.Count(); }


	// project indicators:
	double RF();
	double RU();
	double RS();
	double RC();
	double W(bool prec = true); // WPREC, WALL
	double FS(int x, int u); // FS21, FS22, FS31, FS32

	// lower bounds
	int LBCPM();
	int LBC(TBits* sel = NULL);
	int LBCPC();
	int LBCS();
	int LBCT();
	int LBIP();
	int LBIT();
	int LBNodePacking(int n);
	int LBParallelMachine(int m);
	int LBPR();
	int LBTP();
	int LBAll(int mode);
	int LBItuples(); // lower bound that detect any size resource incompatible tuples (2, 3, ... N)
	void BoundTuple(TVector<int>& i, TBits predOK, TVector<int> resUsage,
		TVector<int>& headPT, TVector<int>& tailPT, int& bound, int bestBound = -1);
	bool BoundPRTuple(TVector<int>& i, TBits predOK, TVector<int> resUsage,
		TVector<int>& headPT, TVector<int>& tailPT, int& bound, int& tw, int weakPairs = 0, int bestBound = -1);
	int LBPRtuples();

	int LBcpmCapacity(TVector<int>& pt);

	// DebugHSL: clone of function in TProcura, to write to a TString
	TString HSL(float h = -1, float s = 1.0, float l = 0.2, bool fundo = true);


	int VarL(int i, int t) { return Var(TString().printf("l %d %d", i, t)); }
	int VarU(int i, int t) { return Var(TString().printf("u %d %d", i, t)); }
	int VarITE(int level, int k, int t, int available) { // IFE(level,resource,t,available)
		return Var(TString().printf("ite %d %d %d %d", level, k, t, available));
	}

	TVector<TString> AddCOV(TVector<int>& inTW, TVector<int>& inFS, int t);
	TVector<TString> AddMFS(TVector<int>& inTW, TVector<int>& inFS, TBits& inFSb, int t);
	TVector<TString> AddBDD(TVector<int>& inTW, TVector<int>& inFS, int t, int resource);
	TVector<TString> AddADD(TVector<int> activities, int t);

	bool ResourcesViolated(TVector<int>& resUsage);

	// check if activities are incompatible (pairs, triplets, and so on)
	bool ResourceIncompatible(TVector<int> activities);

	// generate a critical path from es/ls data
	TVector<int> CriticalPath(TVector<int>& es, TVector<int>& ls, int& holes);

	// show resource usage with colors depending on usage
	TString Res2Str(int usage, int resource, bool compact = false);
	// show ID activity in color
	TString Act2Str(int id, int space = 0, int extra = 0);
	TString PT2Str(int pt, bool headTail = false);

	TVector<TVector<int>> CompatibleActivities(); // return the compatible activities to each activity

	void PropagateTW(TVector<int>& headPT, TVector<int>& tailPT, int i, int j);
	bool PropagateEFLS(TVector<int>& ef, TVector<int>& ls, int i, int bound);
	void MostraCP(TVector<int> criticalPath);

	// change the order of activities to PL and by ID, to get all predecessors index before sucessor index
	void ActivityOrder();
	void UpdateIndex(); // update index with current activity order, to use Index()
	void CleanResources(); // delete resources with sumRU<=capacity + apply resource waste



	// feasible sets 
	// mode - 0 - only precedence relations, 1 - only resources, 2 - precedence relations and resources
	//        3 - resource waste procedure (identify maximal resource usage of an activity in all FS, 
	//            if maximal resource usage is less than capacity, add this to the activity
	//       -1 - count only combinations
	// max - size of the greatest compatible set (if limit is not set)
	// limit - if set, is the limit on the size of the feasible sets, counting only these ones
	// return value is the number of calls (feasible sets of any size)
	int64_t FeasibleSets(int mode, int& max, int limit = -1, TVector<int>* i = NULL, TBits incompatible = {}, TVector<int> resUse = {});
	void DebugFeasibleSet(int mode, TVector<int>& i, int value);
	bool ShowValue(int value);

	// add/remove extra precedence relations (need to call InitialSetup() after all changes
	void ChangePred(int pred, int suc);

	bool LBHead(TBits predBits) {
		int count = 0;
		for (int i = 1; i <= N() && count <= 1; i++)
			if (predBits.GetBit(i))
				count++;
		return count > 1;
	}
	bool LBTail(TBits sucBits) {
		int count = 0;
		for (int i = 1; i <= N() && count <= 1; i++)
			if (sucBits.GetBit(i))
				count++;
		return count > 1;
	}

	void LoadRCPSP(TVector<TString> lines);
	void LoadBinpacking(TVector<TString> lines, int instanceID);

	TBits OriginalBits(TBits &bits); // convert bits in orignal ID bits

	static TVector<TVector<TSubNets>> subNets; // store the best bounds of subnets of activities
	static void ResetSubNets() { 
		subNets.Count(1000);  // create hashtable with default size
		for (auto& bucket : subNets)
			bucket.Count(0);
	}
	static TSubNets& GetSubNet(TBits activities) {
		int index = activities.Hash() % subNets.Count();
		for (auto& subNet : subNets[index])
			if (subNet.activities == activities) 
				return subNet;
		subNets[index] += { activities, -1 };
		return subNets[index].Last();
	}

	void Debug(TSubNets& subNet, int id, int prevBound, bool reused = false);
};