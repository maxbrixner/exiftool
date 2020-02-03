#ifndef EXIFLIB_H_INCLUDED
#define EXIFLIB_H_INCLUDED

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <math.h>
#include <dirent.h>
#include <errno.h>
#include <sys/stat.h>

/* -------------------------------------------------------------------------- */
/* info box                                                                   */
/* -------------------------------------------------------------------------- */
/*                                                                            */
/*    1) Start of an Intel exif file:                                         */
/*       Integers have the form LL-HH or LL-LH-HL-HH                          */
/*                                                                            */
/*       FF-D8|...|FF-E1-HL-LL|45-78-69-66-00-00|49-49-2A-00-08-00-00-00      */
/*       -----|---|-----------|-----------------|-----------------------      */
/*       JPG  |...|EXIFMARKER |EXIF             |TIFF-HEADER                  */
/*                                                                            */
/*    2) Start of a Motorola exif file:                                       */
/*       Integers have the form HH-LL or HH-HL-LH-LL                          */
/*                                                                            */
/*       FF-D8|...|FF-E1-HL-LL|45-78-69-66-00-00|4D-4D-00-2A-00-00-00-08      */
/*       -----|---|-----------|-----------------|-----------------------      */
/*       JPG  |...|EXIFMARKER |EXIF             |TIFF-HEADER                  */
/*                                                                            */
/*    3) Next, there is an image file directory. The last 4 bytes are         */
/*       a link to the next image file directory (IFD)                        */
/*                                                                            */
/*       XX-XX|XX-XX-XX-XX-XX-XX-XX-XX-XX-XX-XX-XX|...                        */
/*       N    |TAG   1                            |...                        */
/*                                                                            */
/*         ...|XX-XX-XX-XX-XX-XX-XX-XX-XX-XX-XX-XX|XX-XX-XX-XX                */
/*         ...|TAG N                              |LINK                       */
/*                                                                            */
/*    4) A tag has the following form:                                        */
/*                                                                            */
/*       XX-XX|XX-XX|XX-XX-XX-XX|XX-XX-XX-XX                                  */
/*       ID   |TYPE |COUNT      |DATA OR                                      */
/*            |     |           |LINK TO DATA                                 */
/*                                                                            */
/*                                                                            */
/* -------------------------------------------------------------------------- */
/* definitions                                                                */
/* -------------------------------------------------------------------------- */

#define SOI_MARKER_LENGTH 2
#define EXIF_MARKER_LENGTH 2
#define EXIF_HEADER_LENGTH 14
#define IFD_HEADER_LENGTH 2
#define EXIF_TAG_LENGTH 12

#define EXIF_FORMAT_INTEL 1
#define EXIF_FORMAT_MOTO 2

#define EXIF_ERR_FILE_OPEN -701
#define EXIF_ERR_NO_JPG -702
#define EXIF_ERR_NO_EXIF -703
#define EXIF_ERR_EXIF_FORMAT -704
#define EXIF_ERR_IFD_QUEUE -705
#define EXIF_ERR_ADD_IFD -706

#define EXIF_ERR_FILE_READ -707
#define EXIF_ERR_MARKER -708
#define EXIF_ERR_INVALID_FORMAT -709
#define EXIF_ERR_MALLOC -710
#define EXIF_ERR_TYPE_SIZE -711

#define IFD_ID_IFD 1
#define IFD_ID_EXIFOFFSET 2
#define IFD_ID_GPSINFO 3

/* -------------------------------------------------------------------------- */
/* structs                                                                    */
/* -------------------------------------------------------------------------- */

struct exifItem
{

    long int exifFormat;
    long int ifdID;

    long int tagPos;
    long int tagID;
    long int tagType;
    long int tagTypeSize;
    long int tagCount;
    long int tagDataPos;

    unsigned char *tagData;
};

struct queueItem
{

    long int ifdPos;
    long int ifdID;
};

/* -------------------------------------------------------------------------- */
/* exif markers                                                               */
/* -------------------------------------------------------------------------- */

static unsigned char soiMarker[SOI_MARKER_LENGTH] = {0xFF, 0xD8};

static unsigned char exifMarker[EXIF_MARKER_LENGTH] = {0xFF, 0xE1};

static unsigned char exifHeaderIntel[EXIF_HEADER_LENGTH] = {0x45, 0x78,
                                                            0x69, 0x66,
                                                            0x00, 0x00,
                                                            0x49, 0x49,
                                                            0x2A, 0x00,
                                                            0x08, 0x00,
                                                            0x00, 0x00};

static unsigned char exifHeaderMoto[EXIF_HEADER_LENGTH] = {0x45, 0x78,
                                                           0x69, 0x66,
                                                           0x00, 0x00,
                                                           0x4D, 0x4D,
                                                           0x00, 0x2A,
                                                           0x00, 0x00,
                                                           0x00, 0x08};

/* -------------------------------------------------------------------------- */
/* extern variables                                                           */
/* -------------------------------------------------------------------------- */

extern int debug;

/* -------------------------------------------------------------------------- */
/* public functions                                                           */
/* -------------------------------------------------------------------------- */

long int extractExifInfo(char *fileName,
                         struct exifItem **exifTable);

long int castUInt8(unsigned char *bytes,
                   long int exifFormat);

long int castUInt16(unsigned char *bytes,
                    long int exifFormat);

long int castUInt32(unsigned char *bytes,
                    long int exifFormat);

long int castInt32(unsigned char *bytes,
                   long int exifFormat);

void debugger(int debugLevel,
              char *fmt,
              ...);

/* -------------------------------------------------------------------------- */
/* static functions                                                           */
/* -------------------------------------------------------------------------- */

static unsigned char *reverseByteOrder(unsigned char *bytes,
                                       long int size);

static long int checkMarker(FILE *fp,
                            unsigned char *marker,
                            long int markerLength,
                            long int markerPos);

static long int findMarkerInFile(FILE *fp,
                                 unsigned char *marker,
                                 long int markerLength,
                                 long int startPos);

static long int getExifFormat(FILE *fp,
                              long int exifMarkerPos);

static long int getIfdTagCount(FILE *fp,
                               long int ifdPos,
                               long int exifFormat);

static long int getIfdLink(FILE *fp,
                           long int ifdPos,
                           long int ifdTagCount,
                           long int exifFormat);

static long int getTagID(FILE *fp,
                         long int tagPos,
                         long int exifFormat);

static long int getTagType(FILE *fp,
                           long int tagPos,
                           long int exifFormat);

static long int getTagTypeSize(long int tagType);

static long int getTagCount(FILE *fp,
                            long int tagPos,
                            long int exifFormat);

static long int getTagDataPos(FILE *fp,
                              long int tagPos,
                              long int exifFormat,
                              long int tagTypeSize,
                              long int tagCount,
                              long int exifMarkerPos);

static long int getTagData(FILE *fp,
                           long int tagDataPos,
                           long int tagTypeSize,
                           long int tagCount,
                           unsigned char *tagData);

static long int allocateExifTable(struct exifItem **exifTable,
                                  long int exifTableItemCount);

static long int allocateIfdQueue(struct queueItem **ifdQueue,
                                 long int ifdQueueItemCount);

static long int addIfdToQueue(struct queueItem **ifdQueue,
                              long int *ifdQueueItemCount,
                              long int ifdPos,
                              long int ifdID);

static long int addItemToExifTable(struct exifItem **exifTable,
                                   long int *exifTableItemCount,
                                   FILE *fp,
                                   long int tagPos,
                                   long int ifdID,
                                   long int exifMarkerPos,
                                   long int exifFormat,
                                   struct queueItem **ifdQueue,
                                   long int *ifdQueueItemCount);

static long int addIfdToExifTable(struct exifItem **exifTable,
                                  long int *exifTableItemCount,
                                  FILE *fp,
                                  long int ifdPos,
                                  long int ifdID,
                                  long int exifMarkerPos,
                                  long int exifFormat,
                                  struct queueItem **ifdQueue,
                                  long int *ifdQueueItemCount);

/* -------------------------------------------------------------------------- */

#endif

/* -------------------------------------------------------------------------- */
