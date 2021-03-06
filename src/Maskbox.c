/*
  This file is part of CDO. CDO is a collection of Operators to
  manipulate and analyse Climate model Data.

  Copyright (C) 2003-2015 Uwe Schulzweida, <uwe.schulzweida AT mpimet.mpg.de>
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

      Maskbox    masklonlatbox   Mask lon/lat box
      Maskbox    maskindexbox    Mask index box
      Maskbox    maskregion      Mask regions
*/

#include <ctype.h>

#include <cdi.h>
#include "cdo.h"
#include "cdo_int.h"
#include "pstream.h"
#include "grid.h"

#define MAX_LINE 256
#define MAX_VALS 1048576

static
int ReadCoords(double *xvals, double *yvals, const char *polyfile, FILE *fp)
{
  double xcoord, ycoord;
  int z = 0, number = 0, jumpedlines = 0;
  int i = 0;
  char line[MAX_LINE];
  char *linep;

  while ( readline(fp, line, MAX_LINE) )
    {
      i = 0;
      xcoord = 0;
      if ( line[0] == '#' ) 
         { 
           jumpedlines++;
           continue;
         }  
      if ( line[0] == '\0' )
         { 
           jumpedlines++;
           continue;
         }  
      if ( line[0] == '&' ) break;
	 
      linep = &line[0];
     
      xcoord = strtod(linep, &linep);
      
      if ( ! (fabs(xcoord) > 0) ) 
	{
	  jumpedlines++;
	}
      
      while( ( ( isdigit( (int) *linep ) == FALSE ) && ( *linep!='-' )) && ( i < 64) )
	{	  
	  if ( *linep == 0 ) 
	    {
	      cdoAbort(" y value missing in file %s at  line %d", polyfile, (number+jumpedlines+1));
	      break;
	    }
          if ( ( isspace( (int) *linep) == FALSE && ( *linep!='-' ) ) && ( linep != NULL ) ) 
	    cdoWarning("unknown character in file %s at line %d", polyfile, (number+jumpedlines+1) );

          linep++;
	  i++;
	}    
      if ( ( i >= 63 ) && ( number != 0 ) ) cdoAbort( "Wrong value format in file %s at line %d", polyfile, (number+jumpedlines+1) );
    
      ycoord = strtod(linep, NULL);
     
      xvals[number] = xcoord;
      yvals[number] = ycoord;

      number++;
    }

 
  if ( ( number != 0 )&& ( ! (IS_EQUAL(xvals[0], xvals[number-1]) && IS_EQUAL(yvals[0], yvals[number-1])) ) )
    {
      xvals[number] = xvals[0];
      yvals[number] = yvals[0];
      number++;
    }
  

  if ( cdoVerbose ) 
    {
      for ( z = 0; z < number; z++ )  fprintf(stderr, "%d %g %g\n",  (z+1),  xvals[z], yvals[z]);
    }

  return number;
}


void genlonlatbox(int argc_offset, int gridID1, int *lat1, int *lat2, int *lon11, int *lon12, int *lon21, int *lon22);

void genindexbox(int argc_offset, int gridID1, int *lat1, int *lat2, int *lon11, int *lon12, int *lon21, int *lon22);


static
void maskbox(int *mask, int gridID,
	     int lat1, int lat2, int lon11, int lon12, int lon21, int lon22)
{
  int nlon, nlat;
  int ilat, ilon;

  nlon = gridInqXsize(gridID);
  nlat = gridInqYsize(gridID);

  for ( ilat = 0; ilat < nlat; ilat++ )
    for ( ilon = 0; ilon < nlon; ilon++ )
      if (  (lat1 <= ilat && ilat <= lat2 && 
	      ((lon11 <= ilon && ilon <= lon12) || (lon21 <= ilon && ilon <= lon22))) )
	mask[nlon*ilat + ilon] = 0;
}

static
void maskregion(int *mask, int gridID, double *xcoords, double *ycoords, int nofcoords)
{
  int i, j;
  int nlon, nlat;
  int ilat, ilon;
  int c;
  double *xvals, *yvals;
  double xval, yval;
  double xmin, xmax, ymin, ymax;

  nlon = gridInqXsize(gridID);
  nlat = gridInqYsize(gridID);

  xvals = (double*) malloc(nlon*sizeof(double));
  yvals = (double*) malloc(nlat*sizeof(double));

  gridInqXvals(gridID, xvals);
  gridInqYvals(gridID, yvals);  

  /* Convert lat/lon units if required */
  {
    char units[CDI_MAX_NAME];
    gridInqXunits(gridID, units);
    grid_to_degree(units, nlon, xvals, "grid center lon");
    gridInqYunits(gridID, units);
    grid_to_degree(units, nlat, yvals, "grid center lat");
  }

  xmin = xvals[0];
  xmax = xvals[0];
  ymin = yvals[0];
  ymax = yvals[0];

  for ( i = 1; i < nlon; i++ )
    {
      if ( xvals[i] < xmin ) xmin = xvals[i];
      if ( xvals[i] > xmax ) xmax = xvals[i];
    }

  for ( i = 1; i < nlat; i++ )
    {
      if ( yvals[i] < ymin ) ymin = yvals[i];
      if ( yvals[i] > ymax ) ymax = yvals[i];
    }

  for ( ilat = 0; ilat < nlat; ilat++ )
    {
      yval = yvals[ilat];
      for ( ilon = 0; ilon < nlon; ilon++ )
	{
          c = 0;
	  xval = xvals[ilon];
	  if (!( ( ( xval > xmin ) || ( xval < xmax ) ) || ( (yval > ymin) || (yval < ymax) ) ) ) c = !c;
	  	  
          if ( c == 0 )
	    {
	      for (i = 0, j = nofcoords-1; i < nofcoords; j = i++)
	    
	      if ((((ycoords[i]<=yval) && (yval<ycoords[j])) ||
		   ((ycoords[j]<=yval) && (yval<ycoords[i]))) &&
		  ((xval) < (xcoords[j] - (xcoords[i])) * (yval - ycoords[i]) / (ycoords[j] - ycoords[i]) +(xcoords[i])))
		c = !c;
	    }

	  if ( c == 0 )
	    {
	      for (i = 0, j = nofcoords-1; i < nofcoords; j = i++)
		{
		  if ( xvals[ilon] > 180 )
		    {
		      if ((((ycoords[i]<=yval) && (yval<ycoords[j])) ||
			   ((ycoords[j]<=yval) && (yval<ycoords[i]))) &&
			  ((xval-360) < (xcoords[j] - (xcoords[i])) * (yval - ycoords[i]) / (ycoords[j] - ycoords[i]) +(xcoords[i])))
			c = !c;
		    }
		}
	    }
 
	  if ( c == 0 )
	    {
	      for ( i = 0, j = nofcoords-1; i< nofcoords; j = i++)
		{		
		  if ( xval<0 )
		    {
		      if ((((ycoords[i]<=yval) && (yval<ycoords[j])) ||
			   ((ycoords[j]<=yval) && (yval<ycoords[i]))) &&
			  ((xval+360) < (xcoords[j] - (xcoords[i])) * (yval - ycoords[i]) / (ycoords[j] - ycoords[i]) +(xcoords[i])))
			c = !c;
		    }
		}
	    }
	     
	  if( c != 0 ) mask[nlon*ilat+ilon] =  0;
	}
    }
      
  free(xvals);
  free(yvals);
}


void *Maskbox(void *argument)
{
  int MASKLONLATBOX, MASKINDEXBOX, MASKREGION;
  int operatorID;
  int streamID1, streamID2;
  int nrecs, nvars;
  int tsID, recID, varID, levelID;
  int gridsize;
  int vlistID1, vlistID2;
  int gridID = -1;
  int index, ngrids, gridtype;
  int nmiss;
  int *vars;
  int i, i2;
  int ndiffgrids;
  int lat1, lat2, lon11, lon12, lon21, lon22;
  int number = 0, nfiles;
  double missval;
  int *mask;
  double *array;
  int taxisID1, taxisID2;
  double *xcoords, *ycoords;
  FILE *fp;
  const char *polyfile;

  cdoInitialize(argument);

  MASKLONLATBOX = cdoOperatorAdd("masklonlatbox", 0, 0, "western and eastern longitude and southern and northern latitude");
  MASKINDEXBOX  = cdoOperatorAdd("maskindexbox",  0, 0, "index of first and last longitude and index of first and last latitude");
  MASKREGION    = cdoOperatorAdd("maskregion",    0, 0, "limiting coordinates of the region");

  operatorID = cdoOperatorID();

  operatorInputArg(cdoOperatorEnter(operatorID));

  streamID1 = streamOpenRead(cdoStreamName(0));

  vlistID1 = streamInqVlist(streamID1);

  ngrids = vlistNgrids(vlistID1);
  ndiffgrids = 0;
  for ( index = 1; index < ngrids; index++ )
    if ( vlistGrid(vlistID1, 0) != vlistGrid(vlistID1, index))
      ndiffgrids++;

  for ( index = 0; index < ngrids; index++ )
    {
      gridID   = vlistGrid(vlistID1, index);
      gridtype = gridInqType(gridID);

      if ( gridtype == GRID_LONLAT || gridtype == GRID_GAUSSIAN ) break;
      if ( gridtype == GRID_CURVILINEAR ) break;
      if ( operatorID == MASKINDEXBOX && gridtype == GRID_GENERIC &&
	  gridInqXsize(gridID) > 0 && gridInqYsize(gridID) > 0 ) break;
    }

  if ( gridInqType(gridID) == GRID_GAUSSIAN_REDUCED )
    cdoAbort("Gaussian reduced grid found. Use option -R to convert it to a regular grid!");

  if ( index == ngrids ) cdoAbort("No regular grid found!");
  if ( ndiffgrids > 0 )  cdoAbort("Too many different grids!");

  operatorInputArg(cdoOperatorEnter(operatorID));

  vlistID2 = vlistDuplicate(vlistID1);

  taxisID1 = vlistInqTaxis(vlistID1);
  taxisID2 = taxisDuplicate(taxisID1);
  vlistDefTaxis(vlistID2, taxisID2);

  nvars = vlistNvars(vlistID1);
  vars  = (int*) malloc(nvars*sizeof(int));
  for ( varID = 0; varID < nvars; varID++ )
    {
      if ( gridID == vlistInqVarGrid(vlistID1, varID) )
	vars[varID] = TRUE;
      else
	vars[varID] = FALSE;
    }

  streamID2 = streamOpenWrite(cdoStreamName(1), cdoFiletype());

  streamDefVlist(streamID2, vlistID2);

  gridsize = gridInqSize(gridID);
  array = (double*) malloc( gridsize*sizeof(double));
  mask  = (int*) malloc( gridsize*sizeof(int));
  for( i=0;  i < gridsize; i++) mask[i] = 1;
 
  if ( operatorID == MASKLONLATBOX )
    {
      genlonlatbox(0, gridID, &lat1, &lat2, &lon11, &lon12, &lon21, &lon22);
      maskbox(mask, gridID, lat1, lat2, lon11, lon12, lon21, lon22);
    }
  if ( operatorID == MASKINDEXBOX )
    {
      genindexbox(0, gridID, &lat1, &lat2, &lon11, &lon12, &lon21, &lon22);
      maskbox(mask, gridID, lat1, lat2, lon11, lon12, lon21, lon22);
    }
  if ( operatorID == MASKREGION )
    {
      xcoords = (double*) malloc( MAX_VALS*sizeof(double));
      ycoords = (double*) malloc( MAX_VALS*sizeof(double));
      nfiles = operatorArgc();
     
      for ( i2 = 0; i2 < nfiles; i2++ )
	{
	  polyfile = operatorArgv()[i2];
	  fp = fopen(polyfile, "r");
	 
	  if ( fp == 0 ) cdoAbort("Open failed on %s", polyfile);   
	  while ( TRUE )
	    {
	      number = ReadCoords (xcoords, ycoords, polyfile, fp );
	      if ( number == 0 ) break;
	      if ( number < 3 ) cdoAbort( "Too less values in file %s", polyfile );
	      maskregion(mask, gridID, xcoords, ycoords, number);
	    }
	  fclose(fp); 
	}
      if ( xcoords ) free ( xcoords );
      if ( ycoords ) free ( ycoords );
    }

  tsID = 0;
  while ( (nrecs = streamInqTimestep(streamID1, tsID)) )
    {
      taxisCopyTimestep(taxisID2, taxisID1);

      streamDefTimestep(streamID2, tsID);
	       
      for ( recID = 0; recID < nrecs; recID++ )
	{
	  streamInqRecord(streamID1, &varID, &levelID);

	  if ( vars[varID] )
	    {
	      streamReadRecord(streamID1, array, &nmiss);
              missval = vlistInqVarMissval(vlistID1, varID);
             
	      for ( i = 0; i < gridsize; i++ )
		{
		  if ( mask[i] ) array[i] = missval;
		}
		
	      nmiss = 0;

	      for ( i = 0; i < gridsize; i++ )
		if ( DBL_IS_EQUAL(array[i], missval) ) nmiss++;

	      streamDefRecord(streamID2, varID, levelID);
	      streamWriteRecord(streamID2, array, nmiss);
	    }
	}
      tsID++;
    }

  streamClose(streamID2);
  streamClose(streamID1);

  if ( vars  ) free(vars);
  if ( array ) free(array);
  if ( mask )  free(mask);

  cdoFinish();

  return (0);
}
