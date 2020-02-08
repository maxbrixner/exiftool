#ifndef EXIFEXTRAS_H_INCLUDED
#define EXIFEXTRAS_H_INCLUDED

#include <dirent.h>
#include <errno.h>
#include <math.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#include "exifparser.h"

/* -------------------------------------------------------------------------- */
/* public functions                                                           */
/* -------------------------------------------------------------------------- */

long int printExifInfo(FILE *stream, struct exifItem *exifTable,
                       int exifTableItemCount, char **tagTable,
                       int tagTableItemCount, int verbose);

long int printExifCsv(FILE *stream, struct exifItem *exifTable,
                      int exifTableItemCount, char **tagTable,
                      int tagTableItemCount, int verbose);

/* -------------------------------------------------------------------------- */
/* static functions                                                           */
/* -------------------------------------------------------------------------- */

static long int isInTagTable(char *parsedTagId, char **tagTable,
                             int tagTableItemCount);

/* -------------------------------------------------------------------------- */

#endif

/* -------------------------------------------------------------------------- */
