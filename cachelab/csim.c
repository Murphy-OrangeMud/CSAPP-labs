/*程芷怡 1800017781 Vegetable*/
/*csim.c - The program simulates a real cache with the
 *appointed s, b and E which the user inputs.
 */
#include <stdio.h>
#include <getopt.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "cachelab.h"
#define S(s) (1 << s)
#define B(b) (1 << b)
#define inf (1 << 30)
int hit_count;
int miss_count;
int eviction_count;
int s = -1, b = -1, E = -1;
typedef struct
{
    long long tag;
    int v;
    int t;
}way;
void printhelp()
{
    printf("Usage: ./csim-ref [-hv] -s <num> -E <num> -b <num> -t <file>\n");
    printf("Options:\n");
    printf("  -h         Print this help message.\n");
    printf("  -v         Optional verbose flag.\n");
    printf("  -s <num>   Number of set index bits.\n");
    printf("  -E <num>   Number of lines per set.\n");
    printf("  -b <num>   Number of block offset bits.\n");
    printf("  -t <file>  Trace file.\n");
    printf("\n");
    printf("Examples:\n");
    printf("  linux>  ./csim-ref -s 4 -E 1 -b 4 -t traces/yi.trace\n");
    printf("  linux>  ./csim-ref -v -s 8 -E 2 -b 4 -t traces/yi.trace\n");
}
void printLS(char cmd, long long addr, int size, int hit, int eviction)
{
    printf("%c %llx,%d%s%s\n", cmd, addr, size, (hit ? " hit" : " miss"), (eviction ? " eviction" : ""));
}
void printM(long long addr, int size, int hit, int eviction)
{
    printf("M %llx,%d%s%s hit\n", addr, size, (hit ? " hit" : " miss"), (eviction ? " eviction" : ""));
}
int S_idx(long long addr)
{
    return (addr % (1 << (b + s))) >> b;
}
int B_idx(long long addr)
{
    return addr % (1 << b);
}
long long tag(long long addr)
{
    return addr >> (s + b);
}
void simulate(char cmd, long long addr, int size, int verbose)
{
    static way cache[S(5)][4];
    static int T;
    int hit = 0, eviction = 0, t = inf, e = -1;
    for (int i = 0; i < E; ++i)
    {
        if (cache[S_idx(addr)][i].v)
        {
            if (cache[S_idx(addr)][i].tag == tag(addr))
            {
                hit = 1;
                ++hit_count;
                cache[S_idx(addr)][i].t = T;
                break;
            }
            if (cache[S_idx(addr)][i].t < t)
            {
                t = cache[S_idx(addr)][i].t;
                e = i;
            }
        }
        else
        {
            t = -1;
            e = i;
        }
    }
    if (!hit)
    {
        if (t != -1)
        {
            ++eviction_count;
            eviction = 1;
        }
        cache[S_idx(addr)][e].tag = tag(addr);
        cache[S_idx(addr)][e].v = 1;
        cache[S_idx(addr)][e].t = T;
        ++miss_count;
    }
    switch (cmd)
    {
    case 'S':
    case 'L':
    {
        if (verbose)
            printLS(cmd, addr, size, hit, eviction);
        break;
    }
    case 'M':
    {
        ++hit_count;
        if (verbose)
            printM(addr, size, hit, eviction);
        break;
    }
    }
    T++;
}
int main(int argc, char *argv[])
{
    int opt;
    int verbose = 0;
    char filename[100];
    while ((opt = getopt(argc, argv, "hvs:E:b:t:")) != -1)
    {
        switch (opt)
        {
        case 'h':
        {
            printhelp();
            return 0;
        }
        case 'v':
        {
            verbose = 1;
            break;
        }
        case 's':
        {
            s = atoi(optarg);
            break;
        }
        case 'b':
        {
            b = atoi(optarg);
            break;
        }
        case 'E':
        {
            E = atoi(optarg);
            break;
        }
        case 't':
        {
            strcpy(filename, optarg);
            break;
        }
        default:
        {
            printf("./csim: invalid option -- '%c'\n", optopt);
            printhelp();
            return 0;
        }
        }
    }
    if (s == -1 || b == -1 || E == -1 || filename == NULL) {
        printf("./csim: Missing required command line argument\n");
        printhelp();
        return 0;
    }
    char dir[100];
    if (!getcwd(dir, sizeof(dir)))
        return 0;
    strcat(dir, "/");
    strcat(dir, filename);
    freopen(dir, "r", stdin);
    char cmd;
    long long addr;
    int size;
    while (scanf(" %c %llx,%d\n", &cmd, &addr, &size) != EOF)
    {
        simulate(cmd, addr, size, verbose);
    }
    printSummary(hit_count, miss_count, eviction_count);
    return 0;
}