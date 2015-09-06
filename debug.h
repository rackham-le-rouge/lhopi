/**
 * @file        debug.h
 * @rogram      Lhopi
 * @brief       Debug macro from http://c.learncodethehardway.org
 * @description There is some usefull debug macro from an online ,book. better than mine...
 * @date        10/2014
 * @licence     Beerware (rev 42)
 * @author      Jerome GRARD - http://c.learncodethehardway.org
 */



#ifndef _DEBUG_H_
#define _DEBUG_H_



/* Debug part */
#define UNUSED(x) (void)(x)



#define debug(M) fprintf(g_FILEOutputLogStream, "DEBUG %s:%d: " M "\n", __FILE__, __LINE__)

#define log_err(M, ...) fprintf(g_FILEOutputLogStream, "[ERROR] (%s:%d: errno: %d) " M "\n", __FILE__, __LINE__, errno, ##__VA_ARGS__)

#define log_warn(M, ...) fprintf(g_FILEOutputLogStream, "[WARN] (%s:%d: errno: %d) " M "\n", __FILE__, __LINE__, errno, ##__VA_ARGS__)

#define log_info(M, ...) fprintf(g_FILEOutputLogStream, "[INFO] (%s:%d) " M "\n", __FILE__, __LINE__, ##__VA_ARGS__)



/** How to use :

debug("I have Red Hair.");
log_err("I believe everything is broken.");
log_err("There are %d problems in %s.", 0, "lack of cookies !");
debug("I am %d years old.", 42);
log_info("It happened %f times today.", 1.3f);

*/



#endif
