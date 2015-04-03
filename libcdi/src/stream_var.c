#if defined (HAVE_CONFIG_H)
#  include "config.h"
#endif

#include <string.h>

#include "dmemory.h"
#include "error.h"

#include "cdi.h"
#include "stream_int.h"


static
void streamvarInitEntry(int streamID, int varID)
{
  stream_t *streamptr;

  streamptr = stream_to_pointer(streamID);

  streamptr->vars[varID].ncvarid      = CDI_UNDEFID;
  streamptr->vars[varID].defmiss      = 0;
  streamptr->vars[varID].nlevs        = 0;
  streamptr->vars[varID].level        = NULL;
  streamptr->vars[varID].lindex       = NULL;

  streamptr->vars[varID].timeID       = CDI_UNDEFID;
  streamptr->vars[varID].gridID       = CDI_UNDEFID;
  streamptr->vars[varID].zaxisID      = CDI_UNDEFID;
  streamptr->vars[varID].level        = NULL;
  streamptr->vars[varID].nlevs        = 0;
}

static
int streamvarNewEntry(int streamID)
{
  int varID = 0;
  int streamvarSize;
  SVARINFO *streamvar;
  stream_t *streamptr;

  streamptr = stream_to_pointer(streamID);

  streamvarSize = streamptr->varsAllocated;
  streamvar     = streamptr->vars;
  /*
    Look for a free slot in streamvar.
    (Create the table the first time through).
  */
  if ( ! streamvarSize )
    {
      int i;

      streamvarSize = 2;
      streamvar = (SVARINFO *) malloc(streamvarSize*sizeof(SVARINFO));
      if ( streamvar == NULL )
	{
          Message("streamvarSize = %d", streamvarSize);
	  SysError("Allocation of SVARINFO failed");
	}

      for ( i = 0; i < streamvarSize; i++ )
	streamvar[i].isUsed = FALSE;
    }
  else
    {
      while ( varID < streamvarSize )
	{
	  if ( ! streamvar[varID].isUsed ) break;
	  varID++;
	}
    }
  /*
    If the table overflows, double its size.
  */
  if ( varID == streamvarSize )
    {
      int i;

      streamvarSize = 2*streamvarSize;
      streamvar = (SVARINFO *) realloc(streamvar, streamvarSize*sizeof(SVARINFO));
      if ( streamvar == NULL )
	{
          Message("streamvarSize = %d", streamvarSize);
	  SysError("Reallocation of SVARINFO failed");
	}
      varID = streamvarSize/2;

      for ( i = varID; i < streamvarSize; i++ )
	streamvar[i].isUsed = FALSE;
    }

  streamptr->varsAllocated = streamvarSize;
  streamptr->vars          = streamvar;

  streamvarInitEntry(streamID, varID);

  streamptr->vars[varID].isUsed = TRUE;

  return (varID);
}


int streamNewVar(int streamID, int gridID, int zaxisID)
{
  int varID;
  int *level;
  int *lindex;
  int nlevs;
  int levID;
  stream_t *streamptr;

  streamptr = stream_to_pointer(streamID);

  if ( CDI_Debug )
    Message("gridID = %d  zaxisID = %d", gridID, zaxisID);

  varID = streamvarNewEntry(streamID);

  streamptr->nvars++;

  streamptr->vars[varID].gridID  = gridID;
  streamptr->vars[varID].zaxisID = zaxisID;

  nlevs = zaxisInqSize(zaxisID);

  level  = (int *) malloc(nlevs*sizeof(int));
  lindex = (int *) malloc(nlevs*sizeof(int));

  for ( levID = 0; levID < nlevs; levID++ )
    level[levID] = CDI_UNDEFID;

  for ( levID = 0; levID < nlevs; levID++ )
    lindex[levID] = levID;

  streamptr->vars[varID].nlevs  = nlevs;
  streamptr->vars[varID].level  = level;
  streamptr->vars[varID].lindex = lindex;

  return (varID);
}
/*
 * Local Variables:
 * c-file-style: "Java"
 * c-basic-offset: 2
 * indent-tabs-mode: nil
 * show-trailing-whitespace: t
 * require-trailing-newline: t
 * End:
 */
