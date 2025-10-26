
uint InitSeed(uint seed)
{
    // Thomas Wang hash 
    // Ref: http://www.burtleburtle.net/bob/hash/integer.html
    seed = (seed ^ 61) ^ (seed >> 16);
    seed *= 9;
    seed = seed ^ (seed >> 4);
    seed *= 0x27d4eb2d;
    seed = seed ^ (seed >> 15);
    return seed;
}

// Generate a random 32-bit integer
uint Random(inout uint state)
{
    // Xorshift algorithm from George Marsaglia's paper.
    state ^= (state << 13);
    state ^= (state >> 17);
    state ^= (state << 5);
    return state;
}

float Random01(inout uint state)
{
    return asfloat(0x3f800000 | Random(state) >> 9) - 1.0;
}

float Random01inclusive(inout uint state)
{
    return Random(state) / float(0xffffffff);
}

// Generate a random integer in the range [lower, upper]
uint Random(inout uint state, uint lower, uint upper)
{
    return lower + uint(float(upper - lower + 1) * Random01(state));
}