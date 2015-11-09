/**
 * @file    interp.h
 * @date    --
 * @brief   Interpreter declaration
 *
 * Declaration of interpreter functions
 */

/**
 * Interpreter entity
 *
 * @defgroup Interpreter
 * @{
 */

#ifndef INTERP_H
#define INTERP_H
extern const int maxdatasize ;


/**
 * Interpreter actions
 *
 * @defgroup interpreter_actions intepreter actions
 * @{
 */

void interp_run(void);
int interp_addcmd(char* cmd, int (*cmd_func)(char*), char* doc);
/***@}*/


/**@}*/
#endif
