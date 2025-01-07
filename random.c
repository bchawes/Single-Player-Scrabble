#include "random.h"
#include <assert.h>

// RANDOM NUMBER FUNCTIONS -- DO NOT CHANGE

static unsigned int lfsr = 0xbaadf00d;  // global variable -- holds current state of LFSR
// if never seeded, this will be the starting LFSR
static unsigned int lfsrNext();  // function to advance the LFSR

// set starting value for LFSR-based random number generator
void seedRandom(const char *seed) {
   assert(*seed);  // can't be an empty string
   // using a standard string-based hash function
   lfsr = 0;
   while (*seed) {
      lfsr = 17 * lfsr + *seed;  // it's ok if this overflows, but can't be zero
      ++seed;
   }
}

// return a random integer between 0 and limit-1
unsigned int getRandom(unsigned int limit) {
   return lfsrNext() % limit;
}

// lfsrNext -- function to advance an LFSR for pseudorandom number generation
// uses global variable (lfsr)
// code from: https://en.wikipedia.org/wiki/Linear-feedback_shift_register
unsigned int lfsrNext(void) {
   lfsr ^= lfsr >> 7;
   lfsr ^= lfsr << 9;
   lfsr ^= lfsr >> 13;
   return lfsr;
}
