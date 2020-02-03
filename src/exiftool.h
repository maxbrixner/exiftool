#ifndef EXIFTOOL_H_INCLUDED
#define EXIFTOOL_H_INCLUDED

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <math.h>
#include <dirent.h>
#include <errno.h>
#include <sys/stat.h>

#include "exiflib.h"
#include "exifparser.h"
#include "exifextras.h"

/* -------------------------------------------------------------------------- */
/* definitions                                                                */
/* -------------------------------------------------------------------------- */

#define TASK_HELP 1
#define TASK_PRINT 2
#define TASK_GPS 3
#define TASK_CSV 4

#define ERR_NO_ARG -601
#define ERR_ARG_INVALID -602
#define ERR_OPT_INVALID -603
#define ERR_NO_FILES -604
#define ERR_MEM -605
#define ERR_HELP -606
#define ERR_MALLOC -607
#define ERR_MEMCPY -608
#define ERR_FILEOPEN -609
#define ERR_FILESTAT -610
#define ERR_DIROPEN -611

/* -------------------------------------------------------------------------- */
/* structs                                                                    */
/* -------------------------------------------------------------------------- */

struct options {

    int recursive;
    int verbose;
    int debug;

};

/* -------------------------------------------------------------------------- */
/* varibles                                                                   */
/* -------------------------------------------------------------------------- */

int debug;

/* -------------------------------------------------------------------------- */
/* public functions                                                           */
/* -------------------------------------------------------------------------- */

int main ( int argc,
           char *argv[] );

/* -------------------------------------------------------------------------- */
/* static functions                                                           */
/* -------------------------------------------------------------------------- */

static long int processArgs ( int argc,
                              char *argv[] );

static long int getTask ( char *arg );

static long int getOptions ( int argc,
                             char *argv[],
                             struct options *opt );

static long int allocateTagTable ( char ***tagTable,
                                   int tagTableItemCount );
                            
static long int getTagList ( int argc,
                             char *argv[],
                             char ***tagTable );
                             
static long int allocateFileTable ( char ***fileTable,
                                    int fileTableItemCount );

static long int addFileToFileTable ( char *fileName,
                                     char ***fileTable,
                                     long int fileTableItemCount,
                                     int recursive );
                                    
static long int getFileList ( int argc,
                              char *argv[],
                              char ***fileTable,
                              int recursive );
                              
static void taskHelp ( FILE *stream );

static long int taskPrint ( FILE *stream,
                            struct options *opt,
                            char **fileTable,
                            long int fileTableItemCount,
                            char **tagTable,
                            long int tagTableItemCount  );
                            
static long int taskGps ( FILE *stream,
                          struct options *opt,
                          char **fileTable,
                          long int fileTableItemCount );

static long int taskCsv ( FILE *stream,
                          struct options *opt,
                          char **fileTable,
                          long int fileTableItemCount,
                          char **tagTable,
                          long int tagTableItemCount );
                           
/* -------------------------------------------------------------------------- */

#endif

/* -------------------------------------------------------------------------- */

