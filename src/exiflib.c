/* -------------------------------------------------------------------------- */

#include "exiflib.h"

/* -------------------------------------------------------------------------- */
/* extractExifInfo                                                            */
/* extracts the exif information from a jpg file to an exif table. returns    */
/* the number of exif items if successful or a negative value otherwise.      */
/* -------------------------------------------------------------------------- */

long int extractExifInfo(char *fileName, struct exifItem **exifTable) {
    long int i = 0;
    long int rc = 0;

    FILE *fp = NULL;

    long int exifTableItemCount = 0;

    struct queueItem *ifdQueue = NULL;
    long int ifdQueueItemCount = 0;

    long int exifFormat = 0;
    long int soiMarkerPos = 0;
    long int exifMarkerPos = 0;

    /* open file */

    if ((fp = fopen(fileName, "r")) == NULL) return EXIF_ERR_FILE_OPEN;

    debugger(1, "\nfileName = %s", fileName);

    /* find soi file marker to make sure it is a jpg */

    if ((soiMarkerPos = findMarkerInFile(fp, soiMarker, SOI_MARKER_LENGTH, 0)) <
        0)
        return EXIF_ERR_NO_JPG;

    debugger(1, "soiMarkerPos = %ld", soiMarkerPos);

    /* find exif file marker to make sure there is exif information */

    if ((exifMarkerPos = findMarkerInFile(fp, exifMarker, EXIF_MARKER_LENGTH,
                                          rc + SOI_MARKER_LENGTH)) < 0)
        return EXIF_ERR_NO_EXIF;

    debugger(1, "exifMarkerPos = %ld", exifMarkerPos);

    /* determine exif format (intel or motorola) */

    if ((exifFormat = getExifFormat(fp, exifMarkerPos)) < 0)
        return EXIF_ERR_EXIF_FORMAT;

    debugger(1, "exifFormat = %ld", exifFormat);

    /* add ifd0 to queue */

    if ((rc = addIfdToQueue(&ifdQueue, &ifdQueueItemCount,
                            exifMarkerPos + EXIF_MARKER_LENGTH +
                                2  // exif marker has two extra bytes
                                + EXIF_HEADER_LENGTH,
                            IFD_ID_IFD)) < 0)
        return EXIF_ERR_IFD_QUEUE;

    /* process queue - more queue items will be added during process */

    for (i = 0; i < ifdQueueItemCount; i++) {
        if ((rc = addIfdToExifTable(exifTable, &exifTableItemCount, fp,
                                    ifdQueue[i].ifdPos, ifdQueue[i].ifdID,
                                    exifMarkerPos, exifFormat, &ifdQueue,
                                    &ifdQueueItemCount)) < 0)
            return EXIF_ERR_ADD_IFD;
    }

    /* clean up and return */

    fclose(fp);

    return exifTableItemCount;
}

/* -------------------------------------------------------------------------- */
/* castUInt8                                                                  */
/* converts "bytes in uint8_t and then in long int using "exifFormat".        */
/* returns the converted value if successful or zero instead.                 */
/* -------------------------------------------------------------------------- */

long int castUInt8(unsigned char *bytes, long int exifFormat) {
    long int cast = 0;

    if (exifFormat == EXIF_FORMAT_INTEL)
        cast = (long int)(*((uint8_t *)bytes));

    else if (exifFormat == EXIF_FORMAT_MOTO)
        cast = (long int)(*((uint8_t *)reverseByteOrder(bytes, 1)));

    else
        return 0;

    return cast;
}

/* -------------------------------------------------------------------------- */
/* castUInt16                                                                 */
/* converts "bytes in uint16_t and then in long int using "exifFormat".       */
/* returns the converted value if successful or zero instead.                 */
/* -------------------------------------------------------------------------- */

long int castUInt16(unsigned char *bytes, long int exifFormat) {
    long int cast = 0;

    if (exifFormat == EXIF_FORMAT_INTEL)
        cast = (long int)(*((uint16_t *)bytes));

    else if (exifFormat == EXIF_FORMAT_MOTO)
        cast = (long int)(*((uint16_t *)reverseByteOrder(bytes, 2)));

    else
        return 0;

    return cast;
}

/* -------------------------------------------------------------------------- */
/* castUInt32                                                                 */
/* converts "bytes in uint32_t and then in long int using "exifFormat".       */
/* returns the converted value if successful or zero instead.                 */
/* -------------------------------------------------------------------------- */

long int castUInt32(unsigned char *bytes, long int exifFormat) {
    long int cast = 0;

    if (exifFormat == EXIF_FORMAT_INTEL)
        cast = (long int)(*((uint32_t *)bytes));

    else if (exifFormat == EXIF_FORMAT_MOTO)
        cast = (long int)(*((uint32_t *)reverseByteOrder(bytes, 4)));

    else
        return 0;

    return cast;
}

/* -------------------------------------------------------------------------- */
/* castInt32                                                                  */
/* converts "bytes in int32_t and then in long int using "exifFormat".        */
/* returns the converted value if successful or zero instead.                 */
/* -------------------------------------------------------------------------- */

long int castInt32(unsigned char *bytes, long int exifFormat) {
    long int cast = 0;

    if (exifFormat == EXIF_FORMAT_INTEL)
        cast = (long int)(*((int32_t *)bytes));

    else if (exifFormat == EXIF_FORMAT_MOTO)
        cast = (long int)(*((int32_t *)reverseByteOrder(bytes, 4)));

    else
        return 0;

    return cast;
}

/* -------------------------------------------------------------------------- */
/* reverseByteOrder                                                           */
/* returns "bytes" in reverse byte order. size specifies the number of bytes  */
/* to reverse. returns the reversed char or NULL in case of an error.         */
/* -------------------------------------------------------------------------- */

static unsigned char *reverseByteOrder(unsigned char *bytes, long int size) {
    long int i = 0;
    unsigned char *buf = NULL;

    if ((buf = (unsigned char *)malloc(size)) == NULL) return NULL;

    for (i = 0; i < size; i++) {
        buf[i] = bytes[size - i - 1];
    }

    return buf;
}

/* -------------------------------------------------------------------------- */
/* checkMarker                                                                */
/* checks if a marker of length "markerLength" can be found in a file "fp"    */
/* at a position "markerPos. returns 1 if the marker is found at this         */
/* position or 0 otherwise. in case of an error a negative value is returned. */
/* -------------------------------------------------------------------------- */

static long int checkMarker(FILE *fp, unsigned char *marker,
                            long int markerLength, long int markerPos) {
    long int markerFound = 0;
    unsigned char *buf = NULL;

    if ((buf = (unsigned char *)malloc(markerLength)) == NULL)
        return EXIF_ERR_MALLOC;

    if (fseek(fp, markerPos, SEEK_SET) != 0) {
        free(buf);
        return EXIF_ERR_FILE_READ;
    }

    if (fread(buf, 1, markerLength, fp) != markerLength) {
        free(buf);
        return EXIF_ERR_FILE_READ;
    }

    if (memcmp(buf, marker, markerLength) == 0) markerFound = 1;

    free(buf);

    return markerFound;
}

/* -------------------------------------------------------------------------- */
/* findMarkerInFile                                                           */
/* searches for a marker "marker" of length "markerLength" in a file "fp",    */
/* starting at "startPos". returns the position of the marker if it exists.   */
/* otherwise it returns a negative value.                                     */
/* -------------------------------------------------------------------------- */

static long int findMarkerInFile(FILE *fp, unsigned char *marker,
                                 long int markerLength, long int startPos) {
    long int fileSize = 0;
    long int markerPos = EXIF_ERR_MARKER;
    long int i = 0;

    if (fseek(fp, 0L, SEEK_END) != 0) return EXIF_ERR_FILE_READ;

    if ((fileSize = ftell(fp)) == -1L) return EXIF_ERR_FILE_READ;

    if (fileSize - markerLength <= startPos) return EXIF_ERR_FILE_READ;

    for (i = startPos; i < fileSize - markerLength; i++) {
        if (checkMarker(fp, marker, markerLength, i) == 1) {
            markerPos = i;
            break;
        }
    }

    return markerPos;
}

/* -------------------------------------------------------------------------- */
/* getExifFormat                                                              */
/* checks an exif header for intel/moto in file "fp" at position              */
/* "exifMarkerPos". returns the type of the header if successful or a         */
/* negative value otherwise.                                                  */
/* -------------------------------------------------------------------------- */

static long int getExifFormat(FILE *fp, long int exifMarkerPos) {
    long int exifFormat = 0;

    if (checkMarker(fp, exifHeaderIntel, EXIF_HEADER_LENGTH,
                    exifMarkerPos + EXIF_MARKER_LENGTH + 2) == 1)
        exifFormat = EXIF_FORMAT_INTEL;

    else if (checkMarker(fp, exifHeaderMoto, EXIF_HEADER_LENGTH,
                         exifMarkerPos + EXIF_MARKER_LENGTH + 2) == 1)
        exifFormat = EXIF_FORMAT_MOTO;

    else
        return EXIF_ERR_INVALID_FORMAT;

    return exifFormat;
}

/* -------------------------------------------------------------------------- */
/* getIfdTagCount                                                             */
/* returns the number of tags in an image file directory located in           */
/* the file "fp" at position "ifdPos". if case of an error a negative value   */
/* is returned.                                                               */
/* -------------------------------------------------------------------------- */

static long int getIfdTagCount(FILE *fp, long int ifdPos, long int exifFormat) {
    long int tagCount = 0;
    char buf[2] = "";

    if (fseek(fp, ifdPos, SEEK_SET) != 0) return EXIF_ERR_FILE_READ;

    if (fread(buf, 1, 2, fp) != 2) return EXIF_ERR_FILE_READ;

    tagCount = castUInt16(buf, exifFormat);

    return tagCount;
}

/* -------------------------------------------------------------------------- */
/* getIfdLink                                                                 */
/* returns the link in an image file directory located in                     */
/* the file "fp" at position "ifdPos" with "ifdTagsCount" tags. in case of    */
/* an error a negative value is returned.                                     */
/* -------------------------------------------------------------------------- */

static long int getIfdLink(FILE *fp, long int ifdPos, long int ifdTagCount,
                           long int exifFormat) {
    long int ifdLink = 0;
    unsigned char buf[4] = "";

    if (fseek(fp, ifdPos + IFD_HEADER_LENGTH + EXIF_TAG_LENGTH * ifdTagCount,
              SEEK_SET) != 0)
        return EXIF_ERR_FILE_READ;

    if (fread(buf, 1, 4, fp) != 4) return EXIF_ERR_FILE_READ;

    ifdLink = castUInt32(buf, exifFormat);

    return ifdLink;
}

/* -------------------------------------------------------------------------- */
/* getTagId                                                                   */
/* returns the tag id of the exif tag in file "fp" at position "tagPos".      */
/* in case of an error a negative value is returned.                          */
/* -------------------------------------------------------------------------- */

static long int getTagID(FILE *fp, long int tagPos, long int exifFormat) {
    long int tagID = 0;
    unsigned char buf[2] = "";

    if (fseek(fp, tagPos, SEEK_SET) != 0) return EXIF_ERR_FILE_READ;

    if (fread(&buf, 1, 2, fp) != 2) return EXIF_ERR_FILE_READ;

    tagID = castUInt16(buf, exifFormat);

    return tagID;
}

/* -------------------------------------------------------------------------- */
/* getTagType                                                                 */
/* returns the tag type of the exif tag in file "fp" at position "tagPos".    */
/* in case of an error a negative value is returned.                          */
/* -------------------------------------------------------------------------- */

static long int getTagType(FILE *fp, long int tagPos, long int exifFormat) {
    long int tagType = 0;
    unsigned char buf[2] = "";

    if (fseek(fp, tagPos + 2, SEEK_SET) != 0) return EXIF_ERR_FILE_READ;

    if (fread(&buf, 1, 2, fp) != 2) return EXIF_ERR_FILE_READ;

    tagType = castUInt16(buf, exifFormat);

    return tagType;
}

/* -------------------------------------------------------------------------- */
/* getTagTypeSize                                                             */
/* returns the data type size of type "tagType". in case of an error a        */
/* negative value is returned.                                                */
/* -------------------------------------------------------------------------- */

static long int getTagTypeSize(long int tagType) {
    long int tagTypeSize = 0;

    switch (tagType) {
        case 1:
        case 6:
            tagTypeSize = 1;
            break;
        case 2:
        case 7:
            tagTypeSize = 1;
            break;
        case 3:
        case 8:
            tagTypeSize = 2;
            break;
        case 4:
        case 9:
        case 11:
            tagTypeSize = 4;
            break;
        case 5:
        case 10:
        case 12:
            tagTypeSize = 8;
            break;
        default:
            tagTypeSize = EXIF_ERR_TYPE_SIZE;
            break;
    }

    return tagTypeSize;
}

/* -------------------------------------------------------------------------- */
/* getTagCount                                                                */
/* returns the tag count of the exif tag in file "fp" at position "tagPos".   */
/* in case of an error a negative value is returned.                          */
/* -------------------------------------------------------------------------- */

static long int getTagCount(FILE *fp, long int tagPos, long int exifFormat) {
    long int tagCount = 0;
    unsigned char buf[4] = "";

    if (fseek(fp, tagPos + 4, SEEK_SET) != 0) return EXIF_ERR_FILE_READ;

    if (fread(buf, 1, 4, fp) != 4) return EXIF_ERR_FILE_READ;

    tagCount = castUInt32(buf, exifFormat);

    return tagCount;
}

/* -------------------------------------------------------------------------- */
/* getTagDataPos                                                              */
/* returns the data position of the exif tag in file "fp" at position         */
/* "tagPos" with tyoe size "tagTypeSize" and tag count "tagCount". uses       */
/* "exifMarkerPos" to determine the actual position in the file, not the      */
/* relative one. in case of an error a negative value  is returned.           */
/* -------------------------------------------------------------------------- */

static long int getTagDataPos(FILE *fp, long int tagPos, long int exifFormat,
                              long int tagTypeSize, long int tagCount,
                              long int exifMarkerPos) {
    long int tagDataPos = 0;
    unsigned char buf[4] = "";

    if (fseek(fp, tagPos + 8, SEEK_SET) != 0) return EXIF_ERR_FILE_READ;

    if (tagTypeSize * tagCount <= 4) {
        tagDataPos = tagPos + 8;
    } else {
        if (fread(buf, 1, 4, fp) != 4) return EXIF_ERR_FILE_READ;

        tagDataPos = castUInt32(buf, exifFormat) + exifMarkerPos + 10;
    }

    return tagDataPos;
}

/* -------------------------------------------------------------------------- */
/* getTagData                                                                 */
/* reads the data of an exif tag in file "fp" at position "tagDataPos".       */
/* the length of the data is given by "tagTypeSize" times "tagCount".         */
/* writes the data to "tagData" and returns 0 if successful. Otherwise a      */
/* negative value is returned.                                                */
/* -------------------------------------------------------------------------- */

static long int getTagData(FILE *fp, long int tagDataPos, long int tagTypeSize,
                           long int tagCount, unsigned char *tagData) {
    if (fseek(fp, tagDataPos, SEEK_SET) != 0) return EXIF_ERR_FILE_READ;

    if (fread(tagData, tagTypeSize, tagCount, fp) != tagCount)
        return EXIF_ERR_FILE_READ;

    return 0;
}

/* -------------------------------------------------------------------------- */
/* allocateExifTable                                                          */
/* allocates memory for a new tag item in an existing or new "exifTable"      */
/* if "exifTable" is an existing table "exifTableItemCount" should be greater */
/* than zero. the function allocates one more table item. returns 0 if        */
/* successful, otherwise a value smaller than 0 is returned.                  */
/* -------------------------------------------------------------------------- */

static long int allocateExifTable(struct exifItem **exifTable,
                                  long int exifTableItemCount) {
    if (exifTableItemCount == 0) {
        if ((*exifTable = (struct exifItem *)malloc(sizeof(struct exifItem))) ==
            NULL)
            return EXIF_ERR_MALLOC;
    } else {
        if ((*exifTable = (struct exifItem *)realloc(
                 *exifTable,
                 sizeof(struct exifItem) * (exifTableItemCount + 1))) == NULL)
            return EXIF_ERR_MALLOC;
    }

    return 0;
}

/* -------------------------------------------------------------------------- */
/* allocateIfdQueue                                                           */
/* allocates memory for a new queue item in an existing or new "ifdQueue".    */
/* if "ifdQueue" is an existing table "ifdQueueItemCount" sould be greater    */
/* than zero. the function allocates one more table item. returns 0 if        */
/* successful, otherwise a value smaller than 0 is returned.                  */
/* -------------------------------------------------------------------------- */

static long int allocateIfdQueue(struct queueItem **ifdQueue,
                                 long int ifdQueueItemCount) {
    if (ifdQueueItemCount == 0) {
        if ((*ifdQueue =
                 (struct queueItem *)malloc(sizeof(struct queueItem))) == NULL)
            return EXIF_ERR_MALLOC;
    } else {
        if ((*ifdQueue = (struct queueItem *)realloc(
                 *ifdQueue,
                 sizeof(struct queueItem) * (ifdQueueItemCount + 1))) == NULL)
            return EXIF_ERR_MALLOC;
    }

    return 0;
}

/* -------------------------------------------------------------------------- */
/* addIfdToQueue                                                              */
/* adds a new ifd position "ifdPos" with an ifd id "ifdID" to the ifd queue.  */
/* returns 0 if successful or a negative value otherwise.                     */
/* -------------------------------------------------------------------------- */

static long int addIfdToQueue(struct queueItem **ifdQueue,
                              long int *ifdQueueItemCount, long int ifdPos,
                              long int ifdID) {
    long int rc = 0;

    long int itemNo = *ifdQueueItemCount;

    /* create new queue item */

    if ((rc = allocateIfdQueue(ifdQueue, itemNo)) < 0) return rc;

    /* write exif format and ifd info */

    (*ifdQueue)[itemNo].ifdPos = ifdPos;
    (*ifdQueue)[itemNo].ifdID = ifdID;

    /* increment queue item count */

    *ifdQueueItemCount = *ifdQueueItemCount + 1;

    return 0;
}

/* -------------------------------------------------------------------------- */
/* addItemToExifTable                                                         */
/* adds a single tag item from file "fp" to "exifTable" which already         */
/* contains "exifTableItemCount" items. the tag is specified by its           */
/* position "tagPos". the "ifdID" will be attached to the item.               */
/* if a known offset tag is identified, it will be added to the "ifdQueue"    */
/* which already contains "ifdQueueItemCount" items. "exifMarkerPos" is used  */
/* to determine the data position. returns 0 if successful or a negative      */
/* value otherwise.                                                           */
/* -------------------------------------------------------------------------- */

static long int addItemToExifTable(struct exifItem **exifTable,
                                   long int *exifTableItemCount, FILE *fp,
                                   long int tagPos, long int ifdID,
                                   long int exifMarkerPos, long int exifFormat,
                                   struct queueItem **ifdQueue,
                                   long int *ifdQueueItemCount) {
    long int i = 0;
    long int rc = 0;

    long int itemNo = *exifTableItemCount;

    long int tagID = 0;
    long int tagType = 0;
    long int tagTypeSize = 0;
    long int tagCount = 0;
    long int tagDataPos = 0;

    long int offset = 0;

    unsigned char *tagData = NULL;

    /* create new table item */

    if ((rc = allocateExifTable(exifTable, *exifTableItemCount)) < 0) return rc;

    debugger(2, "adding exif item %ld", itemNo + 1);

    /* write tag position, format and ifd info */

    (*exifTable)[itemNo].exifFormat = exifFormat;
    (*exifTable)[itemNo].ifdID = ifdID;
    (*exifTable)[itemNo].tagPos = tagPos;

    debugger(3, "exifFormat = %ld", exifFormat);
    debugger(3, "ifdID = %ld", ifdID);
    debugger(3, "tagPos = %ld", tagPos);

    /* get and write tag id */

    if ((tagID = getTagID(fp, tagPos, exifFormat)) < 0) return tagID;

    debugger(3, "tagID = 0x%04x", tagID);

    (*exifTable)[itemNo].tagID = tagID;

    /* get and write tag type */

    if ((tagType = getTagType(fp, tagPos, exifFormat)) < 0) return tagType;

    (*exifTable)[itemNo].tagType = tagType;

    debugger(3, "tagType = %ld", tagType);

    /* get and write tag type size */

    if ((tagTypeSize = getTagTypeSize(tagType)) < 0) return tagTypeSize;

    (*exifTable)[itemNo].tagTypeSize = tagTypeSize;

    debugger(3, "tagTypeSize = %ld", tagTypeSize);

    /* get and write tag count */

    if ((tagCount = getTagCount(fp, tagPos, exifFormat)) < 0) return tagCount;

    (*exifTable)[itemNo].tagCount = tagCount;

    debugger(3, "tagCount = %ld", tagCount);

    /* get and write tag data pos */

    if ((tagDataPos = getTagDataPos(fp, tagPos, exifFormat, tagTypeSize,
                                    tagCount, exifMarkerPos)) < 0)
        return tagDataPos;

    (*exifTable)[itemNo].tagDataPos = tagDataPos;

    debugger(3, "tagDataPos = %ld", tagDataPos);

    /* get and write tag data */

    if (((*exifTable)[itemNo].tagData =
             (unsigned char *)malloc(tagTypeSize * tagCount)) == NULL)
        return EXIF_ERR_MALLOC;

    if ((rc = getTagData(fp, tagDataPos, tagTypeSize, tagCount,
                         (*exifTable)[itemNo].tagData)) < 0)
        return rc;

    /* increment table item count */

    *exifTableItemCount = *exifTableItemCount + 1;

    /* add exitoffset and gpsinfo to ifd queue */

    if (tagID == 0x8769) {
        offset = castUInt32((*exifTable)[itemNo].tagData, exifFormat);

        if ((rc =
                 addIfdToQueue(ifdQueue, ifdQueueItemCount,
                               offset + exifMarkerPos + 10,  // relative to TIFF
                               IFD_ID_EXIFOFFSET)) < 0)
            return rc;
    }

    if (tagID == 0x8825) {
        offset = castUInt32((*exifTable)[itemNo].tagData, exifFormat);

        if ((rc =
                 addIfdToQueue(ifdQueue, ifdQueueItemCount,
                               offset + exifMarkerPos + 10,  // relative to TIFF
                               IFD_ID_GPSINFO)) < 0)
            return rc;
    }

    return 0;
}

/* -------------------------------------------------------------------------- */
/* addIfdToExifTable                                                          */
/* adds a complete ifd from file "fp" to "exifTable" which already            */
/* contains "exifTableItemCount" items. the ifd is specified by its           */
/* position "ifdPos". the "ifdID" will be attached to all items.              */
/* if the ifd contains a link, it will be added to the "ifdQueue"             */
/* which already contains "ifdQueueItemCount" items. "exifMarkerPos" is used  */
/* to determine the data position. returns 0 if successful or a negative      */
/* value otherwise.                                                           */
/* -------------------------------------------------------------------------- */

static long int addIfdToExifTable(struct exifItem **exifTable,
                                  long int *exifTableItemCount, FILE *fp,
                                  long int ifdPos, long int ifdID,
                                  long int exifMarkerPos, long int exifFormat,
                                  struct queueItem **ifdQueue,
                                  long int *ifdQueueItemCount) {
    long int rc = 0;
    long int i = 0;
    long int ifdTagCount = 0;
    long int ifdLink = 0;
    long int tagPos = 0;

    debugger(2, "ifdPos = %ld", ifdPos);

    /* get number of tags in image file directory */

    if ((ifdTagCount = getIfdTagCount(fp, ifdPos, exifFormat)) < 0)
        return ifdTagCount;

    debugger(2, "ifdTagCount = %ld", ifdTagCount);

    /* get link to next image file directory */

    if ((ifdLink = getIfdLink(fp, ifdPos, ifdTagCount, exifFormat)) < 0)
        return ifdLink;

    debugger(2, "ifdLink = %ld", ifdLink);

    /* add link to ifd queue */

    if (ifdLink > 0) {
        if ((rc = addIfdToQueue(ifdQueue, ifdQueueItemCount,
                                ifdLink + exifMarkerPos + 10, ifdID)) <
            0)  // relative to TIFF
            return rc;
    }

    /* add items to exif table */

    tagPos = ifdPos + 2;

    for (i = 0; i < ifdTagCount; i++) {
        if ((rc = addItemToExifTable(exifTable, exifTableItemCount, fp, tagPos,
                                     ifdID, exifMarkerPos, exifFormat, ifdQueue,
                                     ifdQueueItemCount)) < 0)
            return rc;

        tagPos = tagPos + 12;
    }

    return 0;
}

/* -------------------------------------------------------------------------- */

void debugger(int debugLevel, char *fmt, ...) {
    if (debugLevel <= debug) {
        va_list va;

        fprintf(stderr, "[DBG] ");

        va_start(va, fmt);

        vfprintf(stderr, fmt, va);

        va_end(va);

        fprintf(stderr, "\n");
    }
}

/* -------------------------------------------------------------------------- */
