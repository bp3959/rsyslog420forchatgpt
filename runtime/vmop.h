/* The vmop object.
 *
 * Copyright 2008 Rainer Gerhards and Adiscon GmbH.
 *
 * This file is part of rsyslog.
 *
 * This file is part of the rsyslog runtime library.
 *
 * The rsyslog runtime library is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * The rsyslog runtime library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with the rsyslog runtime library.  If not, see <http://www.gnu.org/licenses/>.
 *
 * A copy of the GPL can be found in the file "COPYING" in this distribution.
 * A copy of the LGPL can be found in the file "COPYING.LESSER" in this distribution.
 */
#ifndef INCLUDED_VMOP_H
#define INCLUDED_VMOP_H

#include "ctok_token.h"
#include "stringbuf.h"

/* machine instructions types */
typedef enum {	 /* do NOT start at 0 to detect uninitialized types after calloc() */
	opcode_INVALID = 0,
	/* for simplicity of debugging and reading dumps, we use the same IDs
	 * that the tokenizer uses where this applicable.
	 */
	opcode_OR    = ctok_OR,
	opcode_AND   = ctok_AND,
	opcode_STRADD= ctok_STRADD,
	opcode_PLUS  = ctok_PLUS,
	opcode_MINUS = ctok_MINUS,
	opcode_TIMES = ctok_TIMES,	 /* "*" */
	opcode_DIV   = ctok_DIV,
	opcode_MOD   = ctok_MOD,
	opcode_NOT   = ctok_NOT,
	opcode_CMP_EQ          = ctok_CMP_EQ, /* all compare operations must be in a row */
	opcode_CMP_NEQ         = ctok_CMP_NEQ,
	opcode_CMP_LT          = ctok_CMP_LT,
	opcode_CMP_GT          = ctok_CMP_GT,
	opcode_CMP_LTEQ        = ctok_CMP_LTEQ,
	opcode_CMP_CONTAINS    = ctok_CMP_CONTAINS,
	opcode_CMP_STARTSWITH  = ctok_CMP_STARTSWITH,
	opcode_CMP_CONTAINSI   = ctok_CMP_CONTAINSI,
	opcode_CMP_STARTSWITHI = ctok_CMP_STARTSWITHI,
	opcode_CMP_GTEQ        = ctok_CMP_GTEQ, /* end compare operations */
	/* here we start our own codes */
	opcode_POP             = 1000,	 /* requires var operand to receive result */
	opcode_PUSHSYSVAR      = 1001,	 /* requires var operand */
	opcode_PUSHMSGVAR      = 1002,	 /* requires var operand */
	opcode_PUSHCONSTANT    = 1003,	 /* requires var operand */
	opcode_UNARY_MINUS     = 1010,
	opcode_FUNC_CALL       = 1012,
	opcode_END_PROG        = 2000
} opcode_t;


/* Additional doc, operation specific

  FUNC_CALL
  All parameter passing is via the stack. Parameters are placed onto the stack in reverse order,
  that means the last parameter is on top of the stack, the first at the bottom location. 
  At the actual top of the stack is the number of parameters. This permits functions to be
  called with variable number of arguments. The function itself is responsible for poping
  the right number of parameters of the stack and complaining if the number is incorrect.
  On exit, a single return value must be pushed onto the stack. The FUNC_CALL operation 
  is generic. Its pVar argument contains the function name string (TODO: very slow, make
  faster in later releases).

  Sample Function call:  sampleFunc(p1, p2, p3) ; returns number 4711 (sample)
  Stacklayout on entry (order is top to bottom):
  3
  p3
  p2
  p1
  ... other vars ...

  Stack on exit
  4711
  ... other vars ...
  
 */


/* the vmop object */
typedef struct vmop_s {
	BEGINobjInstance;	/* Data to implement generic object - MUST be the first data element! */
	opcode_t opcode;
	union {
		var_t *pVar; 	/* for function call, this is the name (string) of function to be called */
	} operand;
	struct vmop_s *pNext; /* next operation or NULL, if end of program (logically this belongs to vmprg) */
} vmop_t;


/* interfaces */
BEGINinterface(vmop) /* name must also be changed in ENDinterface macro! */
	INTERFACEObjDebugPrint(vmop);
	rsRetVal (*Construct)(vmop_t **ppThis);
	rsRetVal (*ConstructFinalize)(vmop_t __attribute__((unused)) *pThis);
	rsRetVal (*Destruct)(vmop_t **ppThis);
	rsRetVal (*SetOpcode)(vmop_t *pThis, opcode_t opcode);
	rsRetVal (*SetVar)(vmop_t *pThis, var_t *pVar);
	rsRetVal (*Opcode2Str)(vmop_t *pThis, uchar **ppName);
	rsRetVal (*Obj2Str)(vmop_t *pThis, cstr_t *pstr);
ENDinterface(vmop)
#define vmopCURR_IF_VERSION 1 /* increment whenever you change the interface structure! */

/* the remaining prototypes */
PROTOTYPEObj(vmop);

#endif /* #ifndef INCLUDED_VMOP_H */
