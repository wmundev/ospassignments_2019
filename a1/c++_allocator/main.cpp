/******************************************************************************
 * Assignment 1 Solution provided by Paul Miller as part of COSC1112/1114
 * - Operating Systems Principles, Semester 2, 2019.
 *****************************************************************************/

#include "main.h"

/**
 * load in the string test data
 **/
bool populate_names(names_list array)
{
    /* open file and check that succeeded */
    std::ifstream reader(NAMES_FILE);
    string line;
    size_t line_count = 0;

    if (!reader)
    {
        perror("failed to open file");
        return false;
    }
    /* read in each line from the file */
    while (std::getline(reader, line))
    {
        /* copy the data into the data array */
        strcpy(array[line_count++], line.c_str());
    }
    assert(line_count == TOTAL_NAMES);
    return true;
}

/**
 * display error message when this program has been invoked incorrectly.
 **/
void display_usage(void)
{
    std::cerr << "Please run this program as follows: \n\t./allocate -s <size> "
                 "-a <algorithm>\nWhere size is the size of each array to "
                 "allocate and \n\"algorithm\" should be one of \"firstfit\", "
                 "\"bestfit\" and \"worstfit\".\n";
}

/**
 * extracts a number from a string. The second parameter is a struct holding
 * the range of the number that is allowed. min being the minimum and max
 * being the maximum.
 **/
falsible_long strtonum(const char* input, const range* allowed_range)
{
    struct falsible_long result;
    long lnum;
    char* end;

    /* extract the string */
    lnum = strtol(input, &end, DECIMAL);
    /* skip over any trailing spaces */
    while (isspace(*end))
    {
        ++end;
    }
    /* if there is trailing data, it is not a valid integer */
    if (*end)
    {
        std::cerr << "Errror: " << input << " is not a valid integer number.\n";
        result.success = false;
        return result;
    }
    /* check that the number is within the allowed range */
    if (lnum < allowed_range->first || lnum > allowed_range->second)
    {
        std::cerr << "Error: " << input
                  << " is not a number in the allowed range of "
                  << allowed_range->first << "-" << allowed_range->second
                  << "\n";
        result.success = false;
        return result;
    }
    /* all good so set up the data for return */
    result.success = true;
    result.data = lnum;
    return result;
}

/**
 * extracts the strategy from the string. It must be one of "firstfit",
 * "bestfit" or "worstfit". On error, the function returns the constant
 * INVALID_STRATEGY.
 **/
strategy strtostrat(std::string_view strategy_string)
{
    int strat_count;
    /* the valid strategy strings */
    const std::string strategy_strings[] = { "firstfit", "bestfit",
                                             "worstfit" };
    /* compare against each strategy and return the value for the right
     * strategy
     */
    for (strat_count = 0; strat_count < NUM_STRATEGIES; ++strat_count)
    {
        if (strategy_string == strategy_strings[strat_count])
        {
            return (strategy)strat_count;
        }
    }
    /* return a value to indicate a mismatch, if it happens */
    return INVALID_STRATEGY;
}

/* multiply a fractional double number by 4 billion or thereabouts */
constexpr double expand_fraction(const double fraction)
{
    return fraction * (((long)1) << 32);
}

/**
 * generate a random double in the range of 0 to 4 billion or thereabouts
 **/
double rand_double(void)
{
    /* generate a random number between 0 and 1 */
    double fraction = rand() / (double)RAND_MAX;
    /* increase the fraction to be in the required range */
    return expand_fraction(fraction);
}

/**
 * processes command line arguments down to a result on success
 **/
bool process_args(int argc, char* argv[], args_result& result)
{
    /* stores the current option, for use with getopt */
    int current_option;
    struct
    {
        bool approach_success, size_success;
    } success;
    /* zero out the success struct */
    memset(&success, 0, sizeof(success));
    /* iterate over the command line args */
    while (current_option = getopt(argc, argv, "s:a:"), current_option != EOF)
    {
        switch (current_option)
        {
            /* process the memory allocation strategy */
            case AO_APPROACH:
            {
                result.thestrategy = strtostrat(optarg);
                if (result.thestrategy != INVALID_STRATEGY)
                {
                    success.approach_success = true;
                }
                else
                {
                    return false;
                }
            }
            break;
            /* process the array size */
            case AO_SIZE:
            {
                falsible_long amount = strtonum(optarg, &array_size_range);
                if (amount.success)
                {
                    result.array_size = amount.data;
                    success.size_success = true;
                }
                else
                {
                    return false;
                }
            }
            break;
        }
    }
    /* test that parsing was successful */
    if (!success.approach_success || !success.size_success)
    {
        return false;
    }
    return true;
}

/**
 * convert the timeval struct to a long integer that represents nanoseconds
 **/
long tvtolong(timeval tv)
{
    return tv.tv_sec * 1000000 + tv.tv_usec;
}

/**
 * convert nanoseconds to milliseconds
 **/
#define NSTOMS(NS) ((double)NS) / 1000

/**
 * report on the performance of a single allocation cycle
 **/
void report(void)
{
    rusage theusage;
    struct size_pair thesizes;
    static long elapsed = 0;
    /* print out the current status of the lists that are managed by
     * the allocator */
    thesizes = get_list_sizes();
    std::cout << "the alloc list has " << thesizes.alloc
              << " nodes and the free list has " << thesizes.free
              << " nodes.\n";
    getrusage(RUSAGE_SELF, &theusage);
    std::cout << "This iteration took: "
              << (double)(tvtolong(theusage.ru_utime) - elapsed) / 1000
              << " milliseconds."
              << "\n";
    elapsed = tvtolong(theusage.ru_utime);
    std::cout << "physical memory in use: " << theusage.ru_maxrss << "\n";
}

/**
 * macro that times any event that can be expressed in code. EVENT is some
 * arbitrary chunk of code (line of code, block of code, etc).
 **/
#define TIME_EVENT(EVENT, TIME)                                                \
    {                                                                          \
        rusage usage;                                                          \
        long start, end;                                                       \
        /* get the start time for the event */                                 \
        getrusage(RUSAGE_SELF, &usage);                                        \
        start = tvtolong(usage.ru_utime);                                      \
        /* run the event */                                                    \
        EVENT;                                                                 \
        /* get the end time */                                                 \
        getrusage(RUSAGE_SELF, &usage);                                        \
        end = tvtolong(usage.ru_utime);                                        \
        TIME += end - start;                                                   \
    }

int main(int argc, char* argv[])
{
    size_t arraysize;
    size_t count;
    names_list names;
    int iter_count;

    /* process command line arguments */
    struct args_result claresult;
    if (!process_args(argc, argv, claresult))
    {
        display_usage();
        return EXIT_FAILURE;
    }

    /* load in the names to be used as test data for allocation */
    populate_names(names);
    /* extract the requested array size and strategy */
    arraysize = claresult.array_size;
    set_strategy(claresult.thestrategy);
    /* set up the random number generator */
    srand(time(NULL));
    for (iter_count = 0; iter_count < MAXITERS; ++iter_count)
    {
        int** intarray;
        double** dblarray;
        char** namearray;
        long alloctime = 0, freetime = 0;

        std::cout << "new alloc cycle\n";
        std::cout << "----------------\n";
        /* allocate memory for the int array and it elements and fill them */
        TIME_EVENT(intarray = (int**)alloc(sizeof(int*) * arraysize),
                   alloctime);
        for (count = 0; count < arraysize; ++count)
        {
            TIME_EVENT(intarray[count] = (int*)alloc(sizeof(int)), alloctime);
            *intarray[count] = rand();
        }
        /* allocate memory for the double array and its elements and fill them
         */
        TIME_EVENT(dblarray = (double**)alloc(sizeof(double*) * arraysize),
                   alloctime);
        for (count = 0; count < arraysize; ++count)
        {
            TIME_EVENT(dblarray[count] = (double*)alloc(sizeof(double)),
                       alloctime);
            *dblarray[count] = rand_double();
        }

        /* allocate memory for the char array array and fill it */
        TIME_EVENT(namearray = (char**)alloc(sizeof(char*) * arraysize),
                   alloctime);
        for (count = 0; count < arraysize; ++count)
        {
            unsigned nameind = rand() % TOTAL_NAMES;
            char* name = names[nameind];
            TIME_EVENT(namearray[count] = (char*)alloc(strlen(name) + 1),
                       alloctime);
            strcpy(namearray[count], name);
        }
        for (count = 0; count < arraysize; ++count)
        {
/* print out each element and deallocate it */
#ifdef DEBUG
            std::cerr << *intarray[count] << "\n";
#endif
            TIME_EVENT(dealloc(intarray[count]), freetime);
#ifdef DEBUG
            std::cerr << *dblarray[count] << "\n";
#endif
            TIME_EVENT(dealloc(dblarray[count]), freetime);
#ifdef DEBUG
            std::cerr << namearray[count] << "\n";
#endif
            TIME_EVENT(dealloc(namearray[count]), freetime);
        }
        /* deallocate the arrays */
        TIME_EVENT(dealloc(intarray), freetime);
        TIME_EVENT(dealloc(dblarray), freetime);
        TIME_EVENT(dealloc(namearray), freetime);
        report();
        std::cerr << "Time spend allocing:" << NSTOMS(alloctime)
                  << " milliseconds\n";
        std::cerr << "Time spent freeing:" << NSTOMS(freetime)
                  << " milliseconds\n";
    }
    return EXIT_SUCCESS;
}
