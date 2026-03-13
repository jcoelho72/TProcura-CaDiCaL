# TProcura‑CaDiCaL
Execution of the CaDiCaL SAT solver integrated into the TProcura framework 
for parametric testing, performance evaluation, and SAT model construction.

## 1. Overview
TProcura‑CaDiCaL combines the CaDiCaL SAT solver with the TProcura execution framework, 
enabling large‑scale parametric experiments, automated result extraction, 
and optional SAT instance generation. The project supports two main usage scenarios:
- Users who already have SAT instances and want to evaluate CaDiCaL performance under different parameter configurations.
- Users who have a combinatorial problem and want to convert it to SAT using a modular API (CCaDiCaL) before solving it.

The framework supports:
- batch execution,
- MPI parallelism,
- automatic CSV generation with solver indicators,
- reproducible experiments with seeds,
- and a growing library of SAT modeling utilities.

## 2. Installation and Requirements
Dependencies
- C++ compiler with C++11 or later
- CaDiCaL (included as a submodule or external dependency)
- TProcura framework
- MPI (optional, for distributed execution)

Clonning
```bash
git clone ....
```

Building
```bash
make
```
Running a test
```bash
run.sh
```
Exemple with SAT instances from cadical project. In the run.sh the results are saved in
Resultados folder, in CSV files. 

Check file resultados.xlsx that process the results of CSV files.

(see also results from NDamas...)

## 3. Using Existing SAT Instances (Mode A)
This mode is intended for users who already have .cnf files and want to run CaDiCaL 
under different parameter configurations.

Features
- Run CaDiCaL with any combination of global parameters (P1–P263).
- Execute multiple seeds automatically (P3).
- Collect solver indicators such as:
  - I1(Resultado): 0 not solved, 1 solved (SAT), 2 solved (UNSAT), -1 crash, -2 timeout in conversion, -3 conversion limits reatch
  - I2(Tempo(ms)): time
  - I3(IND_CONFLICTS): number of conflicts
  - I4(IND_MEMORY): maximum resident set size of process (MB)
  - I5(IND_PROPAGATIONS): number of literals propagated, by unit propagation
  - I6(IND_TICKS): internal propagation-effort counter
  - I7(IND_RESTARTS): number of restart events during search
  - I8(IND_LEARNED): number of learned clauses
  - I9(IND_FIXED): number of variables permanently fixed by simplification and unit propagation
  - I10(IND_MAXLEVEL): maximal decision level observed during search
  - I11(IND_NUMVARS): number of variables used in the conversion
  - I12(IND_NUMCLAUSES): number of clauses used in the conversion

Workflow
- set instances in a directory 
- configure parameters in the command line
- use the CSV files (for exemple in an Excel file to analyze the results).

Typical use cases
- Parameter tuning for specific SAT families.
- Benchmarking CaDiCaL on custom datasets.
- Large‑scale experiments with MPI.

## 4. Converting Problems to SAT (Mode B)
Users who want to model a combinatorial problem and convert it to SAT can extend the CCaDiCaL class.

The CCaDiCaL API

The class provides utilities for:
- variable creation - mapping between integer IDs and variable names,
- clause construction,
- cardinality constraints,
- unary encodings,
- and structured SAT modeling.

Extending the framework
To create a new SAT encoding:
- Derive a class from CCaDiCaL.
- Define variables and constraints.
- Generate the CNF.
- Execute CaDiCaL through TProcura.

Example use case

The N‑Queens problem is implemented as a demonstration of:
- variable creation,
- row/column/diagonal constraints,
- multiple encoding strategies.

## 5. Project Architecture
The system is composed of four layers:
- TProcura: Manages parameters, seeds, repetitions, MPI distribution, and CSV output.
- CCaDiCaL: Provides a modular SAT modeling API, and calls CaDiCaL solver.
- CaDiCaL: The SAT solver used for execution.
- Analysis layer: CSV files ready to be processed in Power Query/Excel, Python or R.

## 6. Exemple of N Queens
The repository includes several predefined experiments:

Test 1
Validation of the pipeline and initial comparison of encoding methods.

Test 2
Performance comparison with conversion time removed as a bottleneck.

Test 3
Scalability curves per encoding method:

common region (N = 8–50),

extended region per method (up to N = 225),

100 seeds per N,

each method in its best SAT configuration.

These experiments provide a complete characterization of the encoding strategies.

## 7. Roadmap
Future developments may include:
- additional SAT constraints (totalizers, PB constraints, sorting networks),
- richer modeling utilities in CCaDiCaL,
- method to tune CaDiCaL’s 260 advanced parameters,

