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


/**
 * Interpreter actions
 *
 * @defgroup interpreter_actions intepreter actions
 * @{
 */
void interp_run(void);
void interp_run_auto(int time_command);
int interp_addcmd(char* cmd, int (*cmd_func)(char*), char* doc);
/***@}*/


/**@}*/
#endif
