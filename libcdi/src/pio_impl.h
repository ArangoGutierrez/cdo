#ifndef _PIO_IMPL_H
#define _PIO_IMPL_H

#ifdef HAVE_CONFIG_H
#  include "config.h"
#endif

#include <stdbool.h>

#ifdef USE_MPI
#include "mpi.h"

typedef enum 
{
  IO_Open_file,
  IO_Close_file,
  IO_Get_fp,
  IO_Set_fp,
  IO_Send_buffer,
  IO_Finalize
} IO_Server_command;

struct dBuffer
{
  size_t wr_pointer;
  size_t size;
  unsigned char * buffer;
};

typedef int ( * valDestroyFunction ) ( void * );
typedef bool (*eqPredicate)(void *, void *);

typedef struct listSet listSet;

struct fileOpTag
{
  int id;
  int command;
};

/* pio.c */
int encodeFileOpTag(int fileID, int command);
struct fileOpTag decodeFileOpTag(int);

/* pio_dbuffer.c */
int       dbuffer_init ( struct dBuffer **, size_t );
int       dbuffer_push(struct dBuffer *, const void *, size_t);
size_t    dbuffer_data_size ( struct dBuffer * );
int       dbuffer_reset ( struct dBuffer * );
void      dbuffer_cleanup ( struct dBuffer ** );

/* pio_list_set.c */
listSet *listSetNew(valDestroyFunction, eqPredicate);
void listSetDelete(listSet *);
int listSetAdd(listSet *, void *);
bool listSetIsEmpty(listSet *);
int listSetRemove(listSet *, int (*predicate)(void *, void *),
                  void *data);
void *listSetGet(listSet *q, int (*predicate)(void *, void *), void *data);

typedef void (*elemOp)(void *elem, void *data);
void listSetForeach(listSet *q, elemOp func, void *data);

/* pio_mpinonb.c */
int       fowMPINONB ( const char * );
int       fcMPINONB ( int );
size_t    fwMPINONB( int, int, const void *, size_t );
void      initMPINONB ( void );
void      finalizeMPINONB ( void );


/* common functionality for file split between collectors and writer(s) */
int pioSendClose(int);
int pioSendOpen(const char *);
size_t pioSendWrite(int, int, const void *, size_t);
void pioSendInitialize(void);
void pioSendFinalize(void);


/* pio_posixasynch.c */
#ifndef _SX
void pioWriterAIO(void);
#endif

/* pio_posixfpguardsendrecv.c */
void      fpgPOSIXFPGUARDSENDRECV ( void );
int       fowPOSIXFPGUARDSENDRECV ( const char * );
int       fcPOSIXFPGUARDSENDRECV ( int );
size_t    fwPOSIXFPGUARDSENDRECV ( int, int, const void *, size_t );
void      initPOSIXFPGUARDSENDRECV ( void );
void      finalizePOSIXFPGUARDSENDRECV ( void );

/* pio_posixnonb.c */
void pioWriterStdIO(void);

#endif
#endif
/*
 * Local Variables:
 * c-file-style: "Java"
 * c-basic-offset: 2
 * indent-tabs-mode: nil
 * show-trailing-whitespace: t
 * require-trailing-newline: t
 * End:
 */
