#if defined (HAVE_CONFIG_H)
#  include "config.h"
#endif

#include <string.h>
#include <math.h>
#include <float.h>

#include "dmemory.h"

#include "cdi.h"
#include "cdi_cksum.h"
#include "cdi_int.h"
#include "resource_handle.h"
#include "resource_unpack.h"
#include "varscan.h"
#include "namespace.h"
#include "serialize.h"

#define  LevelUp    1
#define  LevelDown  2


static const struct {
  unsigned char positive;   // 1: up;  2: down
  char *name;
  char *longname;
  char *stdname;
  char *units;
}
ZaxistypeEntry[] = {
  { /*  0 */ 0, "sfc",               "surface",                "",               ""},
  { /*  1 */ 0, "lev",               "generic",                "",               "level"},
  { /*  2 */ 2, "lev",               "hybrid",                 "",               "level"},
  { /*  3 */ 2, "lev",               "hybrid_half",            "",               "level"},
  { /*  4 */ 2, "lev",               "pressure",               "air_pressure",   "Pa"},
  { /*  5 */ 1, "height",            "height",                 "height",         "m"},
  { /*  6 */ 2, "depth",             "depth_below_sea",        "depth",          "m"},
  { /*  7 */ 2, "depth",             "depth_below_land",       "",               "cm"},
  { /*  8 */ 0, "lev",               "isentropic",             "",               "K"},
  { /*  9 */ 0, "lev",               "trajectory",             "",               ""},
  { /* 10 */ 1, "alt",               "altitude",               "",               "m"},
  { /* 11 */ 0, "lev",               "sigma",                  "",               "level"},
  { /* 12 */ 0, "lev",               "meansea",                "",               "level"},
  { /* 13 */ 0, "toa",               "top_of_atmosphere",      "",               ""},
  { /* 14 */ 0, "seabottom",         "sea_bottom",             "",               ""},
  { /* 15 */ 0, "atmosphere",        "atmosphere",             "",               ""},
  { /* 16 */ 0, "cloudbase",         "cloud_base",             "",               ""},
  { /* 17 */ 0, "cloudtop",          "cloud_top",              "",               ""},
  { /* 18 */ 0, "isotherm0",         "isotherm_zero",          "",               ""},
  { /* 19 */ 0, "snow",              "snow",                   "",               ""},
  { /* 20 */ 0, "lakebottom",        "lake_bottom",            "",               ""},
  { /* 21 */ 0, "sedimentbottom",    "sediment_bottom",        "",               ""},
  { /* 22 */ 0, "sedimentbottomta",  "sediment_bottom_ta",     "",               ""},
  { /* 23 */ 0, "sedimentbottomtw",  "sediment_bottom_tw",     "",               ""},
  { /* 24 */ 0, "mixlayer",          "mix_layer",              "",               ""},
  { /* 25 */ 0, "height",            "generalized height",     "height",         ""},
};

enum {
  CDI_NumZaxistype = sizeof(ZaxistypeEntry) / sizeof(ZaxistypeEntry[0]),
};


typedef struct {
  unsigned char positive;
  char     name[CDI_MAX_NAME];
  char     longname[CDI_MAX_NAME];
  char     stdname[CDI_MAX_NAME];
  char     units[CDI_MAX_NAME];
  double  *vals;
  double  *lbounds;
  double  *ubounds;
  double  *weights;
  int      self;
  int      prec;
  int      type;
  int      ltype;    /* GRIB level type */
  int      size;
  int      direction;
  int      vctsize;
  double  *vct;
  int      number;   /* Reference number to a generalized Z-axis */
  int      nhlev;
  char     uuid[17];
}
zaxis_t;

static int zaxisCompareP(zaxis_t *z1, zaxis_t *z2);
static void   zaxisDestroyP    ( void * zaxisptr );
static void   zaxisPrintP      ( void * zaxisptr, FILE * fp );
static int    zaxisGetPackSize ( void * zaxisptr, void *context);
static void   zaxisPack        ( void * zaxisptr, void * buffer, int size, int *pos, void *context);
static int    zaxisTxCode      ( void );

static const resOps zaxisOps = {
  (int (*)(void *, void *))zaxisCompareP,
  zaxisDestroyP,
  zaxisPrintP,
  zaxisGetPackSize,
  zaxisPack,
  zaxisTxCode
};

static int  ZAXIS_Debug = 0;   /* If set to 1, debugging */

static
void zaxisDefaultValue ( zaxis_t *zaxisptr )
{
  zaxisptr->self        = CDI_UNDEFID;
  zaxisptr->name[0]     = 0;
  zaxisptr->longname[0] = 0;
  zaxisptr->stdname[0]  = 0;
  zaxisptr->units[0]    = 0;
  zaxisptr->vals        = NULL;
  zaxisptr->ubounds     = NULL;
  zaxisptr->lbounds     = NULL;
  zaxisptr->weights     = NULL;
  zaxisptr->type        = CDI_UNDEFID;
  zaxisptr->ltype       = 0;
  zaxisptr->positive    = 0;
  zaxisptr->direction   = 0;
  zaxisptr->prec        = 0;
  zaxisptr->size        = 0;
  zaxisptr->vctsize     = 0;
  zaxisptr->vct         = NULL;
  zaxisptr->number      = 0;
  zaxisptr->nhlev       = 0;
  zaxisptr->uuid[0]     = 0;
}


static
zaxis_t *zaxisNewEntry(int id)
{
  zaxis_t *zaxisptr = (zaxis_t *)xmalloc(sizeof(zaxis_t));

  zaxisDefaultValue ( zaxisptr );

  if (id == CDI_UNDEFID)
    zaxisptr->self = reshPut(zaxisptr, &zaxisOps);
  else
    {
      zaxisptr->self = id;
      reshReplace(id, zaxisptr, &zaxisOps);
    }

  return (zaxisptr);
}

static
void zaxisInit(void)
{
  static int zaxisInitialized = 0;
  char *env;

  if ( zaxisInitialized ) return;

  zaxisInitialized = 1;

  env = getenv("ZAXIS_DEBUG");
  if ( env ) ZAXIS_Debug = atoi(env);
}

static
void zaxis_copy(zaxis_t *zaxisptr2, zaxis_t *zaxisptr1)
{
  int zaxisID2 = zaxisptr2->self;
  memcpy(zaxisptr2, zaxisptr1, sizeof(zaxis_t));
  zaxisptr2->self = zaxisID2;
}

static
void zaxisCheckPtr(const char *caller, int zaxisID, zaxis_t *zaxisptr)
{
  if ( zaxisptr == NULL )
    Errorc("zaxis %d undefined!", zaxisID);
}

#define  zaxis_check_ptr(zaxisID, zaxisptr)  zaxisCheckPtr(__func__, zaxisID, zaxisptr)

int zaxisSize(void)
{
  return reshCountType ( &zaxisOps );
}

static int
zaxisCreate_(int zaxistype, int size, int id)
{
  zaxis_t *zaxisptr = zaxisNewEntry(id);

  zaxisptr->type = zaxistype;
  zaxisptr->size = size;

  if ( zaxistype >= CDI_NumZaxistype || zaxistype < 0 )
    Error("Internal problem! zaxistype > CDI_MaxZaxistype");

  int zaxisID = zaxisptr->self;
  zaxisDefName(zaxisID, ZaxistypeEntry[zaxistype].name);
  zaxisDefLongname(zaxisID, ZaxistypeEntry[zaxistype].longname);
  zaxisDefUnits(zaxisID, ZaxistypeEntry[zaxistype].units);

  if ( *ZaxistypeEntry[zaxistype].stdname )
    strcpy(zaxisptr->stdname, ZaxistypeEntry[zaxistype].stdname);

  zaxisptr->positive = ZaxistypeEntry[zaxistype].positive;

  double *vals = zaxisptr->vals = (double *)xmalloc(size*sizeof(double));

  for ( int ilev = 0; ilev < size; ilev++ )
    vals[ilev] = 0.0;

  return zaxisID;
}


/*
@Function  zaxisCreate
@Title     Create a vertical Z-axis

@Prototype int zaxisCreate(int zaxistype, int size)
@Parameter
    @Item  zaxistype  The type of the Z-axis, one of the set of predefined CDI Z-axis types.
                      The valid CDI Z-axis types are @func{ZAXIS_GENERIC}, @func{ZAXIS_SURFACE},
                      @func{ZAXIS_HYBRID}, @func{ZAXIS_SIGMA}, @func{ZAXIS_PRESSURE}, @func{ZAXIS_HEIGHT},
                      @func{ZAXIS_ISENTROPIC}, @func{ZAXIS_ALTITUDE}, @func{ZAXIS_MEANSEA}, @func{ZAXIS_TOA},
                      @func{ZAXIS_SEA_BOTTOM}, @func{ZAXIS_ATMOSPHERE}, @func{ZAXIS_CLOUD_BASE},
                      @func{ZAXIS_CLOUD_TOP}, @func{ZAXIS_ISOTHERM_ZERO}, @func{ZAXIS_SNOW},
                      @func{ZAXIS_LAKE_BOTTOM}, @func{ZAXIS_SEDIMENT_BOTTOM}, @func{ZAXIS_SEDIMENT_BOTTOM_TA},
                      @func{ZAXIS_SEDIMENT_BOTTOM_TW}, @func{ZAXIS_MIX_LAYER},
                      @func{ZAXIS_DEPTH_BELOW_SEA} and @func{ZAXIS_DEPTH_BELOW_LAND}.
    @Item  size       Number of levels.

@Description
The function @func{zaxisCreate} creates a vertical Z-axis.

@Result
@func{zaxisCreate} returns an identifier to the Z-axis.

@Example
Here is an example using @func{zaxisCreate} to create a pressure level Z-axis:

@Source
#include "cdi.h"
   ...
#define  nlev    5
   ...
double levs[nlev] = {101300, 92500, 85000, 50000, 20000};
int zaxisID;
   ...
zaxisID = zaxisCreate(ZAXIS_PRESSURE, nlev);
zaxisDefLevels(zaxisID, levs);
   ...
@EndSource
@EndFunction
*/
int zaxisCreate(int zaxistype, int size)
{
  if ( CDI_Debug )
    Message("zaxistype: %d size: %d ", zaxistype, size);

  zaxisInit ();
  return zaxisCreate_(zaxistype, size, CDI_UNDEFID);
}


static void zaxisDestroyKernel( zaxis_t * zaxisptr )
{
  int id;

  xassert ( zaxisptr );

  id = zaxisptr->self;

  if ( zaxisptr->vals )    free ( zaxisptr->vals );
  if ( zaxisptr->lbounds ) free ( zaxisptr->lbounds );
  if ( zaxisptr->ubounds ) free ( zaxisptr->ubounds );
  if ( zaxisptr->weights ) free ( zaxisptr->weights );
  if ( zaxisptr->vct )     free ( zaxisptr->vct );

  free ( zaxisptr );

  reshRemove ( id, &zaxisOps );
}

/*
@Function  zaxisDestroy
@Title     Destroy a vertical Z-axis

@Prototype void zaxisDestroy(int zaxisID)
@Parameter
    @Item  zaxisID  Z-axis ID, from a previous call to @fref{zaxisCreate}.

@EndFunction
*/
void zaxisDestroy(int zaxisID)
{
  zaxis_t *zaxisptr = reshGetVal(zaxisID, &zaxisOps);

  zaxisDestroyKernel ( zaxisptr );
}


static
void zaxisDestroyP ( void * zaxisptr )
{
  zaxisDestroyKernel (( zaxis_t * ) zaxisptr );
}


char *zaxisNamePtr(int zaxistype)
{
  char *name;

  if ( zaxistype >= 0 && zaxistype < CDI_NumZaxistype )
    name = ZaxistypeEntry[zaxistype].longname;
  else
    name = ZaxistypeEntry[ZAXIS_GENERIC].longname;

  return (name);
}


void zaxisName(int zaxistype, char *zaxisname)
{
  strcpy(zaxisname, zaxisNamePtr(zaxistype));
}

/*
@Function  zaxisDefName
@Title     Define the name of a Z-axis

@Prototype void zaxisDefName(int zaxisID, const char *name)
@Parameter
    @Item  zaxisID  Z-axis ID, from a previous call to @fref{zaxisCreate}.
    @Item  name     Name of the Z-axis.

@Description
The function @func{zaxisDefName} defines the name of a Z-axis.

@EndFunction
*/
void zaxisDefName(int zaxisID, const char *name)
{
  zaxis_t *zaxisptr;

  if ( reshGetStatus ( zaxisID, &zaxisOps ) == RESH_CLOSED )
    {
      Warning("%s", "Operation not executed.");
      return;
    }

  zaxisptr = ( zaxis_t * ) reshGetVal ( zaxisID, &zaxisOps );

  zaxis_check_ptr(zaxisID, zaxisptr);

  if ( name )
    {
      strncpy(zaxisptr->name, name, CDI_MAX_NAME - 1);
      zaxisptr->name[CDI_MAX_NAME - 1] = '\0';
    }
}

/*
@Function  zaxisDefLongname
@Title     Define the longname of a Z-axis

@Prototype void zaxisDefLongname(int zaxisID, const char *longname)
@Parameter
    @Item  zaxisID  Z-axis ID, from a previous call to @fref{zaxisCreate}.
    @Item  longname Longname of the Z-axis.

@Description
The function @func{zaxisDefLongname} defines the longname of a Z-axis.

@EndFunction
*/
void zaxisDefLongname(int zaxisID, const char *longname)
{
  zaxis_t *zaxisptr;

  if ( reshGetStatus ( zaxisID, &zaxisOps ) == RESH_CLOSED )
    {
      Warning("%s", "Operation not executed.");
      return;
    }

  zaxisptr = ( zaxis_t * ) reshGetVal ( zaxisID, &zaxisOps );

  zaxis_check_ptr(zaxisID, zaxisptr);

  if ( longname )
    {
      strncpy(zaxisptr->longname, longname, CDI_MAX_NAME - 1);
      zaxisptr->longname[CDI_MAX_NAME - 1] = '\0';
    }
}

/*
@Function  zaxisDefUnits
@Title     Define the units of a Z-axis

@Prototype void zaxisDefUnits(int zaxisID, const char *units)
@Parameter
    @Item  zaxisID  Z-axis ID, from a previous call to @fref{zaxisCreate}.
    @Item  units    Units of the Z-axis.

@Description
The function @func{zaxisDefUnits} defines the units of a Z-axis.

@EndFunction
*/
void zaxisDefUnits(int zaxisID, const char *units)
{
  zaxis_t *zaxisptr;

  if ( reshGetStatus ( zaxisID, &zaxisOps ) == RESH_CLOSED )
    {
      Warning("%s", "Operation not executed.");
      return;
    }

  zaxisptr = ( zaxis_t * ) reshGetVal ( zaxisID, &zaxisOps );

  zaxis_check_ptr(zaxisID, zaxisptr);

  if ( units )
    {
      strncpy(zaxisptr->units, units, CDI_MAX_NAME - 1);
      zaxisptr->units[CDI_MAX_NAME - 1] = '\0';
    }
}

/*
@Function  zaxisInqName
@Title     Get the name of a Z-axis

@Prototype void zaxisInqName(int zaxisID, char *name)
@Parameter
    @Item  zaxisID  Z-axis ID, from a previous call to @fref{zaxisCreate} or @fref{vlistInqVarZaxis}.
    @Item  name     Name of the Z-axis. The caller must allocate space for the
                    returned string. The maximum possible length, in characters, of
                    the string is given by the predefined constant @func{CDI_MAX_NAME}.

@Description
The function @func{zaxisInqName} returns the name of a Z-axis.

@Result
@func{zaxisInqName} returns the name of the Z-axis to the parameter name.

@EndFunction
*/
void zaxisInqName(int zaxisID, char *name)
{
  zaxis_t *zaxisptr = reshGetVal(zaxisID, &zaxisOps);

  zaxis_check_ptr(zaxisID, zaxisptr);

  strcpy(name, zaxisptr->name);
}

/*
@Function  zaxisInqLongname
@Title     Get the longname of a Z-axis

@Prototype void zaxisInqLongname(int zaxisID, char *longname)
@Parameter
    @Item  zaxisID  Z-axis ID, from a previous call to @fref{zaxisCreate} or @fref{vlistInqVarZaxis}.
    @Item  longname Longname of the Z-axis. The caller must allocate space for the
                    returned string. The maximum possible length, in characters, of
                    the string is given by the predefined constant @func{CDI_MAX_NAME}.

@Description
The function @func{zaxisInqLongname} returns the longname of a Z-axis.

@Result
@func{zaxisInqLongname} returns the longname of the Z-axis to the parameter longname.

@EndFunction
*/
void zaxisInqLongname(int zaxisID, char *longname)
{
  zaxis_t *zaxisptr = reshGetVal(zaxisID, &zaxisOps);

  zaxis_check_ptr(zaxisID, zaxisptr);

  strcpy(longname, zaxisptr->longname);
}

/*
@Function  zaxisInqUnits
@Title     Get the units of a Z-axis

@Prototype void zaxisInqUnits(int zaxisID, char *units)
@Parameter
    @Item  zaxisID  Z-axis ID, from a previous call to @fref{zaxisCreate} or @fref{vlistInqVarZaxis}.
    @Item  units    Units of the Z-axis. The caller must allocate space for the
                    returned string. The maximum possible length, in characters, of
                    the string is given by the predefined constant @func{CDI_MAX_NAME}.

@Description
The function @func{zaxisInqUnits} returns the units of a Z-axis.

@Result
@func{zaxisInqUnits} returns the units of the Z-axis to the parameter units.

@EndFunction
*/
void zaxisInqUnits(int zaxisID, char *units)
{
  zaxis_t *zaxisptr = reshGetVal(zaxisID, &zaxisOps);

  zaxis_check_ptr(zaxisID, zaxisptr);

  strcpy(units, zaxisptr->units);
}


void zaxisInqStdname(int zaxisID, char *stdname)
{
  zaxis_t *zaxisptr = reshGetVal(zaxisID, &zaxisOps);

  zaxis_check_ptr(zaxisID, zaxisptr);

  strcpy(stdname, zaxisptr->stdname);
}


void zaxisDefPrec(int zaxisID, int prec)
{
  zaxis_t *zaxisptr;

  if ( reshGetStatus ( zaxisID, &zaxisOps ) == RESH_CLOSED )
    {
      Warning("%s", "Operation not executed.");
      return;
    }

  zaxisptr = ( zaxis_t * ) reshGetVal ( zaxisID, &zaxisOps );

  zaxis_check_ptr(zaxisID, zaxisptr);

  zaxisptr->prec = prec;
}


int zaxisInqPrec(int zaxisID)
{
  zaxis_t *zaxisptr = reshGetVal(zaxisID, &zaxisOps);

  zaxis_check_ptr(zaxisID, zaxisptr);

  return (zaxisptr->prec);
}


void zaxisDefPositive(int zaxisID, int positive)
{
  zaxis_t *zaxisptr;

  if ( reshGetStatus ( zaxisID, &zaxisOps ) == RESH_CLOSED )
    {
      Warning("%s", "Operation not executed.");
      return;
    }

  zaxisptr = ( zaxis_t * ) reshGetVal ( zaxisID, &zaxisOps );

  zaxis_check_ptr(zaxisID, zaxisptr);

  zaxisptr->positive = positive;
}


int zaxisInqPositive(int zaxisID)
{
  zaxis_t *zaxisptr = reshGetVal(zaxisID, &zaxisOps);

  zaxis_check_ptr(zaxisID, zaxisptr);

  return (zaxisptr->positive);
}


void zaxisDefLtype(int zaxisID, int ltype)
{
  zaxis_t *zaxisptr;

  if ( reshGetStatus ( zaxisID, &zaxisOps ) == RESH_CLOSED )
    {
      Warning("%s", "Operation not executed.");
      return;
    }

  zaxisptr = ( zaxis_t * ) reshGetVal ( zaxisID, &zaxisOps );

  zaxis_check_ptr(zaxisID, zaxisptr);

  zaxisptr->ltype = ltype;
}


int zaxisInqLtype(int zaxisID)
{
  zaxis_t *zaxisptr = reshGetVal(zaxisID, &zaxisOps);

  zaxis_check_ptr(zaxisID, zaxisptr);

  return (zaxisptr->ltype);
}

/*
@Function  zaxisDefLevels
@Title     Define the levels of a Z-axis

@Prototype void zaxisDefLevels(int zaxisID, const double *levels)
@Parameter
    @Item  zaxisID  Z-axis ID, from a previous call to @fref{zaxisCreate}.
    @Item  levels   All levels of the Z-axis.

@Description
The function @func{zaxisDefLevels} defines the levels of a Z-axis.

@EndFunction
*/
void zaxisDefLevels(int zaxisID, const double *levels)
{
  zaxis_t *zaxisptr;

  if ( reshGetStatus ( zaxisID, &zaxisOps ) == RESH_CLOSED )
    {
      Warning("%s", "Operation not executed.");
      return;
    }

  zaxisptr = ( zaxis_t * ) reshGetVal ( zaxisID, &zaxisOps );

  zaxis_check_ptr(zaxisID, zaxisptr);

  int size = zaxisptr->size;

  double *vals = zaxisptr->vals;

  for (int ilev = 0; ilev < size; ilev++ )
    vals[ilev] = levels[ilev];
}

/*
@Function  zaxisDefLevel
@Title     Define one level of a Z-axis

@Prototype void zaxisDefLevel(int zaxisID, int levelID, double level)
@Parameter
    @Item  zaxisID  Z-axis ID, from a previous call to @fref{zaxisCreate}.
    @Item  levelID  Level identifier.
    @Item  level    Level.

@Description
The function @func{zaxisDefLevel} defines one level of a Z-axis.

@EndFunction
*/
void zaxisDefLevel(int zaxisID, int levelID, double level)
{
  zaxis_t *zaxisptr;

  if ( reshGetStatus ( zaxisID, &zaxisOps ) == RESH_CLOSED )
    {
      Warning("%s", "Operation not executed.");
      return;
    }

  zaxisptr = ( zaxis_t * ) reshGetVal ( zaxisID, &zaxisOps );

  zaxis_check_ptr(zaxisID, zaxisptr);

  if ( levelID >= 0 && levelID < zaxisptr->size )
    zaxisptr->vals[levelID] = level;
}


void zaxisDefNlevRef(int zaxisID, const int nhlev)
{
  zaxis_t *zaxisptr;

  if ( reshGetStatus ( zaxisID, &zaxisOps ) == RESH_CLOSED )
    {
      Warning("%s", "Operation not executed.");
      return;
    }

  zaxisptr = ( zaxis_t * ) reshGetVal ( zaxisID, &zaxisOps );

  zaxis_check_ptr(zaxisID, zaxisptr);

  zaxisptr->nhlev = nhlev;
}


int zaxisInqNlevRef(int zaxisID)
{
  zaxis_t *zaxisptr = reshGetVal(zaxisID, &zaxisOps);

  zaxis_check_ptr(zaxisID, zaxisptr);

  return (zaxisptr->nhlev);
}

/*
@Function  zaxisDefNumber
@Title     Define the reference number for a generalized Z-axis

@Prototype void zaxisDefNumber(int zaxisID, const int number)
@Parameter
    @Item  zaxisID     Z-axis ID, from a previous call to @fref{zaxisCreate}.
    @Item  number      Reference number for a generalized Z-axis.

@Description
The function @func{zaxisDefNumber} defines the reference number for a generalized Z-axis.

@EndFunction
*/
void zaxisDefNumber(int zaxisID, const int number)
{
  zaxis_t *zaxisptr;

  if ( reshGetStatus ( zaxisID, &zaxisOps ) == RESH_CLOSED )
    {
      Warning("%s", "Operation not executed.");
      return;
    }

  zaxisptr = ( zaxis_t * ) reshGetVal ( zaxisID, &zaxisOps );

  zaxis_check_ptr(zaxisID, zaxisptr);

  zaxisptr->number = number;
}

/*
@Function  zaxisInqNumber
@Title     Get the reference number to a generalized Z-axis

@Prototype int zaxisInqNumber(int zaxisID)
@Parameter
    @Item  zaxisID  Z-axis ID, from a previous call to @fref{zaxisCreate} or @fref{vlistInqVarZaxis}.

@Description
The function @func{zaxisInqNumber} returns the reference number to a generalized Z-axis.

@Result
@func{zaxisInqNumber} returns the reference number to a generalized Z-axis.
@EndFunction
*/
int zaxisInqNumber(int zaxisID)
{
  zaxis_t *zaxisptr = reshGetVal(zaxisID, &zaxisOps);

  zaxis_check_ptr(zaxisID, zaxisptr);

  return (zaxisptr->number);
}

/*
@Function  zaxisDefUUID
@Title     Define the UUID for a genralized Z-axis

@Prototype void zaxisDefUUID(int zaxisID, const char *uuid)
@Parameter
    @Item  zaxisID     Z-axis ID, from a previous call to @fref{zaxisCreate}.
    @Item  uuid        UUID for a generalized Z-axis.

@Description
The function @func{zaxisDefUUID} defines the UUID for a generalized  Z-axis.

@EndFunction
*/
void zaxisDefUUID(int zaxisID, const char *uuid)
{
  zaxis_t *zaxisptr;

    if ( reshGetStatus ( zaxisID, &zaxisOps ) == RESH_CLOSED )
    {
      Warning("%s", "Operation not executed.");
      return;
    }

  zaxisptr = ( zaxis_t * ) reshGetVal ( zaxisID, &zaxisOps );

  zaxis_check_ptr(zaxisID, zaxisptr);

  memcpy(zaxisptr->uuid, uuid, 16);

  return;
}

/*
@Function  zaxisInqUUID
@Title     Get the uuid to a generalized Z-axis

@Prototype void zaxisInqUUID(int zaxisID, char *uuid)
@Parameter
    @Item  zaxisID  Z-axis ID, from a previous call to @fref{zaxisCreate} or @fref{vlistInqVarZaxis}.

@Description
The function @func{zaxisInqUUID} returns the UUID to a generalized Z-axis.

@Result
@func{zaxisInqUUID} returns the UUID to a generalized Z-axis to the parameter uuid.
@EndFunction
*/
void zaxisInqUUID(int zaxisID, char *uuid)
{
  zaxis_t *zaxisptr = reshGetVal(zaxisID, &zaxisOps);

  zaxis_check_ptr(zaxisID, zaxisptr);

  memcpy(uuid, zaxisptr->uuid, 16);
}

/*
@Function  zaxisInqLevel
@Title     Get one level of a Z-axis

@Prototype double zaxisInqLevel(int zaxisID, int levelID)
@Parameter
    @Item  zaxisID  Z-axis ID, from a previous call to @fref{zaxisCreate} or @fref{vlistInqVarZaxis}.
    @Item  levelID  Level index (range: 0 to nlevel-1).

@Description
The function @func{zaxisInqLevel} returns one level of a Z-axis.

@Result
@func{zaxisInqLevel} returns the level of a Z-axis.
@EndFunction
*/
double zaxisInqLevel(int zaxisID, int levelID)
{
  double level = 0;
  zaxis_t *zaxisptr = reshGetVal(zaxisID, &zaxisOps);

  zaxis_check_ptr(zaxisID, zaxisptr);

  if ( levelID >= 0 && levelID < zaxisptr->size )
    level = zaxisptr->vals[levelID];

  return (level);
}

double zaxisInqLbound(int zaxisID, int index)
{
  double level = 0;
  zaxis_t *zaxisptr = reshGetVal(zaxisID, &zaxisOps);

  zaxis_check_ptr(zaxisID, zaxisptr);

  if ( zaxisptr->lbounds )
    if ( index >= 0 && index < zaxisptr->size )
      level = zaxisptr->lbounds[index];

  return (level);
}


double zaxisInqUbound(int zaxisID, int index)
{
  double level = 0;
  zaxis_t *zaxisptr = reshGetVal(zaxisID, &zaxisOps);

  zaxis_check_ptr(zaxisID, zaxisptr);

  if ( zaxisptr->ubounds )
    if ( index >= 0 && index < zaxisptr->size )
      level = zaxisptr->ubounds[index];

  return (level);
}


const double *zaxisInqLevelsPtr(int zaxisID)
{
  zaxis_t *zaxisptr = reshGetVal(zaxisID, &zaxisOps);

  zaxis_check_ptr(zaxisID, zaxisptr);

  return ( zaxisptr->vals );
}

/*
@Function  zaxisInqLevels
@Title     Get all levels of a Z-axis

@Prototype void zaxisInqLevels(int zaxisID, double *levels)
@Parameter
    @Item  zaxisID  Z-axis ID, from a previous call to @fref{zaxisCreate} or @fref{vlistInqVarZaxis}.
    @Item  levels   Pointer to the location into which the levels are read.
                    The caller must allocate space for the returned values.

@Description
The function @func{zaxisInqLevels} returns all levels of a Z-axis.

@Result
@func{zaxisInqLevels} saves all levels to the parameter @func{levels}.
@EndFunction
*/
void zaxisInqLevels(int zaxisID, double *levels)
{
  zaxis_t *zaxisptr = reshGetVal(zaxisID, &zaxisOps);

  zaxis_check_ptr(zaxisID, zaxisptr);

  int size = zaxisptr->size;
  for (int i = 0; i < size; i++ )
    levels[i] =  zaxisptr->vals[i];
}


int zaxisInqLbounds(int zaxisID, double *lbounds)
{
  int size = 0;
  zaxis_t *zaxisptr = reshGetVal(zaxisID, &zaxisOps);

  zaxis_check_ptr(zaxisID, zaxisptr);

  if ( zaxisptr->lbounds )
    {
      size = zaxisptr->size;

      if ( lbounds )
        for (int i = 0; i < size; i++ )
          lbounds[i] =  zaxisptr->lbounds[i];
    }

  return (size);
}


int zaxisInqUbounds(int zaxisID, double *ubounds)
{
  int size = 0;
  zaxis_t *zaxisptr = reshGetVal(zaxisID, &zaxisOps);

  zaxis_check_ptr(zaxisID, zaxisptr);

  if ( zaxisptr->ubounds )
    {
      size = zaxisptr->size;

      if ( ubounds )
        for (int i = 0; i < size; i++ )
          ubounds[i] =  zaxisptr->ubounds[i];
    }

  return (size);
}


int zaxisInqWeights(int zaxisID, double *weights)
{
  int size = 0;
  zaxis_t *zaxisptr = reshGetVal(zaxisID, &zaxisOps);

  zaxis_check_ptr(zaxisID, zaxisptr);

  if ( zaxisptr->weights )
    {
      size = zaxisptr->size;

      if ( weights )
        for ( int i = 0; i < size; i++ )
          weights[i] =  zaxisptr->weights[i];
    }

  return (size);
}


int zaxisInqLevelID(int zaxisID, double level)
{
  int levelID = CDI_UNDEFID;
  zaxis_t *zaxisptr = reshGetVal(zaxisID, &zaxisOps);

  zaxis_check_ptr(zaxisID, zaxisptr);

  int size = zaxisptr->size;
  for ( int i = 0; i < size; i++ )
    if ( fabs(level-zaxisptr->vals[i]) < DBL_EPSILON )
      {
        levelID = i;
        break;
      }

  return (levelID);
}

/*
@Function  zaxisInqType
@Title     Get the type of a Z-axis

@Prototype int zaxisInqType(int zaxisID)
@Parameter
    @Item  zaxisID  Z-axis ID, from a previous call to @fref{zaxisCreate} or @fref{vlistInqVarZaxis}.

@Description
The function @func{zaxisInqType} returns the type of a Z-axis.

@Result
@func{zaxisInqType} returns the type of the Z-axis,
one of the set of predefined CDI Z-axis types.
The valid CDI Z-axis types are @func{ZAXIS_GENERIC}, @func{ZAXIS_SURFACE},
@func{ZAXIS_HYBRID}, @func{ZAXIS_SIGMA}, @func{ZAXIS_PRESSURE}, @func{ZAXIS_HEIGHT},
@func{ZAXIS_ISENTROPIC}, @func{ZAXIS_ALTITUDE}, @func{ZAXIS_MEANSEA}, @func{ZAXIS_TOA},
@func{ZAXIS_SEA_BOTTOM}, @func{ZAXIS_ATMOSPHERE}, @func{ZAXIS_CLOUD_BASE},
@func{ZAXIS_CLOUD_TOP}, @func{ZAXIS_ISOTHERM_ZERO}, @func{ZAXIS_SNOW},
@func{ZAXIS_LAKE_BOTTOM}, @func{ZAXIS_SEDIMENT_BOTTOM}, @func{ZAXIS_SEDIMENT_BOTTOM_TA},
@func{ZAXIS_SEDIMENT_BOTTOM_TW}, @func{ZAXIS_MIX_LAYER},
@func{ZAXIS_DEPTH_BELOW_SEA} and @func{ZAXIS_DEPTH_BELOW_LAND}.

@EndFunction
*/
int zaxisInqType(int zaxisID)
{
  zaxis_t *zaxisptr = reshGetVal(zaxisID, &zaxisOps);

  zaxis_check_ptr(zaxisID, zaxisptr);

  return (zaxisptr->type);
}

/*
@Function  zaxisInqSize
@Title     Get the size of a Z-axis

@Prototype int zaxisInqSize(int zaxisID)
@Parameter
    @Item  zaxisID  Z-axis ID, from a previous call to @fref{zaxisCreate} or @fref{vlistInqVarZaxis}.

@Description
The function @func{zaxisInqSize} returns the size of a Z-axis.

@Result
@func{zaxisInqSize} returns the number of levels of a Z-axis.

@EndFunction
*/
int zaxisInqSize(int zaxisID)
{
  zaxis_t *zaxisptr = reshGetVal(zaxisID, &zaxisOps);

  zaxis_check_ptr(zaxisID, zaxisptr);

  int size = zaxisptr->size;

  return (size);
}


void cdiCheckZaxis(int zaxisID)
{
  zaxis_t *zaxisptr = reshGetVal(zaxisID, &zaxisOps);

  zaxis_check_ptr(zaxisID, zaxisptr);

  if ( zaxisInqType(zaxisID) == ZAXIS_GENERIC )
    {
      int size = zaxisptr->size;
      if ( size > 1 )
        {
          /* check direction */
          if ( ! zaxisptr->direction )
            {
              int ups = 0, downs = 0;
              for ( int i = 1; i < size; i++ )
                {
                  ups += (zaxisptr->vals[i] > zaxisptr->vals[i-1]);
                  downs += (zaxisptr->vals[i] < zaxisptr->vals[i-1]);
                }
              if ( ups == size-1 )
                {
                  zaxisptr->direction = LevelUp;
                }
              else if ( downs == size-1 )
                {
                  zaxisptr->direction = LevelDown;
                }
              else /* !zaxisptr->direction */
                {
                  Warning("Direction undefined for zaxisID %d", zaxisID);
                }
            }
        }
    }
}


void zaxisDefVct(int zaxisID, int size, const double *vct)
{
  zaxis_t *zaxisptr;

  if ( reshGetStatus ( zaxisID, &zaxisOps ) == RESH_CLOSED )
    {
      Warning("%s", "Operation not executed.");
      return;
    }

  zaxisptr = ( zaxis_t * ) reshGetVal ( zaxisID, &zaxisOps );

  zaxis_check_ptr(zaxisID, zaxisptr);

  if ( zaxisptr->vct == 0 )
    {
      zaxisptr->vctsize = size;
      zaxisptr->vct = (double *) malloc(size*sizeof(double));
      memcpy(zaxisptr->vct, vct, size*sizeof(double));
    }
  else
    if ( zaxisptr->vctsize != size )
      Warning("VCT was already defined");
}


void zaxisInqVct(int zaxisID, double *vct)
{
  zaxis_t *zaxisptr = reshGetVal(zaxisID, &zaxisOps);

  zaxis_check_ptr(zaxisID, zaxisptr);

  memcpy(vct, zaxisptr->vct, zaxisptr->vctsize*sizeof(double));
}


int zaxisInqVctSize(int zaxisID)
{
  zaxis_t *zaxisptr = reshGetVal(zaxisID, &zaxisOps);

  zaxis_check_ptr(zaxisID, zaxisptr);

  return (zaxisptr->vctsize);
}


const double *zaxisInqVctPtr(int zaxisID)
{
  zaxis_t *zaxisptr = reshGetVal(zaxisID, &zaxisOps);

  zaxis_check_ptr(zaxisID, zaxisptr);

  return (zaxisptr->vct);
}


void zaxisDefLbounds(int zaxisID, const double *lbounds)
{
  zaxis_t *zaxisptr;

  if ( reshGetStatus ( zaxisID, &zaxisOps ) == RESH_CLOSED )
    {
      Warning("%s", "Operation not executed.");
      return;
    }

  zaxisptr = ( zaxis_t * ) reshGetVal ( zaxisID, &zaxisOps );

  zaxis_check_ptr(zaxisID, zaxisptr);

  size_t size = zaxisptr->size;

  if ( CDI_Debug )
    if ( zaxisptr->lbounds != NULL )
      Warning("Lower bounds already defined for zaxisID = %d", zaxisID);

  if ( zaxisptr->lbounds == NULL )
    zaxisptr->lbounds = (double *)xmalloc(size*sizeof(double));

  memcpy(zaxisptr->lbounds, lbounds, size*sizeof(double));
}


void zaxisDefUbounds(int zaxisID, const double *ubounds)
{
  zaxis_t *zaxisptr;

  if ( reshGetStatus ( zaxisID, &zaxisOps ) == RESH_CLOSED )
    {
      Warning("%s", "Operation not executed.");
      return;
    }

  zaxisptr = ( zaxis_t * ) reshGetVal ( zaxisID, &zaxisOps );

  zaxis_check_ptr(zaxisID, zaxisptr);

  size_t size = zaxisptr->size;

  if ( CDI_Debug )
    if ( zaxisptr->ubounds != NULL )
      Warning("Upper bounds already defined for zaxisID = %d", zaxisID);

  if ( zaxisptr->ubounds == NULL )
    zaxisptr->ubounds = (double *)xmalloc(size*sizeof(double));

  memcpy(zaxisptr->ubounds, ubounds, size*sizeof(double));
}


void zaxisDefWeights(int zaxisID, const double *weights)
{
  zaxis_t *zaxisptr;

  if ( reshGetStatus ( zaxisID, &zaxisOps ) == RESH_CLOSED )
    {
      Warning("%s", "Operation not executed.");
      return;
    }

  zaxisptr = ( zaxis_t * ) reshGetVal ( zaxisID, &zaxisOps );

  zaxis_check_ptr(zaxisID, zaxisptr);

  size_t size = zaxisptr->size;

  if ( CDI_Debug )
    if ( zaxisptr->weights != NULL )
      Warning("Weights already defined for zaxisID = %d", zaxisID);

  if ( zaxisptr->weights == NULL )
    zaxisptr->weights = (double *)xmalloc(size*sizeof(double));

  memcpy(zaxisptr->weights, weights, size*sizeof(double));
}


void zaxisChangeType(int zaxisID, int zaxistype)
{
  zaxis_t *zaxisptr = reshGetVal(zaxisID, &zaxisOps);

  zaxis_check_ptr(zaxisID, zaxisptr);

  zaxisptr->type = zaxistype;
}


void zaxisResize(int zaxisID, int size)
{
  zaxis_t *zaxisptr = reshGetVal(zaxisID, &zaxisOps);

  zaxis_check_ptr(zaxisID, zaxisptr);

  zaxisptr->size = size;

  if ( zaxisptr->vals )
    zaxisptr->vals = (double *)xrealloc(zaxisptr->vals, size*sizeof(double));
}


int zaxisDuplicate(int zaxisID)
{
  int zaxisIDnew;
  int zaxistype, zaxissize;
  int size;
  zaxis_t *zaxisptr = reshGetVal(zaxisID, &zaxisOps);

  zaxis_check_ptr(zaxisID, zaxisptr);

  zaxistype = zaxisInqType(zaxisID);
  zaxissize = zaxisInqSize(zaxisID);

  zaxisIDnew = zaxisCreate(zaxistype, zaxissize);
  zaxis_t *zaxisptrnew = reshGetVal(zaxisIDnew, &zaxisOps);

  zaxis_copy(zaxisptrnew, zaxisptr);

  strcpy(zaxisptrnew->name, zaxisptr->name);
  strcpy(zaxisptrnew->longname, zaxisptr->longname);
  strcpy(zaxisptrnew->units, zaxisptr->units);

  if ( zaxisptr->vals != NULL )
    {
      size = zaxissize;

      zaxisptrnew->vals = (double *)xmalloc(size*sizeof(double));
      memcpy(zaxisptrnew->vals, zaxisptr->vals, size*sizeof(double));
    }

  if ( zaxisptr->lbounds )
    {
      size = zaxissize;

      zaxisptrnew->lbounds = (double *)xmalloc(size*sizeof(double));
      memcpy(zaxisptrnew->lbounds, zaxisptr->lbounds, size*sizeof(double));
    }

  if ( zaxisptr->ubounds )
    {
      size = zaxissize;

      zaxisptrnew->ubounds = (double *)xmalloc(size*sizeof(double));
      memcpy(zaxisptrnew->ubounds, zaxisptr->ubounds, size*sizeof(double));
    }

  if ( zaxisptr->vct != NULL )
    {
      size = zaxisptr->vctsize;

      if ( size )
        {
          zaxisptrnew->vctsize = size;
          zaxisptrnew->vct = (double *)xmalloc(size*sizeof(double));
          memcpy(zaxisptrnew->vct, zaxisptr->vct, size*sizeof(double));
        }
    }

  return (zaxisIDnew);
}


void zaxisPrintKernel ( zaxis_t * zaxisptr, FILE * fp )
{
  int zaxisID;
  int type;
  char uuid[17];
  int nlevels, levelID;
  int nbyte0, nbyte;
  double level;

  xassert ( zaxisptr );

  zaxisID = zaxisptr->self;

  type    = zaxisptr->type;
  nlevels = zaxisptr->size;

  nbyte0 = 0;
  fprintf(fp, "#\n");
  fprintf(fp, "# zaxisID %d\n", zaxisID);
  fprintf(fp, "#\n");
  fprintf(fp, "zaxistype = %s\n", zaxisNamePtr(type));
  fprintf(fp, "size      = %d\n", nlevels);
  if ( zaxisptr->name[0]     ) fprintf(fp, "name      = %s\n", zaxisptr->name);
  if ( zaxisptr->longname[0] ) fprintf(fp, "longname  = %s\n", zaxisptr->longname);
  if ( zaxisptr->units[0]    ) fprintf(fp, "units     = %s\n", zaxisptr->units);

  nbyte0 = fprintf(fp, "levels    = ");
  nbyte = nbyte0;
  for ( levelID = 0; levelID < nlevels; levelID++ )
    {
      if ( nbyte > 80 )
	{
	  fprintf(fp, "\n");
	  fprintf(fp, "%*s", nbyte0, "");
	  nbyte = nbyte0;
	}
      level = zaxisInqLevel(zaxisID, levelID);
      nbyte += fprintf(fp, "%.9g ", level);
    }
  fprintf(fp, "\n");

  if ( zaxisptr->lbounds && zaxisptr->ubounds )
    {
      double level1, level2;
      nbyte = nbyte0;
      nbyte0 = fprintf(fp, "bounds    = ");
      for ( levelID = 0; levelID < nlevels; levelID++ )
	{
	  if ( nbyte > 80 )
	    {
	      fprintf(fp, "\n");
	      fprintf(fp, "%*s", nbyte0, "");
	      nbyte = nbyte0;
	    }
	  level1 = zaxisInqLbound(zaxisID, levelID);
	  level2 = zaxisInqUbound(zaxisID, levelID);
	  nbyte += fprintf(fp, "%.9g-%.9g ", level1, level2);
	}
      fprintf(fp, "\n");
    }

  if ( type == ZAXIS_HYBRID || type == ZAXIS_HYBRID_HALF )
    {
      int i;
      int vctsize;
      const double *vct;

      vctsize = zaxisptr->vctsize;
      vct     = zaxisptr->vct;
      fprintf(fp, "vctsize   = %d\n", vctsize);
      if ( vctsize )
        {
          nbyte0 = fprintf(fp, "vct       = ");
          nbyte = nbyte0;
          for ( i = 0; i < vctsize; i++ )
            {
              if ( nbyte > 70 || i == vctsize/2 )
                {
                  fprintf(fp, "\n%*s", nbyte0, "");
                  nbyte = nbyte0;
                }
              nbyte += fprintf(fp, "%.9g ", vct[i]);
            }
          fprintf(fp, "\n");
          /*
          nbyte0 = fprintf(fp, "vct_b     = ");
          nbyte  = nbyte0;
          for ( i = 0; i < vctsize/2; i++ )
            {
              if ( nbyte > 70 )
                {
                  fprintf(fp, "\n%*s", nbyte0, "");
                  nbyte = nbyte0;
                }
              nbyte += fprintf(fp, "%.9g ", vct[vctsize/2+i]);
            }
          fprintf(fp, "\n");
          */
        }
    }

  if ( type == ZAXIS_REFERENCE )
    {
      const unsigned char *d;
      zaxisInqUUID(zaxisID, uuid);
      d = (unsigned char *) &uuid;
      fprintf(fp, "uuid      = %02x%02x%02x%02x-%02x%02x-%02x%02x-%02x%02x-%02x%02x%02x%02x%02x%02x\n",
              d[0], d[1], d[2], d[3], d[4], d[5], d[6], d[7],
              d[8], d[9], d[10], d[11], d[12], d[13], d[14], d[15]);
    }
}


void zaxisPrint ( int zaxisID )
{
  zaxis_t *zaxisptr = reshGetVal(zaxisID, &zaxisOps);

  zaxisPrintKernel ( zaxisptr, stdout );
}


static
void zaxisPrintP ( void * voidptr, FILE * fp )
{
  zaxis_t *zaxisptr = ( zaxis_t * ) voidptr;

  xassert ( zaxisptr );

  zaxisPrintKernel(zaxisptr, fp);
}


static int
zaxisCompareP(zaxis_t *z1, zaxis_t *z2)
{
  enum {
    differ = -1,
  };
  int diff = 0;
  xassert(z1 && z2);

  diff |= (z1->type != z2->type)
    | (z1->ltype != z2->ltype)
    | (z1->direction != z2->direction)
    | (z1->prec != z2->prec)
    | (z1->size != z2->size)
    | (z1->vctsize != z2->vctsize)
    | (z1->positive != z2->positive);

  if (diff)
    return differ;
  int size = z1->size;
  int anyPresent = 0;
  int present = (z1->vals != NULL);
  diff |= (present ^ (z2->vals != NULL));
  anyPresent |= present;
  if (!diff && present)
    {
      const double *p = z1->vals, *q = z2->vals;
      for (int i = 0; i < size; i++)
        diff |= IS_NOT_EQUAL(p[i], q[i]);
    }

  present = (z1->lbounds != NULL);
  diff |= (present ^ (z2->lbounds != NULL));
  anyPresent |= present;
  if (!diff && present)
    {
      const double *p = z1->lbounds, *q = z2->lbounds;
      for (int i = 0; i < size; i++)
        diff |= IS_NOT_EQUAL(p[i], q[i]);
    }

  present = (z1->ubounds != NULL);
  diff |= (present ^ (z2->ubounds != NULL));
  anyPresent |= present;
  if (!diff && present)
    {
      const double *p = z1->ubounds, *q = z2->ubounds;
      for (int i = 0; i < size; ++i)
        diff |= IS_NOT_EQUAL(p[i], q[i]);
    }

  present = (z1->weights != NULL);
  diff |= (present ^ (z2->weights != NULL));
  anyPresent |= present;
  if (!diff && present)
    {
      const double *p = z1->weights, *q = z2->weights;
      for (int i = 0; i < size; ++i)
        diff |= IS_NOT_EQUAL(p[i], q[i]);
    }

  present = (z1->vct != NULL);
  diff |= (present ^ (z2->vct != NULL));
  if (!diff && present)
    {
      int vctsize = z1->vctsize;
      xassert(vctsize);
      const double *p = z1->vct, *q = z2->vct;
      for (int i = 0; i < vctsize; ++i)
        diff |= IS_NOT_EQUAL(p[i], q[i]);
    }

  if (anyPresent)
    xassert(size);

  diff |= strcmp(z1->name, z2->name)
    | strcmp(z1->longname, z2->longname)
    | strcmp(z1->stdname, z2->stdname)
    | strcmp(z1->units, z2->units);
  return diff;
}


static int
zaxisTxCode ( void )
{
  return ZAXIS;
}

enum { zaxisNint     = 8,
       zaxisNstrings = 4,
       vals     = 1 << 0,
       lbounds  = 1 << 1,
       ubounds  = 1 << 2,
       weights  = 1 << 3,
       vct      = 1 << 4
};


static
int zaxisGetMemberMask ( zaxis_t * zaxisP )
{
  int memberMask = 0;

  if ( zaxisP->vals )      memberMask |= vals;
  if ( zaxisP->lbounds )   memberMask |= lbounds;
  if ( zaxisP->ubounds )   memberMask |= ubounds;
  if ( zaxisP->weights )   memberMask |= weights;
  if ( zaxisP->vct )       memberMask |= vct;

  return memberMask;
}

static int
zaxisGetPackSize(void * voidP, void *context)
{
  zaxis_t * zaxisP = ( zaxis_t * ) voidP;
  int packBufferSize = serializeGetSize(zaxisNint, DATATYPE_INT, context)
    + serializeGetSize(1, DATATYPE_UINT32, context);

  if (zaxisP->vals || zaxisP->lbounds || zaxisP->ubounds || zaxisP->weights)
    xassert(zaxisP->size);

  if ( zaxisP->vals )
    packBufferSize += serializeGetSize(zaxisP->size, DATATYPE_FLT64, context)
      + serializeGetSize(1, DATATYPE_UINT32, context);

  if ( zaxisP->lbounds )
    packBufferSize += serializeGetSize(zaxisP->size, DATATYPE_FLT64, context)
      + serializeGetSize(1, DATATYPE_UINT32, context);

  if ( zaxisP->ubounds )
    packBufferSize += serializeGetSize(zaxisP->size, DATATYPE_FLT64, context)
      + serializeGetSize(1, DATATYPE_UINT32, context);

  if ( zaxisP->weights )
    packBufferSize += serializeGetSize(zaxisP->size, DATATYPE_FLT64, context)
      + serializeGetSize(1, DATATYPE_UINT32, context);

  if ( zaxisP->vct )
    {
      xassert ( zaxisP->vctsize );
      packBufferSize += serializeGetSize(zaxisP->vctsize, DATATYPE_FLT64, context)
        + serializeGetSize(1, DATATYPE_UINT32, context);
    }

  packBufferSize += serializeGetSize(zaxisNstrings * CDI_MAX_NAME, DATATYPE_TXT, context)
    + serializeGetSize(1, DATATYPE_UINT32, context)
    + serializeGetSize(1, DATATYPE_UCHAR, context);
  return packBufferSize;
}


void
zaxisUnpack(char * unpackBuffer, int unpackBufferSize,
            int * unpackBufferPos, int originNamespace, void *context,
            int force_id)
{
  int intBuffer[zaxisNint], memberMask;
  uint32_t d;
  char charBuffer[zaxisNstrings * CDI_MAX_NAME];

  serializeUnpack(unpackBuffer, unpackBufferSize, unpackBufferPos,
                  intBuffer, zaxisNint, DATATYPE_INT, context);
  serializeUnpack(unpackBuffer, unpackBufferSize, unpackBufferPos,
                  &d, 1, DATATYPE_UINT32, context);

  xassert(cdiCheckSum(DATATYPE_INT, zaxisNint, intBuffer) == d);

  zaxisInit();

  zaxis_t *zaxisP
    = zaxisNewEntry(force_id ? namespaceAdaptKey(intBuffer[0], originNamespace)
                    : CDI_UNDEFID);

  zaxisP->prec      = intBuffer[1];
  zaxisP->type      = intBuffer[2];
  zaxisP->ltype     = intBuffer[3];
  zaxisP->size      = intBuffer[4];
  zaxisP->direction = intBuffer[5];
  zaxisP->vctsize   = intBuffer[6];
  memberMask        = intBuffer[7];

  if (memberMask & vals)
    {
      int size;
      xassert((size = zaxisP->size));

      zaxisP->vals = (double*) xmalloc(size * sizeof ( double ));
      serializeUnpack(unpackBuffer, unpackBufferSize, unpackBufferPos,
                      zaxisP->vals, size, DATATYPE_FLT64, context);
      serializeUnpack(unpackBuffer, unpackBufferSize, unpackBufferPos,
                      &d, 1, DATATYPE_UINT32, context);
      xassert(cdiCheckSum(DATATYPE_FLT, size, zaxisP->vals) == d);
    }

  if (memberMask & lbounds)
    {
      int size;
      xassert((size = zaxisP->size));

      zaxisP->lbounds = (double*) xmalloc(size * sizeof (double));
      serializeUnpack(unpackBuffer, unpackBufferSize, unpackBufferPos,
                      zaxisP->lbounds, size, DATATYPE_FLT64, context);
      serializeUnpack(unpackBuffer, unpackBufferSize, unpackBufferPos,
                      &d, 1, DATATYPE_UINT32, context);
      xassert(cdiCheckSum(DATATYPE_FLT, size, zaxisP->lbounds) == d);
    }

  if (memberMask & ubounds)
    {
      int size;
      xassert((size = zaxisP->size));

      zaxisP->ubounds = (double*) xmalloc(size * sizeof (double));
      serializeUnpack(unpackBuffer, unpackBufferSize, unpackBufferPos,
                      zaxisP->ubounds, size, DATATYPE_FLT64, context);
      serializeUnpack(unpackBuffer, unpackBufferSize, unpackBufferPos,
                      &d, 1, DATATYPE_UINT32, context);
      xassert(cdiCheckSum(DATATYPE_FLT, size, zaxisP->ubounds) == d);
    }

  if (memberMask & weights)
    {
      int size;
      xassert((size = zaxisP->size));

      zaxisP->weights = (double*) xmalloc(size * sizeof (double));
      serializeUnpack(unpackBuffer, unpackBufferSize, unpackBufferPos,
                      zaxisP->weights, size, DATATYPE_FLT64, context);
      serializeUnpack(unpackBuffer, unpackBufferSize, unpackBufferPos,
                      &d, 1, DATATYPE_UINT32, context);
      xassert(cdiCheckSum(DATATYPE_FLT, size, zaxisP->weights) == d);
    }

  if ( memberMask & vct )
    {
      int size;
      xassert((size = zaxisP->vctsize));

      zaxisP->vct = (double*) xmalloc(size * sizeof (double));
      serializeUnpack(unpackBuffer, unpackBufferSize, unpackBufferPos,
                      zaxisP->vct, size, DATATYPE_FLT64, context);
      serializeUnpack(unpackBuffer, unpackBufferSize, unpackBufferPos,
                      &d, 1, DATATYPE_UINT32, context);
      xassert(cdiCheckSum(DATATYPE_FLT64, size, zaxisP->vct) == d);
    }

  serializeUnpack(unpackBuffer, unpackBufferSize, unpackBufferPos,
                  charBuffer, zaxisNstrings * CDI_MAX_NAME, DATATYPE_TXT, context);
  serializeUnpack(unpackBuffer, unpackBufferSize, unpackBufferPos,
                  &d, 1, DATATYPE_UINT32, context);

  xassert(d == cdiCheckSum(DATATYPE_TXT, zaxisNstrings * CDI_MAX_NAME, charBuffer));

  memcpy ( zaxisP->name,     &charBuffer[CDI_MAX_NAME * 0], CDI_MAX_NAME );
  memcpy ( zaxisP->longname, &charBuffer[CDI_MAX_NAME * 1], CDI_MAX_NAME );
  memcpy ( zaxisP->stdname,  &charBuffer[CDI_MAX_NAME * 2], CDI_MAX_NAME );
  memcpy ( zaxisP->units,    &charBuffer[CDI_MAX_NAME * 3], CDI_MAX_NAME );

  serializeUnpack(unpackBuffer, unpackBufferSize, unpackBufferPos,
                  &zaxisP->positive, 1, DATATYPE_UCHAR, context);
}

static void
zaxisPack(void * voidP, void * packBuffer, int packBufferSize,
          int * packBufferPos, void *context)
{
  zaxis_t   * zaxisP = ( zaxis_t * ) voidP;
  int intBuffer[zaxisNint];
  uint32_t d;
  char charBuffer[zaxisNstrings * CDI_MAX_NAME];

  intBuffer[0]  = zaxisP->self;
  intBuffer[1]  = zaxisP->prec;
  intBuffer[2]  = zaxisP->type;
  intBuffer[3]  = zaxisP->ltype;
  intBuffer[4]  = zaxisP->size;
  intBuffer[5]  = zaxisP->direction;
  intBuffer[6]  = zaxisP->vctsize;
  intBuffer[7]  = zaxisGetMemberMask ( zaxisP );

  serializePack(intBuffer, zaxisNint, DATATYPE_INT,
                packBuffer, packBufferSize, packBufferPos, context);
  d = cdiCheckSum(DATATYPE_INT, zaxisNint, intBuffer);
  serializePack(&d, 1, DATATYPE_UINT32,
                packBuffer, packBufferSize, packBufferPos, context);


  if ( zaxisP->vals )
    {
      xassert(zaxisP->size);
      serializePack(zaxisP->vals, zaxisP->size, DATATYPE_FLT64,
                    packBuffer, packBufferSize, packBufferPos, context);
      d = cdiCheckSum(DATATYPE_FLT, zaxisP->size, zaxisP->vals );
      serializePack(&d, 1, DATATYPE_UINT32,
                    packBuffer, packBufferSize, packBufferPos, context);
    }

  if (zaxisP->lbounds)
    {
      xassert(zaxisP->size);
      serializePack(zaxisP->lbounds, zaxisP->size, DATATYPE_FLT64,
                    packBuffer, packBufferSize, packBufferPos, context);
      d = cdiCheckSum(DATATYPE_FLT, zaxisP->size, zaxisP->lbounds);
      serializePack(&d, 1, DATATYPE_UINT32,
                    packBuffer, packBufferSize, packBufferPos, context);
    }

  if (zaxisP->ubounds)
    {
      xassert(zaxisP->size);

      serializePack(zaxisP->ubounds, zaxisP->size, DATATYPE_FLT64,
                    packBuffer, packBufferSize, packBufferPos, context);
      d = cdiCheckSum(DATATYPE_FLT, zaxisP->size, zaxisP->ubounds);
      serializePack(&d, 1, DATATYPE_UINT32,
                    packBuffer, packBufferSize, packBufferPos, context);
    }

  if (zaxisP->weights)
    {
      xassert(zaxisP->size);

      serializePack(zaxisP->weights, zaxisP->size, DATATYPE_FLT64,
                    packBuffer, packBufferSize, packBufferPos, context);
      d = cdiCheckSum(DATATYPE_FLT, zaxisP->size, zaxisP->weights);
      serializePack(&d, 1, DATATYPE_UINT32,
                    packBuffer, packBufferSize, packBufferPos, context);
    }

  if (zaxisP->vct)
    {
      xassert(zaxisP->vctsize);

      serializePack(zaxisP->vct, zaxisP->vctsize, DATATYPE_FLT64,
                    packBuffer, packBufferSize, packBufferPos, context);
      d = cdiCheckSum(DATATYPE_FLT64, zaxisP->vctsize, zaxisP->vct);
      serializePack(&d, 1, DATATYPE_UINT32,
                    packBuffer, packBufferSize, packBufferPos, context);
    }

  memcpy ( &charBuffer[CDI_MAX_NAME * 0], zaxisP->name,     CDI_MAX_NAME );
  memcpy ( &charBuffer[CDI_MAX_NAME * 1], zaxisP->longname, CDI_MAX_NAME );
  memcpy ( &charBuffer[CDI_MAX_NAME * 2], zaxisP->stdname,  CDI_MAX_NAME );
  memcpy ( &charBuffer[CDI_MAX_NAME * 3], zaxisP->units,    CDI_MAX_NAME );

  serializePack(charBuffer, zaxisNstrings * CDI_MAX_NAME, DATATYPE_TXT,
                packBuffer, packBufferSize, packBufferPos, context);
  d = cdiCheckSum(DATATYPE_TXT, zaxisNstrings * CDI_MAX_NAME, charBuffer);
  serializePack(&d, 1, DATATYPE_UINT32,
                packBuffer, packBufferSize, packBufferPos, context);

  serializePack(&zaxisP->positive, 1, DATATYPE_UCHAR,
                packBuffer, packBufferSize, packBufferPos, context);
}


void zaxisGetIndexList ( int nzaxis, int * zaxisResHs )
{
  reshGetResHListOfType ( nzaxis, zaxisResHs, &zaxisOps );
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