/*
 * CS:APP Data Lab
 */

/* Declare different function types */
typedef long long (*funct_t) (void);
typedef long long (*funct1_t)(long long);
typedef long long (*funct2_t)(long long, long long); 
typedef long long (*funct3_t)(long long, long long, long long); 

/* Combine all the information about a function and its tests as structure */
typedef struct {
    char *name;             /* String name */
    funct_t solution_funct; /* Function */
    funct_t test_funct;     /* Test function */
    long long args;               /* Number of function arguments */
    char *ops;              /* List of legal operators. Special case: "$" for floating polong long */
    long long op_limit;           /* Max number of ops allowed in solution */
    long long rating;             /* Problem rating (1 -- 4) */
    long long arg_ranges[3][2];   /* Argument ranges. Always defined for 3 args, even if */
                            /* the function takes fewer. Special case: First arg */
			    /* must be set to {1,1} for f.p. puzzles */
} test_rec, *test_ptr;

extern test_rec test_set[];







