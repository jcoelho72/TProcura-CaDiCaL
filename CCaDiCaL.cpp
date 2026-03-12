#include "CCaDiCaL.h"
#include <stdio.h>
#include <algorithm>
#include <random>
#include <iterator>

// caminho para o executável do solver
TString CCaDiCaL::solver = "./cadical/build/cadical";

void CCaDiCaL::ResetParametros()
{
	TVector<TString> trueFalse = { "false", "true" };

	TProcura::ResetParametros();

	// changing parameters defined in TProcura
	parametro[ALGORITMO] = { "Solver",1,1,1,"Solver of a SAT", { "CaDiCaL" } };
	//parametro[NIVEL_DEBUG] = { "verbose",0,0,3,"more verbose messages" }; /// cannot be changed since is used to congtrol level of information shown during the run
	parametro[SEMENTE] = { "seed",0,0,2000000000,"random seed" };
	parametro[LIMITE_TEMPO] = { "t",10,1,86400,"set wall clock time limit" };
	parametro[LIMITE_ITERACOES] = { "c",0,0,1000000000,"limit the number of conflicts (default unlimited)" };

	// adding CaDiCaL main parameters
	parametro += { "d", 0, 0, 1000000000, "limit the number of decisions (default unlimited)" };
	parametro += { "L", 0, 0, 1000, "run local search initially (default '0' rounds)" };
	parametro += { "P", 0, 0, 1000, "initial preprocessing (default '0' rounds)" };
	parametro += { "Defined configurations", 0, 0, 4, "pre-defined configurations of advanced internal options: default (set default advanced internal options); plain (disable all internal preprocessing options); sat (set internal options to target satisfiable instances); unsat (set internal options to target unsatisfiable instances).",
		{ "no configuration", "default", "plain", "sat", "unsat" }};

	// adding CaDiCaL full set of parameters
	parametro += {
		{ "arena", 1, 0, 1, "allocate clauses in arena[true]", trueFalse },
		{ "arenacompact", 1, 0, 1, "keep clauses compact[true]", trueFalse, { ARENA,1 } },
		{ "arenasort", 1, 0, 1, "sort clauses in arena[true]", trueFalse, { ARENA,1 } },
		{ "arenatype", 3, 1, 3, "1 = clause, 2 = var, 3 = queue[3]", {}, {ARENA,1} }};
	parametro += {
		{ "backbone", 1, 0, 2, "binary clause backbone[1]" },
		{ "backboneeffort", 20, 0, 100000, "binary effort in per mile[20]", {}, { BACKBONE,1 } },
		{ "backbonemaxrounds", 1000, 0, 100000, "backbone rounds limit[1e3]", {}, { BACKBONE,1 } },
		{ "backbonerounds", 100, 0, 100000, "backbone rounds limit[100]", {}, { BACKBONE,1 } },
		{ "backbonethresh", 5, 0, 1000000000, "delay if ticks smaller thresh* clauses[5]", {}, { BACKBONE,1 } }};
	parametro += { "binary", 1, 0, 1, "use binary proof format[true]", trueFalse };
	parametro += {
		{ "block", 0, 0, 1, "blocked clause elimination[false]", trueFalse },
		{ "blockmaxclslim", 100000, 1, 2000000000, "maximum clause size[1e5]", {}, { BLOCK,1 } },
		{ "blockminclslim", 2, 2, 2000000000, "minimum clause size[2]", {}, { BLOCK,1 } },
		{ "blockocclim", 100, 1, 2000000000, "occurrence limit[1e2]", {}, { BLOCK,1 } }};
	parametro += {
		{ "bump", 1, 0, 1, "bump variables[true]", trueFalse },
		{ "bumpreason", 1, 0, 1, "bump reason literals too[true]", trueFalse, { BUMP,1 } },
		{ "bumpreasondepth", 1, 1, 3, "bump reason depth[1]", {}, { BUMPREASON,1 } },
		{ "bumpreasonlimit", 10, 1, 2000000000, "bump reason limit[10]", {}, { BUMPREASON,1 } },
		{ "bumpreasonrate", 100, 1, 2000000000, "bump reason decision rate[100]", {}, { BUMPREASON,1 } }};
	parametro += {
		{ "check", 0, 0, 1, "enable internal checking[false]", trueFalse },
		{ "checkassumptions", 1, 0, 1, "check assumptions satisfied[true]", trueFalse, { CHECK,1 } },
		{ "checkconstraint", 1, 0, 1, "check constraint satisfied[true]", trueFalse, { CHECK,1 } },
		{ "checkfailed", 1, 0, 1, "check failed literals form core[true]", trueFalse, { CHECK,1 } },
		{ "checkfrozen", 0, 0, 1, "check all frozen semantics[false]", trueFalse, { CHECK,1 } },
		{ "checkproof", 3, 0, 3, "1 = drat, 2 = lrat, 3 = both[3]", {}, { CHECK,1 } },
		{ "checkwitness", 1, 0, 1, "check witness internally[true]", trueFalse, { CHECK,1 } }};
	parametro += {
		{ "chrono", 1, 0, 2, "chronological backtracking[1]"},
		{ "chronoalways", 0, 0, 1, "force always chronological[false]", trueFalse, { CHRONO,1,2 } },
		{ "chronolevelim", 100, 0, 2000000000, "chronological level limit[1e2]", {}, { CHRONO,1,2 } },
		{ "chronoreusetrail", 1, 0, 1, "reuse trail chronologically[true]", trueFalse, { CHRONO,1,2 } }};
	parametro += {
		{ "compact", 1, 0, 1, "compact internal variables[true]", trueFalse },
		{ "compactint", 2000, 1, 2000000000, "compacting interval[2e3]", {}, { COMPACT,1 } },
		{ "compactlim", 100, 0, 1000, "inactive limit per mille[1e2]", {}, { COMPACT,1 } },
		{ "compactmin", 100, 1, 2000000000, "minimum inactive limit[1e2]", {}, { COMPACT,1 } }};
	parametro += {
		{ "condition", 0, 0, 1, "globally blocked clause elim[false]", trueFalse },
		{ "conditioneffort", 100, 1, 100000, "relative efficiency per mille[100]", {}, { CONDITION,1 } },
		{ "conditionint", 10000, 1, 2000000000, "initial conflict interval[1e4]", {}, { CONDITION,1 } },
		{ "conditionmaxeff", 10000000, 0, 2000000000, "maximum condition efficiency[1e7]", {}, { CONDITION,1 } },
		{ "conditionmaxrat", 100, 1, 2000000000, "maximum clause variable ratio[100]", {}, { CONDITION,1 } },
		{ "conditionmineff", 0, 0, 2000000000, "minimum condition efficiency[0]", {}, { CONDITION,1 } }};
	parametro += {
		{ "congruence", 1, 0, 1, "congruence closure[true]", trueFalse },
		{ "congruenceand", 1, 0, 1, "extract AND gates[true]", trueFalse, { CONGRUENCE,1 } },
		{ "congruenceandarity", 1000000, 2, 50000000, "AND gate arity limit[1e6]", {}, { CONGRUENCEAND,1 } },
		{ "congruencebinaries", 1, 0, 1, "extract binary and strengthen ternary clauses[true]", trueFalse, { CONGRUENCE,1 } },
		{ "congruenceite", 1, 0, 1, "extract ITE gates[true]", trueFalse, { CONGRUENCE,1 } },
		{ "congruencexor", 1, 0, 1, "extract XOR gates[true]", trueFalse, { CONGRUENCE,1 } },
		{ "congruencexorarity", 4, 2, 31, "XOR gate arity limit[4]", {}, { CONGRUENCEXOR,1 } },
		{ "congruencexorcounts", 1, 1, 5000000, "XOR gate round[1]", {}, { CONGRUENCEXOR,1 } }};
	parametro += {
		{ "cover", 0, 0, 1, "covered clause elimination[false]", trueFalse },
		{ "covereffort", 4, 1, 100000, "relative efficiency per mille[4]", {}, { COVER,1 } },
		{ "covermaxclslim", 100000, 1, 2000000000, "maximum clause size[1e5]", {}, { COVER,1 } },
		{ "covermaxeff", 100000000, 0, 2000000000, "maximum cover efficiency[1e8]", {}, { COVER,1 } },
		{ "coverminclslim", 2, 2, 2000000000, "minimum clause size[2]", {}, { COVER,1 } },
		{ "covermineff", 0, 0, 2000000000, "minimum cover efficiency[0]", {}, { COVER,1 } }};
	parametro += {
		{ "decompose", 1, 0, 1, "decompose BIG in SCCs and ELS[true]", trueFalse },
		{ "decomposerounds", 2, 1, 16, "number of decompose rounds[2]", {}, { DECOMPOSE,1 } }};
	parametro += {
		{ "deduplicate", 1, 0, 1, "remove duplicated binaries[true]", trueFalse },
		{ "deduplicateallinit", 0, 0, 1, "remove duplicated clauses once before solving[false]", trueFalse, { DEDUPLICATE,1 } }};
	parametro += {
		{ "eagersubsume", 1, 0, 1, "subsume recently learned[true]", trueFalse },
		{ "eagersubsumelim", 20, 1, 1000, "limit on subsumed candidates[20]", {}, { EAGERSUBSUME,1 } }};
	parametro += {
		{ "elim", 1, 0, 1, "bounded variable elimination[true]", trueFalse },
		{ "elimands", 1, 0, 1, "find AND gates[true]", trueFalse, { ELIM,1 } },
		{ "elimbackward", 1, 0, 1, "eager backward subsumption[true]", trueFalse, { ELIM,1 } },
		{ "elimboundmax", 16, -1, 2000000, "maximum elimination bound[16]", {}, { ELIM,1 } },
		{ "elimboundmin", 0, -1, 2000000, "minimum elimination bound[0]", {}, { ELIM,1 } },
		{ "elimclslim", 100, 2, 2000000000, "resolvent size limit[1e2]", {}, { ELIM,1 } },
		{ "elimdef", 0, 0, 1, "mine definitions with kitten[false]", trueFalse, { ELIM,1 } },
		{ "elimdefcores", 1, 1, 100, "number of unsat cores[1]", {}, { ELIM,1 } },
		{ "elimdefticks", 200000, 0, 2000000000, "kitten ticks limit[2e5]", {}, { ELIM,1 } },
		{ "elimeffort", 1000, 1, 100000, "relative efficiency per mille[1e3]", {}, { ELIM,1 } },
		{ "elimequivs", 1, 0, 1, "find equivalence gates[true]", trueFalse, { ELIM,1 } },
		{ "elimint", 2000, 1, 2000000000, "elimination interval[2e3]", {}, { ELIM,1 } },
		{ "elimites", 1, 0, 1, "find if - then - else gates[true]", trueFalse, { ELIM,1 } },
		{ "elimlimited", 1, 0, 1, "limit resolutions[true]", trueFalse, { ELIM,1 } },
		{ "elimmaxeff", 1000000000, 0, 2000000000, "maximum elimination efficiency[1e9]", {}, { ELIM,1 } },
		{ "elimmineff", 10000000, 0, 2000000000, "minimum elimination efficiency[1e7]", {}, { ELIM,1 } },
		{ "elimocclim", 100, 0, 2000000000, "occurrence limit[1e2]", {}, { ELIM,1 } },
		{ "elimprod", 1, 0, 10000, "elim score product weight[1]", {}, { ELIM,1 } },
		{ "elimrounds", 2, 1, 512, "usual number of rounds[2]", {}, { ELIM,1 } },
		{ "elimsubst", 1, 0, 1, "elimination by substitution[true]", trueFalse, { ELIM,1 } },
		{ "elimsum", 1, 0, 10000, "elimination score sum weight[1]", {}, { ELIM,1 } },
		{ "elimxorlim", 5, 2, 27, "maximum XOR size[5]", {}, { ELIM,1 } },
		{ "elimxors", 1, 0, 1, "find XOR gates[true]", trueFalse, { ELIM,1 } }};
	parametro += { "emadecisions", 100000, 1, 2000000000, "window decision rate[1e5]"};
	parametro += { "emagluefast", 33, 1, 2000000000, "window fast glue[33]"};
	parametro += { "emaglueslow", 100000, 1, 2000000000, "window slow glue[1e5]"};
	parametro += { "emajump", 100000, 1, 2000000000, "window back - jump level[1e5]"};
	parametro += { "emalevel", 100000, 1, 2000000000, "window back - track level[1e5]"};
	parametro += { "emasize", 100000, 1, 2000000000, "window learned clause size[1e5]"};
	parametro += { "ematrailfast", 100, 1, 2000000000, "window fast trail[1e2]"};
	parametro += { "ematrailslow", 100000, 1, 2000000000, "window slow trail[1e5]"};
	parametro += { "exteagerreasons", 1, 0, 1, "eagerly ask for all reasons(0: only when needed)[true]", trueFalse };
	parametro += { "exteagerrecalc", 1, 0, 1, "after eagerly asking for reasons recalculate all levels(0: trust the external tool)[true]", trueFalse };
	parametro += { "externallrat", 0, 0, 1, "external lrat[false]", trueFalse };
	parametro += {
		{ "factor", 1, 0, 1, "bounded variable addition[true]", trueFalse },
		{ "factorcandrounds", 2, 0, 2000000000, "candidates reduction rounds[2]", {}, { FACTOR,1 } },
		{ "factorcheck", 1, 0, 2, "API checks that variables have been declared(1 = only with factor on, 2 = always)[1]", {}, { FACTOR,1 } },
		{ "factordelay", 4, 0, 12, "delay bounded variable addition between eliminations[4]", {}, { FACTOR,1 } },
		{ "factoreffort", 50, 0, 100000, "relative effort per mille[50]", {}, { FACTOR,1 } },
		{ "factoriniticks", 300, 1, 1000000, "initial effort in millions[300]", {}, { FACTOR,1 } },
		{ "factorsize", 5, 2, 2000000000, "clause size limit[5]", {}, { FACTOR,1 } },
		{ "factorthresh", 7, 0, 100, "delay if ticks smaller thresh* clauses[7]", {}, { FACTOR,1 } },
		{ "factorunbump", 1, 0, 1, "extension variable with lowest importance[1:as in kissat][true]", trueFalse, { FACTOR,1 } }};
	parametro += {
		{ "fastelim", 1, 0, 1, "fast BVE during preprocessing[true]", trueFalse },
		{ "fastelimbound", 8, 1, 1000, "fast BVE bound during preprocessing[8]", {}, { FASTELIM,1 } },
		{ "fastelimclslim", 100, 2, 2000000000, "fast BVE resolvent size limit[1e2]", {}, { FASTELIM,1 } },
		{ "fastelimocclim", 100, 1, 2000000000, "fast BVE occurence limit during preprocessing[100]", {}, { FASTELIM,1 } },
		{ "fastelimrounds", 4, 1, 512, "number of fastelim rounds[4]", {}, { FASTELIM,1 } }};
	parametro += {
		{ "flush", 0, 0, 1, "flush redundant clauses[false]", trueFalse },
		{ "flushfactor", 3, 1, 1000, "interval increase[3]", {}, { FLUSH,1 } },
		{ "flushint", 100000, 1, 2000000000, "initial limit[1e5]", {}, { FLUSH,1 } }};
	parametro += { "forcephase", 0, 0, 1, "always use initial phase[false]", trueFalse };
	parametro += { "frat", 0, 0, 2, "1 = frat(lrat), 2 = frat(drat)[0]"};
	parametro += { "idrup", 0, 0, 1, "incremental proof format[false]", trueFalse };
	parametro += { "ilb", 0, 0, 2, "ILB(incremental lazy backtrack) (0: no, 1 : assumptions only, 2 : everything)[0]"};
	parametro += {
		{ "incdecay", 1, 0, 4, "decay clauses when doing incremental clauses[1]"},
		{ "incdecayint", 1000000, 1, 2000000000, "decay interval when doing incremental clauses[1e6]" }};
	parametro += {
		{ "inprobeint", 100, 1, 2000000000, "inprobing interval[100]", {}, { INPROBING, 1 } },
		{ "inprobing", 1, 0, 1, "enable probe inprocessing[true]", trueFalse }};
	parametro += { "inprocessing", 1, 0, 1, "enable general inprocessing[true]", trueFalse };
	parametro += {
		{ "instantiate", 0, 0, 1, "variable instantiation[false]", trueFalse },
		{ "instantiateclslim", 3, 2, 2000000000, "minimum clause size[3]", {}, { INSTANTIATE,1 } },
		{ "instantiateocclim", 1, 1, 2000000000, "maximum occurrence limit[1]", {}, { INSTANTIATE,1 } },
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
		{ "minimizedepth", 1000, 0, 1000, "minimization depth[1e3]", {}, { MINIMIZE, 1 } },
		{ "minimizeticks", 1, 0, 1, "increment ticks in minimization[true]", trueFalse, { MINIMIZE, 1 } }};
	parametro += { "otfs", 1, 0, 1, "on - the - fly self subsumption[true]", trueFalse };
	parametro += { "phase", 1, 0, 1, "initial phase[true]", trueFalse };
	parametro += { "preprocessinit", 2000000, 0, 2000000000, "initial preprocessing base limit[2e6]"};
	parametro += { "preprocesslight", 1, 0, 1, "lightweight preprocessing[true]", trueFalse };
	parametro += {
		{ "probe", 1, 0, 1, "failed literal probing[true]", trueFalse },
		{ "probeeffort", 8, 1, 100000, "relative efficiency per mille[8]", {}, { PROBE, 1 } },
		{ "probehbr", 1, 0, 1, "learn hyper binary clauses[true]", trueFalse, { PROBE, 1 } },
		{ "probethresh", 0, 0, 100, "delay if ticks smaller thresh* clauses[0]", {}, { PROBE, 1 } }};
	parametro += { "profile", 2, 0, 4, "profiling level[2]"};
	parametro += { "quiet", 0, 0, 1, "disable all messages[false]", trueFalse };
	parametro += { "radixsortlim", 32, 0, 2000000000, "radix sort limit[32]"};
	parametro += {
		{ "randec", 0, 0, 1, "random decisions[false]", trueFalse },
		{ "randecfocused", 1, 0, 1, "random decisions in focused mode[true]", trueFalse, { RANDEC,1 } },
		{ "randecinit", 1000, 2, 2000000000, "inital random decision interval[1e3]", {}, { RANDEC,1 } },
		{ "randecint", 500, 0, 2000000000, "random conflict length[500]", {}, { RANDEC,1 } },
		{ "randeclength", 10, 1, 1000000000, "length random decisions phases[10]", {}, { RANDEC,1 } },
		{ "randecstable", 0, 0, 1, "random decisions in stable mode[false]", trueFalse, { RANDEC,1 } }};
	parametro += { "realtime", 0, 0, 1, "real instead of process time[false]", trueFalse };
	parametro += { "recomputetier", 1, 0, 1, "recompute tiers[true]", trueFalse };
	parametro += {
		{ "reduce", 1, 0, 1, "reduce useless clauses[true]", trueFalse },
		{ "reduceinit", 300, 1, 1000000, "initial interval[300]", {}, { REDUCE,1 } },
		{ "reduceint", 25, 2, 1000000, "reduce interval[25]", {}, { REDUCE,1 } },
		{ "reduceopt", 1, 0, 2, "0 = prct, 1 = sqrt, 2 = max[1]", {}, { REDUCE,1 } },
		{ "reducetarget", 75, 10, 100, "reduce fraction in percent[75]", {}, { REDUCE,1 } },
		{ "reducetier1glue", 2, 1, 2000000000, "glue of kept learned clauses[2]", {}, { REDUCE,1 } },
		{ "reducetier2glue", 6, 1, 2000000000, "glue of tier two clauses[6]", {}, { REDUCE,1 } }};
	parametro += {
		{ "reluctant", 1, 0, 1, "stable reluctant doubling restarts[true]", trueFalse },
		{ "reluctantint", 1024, 0, 2000000000, "reluctant doubling period[1024]", {}, { RELUCTANT,1 } },
		{ "reluctantmax", 1048576, 0, 2000000000, "maximum reluctant doubling period[1048576]", {}, { RELUCTANT,1 } }};
	parametro += {
		{ "rephase", 1, 0, 2, "enable resetting phase(0 = no, 1 = always, 2 = stable - only)[1]"},
		{ "rephaseint", 1000, 1, 2000000000, "rephase interval[1e3]", {}, { REPHASE,1 } }};
	parametro += {
		{ "report", 0, 0, 1, "enable reporting[false]", trueFalse },
		{ "reportall", 0, 0, 1, "report even if not successful[false]", trueFalse, { REPORT,1 } },
		{ "reportsolve", 0, 0, 1, "use solving not process time[false]", trueFalse, { REPORT,1 } }};
	parametro += {
		{ "restart", 1, 0, 1, "enable restarts[true]", trueFalse },
		{ "restartint", 100000, 1, 2000000000, "restart interval[1e5]", {}, { RESTART,1 } },
		{ "restartmarginfocused", 10, 0, 25, "focused slow fast margin in percent[10]", {}, { RESTART,1 } },
		{ "restartmarginstable", 25, 0, 25, "stable slow fast margin in percent[25]", {}, { RESTART,1 } },
		{ "restartreusetrail", 1, 0, 1, "enable trail reuse[true]", trueFalse, { RESTART,1 } }};
	parametro += { "restoreall", 0, 0, 2, "restore all clauses(2 = really)[0]"};
	parametro += { "restoreflush", 0, 0, 1, "remove satisfied clauses[false]", trueFalse };
	parametro += { "reverse", 0, 0, 1, "reverse variable ordering[false]", trueFalse };
	parametro += {
		{ "score", 1, 0, 1, "use EVSIDS scores[true]", trueFalse },
		{ "scorefactor", 950, 500, 1000, "score factor per mille[950]", {}, { SCORE,1 } }};
	parametro += {
		{ "shrink", 3, 0, 3, "shrink conflict clause(1 = binary - only, 2 = minimize - on - pulling, 3 = full)[3]"},
		{ "shrinkreap", 1, 0, 1, "use a reap for shrinking[true]", trueFalse, { SHRINK,1 } }};
	parametro += {
		{ "shuffle", 0, 0, 1, "shuffle variables[false]", trueFalse },
		{ "shufflequeue", 1, 0, 1, "shuffle variable queue[true]", trueFalse, { SHUFFLE,1 } },
		{ "shufflerandom", 0, 0, 1, "not reverse but random[false]", trueFalse, { SHUFFLE,1 } },
		{ "shufflescores", 1, 0, 1, "shuffle variable scores[true]", trueFalse, { SHUFFLE,1 } }};
	parametro += {
		{ "stabilize", 1, 0, 1, "enable stabilizing phases[true]", trueFalse },
		{ "stabilizeinit", 1000, 1, 2000000000, "stabilizing interval[1e3]", {}, { STABILIZE,1 } },
		{ "stabilizeonly", 0, 0, 1, "only stabilizing phases[false]", trueFalse, { STABILIZE,1 } }};
	parametro += { "stats", 0, 0, 1, "print all statistics at the end of the run[false]", trueFalse };
	parametro += { "stubbornIOfocused", 0, 0, 1, "force phases to I / O in focused mode every once in a while (requires rephase == 2)[false]", trueFalse };
	parametro += {
		{ "subsume", 1, 0, 1, "enable clause subsumption[true]", trueFalse },
		{ "subsumebinlim", 10000, 0, 2000000000, "watch list length limit[1e4]", {}, { SUBSUME,1 } },
		{ "subsumeclslim", 100, 0, 2000000000, "clause length limit[1e2]", {}, { SUBSUME,1 } },
		{ "subsumeeffort", 1000, 1, 100000, "relative efficiency per mille[1e3]", {}, { SUBSUME,1 } },
		{ "subsumelimited", 1, 0, 1, "limit subsumption checks[true]", trueFalse, { SUBSUME,1 } },
		{ "subsumemaxeff", 100000000, 0, 2000000000, "maximum subsuming efficiency[1e8]", {}, { SUBSUME,1 } },
		{ "subsumemineff", 0, 0, 2000000000, "minimum subsuming efficiency[0]", {}, { SUBSUME,1 } },
		{ "subsumeocclim", 100, 0, 2000000000, "watch list length limit[1e2]", {}, { SUBSUME,1 } },
		{ "subsumestr", 1, 0, 1, "subsume strengthen[true]", trueFalse, { SUBSUME,1 } }};
	parametro += {
		{ "sweep", 1, 0, 1, "enable SAT sweeping[true]", trueFalse },
		{ "sweepclauses", 1024, 0, 2000000000, "environment clauses[1024]", {}, { SWEEP,1 } },
		{ "sweepcomplete", 0, 0, 1, "run SAT sweeping to completion[false]", trueFalse, { SWEEP,1 } },
		{ "sweepcountbinary", 1, 0, 1, "count binaries to environment[true]", trueFalse, { SWEEP,1 } },
		{ "sweepdepth", 2, 0, 2000000000, "environment depth[2]", {}, { SWEEP,1 } },
		{ "sweepeffort", 100, 0, 10000, "relative effort in ticks per mille[1e2]", {}, { SWEEP,1 } },
		{ "sweepfliprounds", 1, 0, 2000000000, "flipping rounds[1]", {}, { SWEEP,1 } },
		{ "sweepmaxclauses", 300000, 2, 2000000000, "maximum environment clauses[3e5]", {}, { SWEEP,1 } },
		{ "sweepmaxdepth", 3, 1, 2000000000, "maximum environment depth[3]", {}, { SWEEP,1 } },
		{ "sweepmaxvars", 8192, 2, 2000000000, "maximum environment variables[8192]", {}, { SWEEP,1 } },
		{ "sweeprand", 0, 0, 1, "randomize sweeping environment[false]", trueFalse, { SWEEP,1 } },
		{ "sweepthresh", 5, 0, 100, "delay if ticks smaller thresh* clauses[5]", {}, { SWEEP,1 } },
		{ "sweepvars", 256, 0, 2000000000, "environment variables[256]", {}, { SWEEP,1 } }};
	parametro += { "target", 1, 0, 2, "target phases(1 = stable only)[1]"};
	parametro += { "terminateint", 10, 1, 10000, "termination check interval[10]"};
	parametro += {
		{ "ternary", 1, 0, 1, "hyper ternary resolution[true]", trueFalse },
		{ "ternaryeffort", 8, 1, 100000, "relative efficiency per mille[8]", {}, { TERNARY,1 } },
		{ "ternarymaxadd", 1000, 0, 10000, "max clauses added in percent[1e3]", {}, { TERNARY,1 } },
		{ "ternaryocclim", 100, 1, 2000000000, "ternary occurrence limit[1e2]", {}, { TERNARY,1 } },
		{ "ternaryrounds", 2, 1, 16, "maximum ternary rounds[2]", {}, { TERNARY,1 } },
		{ "ternarythresh", 6, 0, 100, "delay if ticks smaller thresh* clauses[6]", {}, { TERNARY,1 } }};
	parametro += { "tier1limit", 50, 0, 100, "limit of tier1 usage in percentage[50]"};
	parametro += { "tier1minglue", 0, 0, 100, "lowest tier1 limit[0]"};
	parametro += { "tier2limit", 90, 0, 100, "limit of tier2 usage in percentage[90]"};
	parametro += { "tier2minglue", 0, 0, 100, "lowest tier2 limit[0]"};
	parametro += {
		{ "transred", 1, 0, 1, "transitive reduction of BIG[true]", trueFalse },
		{ "transredeffort", 100, 1, 100000, "relative efficiency per mille[1e2]", {}, { TRANSRED,1 } },
		{ "transredmaxeff", 1000000000, 0, 2000000000, "maximum[1e9]", {}, { TRANSRED,1 } },
		{ "transredmineff", 0, 0, 2000000000, "minimum efficiency[0]", {}, { TRANSRED,1 } }};
	parametro += { "verbose", 0, 0, 3, "more verbose messages[0]"};
	parametro += { "veripb", 0, 0, 4, "odd = check - deletions, > 2 drat[0]"};
	parametro += {
		{ "vivify", 1, 0, 1, "vivification[true]", trueFalse },
		{ "vivifycalctier", 0, 0, 1, "recalculate tier limits[false]", trueFalse, { VIVIFY,1 } },
		{ "vivifydemote", 0, 0, 1, "demote irredundant or delete directly[false]", trueFalse, { VIVIFY,1 } },
		{ "vivifyefoort", 50, 0, 100000, "overall efficiency per mille[50]", {}, { VIVIFY,1 } },
		{ "vivifyflush", 1, 0, 1, "flush subsumed before vivification rounds[true]", trueFalse, { VIVIFY,1 } },
		{ "vivifyinst", 1, 0, 1, "instantiate last literal when vivify[true]", trueFalse, { VIVIFY,1 } },
		{ "vivifyirred", 1, 0, 1, "vivification of irredundant clauses[true]", trueFalse, { VIVIFY,1 } },
		{ "vivifyirredeff", 3, 1, 100, "irredundant efficiency per mille[3]", {}, { VIVIFY,1 } },
		{ "vivifyonce", 0, 0, 2, "vivify once : 1 = red, 2 = red + irr[0]", {}, { VIVIFY,1 } },
		{ "vivifyretry", 0, 0, 5, "re - vivify clause if vivify was successful[0]", {}, { VIVIFY,1 } },
		{ "vivifyschedmax", 5000, 10, 2000000000, "maximum schedule size[5e3]", {}, { VIVIFY,1 } },
		{ "vivifythresh", 20, 0, 100, "delay if ticks smaller thresh* clauses[20]", {}, { VIVIFY,1 } },
		{ "vivifytier1", 1, 0, 1, "vivification tier1[true]", trueFalse, { VIVIFY,1 } },
		{ "vivifytier1eff", 4, 0, 100, "relative tier1 effort[4]", {}, { VIVIFY,1 } },
		{ "vivifytier2", 1, 0, 1, "vivification tier2[true]", trueFalse, { VIVIFY,1 } },
		{ "vivifytier2eff", 2, 1, 100, "relative tier2 effort[2]", {}, { VIVIFY,1 } },
		{ "vivifytier3", 1, 0, 1, "vivification tier3[true]", trueFalse, { VIVIFY,1 } },
		{ "vivifytier3eff", 1, 1, 100, "relative tier3 effort[1]", {}, { VIVIFY,1 } }};
	parametro += {
		{ "walk", 1, 0, 1, "enable random walks[true]", trueFalse },
		{ "walkeffort", 80, 1, 100000, "relative efficiency per mille[80]", {}, { WALK,1 } },
		{ "walkfullocc", 0, 0, 1, "use Kissat's full occurrences instead of the single watched [false]", trueFalse, { WALK,1 } },
		{ "walkmaxeff", 10000000, 0, 2000000000, "maximum efficiency(in 1e3 ticks)[1e7]", {}, { WALK,1 } },
		{ "walkmineff", 0, 0, 10000000, "minimum efficiency[0]", {}, { WALK,1 } },
		{ "walknonstable", 1, 0, 1, "walk in non - stabilizing phase[true]", trueFalse, { WALK,1 } },
		{ "walkredundant", 0, 0, 1, "walk redundant clauses too[false]", trueFalse, { WALK,1 } }};
	parametro += { "warmup", 1, 0, 1, "warmup before walk using propagation[true]", trueFalse };

	// save all default parameters 
	defaultCadical.Count(parametro.Count());
	for (int i = 0; i < parametro.Count(); i++)
		defaultCadical[i] = parametro[i].valor;

	indicador[IND_ITERACOES] = { "IND_CONFLICTS", "number of conflicts", IND_CONFLICTS };
	indicador += {
		{ "IND_MEMORY", "maximum resident set size of process", IND_MEMORY },
		{ "IND_PROPAGATIONS", "number of literals propagated, by unit propagation", IND_PROPAGATIONS },
		{ "IND_TICKS", "internal propagation-effort counter", IND_TICKS },
		{ "IND_RESTARTS", "number of restart events during search", IND_RESTARTS },
		{ "IND_LEARNED", "number of learned clauses", IND_LEARNED },
		{ "IND_FIXED", "number of variables permanently fixed by simplification and unit propagation", IND_FIXED },
		{ "IND_MAXLEVEL", "maximal decision level observed during search", IND_MAXLEVEL },
		{ "IND_NUMVARS", "number of variables used in the conversion", IND_NUMVARS },
		{ "IND_NUMCLAUSES", "number of clauses used in the conversion", IND_NUMCLAUSES }
	};
	indAtivo += {IND_MEMORY, IND_PROPAGATIONS, IND_TICKS, IND_RESTARTS, IND_LEARNED, IND_FIXED, IND_MAXLEVEL,
		IND_NUMVARS, IND_NUMCLAUSES };

	instancia = { "Instance", 1,1,100000, "ID of the instance (appended to the file name)" };

	indicators.Count(indicador.Count()).Reset(0);
}


int CCaDiCaL::ExecutaAlgoritmo()
{
	TString resultFile, solFile, cmdSTR, options;
	int error;
	resultFile.printf("%s%d.txt", *ficheiroInstancia, mpiID);
	solFile.printf("%s%d.sol", *ficheiroInstancia, mpiID);
	// build options string, with just the non-default parameters
	// special options with just one - and no space
	for (auto i : { LOCAL_SEARCH, PREPROCESSING })
		if (Parametro(i) != defaultCadical[i])
			options.printf("-%s%d ", *parametro[i].nome, Parametro(i));
	// special options of just one -
	TVector<int> aux;
	aux += LIMITE_ITERACOES;
	aux += LIMIT_DECISIONS;
	for (auto i : aux)
		if (Parametro(i) != defaultCadical[i])
			options.printf("-%s %d ", *parametro[i].nome, Parametro(i));
	// options for non-default parameters with --
	for (auto i : { SEMENTE })
		if (Parametro(i) != defaultCadical[i])
			options.printf("--%s=%d ", *parametro[i].nome, Parametro(i));
	// check if is used defined configurations
	if (Parametro(DEFINED_CONFIGURATIONS) != defaultCadical[DEFINED_CONFIGURATIONS])
		options.printf("--%s ", *parametro[DEFINED_CONFIGURATIONS].nomeValores[Parametro(DEFINED_CONFIGURATIONS)]);
	// add the rest of internal CaDiCaL parameters if used
	for (int i = ARENA; i < PARAMETROS_CADICAL; i++)
		if (Parametro(i) != defaultCadical[i])
			options.printf("--%s=%d ", *parametro[i].nome, Parametro(i));

	//system(TString().printf("cat %s%d.cnf", *ficheiroInstancia, instancia.valor)); // print parameters to be used in the run

	// setup all parameters that are not default in the launch line
	cmdSTR.printf("exec %s -t %d -w %s %s %s%d.cnf > %s 2>&1",
		*solver,
		Parametro(LIMITE_TEMPO),
		*solFile,
		*options,
		*ficheiroInstancia, instancia.valor,
		*resultFile);

	// valor para indicador não processado
	indicators[IND_RESULTADO] = -10;

	error = system(cmdSTR); // lauch CaDiCaL solver
	if (WIFEXITED(error) && WEXITSTATUS(error) == 127) {
		printf("\nError launching CaDiCaL solver\nCommand line: %s", *cmdSTR);
		return 0;
	}

	if (WIFSIGNALED(error)) // crash real
	{
		// bug report
		int sig = WTERMSIG(error);
		indicators[IND_RESULTADO] = -1;
		int errorID = (TRand::rand() + mpiID) % 10000; 
		TVector<TString> errorData;
		errorData += TString().printf("CaDiCaL crashed with signal %d", sig);
		errorData += TString().printf("Command line: %s", *cmdSTR);
		errorData += TString().printf("Reproduce: %s -t %d %s input%d.cnf",
			*solver, Parametro(LIMITE_TEMPO), *options, errorID);
		errorData += TString().printf("Host: %s", getenv("HOSTNAME"));
		TString().printf("error%d.txt", errorID).writeLines(errorData);
		bool ok1 = (system(TString().printf("%s --version >> error%d.txt 2>&1", *solver, errorID)) == 0);
		// change input and output file to avoid deleting
		bool ok2 = (system(TString().printf("mv %s%d.cnf input%d.cnf", *ficheiroInstancia, mpiID, errorID)) == 0);
		bool ok3 = (system(TString().printf("mv %s output%d.txt", *resultFile, errorID)) == 0);
		if (ok1 && ok2 && ok3)
			printf("\nError launching CaDiCaL solver\nCommand line: %s\nInput and output files saved as input%d.cnf and output%d.txt",
				*cmdSTR, errorID, errorID);
		return 0;
	} 
	
	// terminou normalmente
	if(WIFEXITED(error)) {
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

		
		TVector<TString> marks = {
			"c total real time since initialization:",
			"c maximum resident set size of process:",
			"c conflicts:",
			"c propagations:",
			"c ticks:",
			"c restarts:",
			"c learned:",
			"c fixed:"
		};
		TVector<int> indMarks = {
			IND_TEMPO,
			IND_MEMORY,
			IND_CONFLICTS,
			IND_PROPAGATIONS,
			IND_TICKS,
			IND_RESTARTS,
			IND_LEARNED,
			IND_FIXED
		};
		int maxLevel = 0;
		bool inSolving = false;
		for (auto& linha : resultFile.readLines()) {
			// Detect lines:
			for (int i = 0; i < indMarks.Count(); i++) {
				if (strstr(linha, marks[i])) {
					double valor = atof(*linha + marks[i].Count() + 1);
					if (i == 0)
						valor *= 1000.0; // in milliseconds
					indicators[indMarks[i]] = (int)(valor + 0.5);
				}
			}
			if (strstr(linha, "[ solving ]"))
				inSolving = true;
			else if (strstr(linha, "[ result ]"))
				inSolving = false;
			if (inSolving && linha[0] == 'c' && isdigit(linha[2])) {
				double nums[32];
				if (sscanf(linha, "c %lg %lg %lg %lg %lg %lg %lg %lg %lg %lg %lg %lg %lg",
					nums, nums + 1, nums + 2, nums + 3, nums + 4, nums + 5, nums + 6,
					nums + 7, nums + 8, nums + 9, nums + 10, nums + 11, nums + 12) == 13) {
					if (nums[2] > maxLevel)
						maxLevel = (int)(nums[2] + 0.5);
				}
			}
		}
		indicators[IND_MAXLEVEL] = maxLevel;

		if(Parametro(NIVEL_DEBUG) < DETALHE)
			remove(resultFile);
		satSol = {};
		for (auto& linha : solFile.readLines()) {
			if (strstr(linha, "v ")) {
				for (auto& token : linha.tok()) {
					int var = atoi(token);
					if (var != 0)
						satSol += var;
				}
			}
		}
		if (Parametro(NIVEL_DEBUG) < DETALHE)
			remove(solFile);
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
	if (id < indicators.Count())
		return indicators[id];
	return TProcura::Indicador(id);
}

// At least one variable is true
TVector<TString> CCaDiCaL::AtLeastOne(const TVector<int>& vars) {
	TString clause;
	for (auto var : vars)
		clause.printf("%d ", var);
	clause.printf("0");
	return { clause };
}

// At most one variable is true
TVector<TString> CCaDiCaL::AtMostOne(const TVector<int>& vars, bool sequentialCounter) {
	TVector<TString> result;
	if (!sequentialCounter) {
		// at most one should be true
		for (int i = 0; i < vars.Count(); i++)
			for (int j = i + 1; j < vars.Count(); j++)
				result += TString().printf("%d %d 0", -vars[i], -vars[j]);
	}
	else if (sequentialCounter) {
		result = AtMostK(vars);
	}
	return result;
}

// At most K (Sequential Counter) -- add new variables 
TVector<TString> CCaDiCaL::AtMostK(const TVector<int>& vars, int K) {
	TVector<TString> result;
	int id = nextSCID++;     // ID único para esta restrição
	int n = vars.Count();

	if (n == 0 || K <= 0) {
		// AtMost0: todas as variáveis têm de ser falsas
		for (int x : vars)
			result += TString().printf("-%d 0", x);
		return result;
	}

	// criar variáveis auxiliares s(i,j)
	auto S = [&](int i, int j) {
		return Var(TString().printf("sc %d %d %d", id, i, j));
		};

	// Sequential Counter (Sinz 2005)
	// s(i,j) = 1 se entre x1..xi existem pelo menos j verdadeiros

	// 1. Para cada i: ¬x_i ∨ s(i,1)
	for (int i = 1; i <= n; i++) {
		int xi = vars[i - 1];
		result += TString().printf("-%d %d 0", xi, S(i, 1));
	}

	// 2. Propagação vertical: ¬s(i-1,j) ∨ s(i,j)
	for (int i = 2; i <= n; i++) {
		for (int j = 1; j <= K; j++) {
			result += TString().printf("-%d %d 0", S(i - 1, j), S(i, j));
		}
	}

	// 3. Propagação diagonal: ¬x_i ∨ ¬s(i-1,j-1) ∨ s(i,j)
	for (int i = 2; i <= n; i++) {
		int xi = vars[i - 1];
		for (int j = 2; j <= K+1; j++) {
			result += TString().printf("-%d -%d %d 0", xi, S(i - 1, j - 1), S(i, j));
		}
	}

	// 4. Proibir overflow: ¬s(i,K+1)
	for (int i = 1; i <= n; i++) {
		result += TString().printf("-%d 0", S(i, K + 1));
	}

	return result;
}



/// mostrar a solução satSol
void CCaDiCaL::MostrarSolucao() {
	// forma básica, mostra só positivos
	for (auto var : satSol)
		if (var > 0)
			printf("%d ", (int)var);
}

// IDs e variáveis
TString CCaDiCaL::Var(int ID) {
	if (ID >= 0 && ID < variaveis.Count())
		return variaveis[ID];
	return TString("WVI"); // wrong variable index
}

int CCaDiCaL::Var(TString var) {
	int hashID = var.Hash() % hashtable.Count();
	// localizar a variável
	for (auto& item : hashtable[hashID])
		if (variaveis[item] == var)
			return item;

	// não existe, criar
	variaveis += var;
	hashtable[hashID] += (variaveis.Count() - 1);
	return variaveis.Count() - 1;
}

void CCaDiCaL::ResetHashtable(int tamanho) {
	variaveis.Count(0);
	hashtable.Count(tamanho);
	for (auto& bucket : hashtable)
		bucket.Count(0);
	Var("WVI"); // adicionar uma variável para o ID começar em 1
}

TVector<TString> CCaDiCaL::CreateUnaryVar(const TString& prefix, int min, int max) {
	TVector<TString> result;

	// criar variáveis unárias
	TVector<int> u;
	for (int j = min; j <= max + 1; j++) 
		u += Var(TString().printf("%s %d", *prefix, j));

	// monotonia: u[j] ∨ ¬u[j+1]
	for (int j = 0; j < u.Count() - 1; j++) 
		result += TString().printf("%d -%d 0", u[j], u[j + 1]);

	// fixar a primeira a 1 e a última a 0
	result += TString().printf("%d 0", u.First()); // u[min] = 1
	result += TString().printf("-%d 0", u.Last()); // u[max+1] = 0

	return result;
}

