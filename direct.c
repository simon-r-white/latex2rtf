/*
 * $Id: direct.c,v 1.10 2001/08/12 19:32:24 prahl Exp $
 * History:
 * $Log: direct.c,v $
 * Revision 1.10  2001/08/12 19:32:24  prahl
 * 1.9f
 * 	Reformatted all source files ---
 * 	    previous hodge-podge replaced by standard GNU style
 * 	Compiles cleanly using -Wall under gcc
 *
 * 	added better translation of \frac, \sqrt, and \int
 * 	forced all access to the LaTeX file to use getTexChar() or ungetTexChar()
 * 	    allows better handling of %
 * 	    simplified and improved error checking
 * 	    eliminates the need for WriteTemp
 * 	    potentially allows elimination of getLineNumber()
 *
 * 	added new verbosity level -v5 for more detail
 * 	fixed bug with in handling documentclass options
 * 	consolidated package and documentclass options
 * 	fixed several memory leaks
 * 	enabled the use of the babel package *needs testing*
 * 	fixed bug in font used in header and footers
 * 	minuscule better support for french
 * 	Added testing file for % comment support
 * 	Enhanced frac.tex to include \sqrt and \int tests also
 * 	Fixed bugs associated with containing font changes in
 * 	    equations, tabbing, and quote environments
 * 	Added essential.tex to the testing suite --- pretty comprehensive test.
 * 	Perhaps fix missing .bbl crashing bug
 * 	Fixed ?` and !`
 *
 * Revision 1.7  1998/10/28 04:09:56  glehner
 * (WriteFontName): Cleaned up. Eliminated unecessary warning
 * and not completed rtf-output when using *Font*.
 *
 * Revision 1.6  1998/07/03 07:03:16  glehner
 * lclint cleaning
 *
 * Revision 1.5  1997/02/15 20:45:41  ralf
 * Some lclint changes and corrected variable declarations
 *
 * Revision 1.4  1995/03/23 15:58:08  ralf
 * Reworked version by Friedrich Polzer and Gerhard Trisko
 *
 *
 * Revision 1.3  1994/06/21  08:14:11  ralf
 * Corrected Bug in keyword search
 *
 * Revision 1.2  1994/06/17  14:19:41  ralf
 * Corrected various bugs, for example interactive read of arguments
 *
 * Revision 1.1  1994/06/17  11:26:29  ralf
 * Initial revision
 *
 */
/***************************************************************************
     name : direct.c
   author : DORNER Fernando, GRANZER Andreas
            POLZER Friedrich,TRISKO Gerhard
  * changed TryDirectConvert: use search on sorted array
  purpose : This file is used for converting LaTeX commands by simply text exchange
 ******************************************************************************/

/**********************************  includes ***********************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "main.h"
#include "direct.h"
#include "l2r_fonts.h"
#include "cfg.h"
/******************************************************************************/

/*************************** prototypes **************************************/

static bool WriteFontName(const char **buffpoint, FILE *fRtf);

/******************************* defines *************************************/
#define MAXFONTLEN 100
/******************************************************************************/


/******************************************************************************/
bool WriteFontName(const char **buffpoint, FILE *fRtf)
/******************************************************************************
  purpose: reads from the font-array to write correct font-number into
           Rtf-File
parameter: buffpoint: font and number
	   fRtf: File-Pointer to Rtf-File
globals:   progname
 ******************************************************************************/
{
  char buffer[MAXFONTLEN+1];
  int i;
  size_t fnumber;

  if (**buffpoint == '*')
  {
    fprintf(fRtf,"*");
    return TRUE;
  }
  i = 0;
  while(**buffpoint != '*')
  {
    if ((i >= MAXFONTLEN) || (**buffpoint == '\0'))
    {
      fprintf(stderr, "\n%s: ERROR: Invalid fontname in direct command",
	      progname);
      exit(EXIT_FAILURE);
    }
    buffer[i] = **buffpoint;
    i++;
    (*buffpoint)++;
  }
  buffer[i] = '\0';
  if ((fnumber = GetFontNumber(buffer)) < 0)
  {
    fprintf(stderr, "\n%s: ERROR: Unknown fontname in direct command",progname);
    fprintf(stderr, "\nprogram aborted\n");
    exit(EXIT_FAILURE);
  }
  else
  {
    fprintf(fRtf,"%u",(unsigned int)fnumber);
    return TRUE;
  }
}


/******************************************************************************
  purpose: reads from the direct-array how some easy LaTex-commands can be
	   converted into Rtf-commands by text exchange
parameter: command: LaTex-command and Rtf-command
	   fRtf: File-Pointer to Rtf-File
globals:   progname
 ******************************************************************************/
bool
TryDirectConvert(char *command, FILE *fRtf)
{
  const char *buffpoint;
  const char *RtfCommand;
  char TexCommand[128];

  if (strlen(command) >= 100)
    {
      fprintf(stderr,"\n%s: WARNING: Command %s is too long in LaTeX-File.\n",progname,command);
      return FALSE;    /* command too long */
    }
  
  TexCommand[0] = '\\';
  TexCommand[1] = '\0';
  strcat (TexCommand, command);
  
  RtfCommand = SearchRtfCmd (TexCommand, DIRECT_A);
  if (RtfCommand == NULL)
    return FALSE;
  
  buffpoint = RtfCommand;
  diagnostics(4, "Direct converting `%s' command to `%s'.",
	      TexCommand, RtfCommand);
  while (buffpoint[0] != '\0')
    {
      if (buffpoint[0] == '*')
	{
	  ++buffpoint;
	  (void)WriteFontName(&buffpoint, fRtf);

	  /* From here on it is not necesarry
	     if (WriteFontName(&buffpoint, fRtf))
	     {
	     fprintf(stderr,
	     "\n%s: WARNING: error in direct command file"
	     " - invalid font name , \n",
	     progname);
	     return FALSE;
	     }
	     */
	}
      else
	{
	  fprintf(fRtf,"%c",*buffpoint);
	}
      
      ++buffpoint;
      
    }  /* end while */
  return TRUE;
}


