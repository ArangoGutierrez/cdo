/*
  This file is part of CDO. CDO is a collection of Operators to
  manipulate and analyse Climate model Data.

  Copyright (C) 2003-2010 Uwe Schulzweida, Uwe.Schulzweida@zmaw.de
  See COPYING file for copying and redistribution conditions.

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; version 2 of the License.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.
*/

/*
   This module contains the following operators:

      Inttime    inttime         Time interpolation
*/

#include <ctype.h>  /* isdigit */

#include "cdi.h"
#include "cdo.h"
#include "cdo_int.h"
#include "pstream.h"
#include "interpol.h"


void *Inttime(void *argument)
{
  static char func[] = "Inttime";
  int streamID1, streamID2;
  int nrecs, nvars, nlevel;
  int i, nrecords;
  int tsID, tsIDo, recID, varID, levelID;
  int gridsize;
  int vlistID1, vlistID2;
  int taxisID1, taxisID2;
  int vdate, vtime;
  int offset;
  int ijulinc, incperiod = 0, incunit = 3600, tunit = TUNIT_HOUR;
  int calendar;
  int year, month, day, hour, minute, second;
  int *recVarID, *recLevelID;
  int **nmiss1, **nmiss2, nmiss3;
  const char *datestr, *timestr;
  double missval1, missval2;
  juldate_t juldate1, juldate2, juldate;
  double fac1, fac2;
  double *array, *single1, *single2;
  double **vardata1, **vardata2, *vardatap;

  cdoInitialize(argument);

  operatorInputArg("date,time<,increment> (format YYYY-MM-DD,hh:mm:ss)");
  if ( operatorArgc() < 2 ) cdoAbort("Not enough arguments!");

  datestr = operatorArgv()[0];
  timestr = operatorArgv()[1];

  if ( strchr(datestr, '-') == NULL )
    {
      vdate = atoi(datestr);
    }
  else
    {
      year = 1; month = 1; day = 1;
      sscanf(datestr, "%d-%d-%d", &year, &month, &day);
      vdate = cdiEncodeDate(year, month, day);
    }

  if ( strchr(timestr, ':') == NULL )
    {
      vtime = atoi(timestr);
    }
  else
    {
      hour = 0; minute = 0; second = 0;
      sscanf(timestr, "%d:%d:%d", &hour, &minute, &second);
      vtime = cdiEncodeTime(hour, minute, second);
    }

  if ( operatorArgc() == 3 )
    {
      size_t len;
      char *unit = operatorArgv()[2];
      incperiod = atoi(unit);
      while ( isdigit((int) *unit) ) unit++;
      len = strlen(unit);
      if ( len )
	{
	  if      ( memcmp(unit, "seconds", len) == 0 ) {incunit =     1; tunit = TUNIT_SECOND;}
	  else if ( memcmp(unit, "minutes", len) == 0 ) {incunit =    60; tunit = TUNIT_MINUTE;}
	  else if ( memcmp(unit, "hours", len)   == 0 ) {incunit =  3600; tunit = TUNIT_HOUR;  }
	  else if ( memcmp(unit, "days", len)    == 0 ) {incunit = 86400; tunit = TUNIT_DAY;   }
	  else if ( memcmp(unit, "months", len)  == 0 ) {incunit =     1; tunit = TUNIT_MONTH; }
	  else if ( memcmp(unit, "years", len)   == 0 ) {incunit =    12; tunit = TUNIT_YEAR;  }
	  else cdoAbort("unsupported time unit >%s<", unit);
	}
    }
  /* increment in seconds */
  ijulinc = incperiod * incunit;

  streamID1 = streamOpenRead(cdoStreamName(0));
  if ( streamID1 < 0 ) cdiError(streamID1, "Open failed on %s", cdoStreamName(0));

  vlistID1 = streamInqVlist(streamID1);
  vlistID2 = vlistDuplicate(vlistID1);

  if ( ijulinc == 0 ) vlistDefNtsteps(vlistID2, 1);

  nvars    = vlistNvars(vlistID1);
  nrecords = vlistNrecs(vlistID1);

  recVarID   = (int *) malloc(nrecords*sizeof(int));
  recLevelID = (int *) malloc(nrecords*sizeof(int));

  gridsize = vlistGridsizeMax(vlistID1);
  array = (double *) malloc(gridsize*sizeof(double));

  nmiss1   = (int **) malloc(nvars*sizeof(int *));
  nmiss2   = (int **) malloc(nvars*sizeof(int *));
  vardata1 = (double **) malloc(nvars*sizeof(double *));
  vardata2 = (double **) malloc(nvars*sizeof(double *));

  for ( varID = 0; varID < nvars; varID++ )
    {
      gridsize = gridInqSize(vlistInqVarGrid(vlistID1, varID));
      nlevel   = zaxisInqSize(vlistInqVarZaxis(vlistID1, varID));
      nmiss1[varID]   = (int *) malloc(nlevel*sizeof(int));
      nmiss2[varID]   = (int *) malloc(nlevel*sizeof(int));
      vardata1[varID] = (double *) malloc(gridsize*nlevel*sizeof(double));
      vardata2[varID] = (double *) malloc(gridsize*nlevel*sizeof(double));
    }

  taxisID1 = vlistInqTaxis(vlistID1);
  taxisID2 = taxisDuplicate(taxisID1);
  if ( taxisHasBounds(taxisID2) )
    {
      cdoWarning("Time bounds unsupported by this operator, removed!");
      taxisDeleteBounds(taxisID2);
    }
  vlistDefTaxis(vlistID2, taxisID2);

  streamID2 = streamOpenWrite(cdoStreamName(1), cdoFiletype());
  if ( streamID2 < 0 ) cdiError(streamID2, "Open failed on %s", cdoStreamName(1));

  streamDefVlist(streamID2, vlistID2);

  calendar = taxisInqCalendar(taxisID1);

  juldate = juldate_encode(calendar, vdate, vtime);

  if ( cdoVerbose )
    {
      cdoPrint("date %d  time %d", vdate, vtime);
      cdoPrint("juldate  = %f", juldate_to_seconds(juldate));
      cdoPrint("ijulinc = %d", ijulinc);
    }

  tsID = 0;
  nrecs = streamInqTimestep(streamID1, tsID++);
  juldate1 = juldate_encode(calendar, taxisInqVdate(taxisID1), taxisInqVtime(taxisID1));
  for ( recID = 0; recID < nrecs; recID++ )
    {
      streamInqRecord(streamID1, &varID, &levelID);
      gridsize = gridInqSize(vlistInqVarGrid(vlistID1, varID));
      offset   = gridsize*levelID;
      single1  = vardata1[varID] + offset;
      streamReadRecord(streamID1, single1, &nmiss1[varID][levelID]);
    }

  if ( cdoVerbose )
    {
      cdoPrint("date %d  time %d", taxisInqVdate(taxisID1), taxisInqVtime(taxisID1));
      cdoPrint("juldate1  = %f", juldate_to_seconds(juldate1));
    }

  if ( juldate_to_seconds(juldate1) > juldate_to_seconds(juldate) )
    cdoWarning("start time %d %d out of range!", vdate, vtime);

  tsIDo = 0;
  while ( juldate_to_seconds(juldate1) <= juldate_to_seconds(juldate) )
    {
      nrecs = streamInqTimestep(streamID1, tsID++);
      if ( nrecs == 0 ) break;

      juldate2 = juldate_encode(calendar, taxisInqVdate(taxisID1), taxisInqVtime(taxisID1));
      if ( cdoVerbose )
	{
	  cdoPrint("date %d  time %d", taxisInqVdate(taxisID1), taxisInqVtime(taxisID1));
	  cdoPrint("juldate2  = %f", juldate_to_seconds(juldate2));
	}

      for ( recID = 0; recID < nrecs; recID++ )
	{
	  streamInqRecord(streamID1, &varID, &levelID);

	  recVarID[recID]   = varID;
	  recLevelID[recID] = levelID;

	  gridsize = gridInqSize(vlistInqVarGrid(vlistID1, varID));
	  offset   = gridsize*levelID;
	  single2  = vardata2[varID] + offset;
	  streamReadRecord(streamID1, single2, &nmiss2[varID][levelID]);
	}

      while ( juldate_to_seconds(juldate) < juldate_to_seconds(juldate2) )
	{
	  if ( juldate_to_seconds(juldate) >= juldate_to_seconds(juldate1) &&
	       juldate_to_seconds(juldate) <  juldate_to_seconds(juldate2) )
	    {
	      juldate_decode(calendar, juldate, &vdate, &vtime);

	      if ( cdoVerbose )
		{
		  char vdatestr[32], vtimestr[32];	  
		  /*
		  cdoPrint("juldate1 %f", juldate_to_seconds(juldate1));
		  cdoPrint("juldate  %f", juldate_to_seconds(juldate));
		  cdoPrint("juldate2 %f", juldate_to_seconds(juldate2));
		  */
		  date2str(vdate, vdatestr, sizeof(vdatestr));
		  time2str(vtime, vtimestr, sizeof(vtimestr));
		  cdoPrint("%s %s  %f  %d", vdatestr, vtimestr, juldate_to_seconds(juldate), calendar);
		}

	      taxisDefVdate(taxisID2, vdate);
	      taxisDefVtime(taxisID2, vtime);
	      streamDefTimestep(streamID2, tsIDo++);

	      fac1 = juldate_to_seconds(juldate_sub(juldate2, juldate)) / 
		     juldate_to_seconds(juldate_sub(juldate2, juldate1));
	      fac2 = juldate_to_seconds(juldate_sub(juldate, juldate1)) / 
	   	     juldate_to_seconds(juldate_sub(juldate2, juldate1));

	      for ( recID = 0; recID < nrecs; recID++ )
		{
		  varID    = recVarID[recID];
		  levelID  = recLevelID[recID];
		  gridsize = gridInqSize(vlistInqVarGrid(vlistID1, varID));
		  offset   = gridsize*levelID;
		  single1  = vardata1[varID] + offset;
		  single2  = vardata2[varID] + offset;

		  nmiss3 = 0;

		  if ( nmiss1[varID][levelID] > 0 || nmiss2[varID][levelID] > 0 )
		    {
		      missval1 = vlistInqVarMissval(vlistID1, varID);
		      missval2 = vlistInqVarMissval(vlistID2, varID);

		      for ( i = 0; i < gridsize; i++ )
			{
			  if ( !DBL_IS_EQUAL(single1[i], missval1) &&
			       !DBL_IS_EQUAL(single2[i], missval2) )
			    array[i] = single1[i]*fac1 + single2[i]*fac2;
			  else if (  DBL_IS_EQUAL(single1[i], missval1) &&
				    !DBL_IS_EQUAL(single2[i], missval2) && fac2 >= 0.5 )
			    array[i] = single2[i];
			  else if (  DBL_IS_EQUAL(single2[i], missval2) &&
				    !DBL_IS_EQUAL(single1[i], missval1) && fac1 >= 0.5 )
			    array[i] = single1[i];
			  else
			    {
			      array[i] = missval1;
			      nmiss3++;
			    }
			}
		    }
		  else
		    {
		      for ( i = 0; i < gridsize; i++ )
			array[i] = single1[i]*fac1 + single2[i]*fac2;
		    }

		  streamDefRecord(streamID2, varID, levelID);
		  streamWriteRecord(streamID2, array, nmiss3);
		}
	    }

	  if ( ijulinc == 0 ) break;

	  if ( tunit == TUNIT_MONTH || tunit == TUNIT_YEAR )
	    {
	      juldate_decode(calendar, juldate, &vdate, &vtime);

	      cdiDecodeDate(vdate, &year, &month, &day);
	      
	      month += ijulinc;

	      while ( month > 12 ) { month -= 12; year++; }
	      while ( month <  1 ) { month += 12; year--; }

	      vdate = cdiEncodeDate(year, month, day);
		
	      juldate = juldate_encode(calendar, vdate, vtime);
	    }
	  else
	    {
	      juldate = juldate_add_seconds(ijulinc, juldate);
	    }
	}

      juldate1 = juldate2;
      for ( varID = 0; varID < nvars; varID++ )
	{
	  vardatap        = vardata1[varID];
	  vardata1[varID] = vardata2[varID];
	  vardata2[varID] = vardatap;
	}
    }

  for ( varID = 0; varID < nvars; varID++ )
    {
      free(nmiss1[varID]);
      free(nmiss2[varID]);
      free(vardata1[varID]);
      free(vardata2[varID]);
    }

  free(nmiss1);
  free(nmiss2);
  free(vardata1);
  free(vardata2);

  if ( array )  free(array);

  streamClose(streamID2);
  streamClose(streamID1);

  cdoFinish();

  return (0);
}