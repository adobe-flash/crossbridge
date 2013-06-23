#ifndef TESTS_H
#define TESTS_H

#define LINEAR_DEPENDENCE      (1 << 0)
#define INDUCTION_VARIABLE     (1 << 1)
#define GLOBAL_DATA_FLOW       (1 << 2)
#define CONTROL_FLOW           (1 << 3)
#define SYMBOLICS              (1 << 4)
#define STATEMENT_REORDERING   (1 << 5)
#define LOOP_RESTRUCTURING     (1 << 6)
#define NODE_SPLITTING         (1 << 7)
#define EXPANSION              (1 << 8)
#define CROSSING_THRESHOLDS    (1 << 9)
#define REDUCTIONS             (1 << 10)
#define RECURRENCES            (1 << 11)
#define SEARCHING              (1 << 12)
#define PACKING                (1 << 13)
#define LOOP_REROLLING         (1 << 14)
#define EQUIVALENCING          (1 << 15)
#define INDIRECT_ADDRESSING    (1 << 16)
#define CONTROL_LOOPS          (1 << 17)

#endif // TESTS_H

