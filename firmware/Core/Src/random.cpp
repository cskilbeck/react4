//////////////////////////////////////////////////////////////////////

#include "main.h"
#include "util.h"

//////////////////////////////////////////////////////////////////////
namespace
{
    int random_seed;
}

namespace random
{

    //////////////////////////////////////////////////////////////////////

    int seed(int seed)
    {
        int old_seed = random_seed;
        random_seed = seed;
        return old_seed;
    }
    
    //////////////////////////////////////////////////////////////////////
 
    int next(int *seed)
    {
        int s = *seed;
        s = s * 1103515245 + 12345;
        *seed = s;
        return (s >> 16) & 0xffff;
    }

    //////////////////////////////////////////////////////////////////////
    // if you don't set the seed, this will give you the same set
    // of numbers each time

    int get()
    {
        return next(&random_seed);
    }

}
