/**
 * @file 
 * Debug options for "make debug", including the level of verbosity: 1,2,4 
 * The level of verbosity is a bitmask. Therefore, you can blend multiple levels by setting the mask to intermediate values like 3,5,7
 */

#ifdef DEBUG_MODE	/**< Enables the following verbosity settings IF DEBUG_MODE flag is passed to the compiler */

#define MASK 2		/**< HERE you can define the level of verbosity */

#define DBG(A, B) {if ((A) & MASK) {B; } }	/**< Bit mask definition */
#else
#define DBG(A, B)
#endif

#define D1(a) DBG(1, a)		/**< level 1 debugging: print game status, player lives and so on.. */
#define D2(a) DBG(2, a)		/**< level 2 debugging: print ball hits, ball speed informations, physics and so on.. */
#define D4(a) DBG(4, a) 	/**< level 3 debugging: print informations regarding allocation/initialization status of the objects */

#ifndef DEBUG_MODE		/**< this macro enables asserts when debug mode is enabled */
#define NDEBUG
#endif
#include <cassert>
