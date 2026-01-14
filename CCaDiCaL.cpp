#include "CCaDiCaL.h"
#include <stdio.h>
#include <algorithm>
#include <random>
#include <iterator>

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
	IND_MEMORY = IND_PROCURA, ///< retorna a memória usada em MB
};

void CCaDiCaL::ResetParametros()
{
	static const char* nomesMetodo[] = {
		"CaDiCaL"
	};
	static const char* definedConfigurations[] = {
		"no configuration", "default", "plain", "sat", "unsat"
	};
	static const char* trueFalse[] = {
		"false", "true"
	};

	TProcura::ResetParametros();

	// changing parameters defined in TProcura
	parametro[ALGORITMO] = { "Solver",1,1,1,"Solver of a SAT", nomesMetodo };
	//parametro[NIVEL_DEBUG] = { "verbose",0,0,3,"more verbose messages", NULL }; /// cannot be changed since is used to congtrol level of information shown during the run
	parametro[SEMENTE] = { "seed",0,0,2000000000,"random seed", NULL };
	parametro[LIMITE_TEMPO] = { "t",10,1,86400,"set wall clock time limit", NULL };
	parametro[LIMITE_ITERACOES] = { "c",0,0,1000000000,"limit the number of conflicts (default unlimited)", NULL };

	// adding CaDiCaL main parameters
	parametro += { "d", 0, 0, 1000000000, "limit the number of decisions (default unlimited)", NULL };
	parametro += { "L", 0, 0, 1000, "run local search initially (default '0' rounds)", NULL };
	parametro += { "P", 0, 0, 1000, "initial preprocessing (default '0' rounds)", NULL };
	parametro += { "Defined configurations", 0, 0, 4, "pre-defined configurations of advanced internal options: default (set default advanced internal options); plain (disable all internal preprocessing options); sat (set internal options to target satisfiable instances); unsat (set internal options to target unsatisfiable instances).", definedConfigurations };

	// adding CaDiCaL full set of parameters
	parametro += {
		{ "arena", 1, 0, 1, "allocate clauses in arena[true]", trueFalse },
		{ "arenacompact", 1, 0, 1, "keep clauses compact[true]", trueFalse, { ARENA,1 } },
		{ "arenasort", 1, 0, 1, "sort clauses in arena[true]", trueFalse, { ARENA,1 } },
		{ "arenatype", 3, 1, 3, "1 = clause, 2 = var, 3 = queue[3]", NULL, { ARENA,1 } }};
	parametro += {
		{ "backbone", 1, 0, 2, "binary clause backbone[1]", NULL },
		{ "backboneeffort", 20, 0, 100000, "binary effort in per mile[20]", NULL, { BACKBONE,1 } },
		{ "backbonemaxrounds", 1000, 0, 100000, "backbone rounds limit[1e3]", NULL, { BACKBONE,1 } },
		{ "backbonerounds", 100, 0, 100000, "backbone rounds limit[100]", NULL, { BACKBONE,1 } },
		{ "backbonethresh", 5, 0, 1000000000, "delay if ticks smaller thresh* clauses[5]", NULL, { BACKBONE,1 } }};
	parametro += { "binary", 1, 0, 1, "use binary proof format[true]", trueFalse };
	parametro += {
		{ "block", 0, 0, 1, "blocked clause elimination[false]", trueFalse },
		{ "blockmaxclslim", 100000, 1, 2000000000, "maximum clause size[1e5]", NULL, { BLOCK,1 } },
		{ "blockminclslim", 2, 2, 2000000000, "minimum clause size[2]", NULL, { BLOCK,1 } },
		{ "blockocclim", 100, 1, 2000000000, "occurrence limit[1e2]", NULL, { BLOCK,1 } }};
	parametro += {
		{ "bump", 1, 0, 1, "bump variables[true]", trueFalse },
		{ "bumpreason", 1, 0, 1, "bump reason literals too[true]", trueFalse, { BUMP,1 } },
		{ "bumpreasondepth", 1, 1, 3, "bump reason depth[1]", NULL, { BUMPREASON,1 } },
		{ "bumpreasonlimit", 10, 1, 2000000000, "bump reason limit[10]", NULL, { BUMPREASON,1 } },
		{ "bumpreasonrate", 100, 1, 2000000000, "bump reason decision rate[100]", NULL, { BUMPREASON,1 } }};
	parametro += {
		{ "check", 0, 0, 1, "enable internal checking[false]", trueFalse },
		{ "checkassumptions", 1, 0, 1, "check assumptions satisfied[true]", trueFalse, { CHECK,1 } },
		{ "checkconstraint", 1, 0, 1, "check constraint satisfied[true]", trueFalse, { CHECK,1 } },
		{ "checkfailed", 1, 0, 1, "check failed literals form core[true]", trueFalse, { CHECK,1 } },
		{ "checkfrozen", 0, 0, 1, "check all frozen semantics[false]", trueFalse, { CHECK,1 } },
		{ "checkproof", 3, 0, 3, "1 = drat, 2 = lrat, 3 = both[3]", NULL, { CHECK,1 } },
		{ "checkwitness", 1, 0, 1, "check witness internally[true]", trueFalse, { CHECK,1 } }};
	parametro += {
		{ "chrono", 1, 0, 2, "chronological backtracking[1]", NULL },
		{ "chronoalways", 0, 0, 1, "force always chronological[false]", trueFalse, { CHRONO,1,2 } },
		{ "chronolevelim", 100, 0, 2000000000, "chronological level limit[1e2]", NULL, { CHRONO,1,2 } },
		{ "chronoreusetrail", 1, 0, 1, "reuse trail chronologically[true]", trueFalse, { CHRONO,1,2 } }};
	parametro += {
		{ "compact", 1, 0, 1, "compact internal variables[true]", trueFalse },
		{ "compactint", 2000, 1, 2000000000, "compacting interval[2e3]", NULL, { COMPACT,1 } },
		{ "compactlim", 100, 0, 1000, "inactive limit per mille[1e2]", NULL, { COMPACT,1 } },
		{ "compactmin", 100, 1, 2000000000, "minimum inactive limit[1e2]", NULL, { COMPACT,1 } }};
	parametro += {
		{ "condition", 0, 0, 1, "globally blocked clause elim[false]", trueFalse },
		{ "conditioneffort", 100, 1, 100000, "relative efficiency per mille[100]", NULL, { CONDITION,1 } },
		{ "conditionint", 10000, 1, 2000000000, "initial conflict interval[1e4]", NULL, { CONDITION,1 } },
		{ "conditionmaxeff", 10000000, 0, 2000000000, "maximum condition efficiency[1e7]", NULL, { CONDITION,1 } },
		{ "conditionmaxrat", 100, 1, 2000000000, "maximum clause variable ratio[100]", NULL, { CONDITION,1 } },
		{ "conditionmineff", 0, 0, 2000000000, "minimum condition efficiency[0]", NULL, { CONDITION,1 } }};
	parametro += {
		{ "congruence", 1, 0, 1, "congruence closure[true]", trueFalse },
		{ "congruenceand", 1, 0, 1, "extract AND gates[true]", trueFalse, { CONGRUENCE,1 } },
		{ "congruenceandarity", 1000000, 2, 50000000, "AND gate arity limit[1e6]", NULL, { CONGRUENCEAND,1 } },
		{ "congruencebinaries", 1, 0, 1, "extract binary and strengthen ternary clauses[true]", trueFalse, { CONGRUENCE,1 } },
		{ "congruenceite", 1, 0, 1, "extract ITE gates[true]", trueFalse, { CONGRUENCE,1 } },
		{ "congruencexor", 1, 0, 1, "extract XOR gates[true]", trueFalse, { CONGRUENCE,1 } },
		{ "congruencexorarity", 4, 2, 31, "XOR gate arity limit[4]", NULL, { CONGRUENCEXOR,1 } },
		{ "congruencexorcounts", 1, 1, 5000000, "XOR gate round[1]", NULL, { CONGRUENCEXOR,1 } }};
	parametro += {
		{ "cover", 0, 0, 1, "covered clause elimination[false]", trueFalse },
		{ "covereffort", 4, 1, 100000, "relative efficiency per mille[4]", NULL, { COVER,1 } },
		{ "covermaxclslim", 100000, 1, 2000000000, "maximum clause size[1e5]", NULL, { COVER,1 } },
		{ "covermaxeff", 100000000, 0, 2000000000, "maximum cover efficiency[1e8]", NULL, { COVER,1 } },
		{ "coverminclslim", 2, 2, 2000000000, "minimum clause size[2]", NULL, { COVER,1 } },
		{ "covermineff", 0, 0, 2000000000, "minimum cover efficiency[0]", NULL, { COVER,1 } }};
	parametro += {
		{ "decompose", 1, 0, 1, "decompose BIG in SCCs and ELS[true]", trueFalse },
		{ "decomposerounds", 2, 1, 16, "number of decompose rounds[2]", NULL, { DECOMPOSE,1 } }};
	parametro += {
		{ "deduplicate", 1, 0, 1, "remove duplicated binaries[true]", trueFalse },
		{ "deduplicateallinit", 0, 0, 1, "remove duplicated clauses once before solving[false]", trueFalse, { DEDUPLICATE,1 } }};
	parametro += {
		{ "eagersubsume", 1, 0, 1, "subsume recently learned[true]", trueFalse },
		{ "eagersubsumelim", 20, 1, 1000, "limit on subsumed candidates[20]", NULL, { EAGERSUBSUME,1 } }};
	parametro += {
		{ "elim", 1, 0, 1, "bounded variable elimination[true]", trueFalse },
		{ "elimands", 1, 0, 1, "find AND gates[true]", trueFalse, { ELIM,1 } },
		{ "elimbackward", 1, 0, 1, "eager backward subsumption[true]", trueFalse, { ELIM,1 } },
		{ "elimboundmax", 16, -1, 2000000, "maximum elimination bound[16]", NULL, { ELIM,1 } },
		{ "elimboundmin", 0, -1, 2000000, "minimum elimination bound[0]", NULL, { ELIM,1 } },
		{ "elimclslim", 100, 2, 2000000000, "resolvent size limit[1e2]", NULL, { ELIM,1 } },
		{ "elimdef", 0, 0, 1, "mine definitions with kitten[false]", trueFalse, { ELIM,1 } },
		{ "elimdefcores", 1, 1, 100, "number of unsat cores[1]", NULL, { ELIM,1 } },
		{ "elimdefticks", 200000, 0, 2000000000, "kitten ticks limit[2e5]", NULL, { ELIM,1 } },
		{ "elimeffort", 1000, 1, 100000, "relative efficiency per mille[1e3]", NULL, { ELIM,1 } },
		{ "elimequivs", 1, 0, 1, "find equivalence gates[true]", trueFalse, { ELIM,1 } },
		{ "elimint", 2000, 1, 2000000000, "elimination interval[2e3]", NULL, { ELIM,1 } },
		{ "elimites", 1, 0, 1, "find if - then - else gates[true]", trueFalse, { ELIM,1 } },
		{ "elimlimited", 1, 0, 1, "limit resolutions[true]", trueFalse, { ELIM,1 } },
		{ "elimmaxeff", 1000000000, 0, 2000000000, "maximum elimination efficiency[1e9]", NULL, { ELIM,1 } },
		{ "elimmineff", 10000000, 0, 2000000000, "minimum elimination efficiency[1e7]", NULL, { ELIM,1 } },
		{ "elimocclim", 100, 0, 2000000000, "occurrence limit[1e2]", NULL, { ELIM,1 } },
		{ "elimprod", 1, 0, 10000, "elim score product weight[1]", NULL, { ELIM,1 } },
		{ "elimrounds", 2, 1, 512, "usual number of rounds[2]", NULL, { ELIM,1 } },
		{ "elimsubst", 1, 0, 1, "elimination by substitution[true]", trueFalse, { ELIM,1 } },
		{ "elimsum", 1, 0, 10000, "elimination score sum weight[1]", NULL, { ELIM,1 } },
		{ "elimxorlim", 5, 2, 27, "maximum XOR size[5]", NULL, { ELIM,1 } },
		{ "elimxors", 1, 0, 1, "find XOR gates[true]", trueFalse, { ELIM,1 } }};
	parametro += { "emadecisions", 100000, 1, 2000000000, "window decision rate[1e5]", NULL };
	parametro += { "emagluefast", 33, 1, 2000000000, "window fast glue[33]", NULL };
	parametro += { "emaglueslow", 100000, 1, 2000000000, "window slow glue[1e5]", NULL };
	parametro += { "emajump", 100000, 1, 2000000000, "window back - jump level[1e5]", NULL };
	parametro += { "emalevel", 100000, 1, 2000000000, "window back - track level[1e5]", NULL };
	parametro += { "emasize", 100000, 1, 2000000000, "window learned clause size[1e5]", NULL };
	parametro += { "ematrailfast", 100, 1, 2000000000, "window fast trail[1e2]", NULL };
	parametro += { "ematrailslow", 100000, 1, 2000000000, "window slow trail[1e5]", NULL };
	parametro += { "exteagerreasons", 1, 0, 1, "eagerly ask for all reasons(0: only when needed)[true]", trueFalse };
	parametro += { "exteagerrecalc", 1, 0, 1, "after eagerly asking for reasons recalculate all levels(0: trust the external tool)[true]", trueFalse };
	parametro += { "externallrat", 0, 0, 1, "external lrat[false]", trueFalse };
	parametro += {
		{ "factor", 1, 0, 1, "bounded variable addition[true]", trueFalse },
		{ "factorcandrounds", 2, 0, 2000000000, "candidates reduction rounds[2]", NULL, { FACTOR,1 } },
		{ "factorcheck", 1, 0, 2, "API checks that variables have been declared(1 = only with factor on, 2 = always)[1]", NULL, { FACTOR,1 } },
		{ "factordelay", 4, 0, 12, "delay bounded variable addition between eliminations[4]", NULL, { FACTOR,1 } },
		{ "factoreffort", 50, 0, 100000, "relative effort per mille[50]", NULL, { FACTOR,1 } },
		{ "factoriniticks", 300, 1, 1000000, "initial effort in millions[300]", NULL, { FACTOR,1 } },
		{ "factorsize", 5, 2, 2000000000, "clause size limit[5]", NULL, { FACTOR,1 } },
		{ "factorthresh", 7, 0, 100, "delay if ticks smaller thresh* clauses[7]", NULL, { FACTOR,1 } },
		{ "factorunbump", 1, 0, 1, "extension variable with lowest importance[1:as in kissat][true]", trueFalse, { FACTOR,1 } }};
	parametro += {
		{ "fastelim", 1, 0, 1, "fast BVE during preprocessing[true]", trueFalse },
		{ "fastelimbound", 8, 1, 1000, "fast BVE bound during preprocessing[8]", NULL, { FASTELIM,1 } },
		{ "fastelimclslim", 100, 2, 2000000000, "fast BVE resolvent size limit[1e2]", NULL, { FASTELIM,1 } },
		{ "fastelimocclim", 100, 1, 2000000000, "fast BVE occurence limit during preprocessing[100]", NULL, { FASTELIM,1 } },
		{ "fastelimrounds", 4, 1, 512, "number of fastelim rounds[4]", NULL, { FASTELIM,1 } }};
	parametro += {
		{ "flush", 0, 0, 1, "flush redundant clauses[false]", trueFalse },
		{ "flushfactor", 3, 1, 1000, "interval increase[3]", NULL, { FLUSH,1 } },
		{ "flushint", 100000, 1, 2000000000, "initial limit[1e5]", NULL, { FLUSH,1 } }};
	parametro += { "forcephase", 0, 0, 1, "always use initial phase[false]", trueFalse };
	parametro += { "frat", 0, 0, 2, "1 = frat(lrat), 2 = frat(drat)[0]", NULL };
	parametro += { "idrup", 0, 0, 1, "incremental proof format[false]", trueFalse };
	parametro += { "ilb", 0, 0, 2, "ILB(incremental lazy backtrack) (0: no, 1 : assumptions only, 2 : everything)[0]", NULL };
	parametro += {
		{ "incdecay", 1, 0, 4, "decay clauses when doing incremental clauses[1]", NULL },
		{ "incdecayint", 1000000, 1, 2000000000, "decay interval when doing incremental clauses[1e6]", NULL }};
	parametro += {
		{ "inprobeint", 100, 1, 2000000000, "inprobing interval[100]", NULL, { INPROBING, 1 } },
		{ "inprobing", 1, 0, 1, "enable probe inprocessing[true]", trueFalse }};
	parametro += { "inprocessing", 1, 0, 1, "enable general inprocessing[true]", trueFalse };
	parametro += {
		{ "instantiate", 0, 0, 1, "variable instantiation[false]", trueFalse },
		{ "instantiateclslim", 3, 2, 2000000000, "minimum clause size[3]", NULL, { INSTANTIATE,1 } },
		{ "instantiateocclim", 1, 1, 2000000000, "maximum occurrence limit[1]", NULL, { INSTANTIATE,1 } },
		{ "instantiateonce", 1, 0, 1, "instantiate each clause once[true]", trueFalse, { INSTANTIATE,1 } }};
	parametro += { "lidrup", 0, 0, 1, "linear incremental proof format[false]", trueFalse };
	parametro += { "lrat", 0, 0, 1, "use LRAT proof format[false]", trueFalse };
	parametro += {
		{ "lucky", 1, 0, 1, "lucky phases[true]", trueFalse },
		{ "luckyassumptions", 1, 0, 1, "lucky phases with assumptions[true]", trueFalse, { LUCKY, 1 } },
		{ "luckyearly", 1, 0, 1, "lucky phases before preprocessing[true]", trueFalse, { LUCKY, 1 } },
		{ "luckylate", 1, 0, 1, "lucky phases after preprocessing[true]", trueFalse, { LUCKY, 1 } }};
	parametro += {
		{ "minimize", 1, 0, 1, "minimize learned clauses[true]", trueFalse },
		{ "minimizedepth", 1000, 0, 1000, "minimization depth[1e3]", NULL, { MINIMIZE, 1 } },
		{ "minimizeticks", 1, 0, 1, "increment ticks in minimization[true]", trueFalse, { MINIMIZE, 1 } }};
	parametro += { "otfs", 1, 0, 1, "on - the - fly self subsumption[true]", trueFalse };
	parametro += { "phase", 1, 0, 1, "initial phase[true]", trueFalse };
	parametro += { "preprocessinit", 2000000, 0, 2000000000, "initial preprocessing base limit[2e6]", NULL };
	parametro += { "preprocesslight", 1, 0, 1, "lightweight preprocessing[true]", trueFalse };
	parametro += {
		{ "probe", 1, 0, 1, "failed literal probing[true]", trueFalse },
		{ "probeeffort", 8, 1, 100000, "relative efficiency per mille[8]", NULL, { PROBE, 1 } },
		{ "probehbr", 1, 0, 1, "learn hyper binary clauses[true]", trueFalse, { PROBE, 1 } },
		{ "probethresh", 0, 0, 100, "delay if ticks smaller thresh* clauses[0]", NULL, { PROBEHBR, 1 } }};
	parametro += { "profile", 2, 0, 4, "profiling level[2]", NULL };
	parametro += { "quiet", 0, 0, 1, "disable all messages[false]", trueFalse };
	parametro += { "radixsortlim", 32, 0, 2000000000, "radix sort limit[32]", NULL };
	parametro += {
		{ "randec", 0, 0, 1, "random decisions[false]", trueFalse },
		{ "randecfocused", 1, 0, 1, "random decisions in focused mode[true]", trueFalse, { RANDEC,1 } },
		{ "randecinit", 1000, 2, 2000000000, "inital random decision interval[1e3]", NULL, { RANDEC,1 } },
		{ "randecint", 500, 0, 2000000000, "random conflict length[500]", NULL, { RANDEC,1 } },
		{ "randeclength", 10, 1, 1000000000, "length random decisions phases[10]", NULL, { RANDEC,1 } },
		{ "randecstable", 0, 0, 1, "random decisions in stable mode[false]", trueFalse, { RANDEC,1 } }};
	parametro += { "realtime", 0, 0, 1, "real instead of process time[false]", trueFalse };
	parametro += { "recomputetier", 1, 0, 1, "recompute tiers[true]", trueFalse };
	parametro += {
		{ "reduce", 1, 0, 1, "reduce useless clauses[true]", trueFalse },
		{ "reduceinit", 300, 1, 1000000, "initial interval[300]", NULL, { REDUCE,1 } },
		{ "reduceint", 25, 2, 1000000, "reduce interval[25]", NULL, { REDUCE,1 } },
		{ "reduceopt", 1, 0, 2, "0 = prct, 1 = sqrt, 2 = max[1]", NULL, { REDUCE,1 } },
		{ "reducetarget", 75, 10, 100, "reduce fraction in percent[75]", NULL, { REDUCE,1 } },
		{ "reducetier1glue", 2, 1, 2000000000, "glue of kept learned clauses[2]", NULL, { REDUCE,1 } },
		{ "reducetier2glue", 6, 1, 2000000000, "glue of tier two clauses[6]", NULL, { REDUCE,1 } }};
	parametro += {
		{ "reluctant", 1, 0, 1, "stable reluctant doubling restarts[true]", trueFalse },
		{ "reluctantint", 1024, 0, 2000000000, "reluctant doubling period[1024]", NULL, { RELUCTANT,1 } },
		{ "reluctantmax", 1048576, 0, 2000000000, "maximum reluctant doubling period[1048576]", NULL, { RELUCTANT,1 } }};
	parametro += {
		{ "rephase", 1, 0, 2, "enable resetting phase(0 = no, 1 = always, 2 = stable - only)[1]", NULL },
		{ "rephaseint", 1000, 1, 2000000000, "rephase interval[1e3]", NULL, { REPHASE,1 } }};
	parametro += {
		{ "report", 0, 0, 1, "enable reporting[false]", trueFalse },
		{ "reportall", 0, 0, 1, "report even if not successful[false]", trueFalse, { REPORT,1 } },
		{ "reportsolve", 0, 0, 1, "use solving not process time[false]", trueFalse, { REPORT,1 } }};
	parametro += {
		{ "restart", 1, 0, 1, "enable restarts[true]", trueFalse },
		{ "restartint", 100000, 1, 2000000000, "restart interval[1e5]", NULL, { RESTART,1 } },
		{ "restartmarginfocused", 10, 0, 25, "focused slow fast margin in percent[10]", NULL, { RESTART,1 } },
		{ "restartmarginstable", 25, 0, 25, "stable slow fast margin in percent[25]", NULL, { RESTART,1 } },
		{ "restartreusetrail", 1, 0, 1, "enable trail reuse[true]", trueFalse, { RESTART,1 } }};
	parametro += { "restoreall", 0, 0, 2, "restore all clauses(2 = really)[0]", NULL };
	parametro += { "restoreflush", 0, 0, 1, "remove satisfied clauses[false]", trueFalse };
	parametro += { "reverse", 0, 0, 1, "reverse variable ordering[false]", trueFalse };
	parametro += {
		{ "score", 1, 0, 1, "use EVSIDS scores[true]", trueFalse },
		{ "scorefactor", 950, 500, 1000, "score factor per mille[950]", NULL, { SCORE,1 } }};
	parametro += {
		{ "shrink", 3, 0, 3, "shrink conflict clause(1 = binary - only, 2 = minimize - on - pulling, 3 = full)[3]", NULL },
		{ "shrinkreap", 1, 0, 1, "use a reap for shrinking[true]", trueFalse, { SHRINK,1 } }};
	parametro += {
		{ "shuffle", 0, 0, 1, "shuffle variables[false]", trueFalse },
		{ "shufflequeue", 1, 0, 1, "shuffle variable queue[true]", trueFalse, { SHUFFLE,1 } },
		{ "shufflerandom", 0, 0, 1, "not reverse but random[false]", trueFalse, { SHUFFLE,1 } },
		{ "shufflescores", 1, 0, 1, "shuffle variable scores[true]", trueFalse, { SHUFFLE,1 } }};
	parametro += {
		{ "stabilize", 1, 0, 1, "enable stabilizing phases[true]", trueFalse },
		{ "stabilizeinit", 1000, 1, 2000000000, "stabilizing interval[1e3]", NULL, { STABILIZE,1 } },
		{ "stabilizeonly", 0, 0, 1, "only stabilizing phases[false]", trueFalse, { STABILIZE,1 } }};
	parametro += { "stats", 0, 0, 1, "print all statistics at the end of the run[false]", trueFalse };
	parametro += { "stubbornIOfocused", 0, 0, 1, "force phases to I / O in focused mode every once in a while (requires rephase == 2)[false]", trueFalse };
	parametro += {
		{ "subsume", 1, 0, 1, "enable clause subsumption[true]", trueFalse },
		{ "subsumebinlim", 10000, 0, 2000000000, "watch list length limit[1e4]", NULL, { SUBSUME,1 } },
		{ "subsumeclslim", 100, 0, 2000000000, "clause length limit[1e2]", NULL, { SUBSUME,1 } },
		{ "subsumeeffort", 1000, 1, 100000, "relative efficiency per mille[1e3]", NULL, { SUBSUME,1 } },
		{ "subsumelimited", 1, 0, 1, "limit subsumption checks[true]", trueFalse, { SUBSUME,1 } },
		{ "subsumemaxeff", 100000000, 0, 2000000000, "maximum subsuming efficiency[1e8]", NULL, { SUBSUME,1 } },
		{ "subsumemineff", 0, 0, 2000000000, "minimum subsuming efficiency[0]", NULL, { SUBSUME,1 } },
		{ "subsumeocclim", 100, 0, 2000000000, "watch list length limit[1e2]", NULL, { SUBSUME,1 } },
		{ "subsumestr", 1, 0, 1, "subsume strengthen[true]", trueFalse, { SUBSUME,1 } }};
	parametro += {
		{ "sweep", 1, 0, 1, "enable SAT sweeping[true]", trueFalse },
		{ "sweepclauses", 1024, 0, 2000000000, "environment clauses[1024]", NULL, { SWEEP,1 } },
		{ "sweepcomplete", 0, 0, 1, "run SAT sweeping to completion[false]", trueFalse, { SWEEP,1 } },
		{ "sweepcountbinary", 1, 0, 1, "count binaries to environment[true]", trueFalse, { SWEEP,1 } },
		{ "sweepdepth", 2, 0, 2000000000, "environment depth[2]", NULL, { SWEEP,1 } },
		{ "sweepeffort", 100, 0, 10000, "relative effort in ticks per mille[1e2]", NULL, { SWEEP,1 } },
		{ "sweepfliprounds", 1, 0, 2000000000, "flipping rounds[1]", NULL, { SWEEP,1 } },
		{ "sweepmaxclauses", 300000, 2, 2000000000, "maximum environment clauses[3e5]", NULL, { SWEEP,1 } },
		{ "sweepmaxdepth", 3, 1, 2000000000, "maximum environment depth[3]", NULL, { SWEEP,1 } },
		{ "sweepmaxvars", 8192, 2, 2000000000, "maximum environment variables[8192]", NULL, { SWEEP,1 } },
		{ "sweeprand", 0, 0, 1, "randomize sweeping environment[false]", trueFalse, { SWEEP,1 } },
		{ "sweepthresh", 5, 0, 100, "delay if ticks smaller thresh* clauses[5]", NULL, { SWEEP,1 } },
		{ "sweepvars", 256, 0, 2000000000, "environment variables[256]", NULL, { SWEEP,1 } }};
	parametro += { "target", 1, 0, 2, "target phases(1 = stable only)[1]", NULL };
	parametro += { "terminateint", 10, 1, 10000, "termination check interval[10]", NULL };
	parametro += {
		{ "ternary", 1, 0, 1, "hyper ternary resolution[true]", trueFalse },
		{ "ternaryeffort", 8, 1, 100000, "relative efficiency per mille[8]", NULL, { TERNARY,1 } },
		{ "ternarymaxadd", 1000, 0, 10000, "max clauses added in percent[1e3]", NULL, { TERNARY,1 } },
		{ "ternaryocclim", 100, 1, 2000000000, "ternary occurrence limit[1e2]", NULL, { TERNARY,1 } },
		{ "ternaryrounds", 2, 1, 16, "maximum ternary rounds[2]", NULL, { TERNARY,1 } },
		{ "ternarythresh", 6, 0, 100, "delay if ticks smaller thresh* clauses[6]", NULL, { TERNARY,1 } }};
	parametro += { "tier1limit", 50, 0, 100, "limit of tier1 usage in percentage[50]", NULL };
	parametro += { "tier1minglue", 0, 0, 100, "lowest tier1 limit[0]", NULL };
	parametro += { "tier2limit", 90, 0, 100, "limit of tier2 usage in percentage[90]", NULL };
	parametro += { "tier2minglue", 0, 0, 100, "lowest tier2 limit[0]", NULL };
	parametro += {
		{ "transred", 1, 0, 1, "transitive reduction of BIG[true]", trueFalse },
		{ "transredeffort", 100, 1, 100000, "relative efficiency per mille[1e2]", NULL, { TRANSRED,1 } },
		{ "transredmaxeff", 1000000000, 0, 2000000000, "maximum[1e9]", NULL, { TRANSRED,1 } },
		{ "transredmineff", 0, 0, 2000000000, "minimum efficiency[0]", NULL, { TRANSRED,1 } }};
	parametro += { "verbose", 0, 0, 3, "more verbose messages[0]", NULL };
	parametro += { "veripb", 0, 0, 4, "odd = check - deletions, > 2 drat[0]", NULL };
	parametro += {
		{ "vivify", 1, 0, 1, "vivification[true]", trueFalse },
		{ "vivifycalctier", 0, 0, 1, "recalculate tier limits[false]", trueFalse, { VIVIFY,1 } },
		{ "vivifydemote", 0, 0, 1, "demote irredundant or delete directly[false]", trueFalse, { VIVIFY,1 } },
		{ "vivifyefoort", 50, 0, 100000, "overall efficiency per mille[50]", NULL, { VIVIFY,1 } },
		{ "vivifyflush", 1, 0, 1, "flush subsumed before vivification rounds[true]", trueFalse, { VIVIFY,1 } },
		{ "vivifyinst", 1, 0, 1, "instantiate last literal when vivify[true]", trueFalse, { VIVIFY,1 } },
		{ "vivifyirred", 1, 0, 1, "vivification of irredundant clauses[true]", trueFalse, { VIVIFY,1 } },
		{ "vivifyirredeff", 3, 1, 100, "irredundant efficiency per mille[3]", NULL, { VIVIFY,1 } },
		{ "vivifyonce", 0, 0, 2, "vivify once : 1 = red, 2 = red + irr[0]", NULL, { VIVIFY,1 } },
		{ "vivifyretry", 0, 0, 5, "re - vivify clause if vivify was successful[0]", NULL, { VIVIFY,1 } },
		{ "vivifyschedmax", 5000, 10, 2000000000, "maximum schedule size[5e3]", NULL, { VIVIFY,1 } },
		{ "vivifythresh", 20, 0, 100, "delay if ticks smaller thresh* clauses[20]", NULL, { VIVIFY,1 } },
		{ "vivifytier1", 1, 0, 1, "vivification tier1[true]", trueFalse, { VIVIFY,1 } },
		{ "vivifytier1eff", 4, 0, 100, "relative tier1 effort[4]", NULL, { VIVIFY,1 } },
		{ "vivifytier2", 1, 0, 1, "vivification tier2[true]", trueFalse, { VIVIFY,1 } },
		{ "vivifytier2eff", 2, 1, 100, "relative tier2 effort[2]", NULL, { VIVIFY,1 } },
		{ "vivifytier3", 1, 0, 1, "vivification tier3[true]", trueFalse, { VIVIFY,1 } },
		{ "vivifytier3eff", 1, 1, 100, "relative tier3 effort[1]", NULL, { VIVIFY,1 } }};
	parametro += {
		{ "walk", 1, 0, 1, "enable random walks[true]", trueFalse },
		{ "walkeffort", 80, 1, 100000, "relative efficiency per mille[80]", NULL, { WALK,1 } },
		{ "walkfullocc", 0, 0, 1, "use Kissat's full occurrences instead of the single watched [false]", trueFalse, { WALK,1 } },
		{ "walkmaxeff", 10000000, 0, 2000000000, "maximum efficiency(in 1e3 ticks)[1e7]", NULL, { WALK,1 } },
		{ "walkmineff", 0, 0, 10000000, "minimum efficiency[0]", NULL, { WALK,1 } },
		{ "walknonstable", 1, 0, 1, "walk in non - stabilizing phase[true]", trueFalse, { WALK,1 } },
		{ "walkredundant", 0, 0, 1, "walk redundant clauses too[false]", trueFalse, { WALK,1 } }};
	parametro += { "warmup", 1, 0, 1, "warmup before walk using propagation[true]", trueFalse };

	// save all default parameters 
	defaultCadical.Count(parametro.Count());
	for (int i = 0; i < parametro.Count(); i++)
		defaultCadical[i] = parametro[i].valor;

	indicador += { "IND_MEMORY", "maximum resident set size of process", IND_MEMORY };
	indAtivo += IND_MEMORY;

	instancia = { "Instance", 1,1,100000, "ID of the instance (appended to the file name)", NULL };
}


int CCaDiCaL::ExecutaAlgoritmo()
{
	FILE* f;
	TString resultFile, solFile, cmdSTR, options;
	int error;
	resultFile.printf("%s%d.txt", (const char*)ficheiroInstancia, instancia.valor);
	solFile.printf("%s%d.sol", (const char*)ficheiroInstancia, instancia.valor);
	// build options string, with just the non-default parameters
	// special options with just one - and no space
	for (auto i : { LOCAL_SEARCH, PREPROCESSING })
		if (Parametro(i) != defaultCadical[i]) 
			options.printf("-%s%d ", parametro[i].nome, Parametro(i));
	// special options of just one -
	TVector<int> aux;
	aux += LIMITE_ITERACOES;
	aux += LIMIT_DECISIONS;
	for (auto i : aux)
		if (Parametro(i) != defaultCadical[i]) 
			options.printf("-%s %d ", parametro[i].nome, Parametro(i));
	// options for non-default parameters with --
	for (auto i : { SEMENTE })
		if (Parametro(i) != defaultCadical[i]) 
			options.printf("--%s=%d ", parametro[i].nome, Parametro(i));
	// check if is used defined configurations
	if (Parametro(DEFINED_CONFIGURATIONS) != defaultCadical[DEFINED_CONFIGURATIONS]) 
		options.printf("--%s ", parametro[DEFINED_CONFIGURATIONS].nomeValores[Parametro(DEFINED_CONFIGURATIONS)]);
	// add the rest of internal CaDiCaL parameters if used
	for (int i = ARENA; i < PARAMETROS_CADICAL; i++)
		if (Parametro(i) != defaultCadical[i]) 
			options.printf("--%s=%d ", parametro[i].nome, Parametro(i));

	// clean indicators
	indicators.Reset(0);
	// setup all parameters that are not default in the launch line
	cmdSTR.printf("cadical/build/cadical -t %d -w %s %s %s%d.cnf > %s",
		Parametro(LIMITE_TEMPO),
		(const char*)solFile,
		(const char*)options,
		(const char*)ficheiroInstancia, instancia.valor,
		(const char*)resultFile);

	error = system(cmdSTR); // lauch CaDiCaL solver
	if(error == -1)
	{
		printf("\nError launching CaDiCaL solver\nCommand line: %s", (const char*)cmdSTR);
		return 0;
	}
	else {
		error = WEXITSTATUS(error);

		if (error == 10) {
			indicators[IND_RESULTADO] = 1;
			//printf("SAT\n");
			// processa ficheiros
		}
		else if (error == 20) {
			indicators[IND_RESULTADO] = 2;
			//printf("UNSAT\n");
			// processa ficheiros
		}
		else if (error == 0) {
			indicators[IND_RESULTADO] = 0;
			//printf("Sucesso sem solução\n");
		}
		// extract indicators from the last run
		f = fopen(resultFile, "rt");
		if (f != NULL) {
			while(!feof(f)) {
				char linha[1024];
				// Detect lines:
				// c total real time since initialization:            0.01    seconds
				// c maximum resident set size of process: 4.38    MB
				if (fgets(linha, sizeof(linha), f) != NULL) {
					//printf("%s", linha);
					if (strstr(linha, "c total real time since initialization")) {
						double tempo;
						sscanf(linha, "c total real time since initialization: %lg seconds",&tempo);
						indicators[IND_TEMPO] = (int)(tempo * 1000); // in milliseconds
					}
					else if (strstr(linha, "c maximum resident set size of process")) {
						double memoria;
						sscanf(linha, "c maximum resident set size of process: %lg MB", &memoria);
						indicators[IND_MEMORY] = (int)memoria; // in MB
					}
				}
			}
			// process the file and extract indicators and solution
			fclose(f);
			remove(resultFile);
		}
		f=fopen(solFile, "rt");
		if(f!=NULL) {
			// read solution
			char linha[1024];
			satSol = {};
			while(!feof(f)) {
				if (fgets(linha, sizeof(linha), f) != NULL) {
					//printf("%s", linha);
					if (strstr(linha, "v ")) {
						char* pch;
						pch = strtok(linha, " ");
						pch = strtok(NULL, " ");
						while (pch != NULL) {
							int var = atoi(pch);
							if (var != 0)
								satSol += var;
							pch = strtok(NULL, " ");
						}
					}
				}
			}
			fclose(f);
			remove(solFile);
		}
	}
	return 1;
}

/* $ ./cadical --help
usage: cadical [ <option> ... ] [ <input> [ <proof> ] ]

where '<option>' is one of the following common options:

  -h             print alternatively only a list of common options
  --help         print this complete list of all options
  --version      print version

  -n             do not print witness (same as '--no-witness')
  -v             increase verbosity (see also '--verbose' below)
  -q             be quiet (same as '--quiet')
  -t <sec>       set wall clock time limit                        ------ DONE

Or '<option>' is one of the less common options

  -L<rounds>     run local search initially (default '0' rounds) ------ DONE
  -O<level>      increase limits by '2^<level>' or '10^<level>'  --- ? NOT DONE
  -P<rounds>     initial preprocessing (default '0' rounds)      ------ DONE

Note there is no separating space for the options above while the
following options require a space after the option name:

  -c <limit>     limit the number of conflicts (default unlimited) ------ DONE
  -d <limit>     limit the number of decisions (default unlimited) ------ DONE

  -o <output>    write simplified CNF in DIMACS format to file
  -e <extend>    write reconstruction/extension stack to file

  --force | -f   parsing broken DIMACS header and writing proofs
  --strict       strict parsing (no white space in header)

  -r <sol>       read solution in competition output format
				 to check consistency of learned clauses
				 during testing and debugging

  -w <sol>       write result including a potential witness      ------ DONE
				 solution in competition format to the given file

  --colors       force colored output
  --no-colors    disable colored output to terminal
  --no-witness   do not print witness (see also '-n' above)

  --build        print build configuration
  --copyright    print copyright information

There are pre-defined configurations of advanced internal options:   ---- DONE

  --default      set default advanced internal options
  --plain        disable all internal preprocessing options
  --sat          set internal options to target satisfiable instances
  --unsat        set internal options to target unsatisfiable instances

Or '<option>' is one of the following advanced internal options:

  --arena=bool               allocate clauses in arena [true]
  --arenacompact=bool        keep clauses compact [true]
  --arenasort=bool           sort clauses in arena [true]
  --arenatype=1..3           1=clause, 2=var, 3=queue [3]
  --backbone=0..2            binary clause backbone [1]
  --backboneeffort=0..1e5    binary effort in per mile [20]
  --backbonemaxrounds=0..1e5 backbone rounds limit [1e3]
  --backbonerounds=0..1e5    backbone rounds limit [100]
  --backbonethresh=0..1e9    delay if ticks smaller thresh*clauses [5]
  --binary=bool              use binary proof format [true]
  --block=bool               blocked clause elimination [false]
  --blockmaxclslim=1..2e9    maximum clause size [1e5]
  --blockminclslim=2..2e9    minimum clause size [2]
  --blockocclim=1..2e9       occurrence limit [1e2]
  --bump=bool                bump variables [true]
  --bumpreason=bool          bump reason literals too [true]
  --bumpreasondepth=1..3     bump reason depth [1]
  --bumpreasonlimit=1..2e9   bump reason limit [10]
  --bumpreasonrate=1..2e9    bump reason decision rate [100]
  --check=bool               enable internal checking [false]
  --checkassumptions=bool    check assumptions satisfied [true]
  --checkconstraint=bool     check constraint satisfied [true]
  --checkfailed=bool         check failed literals form core [true]
  --checkfrozen=bool         check all frozen semantics [false]
  --checkproof=0..3          1=drat, 2=lrat, 3=both [3]
  --checkwitness=bool        check witness internally [true]
  --chrono=0..2              chronological backtracking [1]
  --chronoalways=bool        force always chronological [false]
  --chronolevelim=0..2e9     chronological level limit [1e2]
  --chronoreusetrail=bool    reuse trail chronologically [true]
  --compact=bool             compact internal variables [true]
  --compactint=1..2e9        compacting interval [2e3]
  --compactlim=0..1e3        inactive limit per mille [1e2]
  --compactmin=1..2e9        minimum inactive limit [1e2]
  --condition=bool           globally blocked clause elim [false]
  --conditioneffort=1..1e5   relative efficiency per mille [100]
  --conditionint=1..2e9      initial conflict interval [1e4]
  --conditionmaxeff=0..2e9   maximum condition efficiency [1e7]
  --conditionmaxrat=1..2e9   maximum clause variable ratio [100]
  --conditionmineff=0..2e9   minimum condition efficiency [0]
  --congruence=bool          congruence closure [true]
  --congruenceand=bool       extract AND gates [true]
  --congruenceandarity=2..5e7 AND gate arity limit [1e6]
  --congruencebinaries=bool  extract binary and strengthen ternary clauses [true]
  --congruenceite=bool       extract ITE gates [true]
  --congruencexor=bool       extract XOR gates [true]
  --congruencexorarity=2..31 XOR gate arity limit [4]
  --congruencexorcounts=1..5e6 XOR gate round [1]
  --cover=bool               covered clause elimination [false]
  --covereffort=1..1e5       relative efficiency per mille [4]
  --covermaxclslim=1..2e9    maximum clause size [1e5]
  --covermaxeff=0..2e9       maximum cover efficiency [1e8]
  --coverminclslim=2..2e9    minimum clause size [2]
  --covermineff=0..2e9       minimum cover efficiency [0]
  --decompose=bool           decompose BIG in SCCs and ELS [true]
  --decomposerounds=1..16    number of decompose rounds [2]
  --deduplicate=bool         remove duplicated binaries [true]
  --deduplicateallinit=bool  remove duplicated clauses once before solving [false]
  --eagersubsume=bool        subsume recently learned [true]
  --eagersubsumelim=1..1e3   limit on subsumed candidates [20]
  --elim=bool                bounded variable elimination [true]
  --elimands=bool            find AND gates [true]
  --elimbackward=bool        eager backward subsumption [true]
  --elimboundmax=-1..2e6     maximum elimination bound [16]
  --elimboundmin=-1..2e6     minimum elimination bound [0]
  --elimclslim=2..2e9        resolvent size limit [1e2]
  --elimdef=bool             mine definitions with kitten [false]
  --elimdefcores=1..100      number of unsat cores [1]
  --elimdefticks=0..2e9      kitten ticks limit [2e5]
  --elimeffort=1..1e5        relative efficiency per mille [1e3]
  --elimequivs=bool          find equivalence gates [true]
  --elimint=1..2e9           elimination interval [2e3]
  --elimites=bool            find if-then-else gates [true]
  --elimlimited=bool         limit resolutions [true]
  --elimmaxeff=0..2e9        maximum elimination efficiency [2e9]
  --elimmineff=0..2e9        minimum elimination efficiency [1e7]
  --elimocclim=0..2e9        occurrence limit [1e2]
  --elimprod=0..1e4          elim score product weight [1]
  --elimrounds=1..512        usual number of rounds [2]
  --elimsubst=bool           elimination by substitution [true]
  --elimsum=0..1e4           elimination score sum weight [1]
  --elimxorlim=2..27         maximum XOR size [5]
  --elimxors=bool            find XOR gates [true]
  --emadecisions=1..2e9      window decision rate [1e5]
  --emagluefast=1..2e9       window fast glue [33]
  --emaglueslow=1..2e9       window slow glue [1e5]
  --emajump=1..2e9           window back-jump level [1e5]
  --emalevel=1..2e9          window back-track level [1e5]
  --emasize=1..2e9           window learned clause size [1e5]
  --ematrailfast=1..2e9      window fast trail [1e2]
  --ematrailslow=1..2e9      window slow trail [1e5]
  --exteagerreasons=bool     eagerly ask for all reasons (0: only when needed) [true]
  --exteagerrecalc=bool      after eagerly asking for reasons recalculate all levels (0: trust the external tool) [true]
  --externallrat=bool        external lrat [false]
  --factor=bool              bounded variable addition [true]
  --factorcandrounds=0..2e9  candidates reduction rounds [2]
  --factorcheck=0..2         API checks that variables have been declared (1 = only with factor on, 2 = always) [1]
  --factordelay=0..12        delay bounded variable addition between eliminations [4]
  --factoreffort=0..1e6      relative effort per mille [50]
  --factoriniticks=1..1e6    initial effort in millions [300]
  --factorsize=2..2e9        clause size limit [5]
  --factorthresh=0..100      delay if ticks smaller thresh*clauses [7]
  --factorunbump=bool        extension variable with lowest importance [1: as in kissat] [true]
  --fastelim=bool            fast BVE during preprocessing [true]
  --fastelimbound=1..1e3     fast BVE bound during preprocessing [8]
  --fastelimclslim=2..2e9    fast BVE resolvent size limit [1e2]
  --fastelimocclim=1..2e9    fast BVE occurence limit during preprocessing [100]
  --fastelimrounds=1..512    number of fastelim rounds [4]
  --flush=bool               flush redundant clauses [false]
  --flushfactor=1..1e3       interval increase [3]
  --flushint=1..2e9          initial limit [1e5]
  --forcephase=bool          always use initial phase [false]
  --frat=0..2                1=frat(lrat), 2=frat(drat) [0]
  --idrup=bool               incremental proof format [false]
  --ilb=0..2                 ILB (incremental lazy backtrack) (0: no, 1: assumptions only, 2: everything) [0]
  --incdecay=0..4            decay clauses when doing incremental clauses [1]
  --incdecayint=1..2e9       decay interval when doing incremental clauses [1e6]
  --inprobeint=1..2e9        inprobing interval [100]
  --inprobing=bool           enable probe inprocessing [true]
  --inprocessing=bool        enable general inprocessing [true]
  --instantiate=bool         variable instantiation [false]
  --instantiateclslim=2..2e9 minimum clause size [3]
  --instantiateocclim=1..2e9 maximum occurrence limit [1]
  --instantiateonce=bool     instantiate each clause once [true]
  --lidrup=bool              linear incremental proof format [false]
  --lrat=bool                use LRAT proof format [false]
  --lucky=bool               lucky phases [true]
  --luckyassumptions=bool    lucky phases with assumptions [true]
  --luckyearly=bool          lucky phases before preprocessing [true]
  --luckylate=bool           lucky phases after preprocessing [true]
  --minimize=bool            minimize learned clauses [true]
  --minimizedepth=0..1e3     minimization depth [1e3]
  --minimizeticks=bool       increment ticks in minimization [true]
  --otfs=bool                on-the-fly self subsumption [true]
  --phase=bool               initial phase [true]
  --preprocessinit=0..2e9    initial preprocessing base limit [2e6]
  --preprocesslight=bool     lightweight preprocessing [true]
  --probe=bool               failed literal probing [true]
  --probeeffort=1..1e5       relative efficiency per mille [8]
  --probehbr=bool            learn hyper binary clauses [true]
  --probethresh=0..100       delay if ticks smaller thresh*clauses [0]
  --profile=0..4             profiling level [2]
  --quiet=bool               disable all messages [false]
  --radixsortlim=0..2e9      radix sort limit [32]
  --randec=bool              random decisions [false]
  --randecfocused=bool       random decisions in focused mode [true]
  --randecinit=2..2e9        inital random decision interval [1e3]
  --randecint=0..2e9         random conflict length [500]
  --randeclength=1..1e9      length random decisions phases [10]
  --randecstable=bool        random decisions in stable mode [false]
  --realtime=bool            real instead of process time [false]
  --recomputetier=bool       recompute tiers [true]
  --reduce=bool              reduce useless clauses [true]
  --reduceinit=1..1e6        initial interval [300]
  --reduceint=2..1e6         reduce interval [25]
  --reduceopt=0..2           0=prct,1=sqrt,2=max [1]
  --reducetarget=10..1e2     reduce fraction in percent [75]
  --reducetier1glue=1..2e9   glue of kept learned clauses [2]
  --reducetier2glue=1..2e9   glue of tier two clauses [6]
  --reluctant=bool           stable reluctant doubling restarts [true]
  --reluctantint=0..2e9      reluctant doubling period [1024]
  --reluctantmax=0..2e9      maximum reluctant doubling period [1048576]
  --rephase=0..2             enable resetting phase (0=no,1=always,2=stable-only) [1]
  --rephaseint=1..2e9        rephase interval [1e3]
  --report=bool              enable reporting [false]
  --reportall=bool           report even if not successful [false]
  --reportsolve=bool         use solving not process time [false]
  --restart=bool             enable restarts [true]
  --restartint=1..2e9        restart interval [2]
  --restartmarginfocused=0..25 focused slow fast margin in percent [10]
  --restartmarginstable=0..25 stable slow fast margin in percent [25]
  --restartreusetrail=bool   enable trail reuse [true]
  --restoreall=0..2          restore all clauses (2=really) [0]
  --restoreflush=bool        remove satisfied clauses [false]
  --reverse=bool             reverse variable ordering [false]
  --score=bool               use EVSIDS scores [true]
  --scorefactor=500..1e3     score factor per mille [950]
  --seed=0..2e9              random seed [0]                         ------ DONE
  --shrink=0..3              shrink conflict clause (1=binary-only,2=minimize-on-pulling,3=full) [3]
  --shrinkreap=bool          use a reap for shrinking [true]
  --shuffle=bool             shuffle variables [false]
  --shufflequeue=bool        shuffle variable queue [true]
  --shufflerandom=bool       not reverse but random [false]
  --shufflescores=bool       shuffle variable scores [true]
  --stabilize=bool           enable stabilizing phases [true]
  --stabilizeinit=1..2e9     stabilizing interval [1e3]
  --stabilizeonly=bool       only stabilizing phases [false]
  --stats=bool               print all statistics at the end of the run [false]
  --stubbornIOfocused=bool   force phases to I/O in focused mode every once in a while (requires rephase==2) [false]
  --subsume=bool             enable clause subsumption [true]
  --subsumebinlim=0..2e9     watch list length limit [1e4]
  --subsumeclslim=0..2e9     clause length limit [1e2]
  --subsumeeffort=1..1e5     relative efficiency per mille [1e3]
  --subsumelimited=bool      limit subsumption checks [true]
  --subsumemaxeff=0..2e9     maximum subsuming efficiency [1e8]
  --subsumemineff=0..2e9     minimum subsuming efficiency [0]
  --subsumeocclim=0..2e9     watch list length limit [1e2]
  --subsumestr=bool          subsume strengthen [true]
  --sweep=bool               enable SAT sweeping [true]
  --sweepclauses=0..2e9      environment clauses [1024]
  --sweepcomplete=bool       run SAT sweeping to completion [false]
  --sweepcountbinary=bool    count binaries to environment [true]
  --sweepdepth=0..2e9        environment depth [2]
  --sweepeffort=0..1e4       relative effort in ticks per mille [1e2]
  --sweepfliprounds=0..2e9   flipping rounds [1]
  --sweepmaxclauses=2..2e9   maximum environment clauses [3e5]
  --sweepmaxdepth=1..2e9     maximum environment depth [3]
  --sweepmaxvars=2..2e9      maximum environment variables [8192]
  --sweeprand=bool           randomize sweeping environment [false]
  --sweepthresh=0..100       delay if ticks smaller thresh*clauses [5]
  --sweepvars=0..2e9         environment variables [256]
  --target=0..2              target phases (1=stable only) [1]
  --terminateint=0..1e4      termination check interval [10]
  --ternary=bool             hyper ternary resolution [true]
  --ternaryeffort=1..1e5     relative efficiency per mille [8]
  --ternarymaxadd=0..1e4     max clauses added in percent [1e3]
  --ternaryocclim=1..2e9     ternary occurrence limit [1e2]
  --ternaryrounds=1..16      maximum ternary rounds [2]
  --ternarythresh=0..100     delay if ticks smaller thresh*clauses [6]
  --tier1limit=0..100        limit of tier1 usage in percentage [50]
  --tier1minglue=0..100      lowest tier1 limit [0]
  --tier2limit=0..100        limit of tier2 usage in percentage [90]
  --tier2minglue=0..100      lowest tier2 limit [0]
  --transred=bool            transitive reduction of BIG [true]
  --transredeffort=1..1e5    relative efficiency per mille [1e2]
  --transredmaxeff=0..2e9    maximum efficiency [1e8]
  --transredmineff=0..2e9    minimum efficiency [0]
  --verbose=0..3             more verbose messages [0]              ------ DONE
  --veripb=0..4              odd=check-deletions, >2 drat [0]
  --vivify=bool              vivification [true]
  --vivifycalctier=bool      recalculate tier limits [false]
  --vivifydemote=bool        demote irredundant or delete directly [false]
  --vivifyeffort=0..1e5      overall efficiency per mille [50]
  --vivifyflush=bool         flush subsumed before vivification rounds [true]
  --vivifyinst=bool          instantiate last literal when vivify [true]
  --vivifyirred=bool         vivification of irredundant clauses [true]
  --vivifyirredeff=1..100    irredundant efficiency per mille [3]
  --vivifyonce=0..2          vivify once: 1=red, 2=red+irr [0]
  --vivifyretry=0..5         re-vivify clause if vivify was successful [0]
  --vivifyschedmax=10..2e9   maximum schedule size [5e3]
  --vivifythresh=0..100      delay if ticks smaller thresh*clauses [20]
  --vivifytier1=bool         vivification tier1 [true]
  --vivifytier1eff=0..100    relative tier1 effort [4]
  --vivifytier2=bool         vivification tier2 [true]
  --vivifytier2eff=1..100    relative tier2 effort [2]
  --vivifytier3=bool         vivification tier3 [true]
  --vivifytier3eff=1..100    relative tier3 effort [1]
  --walk=bool                enable random walks [true]
  --walkeffort=1..1e5        relative efficiency per mille [80]
  --walkfullocc=bool         use Kissat's full occurrences instead of the single watched [false]
  --walkmaxeff=0..2e9        maximum efficiency (in 1e3 ticks) [1e7]
  --walkmineff=0..1e7        minimum efficiency [0]
  --walknonstable=bool       walk in non-stabilizing phase [true]
  --walkredundant=bool       walk redundant clauses too [false]
  --warmup=bool              warmup before walk using propagation [true]

The internal options have their default value printed in brackets
after their description.  They can also be used in the form
'--<name>' which is equivalent to '--<name>=1' and in the form
'--no-<name>' which is equivalent to '--<name>=0'.  One can also
use 'true' instead of '1', 'false' instead of '0', as well as
numbers with positive exponent such as '1e3' instead of '1000'.

Alternatively option values can also be specified in the header
of the DIMACS file, e.g., 'c --elim=false', or through environment
variables, such as 'CADICAL_ELIM=false'.  The embedded options in
the DIMACS file have highest priority, followed by command line
options and then values specified through environment variables.

The input is read from '<input>' assumed to be in DIMACS format.
Incremental 'p inccnf' files are supported too with cubes at the end.
If '<proof>' is given then a DRAT proof is written to that file.

If '<input>' is missing then the solver reads from '<stdin>',
also if '-' is used as input path name '<input>'.  Similarly,

For incremental files each cube is solved in turn. The solver
stops at the first satisfied cube if there is one and uses that
one for the witness to print.  Conflict and decision limits are
applied to each individual cube solving call while '-P', '-L' and
'-t' remain global.  Only if all cubes were unsatisfiable the solver
prints the standard unsatisfiable solution line ('s UNSATISFIABLE').

By default the proof is stored in the binary DRAT format unless
the option '--no-binary' is specified or the proof is written
to '<stdout>' and '<stdout>' is connected to a terminal.

The input is assumed to be compressed if it is given explicitly
and has a '.gz', '.bz2', '.xz' or '.7z' suffix.  The same applies
to the output file.  In order to use compression and decompression
the corresponding utilities 'gzip', 'bzip', 'xz', and '7z' (depending
on the format) are required and need to be installed on the system.
The solver checks file type signatures though and falls back to
non-compressed file reading if the signature does not match.
*/

int64_t CCaDiCaL::Indicador(int id)
{
	if (id <= indicators.Count())
		return indicators[id];
	return TProcura::Indicador(id);
}
