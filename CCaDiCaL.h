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
};


enum EParametrosCaDiCaL {
	LIMIT_DECISIONS = PARAMETROS_PROCURA,
	LOCAL_SEARCH,
	PREPROCESSING,
	DEFINED_CONFIGURATIONS,
	// all the rest of CaDiCaL parameters

	ARENA,        // --arena = bool               allocate clauses in arena[true]
	ARENACOMPACT, // --arenacompact = bool        keep clauses compact[true]
	ARENASORT,    // --arenasort = bool           sort clauses in arena[true]
	ARENATYPE,    // --arenatype = 1..3           1 = clause, 2 = var, 3 = queue[3]
	BACKBONE,     // --backbone = 0..2            binary clause backbone[1]
	BACKBONEEFFORT, // --backboneeffort = 0..1e5    binary effort in per mile[20]
	BACKBONEMAXROUNDS, // --backbonemaxrounds = 0..1e5 backbone rounds limit[1e3]
	BACKBONEROUNDS, // --backbonerounds = 0..1e5    backbone rounds limit[100]
	BACKBONETHRESH, //--backbonethresh = 0..1e9    delay if ticks smaller thresh* clauses[5]
	BINARY,       // --binary = bool              use binary proof format[true]
	BLOCK,			// 	--block = bool               blocked clause elimination[false]
	BLOCKMAXCLSLIM, // 	--blockmaxclslim = 1..2e9    maximum clause size[1e5]
	BLOCKMINCLSLIM, // 	--blockminclslim = 2..2e9    minimum clause size[2]
	BLOCKOCCLIM, // 	--blockocclim = 1..2e9       occurrence limit[1e2]
	BUMP,			// 	--bump = bool                bump variables[true]
	BUMPREASON,  // 	--bumpreason = bool          bump reason literals too[true]
	BUMPREASONDEPTH, // 	--bumpreasondepth = 1..3     bump reason depth[1]
	BUMPREASONLIMIT, // 	--bumpreasonlimit = 1..2e9   bump reason limit[10]
	BUMPREASONRATE, // 	--bumpreasonrate = 1..2e9    bump reason decision rate[100]
	CHECK,        //	--check = bool               enable internal checking[false]
	CHECKASSUMPTIONS, //--checkassumptions = bool    check assumptions satisfied[true]
	CHECKCONSTRAINT,	// 	--checkconstraint = bool     check constraint satisfied[true]
	CHECKFAILED,  // 	--checkfailed = bool         check failed literals form core[true]
	CHECKFROZEN,  // 	--checkfrozen = bool         check all frozen semantics[false]
	CHECKPROOF,   // 	--checkproof = 0..3          1 = drat, 2 = lrat, 3 = both[3]
	CHECKWITNESS, // 	--checkwitness = bool        check witness internally[true]
	CHRONO,       // 	--chrono = 0..2              chronological backtracking[1]
	CHRONOALWAYS, // 	--chronoalways = bool        force always chronological[false]
	CHRONOLEVELIM, // 	--chronolevelim = 0..2e9     chronological level limit[1e2]
	CHRONOREUSETRAIL, // 	--chronoreusetrail = bool    reuse trail chronologically[true]
	COMPACT,      // 	--compact = bool             compact internal variables[true]
	COMPACTINT,   // 	--compactint = 1..2e9        compacting interval[2e3]
	COMPACTLIM,   // 	--compactlim = 0..1e3        inactive limit per mille[1e2]
	COMPACTMIN,   // 	--compactmin = 1..2e9        minimum inactive limit[1e2]
	CONDITION,    // 	--condition = bool           globally blocked clause elim[false]
	CONDITIONEFFORT, // 	--conditioneffort = 1..1e5   relative efficiency per mille[100]
	CONDITIONINT, // 	--conditionint = 1..2e9      initial conflict interval[1e4]
	CONDITIONMAXEFF, // 	--conditionmaxeff = 0..2e9   maximum condition efficiency[1e7]
	CONDITIONMAXRAT, // 	--conditionmaxrat = 1..2e9   maximum clause variable ratio[100]
	CONDITIONMINEFF, // 	--conditionmineff = 0..2e9   minimum condition efficiency[0]
	CONGRUENCE,   // 	--congruence = bool          congruence closure[true]
	CONGRUENCEAND, // 	--congruenceand = bool       extract AND gates[true]
	CONGRUENCEANDARITY, // 	--congruenceandarity = 2..5e7 AND gate arity limit[1e6]
	CONGRUENCEBINARIES, // 	--congruencebinaries = bool  extract binary and strengthen ternary clauses[true]
	CONGRUENCEITE, // 	--congruenceite = bool       extract ITE gates[true]
	CONGRUENCEXOR, // 	--congruencexor = bool       extract XOR gates[true]
	CONGRUENCEXORARITY, // 	--congruencexorarity = 2..31 XOR gate arity limit[4]
	CONGRUENCEXORCOUNTS, // 	--congruencexorcounts = 1..5e6 XOR gate round[1]
	COVER,        // 	--cover = bool               covered clause elimination[false]
	COVEREFFORT,  // 	--covereffort = 1..1e5       relative efficiency per mille[4]
	COVERMAXCLSLIM, // 	--covermaxclslim = 1..2e9    maximum clause size[1e5]
	COVERMAXEFF,  // 	--covermaxeff = 0..2e9       maximum cover efficiency[1e8]
	COVERMINCLSLIM, // 	--coverminclslim = 2..2e9    minimum clause size[2]
	COVERMINEFF,  // 	--covermineff = 0..2e9       minimum cover efficiency[0]
	DECOMPOSE,     //	--decompose = bool           decompose BIG in SCCs and ELS[true]
	DECOMPOSEROUNDS, // --decomposerounds = 1..16    number of decompose rounds[2]
	DEDUPLICATE,  //	--deduplicate = bool         remove duplicated binaries[true]
	DEDUPLICATEALLINIT, //	--deduplicateallinit = bool  remove duplicated clauses once before solving[false]
	EAGERSUBSUME,  //	--eagersubsume = bool        subsume recently learned[true]
	EAGERSUBSUMELIM, //	--eagersubsumelim = 1..1e3   limit on subsumed candidates[20]
	ELIM,         //	--elim = bool                bounded variable elimination[true]
	ELIMANDS,      //	--elimands = bool            find AND gates[true]
	ELIMBACKWARD,  //	--elimbackward = bool        eager backward subsumption[true]
	ELIMBOUNDMAX,  //	--elimboundmax = -1..2e6     maximum elimination bound[16]
	ELIMBOUNDMIN,  //	--elimboundmin = -1..2e6     minimum elimination bound[0]
	ELIMCLSLIM,    //	--elimclslim = 2..2e9        resolvent size limit[1e2]
	ELIMDEF,      //	--elimdef = bool             mine definitions with kitten[false]
	ELIMDEFCORES, //	--elimdefcores = 1..100      number of unsat cores[1]
	ELIMDEFTICKS,  //	--elimdefticks = 0..2e9      kitten ticks limit[2e5]
	ELIMEFFORT,    //	--elimeffort = 1..1e5        relative efficiency per mille[1e3]
	ELIMEQUIVS,    //	--elimequivs = bool          find equivalence gates[true]
	ELIMINT,      //	--elimint = 1..2e9           elimination interval[2e3]
	ELIMITES,     //	--elimites = bool            find if - then - else gates[true]
	ELIMLIMITED,  //	--elimlimited = bool         limit resolutions[true]
	ELIMMAXEFF,   //	--elimmaxeff = 0..2e9        maximum elimination efficiency[2e9]
	ELIMMINEFF,   //	--elimmineff = 0..2e9        minimum elimination efficiency[1e7]
	ELIMOCCLIM,   //	--elimocclim = 0..2e9        occurrence limit[1e2]
	ELIMPROD,     //	--elimprod = 0..1e4          elim score product weight[1]
	ELIMROUNDS,   //	--elimrounds = 1..512        usual number of rounds[2]
	ELIMSUBST,    //	--elimsubst = bool           elimination by substitution[true]
	ELIMSUM,      //	--elimsum = 0..1e4           elimination score sum weight[1]
	ELIMXORLIM,   //	--elimxorlim = 2..27         maximum XOR size[5]
	ELIMXORS,     //	--elimxors = bool            find XOR gates[true]
	EMADECISIONS, //	--emadecisions = 1..2e9      window decision rate[1e5]
	EMAGLUEFAST,  //	--emagluefast = 1..2e9       window fast glue[33]
	EMAGLUESLOW,  //	--emaglueslow = 1..2e9       window slow glue[1e5]
	EMAJUMP,      //	--emajump = 1..2e9           window back - jump level[1e5]
	EMALEVEL,     //	--emalevel = 1..2e9          window back - track level[1e5]
	EMASIZE,      //	--emasize = 1..2e9           window learned clause size[1e5]
	EMATRAILFAST, //	--ematrailfast = 1..2e9      window fast trail[1e2]
	EMATRAILSLOW, //	--ematrailslow = 1..2e9      window slow trail[1e5]
	EXTEAGERREASONS, //	--exteagerreasons = bool     eagerly ask for all reasons(0: only when needed)[true]
	EXTEAGERRECALC, //	--exteagerrecalc = bool      after eagerly asking for reasons recalculate all levels(0: trust the external tool)[true]
	EXTERNALLRAT, //	--externallrat = bool        external lrat[false]
	FACTOR,			//	--factor = bool              bounded variable addition[true]
	FACTORCANDROUNDS, //--factorcandrounds = 0..2e9  candidates reduction rounds[2]
	FACTORCHECK,  //	--factorcheck = 0..2         API checks that variables have been declared(1 = only with factor on, 2 = always)[1]
	FACTORDELAY,  //	--factordelay = 0..12        delay bounded variable addition between eliminations[4]
	FACTOREFFORT, //	--factoreffort = 0..1e6      relative effort per mille[50]
	FACTORINITICKS, //	--factoriniticks = 1..1e6    initial effort in millions[300]
	FACTORSIZE,   //	--factorsize = 2..2e9        clause size limit[5]
	FACTORTHRESH, //	--factorthresh = 0..100      delay if ticks smaller thresh* clauses[7]
	FACTORUNBUMP, //	--factorunbump = bool        extension variable with lowest importance[1:as in kissat][true]
	FASTELIM,    //	--fastelim = bool            fast BVE during preprocessing[true]
	FASTELIMBOUND, //	--fastelimbound = 1..1e3     fast BVE bound during preprocessing[8]
	FASTELIMCLSLIM, //	--fastelimclslim = 2..2e9    fast BVE resolvent size limit[1e2]
	FASTELIMOCCLIM, //	--fastelimocclim = 1..2e9    fast BVE occurence limit during preprocessing[100]
	FASTELIMROUNDS, //	--fastelimrounds = 1..512    number of fastelim rounds[4]
	FLUSH,       //	--flush = bool               flush redundant clauses[false]
	FLUSHFACTOR, //	--flushfactor = 1..1e3       interval increase[3]
	FLUSHINT,    //	--flushint = 1..2e9          initial limit[1e5]
	FORCEPHASE,  //	--forcephase = bool          always use initial phase[false]
	FRAT,        //	--frat = 0..2                1 = frat(lrat), 2 = frat(drat)[0]
	IDRUP,       //	--idrup = bool               incremental proof format[false]
	ILB,         //	--ilb = 0..2                 ILB(incremental lazy backtrack) (0: no, 1 : assumptions only, 2 : everything)[0]
	INCDECAY,    //	--incdecay = 0..4            decay clauses when doing incremental clauses[1]
	INCDECAYINT, //	--incdecayint = 1..2e9       decay interval when doing incremental clauses[1e6]
	INPROBEINT,  //	--inprobeint = 1..2e9        inprobing interval[100]
	INPROBING,   //	--inprobing = bool           enable probe inprocessing[true]
	INPROCESSING, //	--inprocessing = bool        enable general inprocessing[true]
	INSTANTIATE, //	--instantiate = bool         variable instantiation[false]
	INSTANTIATECLSLIM, //	--instantiateclslim = 2..2e9 minimum clause size[3]
	INSTANTIATEOCCLIM, //	--instantiateocclim = 1..2e9 maximum occurrence limit[1]
	INSTANTIATEONCE, //	--instantiateonce = bool     instantiate each clause once[true]
	LIDRUP,      //	--lidrup = bool              linear incremental proof format[false]
	LRAT,        //	--lrat = bool                use LRAT proof format[false]
	LUCKY,       //	--lucky = bool               lucky phases[true]
	LUCKYASSUMPTIONS, //	--luckyassumptions = bool    lucky phases with assumptions[true]
	LUCKYEARLY,  //	--luckyearly = bool          lucky phases before preprocessing[true]
	LUCKYLATE,   //	--luckylate = bool           lucky phases after preprocessing[true]
	MINIMIZE,    //	--minimize = bool            minimize learned clauses[true]
	MINIMIZEDEPTH, //	--minimizedepth = 0..1e3     minimization depth[1e3]
	MINIMIZETICKS, //	--minimizeticks = bool       increment ticks in minimization[true]
	OTFS,        //	--otfs = bool                on - the - fly self subsumption[true]
	PHASE,       //	--phase = bool               initial phase[true]
	PREPROCESSINIT, //	--preprocessinit = 0..2e9    initial preprocessing base limit[2e6]
	PREPROCESSLIGHT, //	--preprocesslight = bool     lightweight preprocessing[true]
	PROBE,       //	--probe = bool               failed literal probing[true]
	PROBEEFFORT, //	--probeeffort = 1..1e5       relative efficiency per mille[8]
	PROBEHBR,    //	--probehbr = bool            learn hyper binary clauses[true]
	PROBETHRESH, //	--probethresh = 0..100       delay if ticks smaller thresh* clauses[0]
	PROFILE,     //	--profile = 0..4             profiling level[2]
	QUIET,       //	--quiet = bool               disable all messages[false]
	RADIXSORTLIM, //	--radixsortlim = 0..2e9      radix sort limit[32]
	RANDEC,      //	--randec = bool              random decisions[false]
	RANDECFOCUSED, //	--randecfocused = bool       random decisions in focused mode[true]
	RANDECINIT,  //	--randecinit = 2..2e9        inital random decision interval[1e3]
	RANDECINT,   //	--randecint = 0..2e9         random conflict length[500]
	RANDECLENGTH, //	--randeclength = 1..1e9      length random decisions phases[10]
	RANDECSTABLE, //	--randecstable = bool        random decisions in stable mode[false]
	REALTIME,    //	--realtime = bool            real instead of process time[false]
	RECOMPUTETIER, //	--recomputetier = bool       recompute tiers[true]
	REDUCE,      //	--reduce = bool              reduce useless clauses[true]
	REDUCEINIT,  //	--reduceinit = 1..1e6        initial interval[300]
	REDUCEINT,   //	--reduceint = 2..1e6         reduce interval[25]
	REDUCEOPT,   //	--reduceopt = 0..2           0 = prct, 1 = sqrt, 2 = max[1]
	REDUCETARGET, //	--reducetarget = 10..1e2     reduce fraction in percent[75]
	REDUCETIER1GLUE, //	--reducetier1glue = 1..2e9   glue of kept learned clauses[2]
	REDUCETIER2GLUE, //	--reducetier2glue = 1..2e9   glue of tier two clauses[6]
	RELUCTANT,   //	--reluctant = bool           stable reluctant doubling restarts[true]
	RELUCTANTINT, //	--reluctantint = 0..2e9      reluctant doubling period[1024]
	RELUCTANTMAX, //	--reluctantmax = 0..2e9      maximum reluctant doubling period[1048576]
	REPHASE,     //	--rephase = 0..2             enable resetting phase(0 = no, 1 = always, 2 = stable - only)[1]
	REPHASEINT,  //	--rephaseint = 1..2e9        rephase interval[1e3]
	REPORT,      //	--report = bool              enable reporting[false]
	REPORTALL,   //	--reportall = bool           report even if not successful[false]
	REPORTSOLVE, //	--reportsolve = bool         use solving not process time[false]
	RESTART,     //	--restart = bool             enable restarts[true]
	RESTARTINT,  //	--restartint = 1..2e9        restart interval[2]
	RESTARTMARGINFOCUSED, //	--restartmarginfocused = 0..25 focused slow fast margin in percent[10]
	RESTARTMARGINSTABLE, //	--restartmarginstable = 0..25 stable slow fast margin in percent[25]
	RESTARTREUSETRAIL, //	--restartreusetrail = bool   enable trail reuse[true]
	RESTOREALL,  //	--restoreall = 0..2          restore all clauses(2 = really)[0]
	RESTOREFLUSH, //	--restoreflush = bool        remove satisfied clauses[false]
	REVERSE,     //	--reverse = bool             reverse variable ordering[false]
	SCORE,       //	--score = bool               use EVSIDS scores[true]
	SCOREFACTOR,   //	--scorefactor = 500..1e3     score factor per mille[950]
	SHRINK,      //	--shrink = 0..3              shrink conflict clause(1 = binary - only, 2 = minimize - on - pulling, 3 = full)[3]
	SHRINKREAP,   //	--shrinkreap = bool          use a reap for shrinking[true]
	SHUFFLE,      //	--shuffle = bool             shuffle variables[false]
	SHUFFLEQUEUE,  //	--shufflequeue = bool        shuffle variable queue[true]
	SHUFFLERANDOM, //	--shufflerandom = bool       not reverse but random[false]
	SHUFFLESCORES, //	--shufflescores = bool       shuffle variable scores[true]
	STABILIZE,    //	--stabilize = bool           enable stabilizing phases[true]
	STABILIZEINIT, //	--stabilizeinit = 1..2e9     stabilizing interval[1e3]
	STABILIZEONLY,  //	--stabilizeonly = bool       only stabilizing phases[false]
	STATS,        //	--stats = bool               print all statistics at the end of the run[false]
	STUBBORNIOFOCUSED, //	--stubbornIOfocused = bool   force phases to I / O in focused mode every once in a while (requires rephase == 2)[false]
	SUBSUME,      //	--subsume = bool             enable clause subsumption[true]
	SUBSUMEBINLIM, //	--subsumebinlim = 0..2e9     watch list length limit[1e4]
	SUBSUMECLSLIM, //	--subsumeclslim = 0..2e9     clause length limit[1e2]
	SUBSUMEEFFORT, //	--subsumeeffort = 1..1e5     relative efficiency per mille[1e3]
	SUBSUMELIMITED, //	--subsumelimited = bool      limit subsumption checks[true]
	SUBSUMEMAXEFF, //	--subsumemaxeff = 0..2e9     maximum subsuming efficiency[1e8]
	SUBSUMEMINEFF, //	--subsumemineff = 0..2e9     minimum subsuming efficiency[0]
	SUBSUMEOCCLIM, //	--subsumeocclim = 0..2e9     watch list length limit[1e2]
	SUBSUMESTR,   //	--subsumestr = bool          subsume strengthen[true]
	SWEEP,        //	--sweep = bool               enable SAT sweeping[true]
	SWEEPCLAUSES, //	--sweepclauses = 0..2e9      environment clauses[1024]
	SWEEPCOMPLETE, //	--sweepcomplete = bool       run SAT sweeping to completion[false]
	SWEEPCOUNTBINARY, //	--sweepcountbinary = bool    count binaries to environment[true]
	SWEEPDEPTH,   //	--sweepdepth = 0..2e9        environment depth[2]
	SWEEPEFFORT,  //	--sweepeffort = 0..1e4       relative effort in ticks per mille[1e2]
	SWEEPFLIPROUNDS, //	--sweepfliprounds = 0..2e9   flipping rounds[1]
	SWEEPMAXCLAUSES, //	--sweepmaxclauses = 2..2e9   maximum environment clauses[3e5]
	SWEEPMAXDEPTH, //	--sweepmaxdepth = 1..2e9     maximum environment depth[3]
	SWEEPMAXVARS, //	--sweepmaxvars = 2..2e9      maximum environment variables[8192]
	SWEEPRAND,    //	--sweeprand = bool           randomize sweeping environment[false]
	SWEEPTHRESH,  //	--sweepthresh = 0..100       delay if ticks smaller thresh* clauses[5]
	SWEEPVARS,    //	--sweepvars = 0..2e9         environment variables[256]
	TARGET,       //	--target = 0..2              target phases(1 = stable only)[1]
	TERMINATEINT, //	--terminateint = 0..1e4      termination check interval[10]
	TERNARY,      //	--ternary = bool             hyper ternary resolution[true]
	TERNARYEFFORT, //	--ternaryeffort = 1..1e5     relative efficiency per mille[8]
	TERNARYMAXADD, //	--ternarymaxadd = 0..1e4     max clauses added in percent[1e3]
	TERNARYOCCLIM, //	--ternaryocclim = 1..2e9     ternary occurrence limit[1e2]
	TERNARYROUNDS, //	--ternaryrounds = 1..16      maximum ternary rounds[2]
	TERNARYTHRESH, //	--ternarythresh = 0..100     delay if ticks smaller thresh* clauses[6]
	TIER1LIMIT,   //	--tier1limit = 0..100        limit of tier1 usage in percentage[50]
	TIER1MINGLUE, //	--tier1minglue = 0..100      lowest tier1 limit[0]
	TIER2LIMIT,   //	--tier2limit = 0..100        limit of tier2 usage in percentage[90]
	TIER2MINGLUE, //	--tier2minglue = 0..100      lowest tier2 limit[0]
	TRANSRED,     //	--transred = bool            transitive reduction of BIG[true]
	TRANSREDEFFORT, //	--transredeffort = 1..1e5    relative efficiency per mille[1e2]
	TRANSREDMAXEFF, //	--transredmaxeff = 0..2e9    maximum
	TRANSREDMINEFF, //	--transredmineff = 0..2e9    minimum efficiency[0]
	VERBOSE,     //	--verbose = 0..3             more verbose messages[0]              
	VERIPB,      //	--veripb = 0..4              odd = check - deletions, > 2 drat[0]
	VIVIFY,      //	--vivify = bool              vivification[true]
	VIVIFYCALCTIER, //	--vivifycalctier = bool      recalculate tier limits[false]
	VIVIFYDEMOTE, //	--vivifydemote = bool        demote irredundant or delete directly[false]
	VIVIFYEFFORT, //	--vivifyeffort = 0..1e5      overall efficiency per mille[50]
	VIVIFYFLUSH, //	--vivifyflush = bool         flush subsumed before vivification rounds[true]
	VIVIFYINST,  //	--vivifyinst = bool          instantiate last literal when vivify[true]
	VIVIFYIRRED, //	--vivifyirred = bool         vivification of irredundant clauses[true]
	VIVIFYIRREDEFF, //	--vivifyirredeff = 1..100    irredundant efficiency per mille[3]
	VIVIFYONCE,  //	--vivifyonce = 0..2          vivify once : 1 = red, 2 = red + irr[0]
	VIVIFYRETRY, //	--vivifyretry = 0..5         re - vivify clause if vivify was successful[0]
	VIVIFYSCHEDMAX, //	--vivifyschedmax = 10..2e9   maximum schedule size[5e3]
	VIVIFYTHRESH, //	--vivifythresh = 0..100      delay if ticks smaller thresh* clauses[20]
	VIVIFYTIER1, //	--vivifytier1 = bool         vivification tier1[true]
	VIVIFYTIER1EFF, //	--vivifytier1eff = 0..100    relative tier1 effort[4]
	VIVIFYTIER2, //	--vivifytier2 = bool         vivification tier2[true]
	VIVIFYTIER2EFF, //	--vivifytier2eff = 1..100    relative tier2 effort[2]
	VIVIFYTIER3, //	--vivifytier3 = bool         vivification tier3[true]
	VIVIFYTIER3EFF, //	--vivifytier3eff = 1..100    relative tier3 effort[1]
	WALK,        //	--walk = bool                enable random walks[true]
	WALKEFFORT,  //	--walkeffort = 1..1e5        relative efficiency per mille[80]
	WALKFULLOCC, //	--walkfullocc = bool         use Kissat's full occurrences instead of the single watched [false]
	WALKMAXEFF,  //	--walkmaxeff = 0..2e9        maximum efficiency(in 1e3 ticks)[1e7]
	WALKMINEFF,  //	--walkmineff = 0..1e7        minimum efficiency[0]
	WALKNONSTABLE, //	--walknonstable = bool       walk in non - stabilizing phase[true]
	WALKREDUNDANT, //	--walkredundant = bool       walk redundant clauses too[false]
	WARMUP,      //	--warmup = bool              warmup before walk using propagation[true]

	PARAMETROS_CADICAL
};

enum EIndicadoresCaDiCaL {
	IND_MEMORY = IND_PROCURA,
	IND_PROPAGATIONS,
	IND_TICKS,
	IND_RESTARTS,
	IND_LEARNED,
	IND_FIXED,
	IND_MAXLEVEL,
	IND_NUMVARS,   // indicator of the conversion
	IND_NUMCLAUSES, // indicator of the conversion
	IND_CADICAL,
	IND_CONFLICTS = IND_ITERACOES
};
