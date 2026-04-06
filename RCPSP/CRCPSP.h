#pragma once
#include "../CCaDiCaL.h"

typedef struct {
	int pt = 0; // processing time
	TVector<int> res = {}; // resource usage
	TVector<int> pred = {}; // predecessors
	TVector<int> suc = {}; // sucessors
	TBits predBits; // bitset of predecessors for quick checking
} TAtivity;

enum EIndicadoresRCPSP {
	IND_UB = IND_CADICAL,
	IND_LB,
	IND_CALLS,
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
	IND_RC,
	// lower bounds: 
	IND_LB_CPM,
	IND_LB_C,
	IND_LB_CPC,
	IND_LB_CS,
	IND_LB_IP,
	IND_LB_IT,
	IND_LB_NP0,
	IND_LB_NP1,
	IND_LB_NP2,
	IND_LB_PM0,
	IND_LB_PM1,
	IND_LB_PM2,
	IND_LB_PM3,
	IND_LB_PR,
	IND_LB_CT,
	IND_LB_TP,
	IND_LB_ALL
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
	static TVector<TAtivity> act; // activities (including dummy start and end)
	static TVector<int> capacity; // resource capacities

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
	void BestPR();

	// correct id order according to network of predecessors and successors (for scheduling scheme methods)
	void TopologicalSort(TVector<int>& ids);

	/// retorna indicadores após execução
	int64_t Indicador(int id);

protected:
	// Convert to SAT
	TVector<TString> SATConverter(int metodo = 0);

	// extrai e verifica uma solução convertida por um dado método
	bool VerificarSolucao(const TVector<int64_t>& satSol, int metodo = 0);

	bool TamanhoOK(TVector<TString>& cnf);

private:
	// auxiliar variables, to share calculations between methods
	static bool mophologicOK;
	static double indCNC, indOS, indSP, indAD, indLA, indI5, indTF;
	static TVector<TBits> predBits; // bitset of predecessors (direct and indirect)
	static int bestUB, bestLB; // best upper and lower bounds, to use in conversion
	static TVector<int> es, ls; // time windows of SAT conversion
	static int calls; // number of calls to SAT solver

	void MorphologicalIndicators();

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
	int LBC();
	int LBCPC();
	int  LBCS();
	int  LBCT();
	int  LBIP(); 
	int  LBIT();
	int  LBNodePacking(int n);
	int  LBParallelMachine(int m);
	int  LBPR();
	int  LBTP();
	int  LBAll();


	// DebugHSL: clone of function in TProcura, to write to a TString
	TString HSL(float h = -1, float s = 1.0, float l = 0.2, bool fundo = true);


	int VarL(int i, int t) { return Var(TString().printf("l %d %d", i, t)); }
	int VarU(int i, int t) { return Var(TString().printf("u %d %d", i, t)); }

	TVector<TString> AddCOV(TVector<int>& inTW, TVector<int>& inFS, int t);
	TVector<TString> AddMFS(TVector<int> activities, int t);
	TVector<TString> AddBDD(TVector<int> activities, int t);
	TVector<TString> AddADD(TVector<int> activities, int t);

	bool ResourcesViolated(TVector<int>& resUsage);

	// check if activities are incompatible (pairs, triplets, and so on)
	bool ResourceIncompatible(TVector<int> activities);

	// generate a critical path from es/ls data
	TVector<int> CriticalPath(TVector<int> &es, TVector<int> &ls);

	// show resource usage with colors depending on usage
	TString Res2Str(int usage, int resource, bool compact = false);
	// show ID activity in color
	TString Act2Str(int id, int space = 0, int extra = 0);

	int BoundIP(int i, int j, TVector<int>&es, TVector<int>& ls);
	int BoundIT(int i, int j, int l, TVector<int>& es, TVector<int>& ls, int bound);

	TVector<TVector<int>> CompatibleActivities(); // return the compatible activities to each activity

	void PropagateTW(TVector<int>&headPT, TVector<int>&tailPT, int i, int j);
	bool PropagateEFLS(TVector<int>& ef, TVector<int>& ls, int i, int bound);
	void MostraCP(TVector<int> criticalPath);
};