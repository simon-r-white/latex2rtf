/*
 * $Id: stack.c,v 1.1 2001/08/12 15:32:29 prahl Exp $
 * History:
 * $Log: stack.c,v $
 * Revision 1.1  2001/08/12 15:32:29  prahl
 * Initial revision
 *
 * Revision 1.1  1994/06/17  11:26:29  ralf
 * Initial revision
 *
 */
/***************************************************************************
     name : stack.c
    autor : DORNER Fernando, GRANZER Andreas
  purpose : this is an stack-model which handles the recursions-levels
	    occurred by environments, and open and closing-braces
 ******************************************************************************/

/********************************* includes **********************************/
#include <stdio.h>
#include <stdlib.h>
/******************************************************************************/


/********************************** extern variables *************************/
extern int BracketLevel;
extern char *progname;
extern char *latexname;
extern long linenumber;
/******************************************************************************/

/********************************* defines ***********************************/
#define STACKSIZE 300
/******************************************************************************/

/******************************** global variables *****************************/
int stack[STACKSIZE];
int top = 0;
/******************************************************************************/

/******************************************************************************/
int Push(int lev, int brack)
/******************************************************************************
  purpose: pushes the parameter lev and brack on the stack
parameter: lev...level
	   brack...brackets
 return: top of stack
 ******************************************************************************/
{
  ++top;
  stack[top] = lev;
  ++top;
  stack[top] = brack;

  if (top >= STACKSIZE)
  {
    fprintf(stderr,"\n%s: ERROR: too deep nesting -> internal stack-overflow",progname);
    fprintf(stderr,"\nprogram aborted\n");
    exit(-1);
  }
  return top;
}

int Pop(int *lev, int *brack)
/******************************************************************************
  purpose: pops the parameter lev and brack from the stack
parameter: lev...level
	   brack...brackets
 return: top of stack
 ******************************************************************************/
{
  *brack = stack[top];
  --top;
  *lev = stack[top];
  --top;
  if (top < 0)
  {
    fprintf(stderr,"\n%s: ERROR: error in LaTeX-File: %s at linenumber: %ld\n-> internal stack-overflow",progname,latexname,linenumber);
    fprintf(stderr,"\nprogram aborted\n");
    exit(-1);
  }
  return top;
}


/* The use of stack */

/*
each stack elem consist of 2 integers RecursLevel and BracketLevel. Recurs-
Level is the number of recursive calls of convert function, BracketLevel is
the number of open curly braces. The value on top of stack represents the
current value of the two global variables (RecursLevel and BracketLevel).
Before every command and on an opening curly brace the current settings are
written on the stack. On appearance of a closing curly brace the
corresponding RecursLevel is found by search on the stack. It is the lowest
RecursLevel with the same BracketLevel as now (after subtract of the 1
closing brace found). The initial value RecLev 1, BracketLev 0 remains
always on the stack. The begin document command Pushes 1,1
examples:
{ Text {\em Text} Text }
1      2 3	4      5
1 Push 12
2 Push 13
3 Push 23
4 Bracket 3->2 Pop 23  Pop 13 Pop 12 Pop 11 -found- Push back 11
  return to level 1
5 Bracket 2->1
  return to level 1 = current -> no return

\mbox{\em Text}
1    2 3      4
1 Push 11  RecursLevel+1
2 Push 22
3 Push 32
4 Bracket 2->1 Pop 32 Pop 22 Pop 11 -found-
  return to level 1 from level 3 -> double return from convert

The necessary Push before every command increases the stack size. If the
commands don't include a recursiv call the stack is not cleaned up.
After every TranslateCommand-function the stack is cleaned
example
\ldots \LaTeX \today \TeX
 1	2      3      4
1 Push 11
2 Push 11
3 Push 11
4 Push 11
The cleanup loop pops till the values are not ident and pushes back the last
Therefore 11 is only 1 times on the stack.
*/
