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
    CCaDiCaL() { ResetHashtable(); }
    ~CCaDiCaL() {}

	TVector<int64_t> indicators; ///< Vector to store indicators extracted from the last run.
	TVector<int> defaultCadical; ///< default parameters for CaDiCaL
	// SAT solution, a set of integers
	TVector<int64_t> satSol;
	static TString solver; // caminho para o executável do solver
	
	TVector<TString> variaveis; // nomes das variáveis 
	TVector<TVector<int>> hashtable; // buckets 

	int nextSCID = 0;


	/// Reset existing parameters of CaDiCaL.
    void ResetParametros();
	/// Execute the CaDiCaL with current parameters.
    int ExecutaAlgoritmo();
	/// return the indicator value after execution.
    int64_t Indicador(int id);
	/// @brief  retorna um vetor de inteiros com a codificação da solução (esta codificação será adicionada aos indicadores, no ficheiro CSV de resultados)
	TVector<int64_t> CodificarSolucao();

	/// mostrar a solução satSol
	void MostrarSolucao();

	// IDs e variáveis
	TString Var(int ID);
	int Var(TString var);
	void ResetHashtable(int tamanho = 10007); // tamanho: número primo grande

	// Restrictions (organize and complet later):
	// At least one variable is true
	TVector<TString> AtLeastOne(const TVector<int>& vars);
	// At most one variable is true (quadratic)
	TVector<TString> AtMostOne(const TVector<int>& vars, bool sequentialCounter = false);
	// Exactly one variable is true
	TVector<TString> ExactlyOne(const TVector<int>& vars, bool sequentialCounter = false) {
		return AtLeastOne(vars) += AtMostOne(vars, sequentialCounter);
	}
	// At most K (Sequential Counter) -- add new variables 
	TVector<TString> AtMostK(const TVector<int>& vars, int K = 1);
	// unary code: exactly one variable is true, with O(n) clauses
	TVector<TString> CreateUnaryVar(const TString& prefix, int min, int max);

	// gate = ITE(decision,valueTrue,valueFalse) --- if valueTrue=0 is considered false, if valueFalse=0 is considered true
	TVector<TString> GateITE(int gate, int decision, int valueTrue, int valueFalse);

	// Indicadores
	static int IND_MEMORY;
	static int IND_PROPAGATIONS;
	static int IND_TICKS;
	static int IND_RESTARTS;
	static int IND_LEARNED;
	static int IND_FIXED;
	static int IND_MAXLEVEL;
	static int IND_NUMVARS;
	static int IND_NUMCLAUSES;
	static int IND_CADICAL;
	static int IND_CONFLICTS; // IND_CONFLICTS = IND_ITERACOES


	// Parâmetros CaDiCaL 
	static int LIMIT_DECISIONS;
	static int LOCAL_SEARCH;
	static int PREPROCESSING;
	static int DEFINED_CONFIGURATIONS;
	// all the rest of CaDiCaL parameters
	static int ARENA;        // --arena = bool               allocate clauses in arena[true]
	static int ARENACOMPACT; // --arenacompact = bool        keep clauses compact[true]
	static int ARENASORT;    // --arenasort = bool           sort clauses in arena[true]
	static int ARENATYPE;    // --arenatype = 1..3           1 = clause, 2 = var, 3 = queue[3]
	static int BACKBONE;     // --backbone = 0..2            binary clause backbone[1]
	static int BACKBONEEFFORT; // --backboneeffort = 0..1e5    binary effort in per mile[20]
	static int BACKBONEMAXROUNDS; // --backbonemaxrounds = 0..1e5 backbone rounds limit[1e3]
	static int BACKBONEROUNDS; // --backbonerounds = 0..1e5    backbone rounds limit[100]
	static int BACKBONETHRESH; //--backbonethresh = 0..1e9    delay if ticks smaller thresh* clauses[5]
	static int BINARY;       // --binary = bool              use binary proof format[true]
	static int BLOCK;			// 	--block = bool               blocked clause elimination[false]
	static int 	BLOCKMAXCLSLIM; // 	--blockmaxclslim = 1..2e9    maximum clause size[1e5]
	static int 	BLOCKMINCLSLIM; // 	--blockminclslim = 2..2e9    minimum clause size[2]
	static int BLOCKOCCLIM; // 	--blockocclim = 1..2e9       occurrence limit[1e2]
	static int BUMP;			// 	--bump = bool                bump variables[true]
	static int BUMPREASON;  // 	--bumpreason = bool          bump reason literals too[true]
	static int 	BUMPREASONDEPTH; // 	--bumpreasondepth = 1..3     bump reason depth[1]
	static int BUMPREASONLIMIT; // 	--bumpreasonlimit = 1..2e9   bump reason limit[10]
	static int BUMPREASONRATE; // 	--bumpreasonrate = 1..2e9    bump reason decision rate[100]
	static int CHECK;        //	--check = bool               enable internal checking[false]
	static int CHECKASSUMPTIONS; //--checkassumptions = bool    check assumptions satisfied[true]
	static int CHECKCONSTRAINT;	// 	--checkconstraint = bool     check constraint satisfied[true]
	static int CHECKFAILED;  // 	--checkfailed = bool         check failed literals form core[true]
	static int CHECKFROZEN;  // 	--checkfrozen = bool         check all frozen semantics[false]
	static int CHECKPROOF;   // 	--checkproof = 0..3          1 = drat, 2 = lrat, 3 = both[3]
	static int CHECKWITNESS; // 	--checkwitness = bool        check witness internally[true]
	static int CHRONO;       // 	--chrono = 0..2              chronological backtracking[1]
	static int CHRONOALWAYS; // 	--chronoalways = bool        force always chronological[false]
	static int CHRONOLEVELIM; // 	--chronolevelim = 0..2e9     chronological level limit[1e2]
	static int CHRONOREUSETRAIL; // 	--chronoreusetrail = bool    reuse trail chronologically[true]
	static int COMPACT;      // 	--compact = bool             compact internal variables[true]
	static int COMPACTINT;   // 	--compactint = 1..2e9        compacting interval[2e3]
	static int COMPACTLIM;   // 	--compactlim = 0..1e3        inactive limit per mille[1e2]
	static int COMPACTMIN;   // 	--compactmin = 1..2e9        minimum inactive limit[1e2]
	static int CONDITION;    // 	--condition = bool           globally blocked clause elim[false]
	static int CONDITIONEFFORT; // 	--conditioneffort = 1..1e5   relative efficiency per mille[100]
	static int CONDITIONINT; // 	--conditionint = 1..2e9      initial conflict interval[1e4]
	static int CONDITIONMAXEFF; // 	--conditionmaxeff = 0..2e9   maximum condition efficiency[1e7]
	static int CONDITIONMAXRAT; // 	--conditionmaxrat = 1..2e9   maximum clause variable ratio[100]
	static int CONDITIONMINEFF; // 	--conditionmineff = 0..2e9   minimum condition efficiency[0]
	static int CONGRUENCE;   // 	--congruence = bool          congruence closure[true]
	static int CONGRUENCEAND; // 	--congruenceand = bool       extract AND gates[true]
	static int CONGRUENCEANDARITY; // 	--congruenceandarity = 2..5e7 AND gate arity limit[1e6]
	static int CONGRUENCEBINARIES; // 	--congruencebinaries = bool  extract binary and strengthen ternary clauses[true]
	static int CONGRUENCEITE; // 	--congruenceite = bool       extract ITE gates[true]
	static int CONGRUENCEXOR; // 	--congruencexor = bool       extract XOR gates[true]
	static int CONGRUENCEXORARITY; // 	--congruencexorarity = 2..31 XOR gate arity limit[4]
	static int CONGRUENCEXORCOUNTS; // 	--congruencexorcounts = 1..5e6 XOR gate round[1]
	static int COVER;        // 	--cover = bool               covered clause elimination[false]
	static int COVEREFFORT;  // 	--covereffort = 1..1e5       relative efficiency per mille[4]
	static int COVERMAXCLSLIM; // 	--covermaxclslim = 1..2e9    maximum clause size[1e5]
	static int COVERMAXEFF;  // 	--covermaxeff = 0..2e9       maximum cover efficiency[1e8]
	static int COVERMINCLSLIM; // 	--coverminclslim = 2..2e9    minimum clause size[2]
	static int COVERMINEFF;  // 	--covermineff = 0..2e9       minimum cover efficiency[0]
	static int DECOMPOSE;     //	--decompose = bool           decompose BIG in SCCs and ELS[true]
	static int DECOMPOSEROUNDS; // --decomposerounds = 1..16    number of decompose rounds[2]
	static int DEDUPLICATE;  //	--deduplicate = bool         remove duplicated binaries[true]
	static int DEDUPLICATEALLINIT; //	--deduplicateallinit = bool  remove duplicated clauses once before solving[false]
	static int EAGERSUBSUME;  //	--eagersubsume = bool        subsume recently learned[true]
	static int EAGERSUBSUMELIM; //	--eagersubsumelim = 1..1e3   limit on subsumed candidates[20]
	static int ELIM;         //	--elim = bool                bounded variable elimination[true]
	static int ELIMANDS;      //	--elimands = bool            find AND gates[true]
	static int ELIMBACKWARD;  //	--elimbackward = bool        eager backward subsumption[true]
	static int ELIMBOUNDMAX;  //	--elimboundmax = -1..2e6     maximum elimination bound[16]
	static int ELIMBOUNDMIN;  //	--elimboundmin = -1..2e6     minimum elimination bound[0]
	static int ELIMCLSLIM;    //	--elimclslim = 2..2e9        resolvent size limit[1e2]
	static int ELIMDEF;      //	--elimdef = bool             mine definitions with kitten[false]
	static int ELIMDEFCORES; //	--elimdefcores = 1..100      number of unsat cores[1]
	static int ELIMDEFTICKS;  //	--elimdefticks = 0..2e9      kitten ticks limit[2e5]
	static int ELIMEFFORT;    //	--elimeffort = 1..1e5        relative efficiency per mille[1e3]
	static int ELIMEQUIVS;    //	--elimequivs = bool          find equivalence gates[true]
	static int ELIMINT;      //	--elimint = 1..2e9           elimination interval[2e3]
	static int ELIMITES;     //	--elimites = bool            find if - then - else gates[true]
	static int ELIMLIMITED;  //	--elimlimited = bool         limit resolutions[true]
	static int ELIMMAXEFF;   //	--elimmaxeff = 0..2e9        maximum elimination efficiency[2e9]
	static int ELIMMINEFF;   //	--elimmineff = 0..2e9        minimum elimination efficiency[1e7]
	static int ELIMOCCLIM;   //	--elimocclim = 0..2e9        occurrence limit[1e2]
	static int ELIMPROD;     //	--elimprod = 0..1e4          elim score product weight[1]
	static int ELIMROUNDS;   //	--elimrounds = 1..512        usual number of rounds[2]
	static int ELIMSUBST;    //	--elimsubst = bool           elimination by substitution[true]
	static int ELIMSUM;      //	--elimsum = 0..1e4           elimination score sum weight[1]
	static int ELIMXORLIM;   //	--elimxorlim = 2..27         maximum XOR size[5]
	static int ELIMXORS;     //	--elimxors = bool            find XOR gates[true]
	static int EMADECISIONS; //	--emadecisions = 1..2e9      window decision rate[1e5]
	static int EMAGLUEFAST;  //	--emagluefast = 1..2e9       window fast glue[33]
	static int EMAGLUESLOW;  //	--emaglueslow = 1..2e9       window slow glue[1e5]
	static int EMAJUMP;      //	--emajump = 1..2e9           window back - jump level[1e5]
	static int EMALEVEL;     //	--emalevel = 1..2e9          window back - track level[1e5]
	static int EMASIZE;      //	--emasize = 1..2e9           window learned clause size[1e5]
	static int EMATRAILFAST; //	--ematrailfast = 1..2e9      window fast trail[1e2]
	static int EMATRAILSLOW; //	--ematrailslow = 1..2e9      window slow trail[1e5]
	static int EXTEAGERREASONS; //	--exteagerreasons = bool     eagerly ask for all reasons(0: only when needed)[true]
	static int EXTEAGERRECALC; //	--exteagerrecalc = bool      after eagerly asking for reasons recalculate all levels(0: trust the external tool)[true]
	static int EXTERNALLRAT; //	--externallrat = bool        external lrat[false]
	static int FACTOR;			//	--factor = bool              bounded variable addition[true]
	static int FACTORCANDROUNDS; //--factorcandrounds = 0..2e9  candidates reduction rounds[2]
	static int FACTORCHECK;  //	--factorcheck = 0..2         API checks that variables have been declared(1 = only with factor on; 2 = always)[1]
	static int FACTORDELAY;  //	--factordelay = 0..12        delay bounded variable addition between eliminations[4]
	static int FACTOREFFORT; //	--factoreffort = 0..1e6      relative effort per mille[50]
	static int FACTORINITICKS; //	--factoriniticks = 1..1e6    initial effort in millions[300]
	static int FACTORSIZE;   //	--factorsize = 2..2e9        clause size limit[5]
	static int FACTORTHRESH; //	--factorthresh = 0..100      delay if ticks smaller thresh* clauses[7]
	static int FACTORUNBUMP; //	--factorunbump = bool        extension variable with lowest importance[1:as in kissat][true]
	static int FASTELIM;    //	--fastelim = bool            fast BVE during preprocessing[true]
	static int FASTELIMBOUND; //	--fastelimbound = 1..1e3     fast BVE bound during preprocessing[8]
	static int FASTELIMCLSLIM; //	--fastelimclslim = 2..2e9    fast BVE resolvent size limit[1e2]
	static int FASTELIMOCCLIM; //	--fastelimocclim = 1..2e9    fast BVE occurence limit during preprocessing[100]
	static int FASTELIMROUNDS; //	--fastelimrounds = 1..512    number of fastelim rounds[4]
	static int FLUSH;       //	--flush = bool               flush redundant clauses[false]
	static int FLUSHFACTOR; //	--flushfactor = 1..1e3       interval increase[3]
	static int FLUSHINT;    //	--flushint = 1..2e9          initial limit[1e5]
	static int FORCEPHASE;  //	--forcephase = bool          always use initial phase[false]
	static int FRAT;        //	--frat = 0..2                1 = frat(lrat); 2 = frat(drat)[0]
	static int IDRUP;       //	--idrup = bool               incremental proof format[false]
	static int ILB;         //	--ilb = 0..2                 ILB(incremental lazy backtrack) (0: no; 1 : assumptions only; 2 : everything)[0]
	static int INCDECAY;    //	--incdecay = 0..4            decay clauses when doing incremental clauses[1]
	static int INCDECAYINT; //	--incdecayint = 1..2e9       decay interval when doing incremental clauses[1e6]
	static int INPROBEINT;  //	--inprobeint = 1..2e9        inprobing interval[100]
	static int INPROBING;   //	--inprobing = bool           enable probe inprocessing[true]
	static int INPROCESSING; //	--inprocessing = bool        enable general inprocessing[true]
	static int INSTANTIATE; //	--instantiate = bool         variable instantiation[false]
	static int INSTANTIATECLSLIM; //	--instantiateclslim = 2..2e9 minimum clause size[3]
	static int INSTANTIATEOCCLIM; //	--instantiateocclim = 1..2e9 maximum occurrence limit[1]
	static int INSTANTIATEONCE; //	--instantiateonce = bool     instantiate each clause once[true]
	static int LIDRUP;      //	--lidrup = bool              linear incremental proof format[false]
	static int LRAT;        //	--lrat = bool                use LRAT proof format[false]
	static int LUCKY;       //	--lucky = bool               lucky phases[true]
	static int LUCKYASSUMPTIONS; //	--luckyassumptions = bool    lucky phases with assumptions[true]
	static int LUCKYEARLY;  //	--luckyearly = bool          lucky phases before preprocessing[true]
	static int LUCKYLATE;   //	--luckylate = bool           lucky phases after preprocessing[true]
	static int MINIMIZE;    //	--minimize = bool            minimize learned clauses[true]
	static int MINIMIZEDEPTH; //	--minimizedepth = 0..1e3     minimization depth[1e3]
	static int MINIMIZETICKS; //	--minimizeticks = bool       increment ticks in minimization[true]
	static int OTFS;        //	--otfs = bool                on - the - fly self subsumption[true]
	static int PHASE;       //	--phase = bool               initial phase[true]
	static int PREPROCESSINIT; //	--preprocessinit = 0..2e9    initial preprocessing base limit[2e6]
	static int PREPROCESSLIGHT; //	--preprocesslight = bool     lightweight preprocessing[true]
	static int PROBE;       //	--probe = bool               failed literal probing[true]
	static int PROBEEFFORT; //	--probeeffort = 1..1e5       relative efficiency per mille[8]
	static int PROBEHBR;    //	--probehbr = bool            learn hyper binary clauses[true]
	static int PROBETHRESH; //	--probethresh = 0..100       delay if ticks smaller thresh* clauses[0]
	static int PROFILE;     //	--profile = 0..4             profiling level[2]
	static int QUIET;       //	--quiet = bool               disable all messages[false]
	static int RADIXSORTLIM; //	--radixsortlim = 0..2e9      radix sort limit[32]
	static int RANDEC;      //	--randec = bool              random decisions[false]
	static int RANDECFOCUSED; //	--randecfocused = bool       random decisions in focused mode[true]
	static int RANDECINIT;  //	--randecinit = 2..2e9        inital random decision interval[1e3]
	static int RANDECINT;   //	--randecint = 0..2e9         random conflict length[500]
	static int RANDECLENGTH; //	--randeclength = 1..1e9      length random decisions phases[10]
	static int RANDECSTABLE; //	--randecstable = bool        random decisions in stable mode[false]
	static int REALTIME;    //	--realtime = bool            real instead of process time[false]
	static int RECOMPUTETIER; //	--recomputetier = bool       recompute tiers[true]
	static int REDUCE;      //	--reduce = bool              reduce useless clauses[true]
	static int REDUCEINIT;  //	--reduceinit = 1..1e6        initial interval[300]
	static int REDUCEINT;   //	--reduceint = 2..1e6         reduce interval[25]
	static int REDUCEOPT;   //	--reduceopt = 0..2           0 = prct; 1 = sqrt; 2 = max[1]
	static int REDUCETARGET; //	--reducetarget = 10..1e2     reduce fraction in percent[75]
	static int REDUCETIER1GLUE; //	--reducetier1glue = 1..2e9   glue of kept learned clauses[2]
	static int REDUCETIER2GLUE; //	--reducetier2glue = 1..2e9   glue of tier two clauses[6]
	static int RELUCTANT;   //	--reluctant = bool           stable reluctant doubling restarts[true]
	static int RELUCTANTINT; //	--reluctantint = 0..2e9      reluctant doubling period[1024]
	static int RELUCTANTMAX; //	--reluctantmax = 0..2e9      maximum reluctant doubling period[1048576]
	static int REPHASE;     //	--rephase = 0..2             enable resetting phase(0 = no; 1 = always; 2 = stable - only)[1]
	static int REPHASEINT;  //	--rephaseint = 1..2e9        rephase interval[1e3]
	static int REPORT;      //	--report = bool              enable reporting[false]
	static int REPORTALL;   //	--reportall = bool           report even if not successful[false]
	static int REPORTSOLVE; //	--reportsolve = bool         use solving not process time[false]
	static int RESTART;     //	--restart = bool             enable restarts[true]
	static int RESTARTINT;  //	--restartint = 1..2e9        restart interval[2]
	static int RESTARTMARGINFOCUSED; //	--restartmarginfocused = 0..25 focused slow fast margin in percent[10]
	static int RESTARTMARGINSTABLE; //	--restartmarginstable = 0..25 stable slow fast margin in percent[25]
	static int RESTARTREUSETRAIL; //	--restartreusetrail = bool   enable trail reuse[true]
	static int RESTOREALL;  //	--restoreall = 0..2          restore all clauses(2 = really)[0]
	static int RESTOREFLUSH; //	--restoreflush = bool        remove satisfied clauses[false]
	static int REVERSE;     //	--reverse = bool             reverse variable ordering[false]
	static int SCORE;       //	--score = bool               use EVSIDS scores[true]
	static int SCOREFACTOR;   //	--scorefactor = 500..1e3     score factor per mille[950]
	static int SHRINK;      //	--shrink = 0..3              shrink conflict clause(1 = binary - only; 2 = minimize - on - pulling; 3 = full)[3]
	static int SHRINKREAP;   //	--shrinkreap = bool          use a reap for shrinking[true]
	static int SHUFFLE;      //	--shuffle = bool             shuffle variables[false]
	static int SHUFFLEQUEUE;  //	--shufflequeue = bool        shuffle variable queue[true]
	static int SHUFFLERANDOM; //	--shufflerandom = bool       not reverse but random[false]
	static int SHUFFLESCORES; //	--shufflescores = bool       shuffle variable scores[true]
	static int STABILIZE;    //	--stabilize = bool           enable stabilizing phases[true]
	static int STABILIZEINIT; //	--stabilizeinit = 1..2e9     stabilizing interval[1e3]
	static int STABILIZEONLY;  //	--stabilizeonly = bool       only stabilizing phases[false]
	static int STATS;        //	--stats = bool               print all statistics at the end of the run[false]
	static int STUBBORNIOFOCUSED; //	--stubbornIOfocused = bool   force phases to I / O in focused mode every once in a while (requires rephase == 2)[false]
	static int SUBSUME;      //	--subsume = bool             enable clause subsumption[true]
	static int SUBSUMEBINLIM; //	--subsumebinlim = 0..2e9     watch list length limit[1e4]
	static int SUBSUMECLSLIM; //	--subsumeclslim = 0..2e9     clause length limit[1e2]
	static int SUBSUMEEFFORT; //	--subsumeeffort = 1..1e5     relative efficiency per mille[1e3]
	static int SUBSUMELIMITED; //	--subsumelimited = bool      limit subsumption checks[true]
	static int SUBSUMEMAXEFF; //	--subsumemaxeff = 0..2e9     maximum subsuming efficiency[1e8]
	static int SUBSUMEMINEFF; //	--subsumemineff = 0..2e9     minimum subsuming efficiency[0]
	static int SUBSUMEOCCLIM; //	--subsumeocclim = 0..2e9     watch list length limit[1e2]
	static int SUBSUMESTR;   //	--subsumestr = bool          subsume strengthen[true]
	static int SWEEP;        //	--sweep = bool               enable SAT sweeping[true]
	static int SWEEPCLAUSES; //	--sweepclauses = 0..2e9      environment clauses[1024]
	static int SWEEPCOMPLETE; //	--sweepcomplete = bool       run SAT sweeping to completion[false]
	static int SWEEPCOUNTBINARY; //	--sweepcountbinary = bool    count binaries to environment[true]
	static int SWEEPDEPTH;   //	--sweepdepth = 0..2e9        environment depth[2]
	static int SWEEPEFFORT;  //	--sweepeffort = 0..1e4       relative effort in ticks per mille[1e2]
	static int SWEEPFLIPROUNDS; //	--sweepfliprounds = 0..2e9   flipping rounds[1]
	static int SWEEPMAXCLAUSES; //	--sweepmaxclauses = 2..2e9   maximum environment clauses[3e5]
	static int SWEEPMAXDEPTH; //	--sweepmaxdepth = 1..2e9     maximum environment depth[3]
	static int SWEEPMAXVARS; //	--sweepmaxvars = 2..2e9      maximum environment variables[8192]
	static int SWEEPRAND;    //	--sweeprand = bool           randomize sweeping environment[false]
	static int SWEEPTHRESH;  //	--sweepthresh = 0..100       delay if ticks smaller thresh* clauses[5]
	static int SWEEPVARS;    //	--sweepvars = 0..2e9         environment variables[256]
	static int TARGET;       //	--target = 0..2              target phases(1 = stable only)[1]
	static int TERMINATEINT; //	--terminateint = 0..1e4      termination check interval[10]
	static int TERNARY;      //	--ternary = bool             hyper ternary resolution[true]
	static int TERNARYEFFORT; //	--ternaryeffort = 1..1e5     relative efficiency per mille[8]
	static int TERNARYMAXADD; //	--ternarymaxadd = 0..1e4     max clauses added in percent[1e3]
	static int TERNARYOCCLIM; //	--ternaryocclim = 1..2e9     ternary occurrence limit[1e2]
	static int TERNARYROUNDS; //	--ternaryrounds = 1..16      maximum ternary rounds[2]
	static int TERNARYTHRESH; //	--ternarythresh = 0..100     delay if ticks smaller thresh* clauses[6]
	static int TIER1LIMIT;   //	--tier1limit = 0..100        limit of tier1 usage in percentage[50]
	static int TIER1MINGLUE; //	--tier1minglue = 0..100      lowest tier1 limit[0]
	static int TIER2LIMIT;   //	--tier2limit = 0..100        limit of tier2 usage in percentage[90]
	static int TIER2MINGLUE; //	--tier2minglue = 0..100      lowest tier2 limit[0]
	static int TRANSRED;     //	--transred = bool            transitive reduction of BIG[true]
	static int TRANSREDEFFORT; //	--transredeffort = 1..1e5    relative efficiency per mille[1e2]
	static int TRANSREDMAXEFF; //	--transredmaxeff = 0..2e9    maximum
	static int TRANSREDMINEFF; //	--transredmineff = 0..2e9    minimum efficiency[0]
	static int VERBOSE;     //	--verbose = 0..3             more verbose messages[0]              
	static int VERIPB;      //	--veripb = 0..4              odd = check - deletions; > 2 drat[0]
	static int VIVIFY;      //	--vivify = bool              vivification[true]
	static int VIVIFYCALCTIER; //	--vivifycalctier = bool      recalculate tier limits[false]
	static int VIVIFYDEMOTE; //	--vivifydemote = bool        demote irredundant or delete directly[false]
	static int VIVIFYEFFORT; //	--vivifyeffort = 0..1e5      overall efficiency per mille[50]
	static int VIVIFYFLUSH; //	--vivifyflush = bool         flush subsumed before vivification rounds[true]
	static int VIVIFYINST;  //	--vivifyinst = bool          instantiate last literal when vivify[true]
	static int VIVIFYIRRED; //	--vivifyirred = bool         vivification of irredundant clauses[true]
	static int VIVIFYIRREDEFF; //	--vivifyirredeff = 1..100    irredundant efficiency per mille[3]
	static int VIVIFYONCE;  //	--vivifyonce = 0..2          vivify once : 1 = red; 2 = red + irr[0]
	static int VIVIFYRETRY; //	--vivifyretry = 0..5         re - vivify clause if vivify was successful[0]
	static int VIVIFYSCHEDMAX; //	--vivifyschedmax = 10..2e9   maximum schedule size[5e3]
	static int VIVIFYTHRESH; //	--vivifythresh = 0..100      delay if ticks smaller thresh* clauses[20]
	static int VIVIFYTIER1; //	--vivifytier1 = bool         vivification tier1[true]
	static int VIVIFYTIER1EFF; //	--vivifytier1eff = 0..100    relative tier1 effort[4]
	static int VIVIFYTIER2; //	--vivifytier2 = bool         vivification tier2[true]
	static int VIVIFYTIER2EFF; //	--vivifytier2eff = 1..100    relative tier2 effort[2]
	static int VIVIFYTIER3; //	--vivifytier3 = bool         vivification tier3[true]
	static int VIVIFYTIER3EFF; //	--vivifytier3eff = 1..100    relative tier3 effort[1]
	static int WALK;        //	--walk = bool                enable random walks[true]
	static int WALKEFFORT;  //	--walkeffort = 1..1e5        relative efficiency per mille[80]
	static int WALKFULLOCC; //	--walkfullocc = bool         use Kissat's full occurrences instead of the single watched [false]
	static int WALKMAXEFF;  //	--walkmaxeff = 0..2e9        maximum efficiency(in 1e3 ticks)[1e7]
	static int WALKMINEFF;  //	--walkmineff = 0..1e7        minimum efficiency[0]
	static int WALKNONSTABLE; //	--walknonstable = bool       walk in non - stabilizing phase[true]
	static int WALKREDUNDANT; //	--walkredundant = bool       walk redundant clauses too[false]
	static int WARMUP;      //	--warmup = bool              warmup before walk using propagation[true]

};

