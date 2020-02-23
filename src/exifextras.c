/* -------------------------------------------------------------------------- */

#include "exifextras.h"

/* -------------------------------------------------------------------------- */
/* printExifInfo                                                              */
/* prints info from "exifTable" of length "exifTableItemCount" to "stream".   */
/* if "tagTable" is not null, only tags from "tagTable" are printed.          */
/* "tagTableItemCount" specifies the length of "tagTable". verbose output     */
/* can be toggled.                                                            */
/* -------------------------------------------------------------------------- */

long int printExifInfo(FILE *stream, struct exifItem *exifTable,
                       int exifTableItemCount, char **tagTable,
                       int tagTableItemCount, int verbose) {
    long int i = 0;

    char *parsedTagID = NULL;
    char *parsedTagData = NULL;

    if (stream == NULL) return 0;

    for (i = 0; i < exifTableItemCount; i++) {
        parsedTagID = parseTagID(&exifTable[i]);
        parsedTagData = parseTagData(&exifTable[i]);

        if (!isInTagTable(parsedTagID, tagTable, tagTableItemCount)) continue;

        if (parsedTagID == NULL) {
            fprintf(stream, "[unknown]");
            fprintf(stream, "%*c", 36 - 9, ' ');
        } else {
            fprintf(stream, "%s", parseTagID(&exifTable[i]));
            fprintf(stream, "%*c", 36 - strlen(parsedTagID), ' ');
        }

        if (parsedTagData == NULL) {
            fprintf(stream, "= [unknown]\n");
        } else {
            fprintf(stream, "= %s\n", parsedTagData);
        }

        if (verbose) {
            fprintf(stream, "\ttagNo         = %ld\n", i + 1);
            fprintf(stream, "\texifFormat    = %ld\n", exifTable[i].exifFormat);
            fprintf(stream, "\tifdID         = %ld\n", exifTable[i].ifdID);
            fprintf(stream, "\ttagPos        = %ld\n", exifTable[i].tagPos);
            fprintf(stream, "\ttagID         = 0x%04x\n", exifTable[i].tagID);
            fprintf(stream, "\ttagType       = %ld\n", exifTable[i].tagType);
            fprintf(stream, "\ttagTypeSize   = %ld\n", exifTable[i].tagType);
            fprintf(stream, "\ttagCount      = %ld\n", exifTable[i].tagCount);
            fprintf(stream, "\ttagDataPos    = %ld\n", exifTable[i].tagDataPos);
        }
    }

    return 0;
}

/* -------------------------------------------------------------------------- */
/* printExifCsv                                                               */
/* prints info from "exifTable" of length "exifTableItemCount" to "stream" in */
/* a cav format. if "tagTable" is not null, only tags from "tagTable" are
 * printed.          */
/* "tagTableItemCount" specifies the length of "tagTable". verbose output     */
/* can be toggled.                                                            */
/* -------------------------------------------------------------------------- */

long int printExifCsv(FILE *stream, struct exifItem *exifTable,
                      int exifTableItemCount, char **tagTable,
                      int tagTableItemCount, int verbose) {
    long int i = 0;

    char *tagData = NULL;
    static struct exifItem *exifTag = NULL;

    for (i = 0; i < tagTableItemCount; i++) {
        if ((exifTag = findTagByName(exifTable, exifTableItemCount,
                                     tagTable[i])) == NULL) {
            fprintf(stream, "n/a,");
            continue;
        }

        if ((tagData = parseTagData(exifTag)) != NULL)
            fprintf(stream, "%s,", tagData);
        else
            fprintf(stream, "n/a,");
    }

    fprintf(stream, "\n");

    return 0;
}

/* -------------------------------------------------------------------------- */
/* isInTagTable                                                               */
/* returns true if "parsedTagId" is a member of "tagTable" of length          */
/* "tagTableItemCount" or false otherwise. if no tagTable is specified, true  */
/* is returned, since then it is implied that all tags are relevant.          */
/* -------------------------------------------------------------------------- */

static long int isInTagTable(char *parsedTagId, char **tagTable,
                             int tagTableItemCount) {
    long int rc = 0;
    long int i = 0;

    if (tagTable == NULL) return 1;
    if (parsedTagId == NULL) return 0;

    for (i = 0; i < tagTableItemCount; i++) {
        if (strcmp(tagTable[i], parsedTagId) == 0) {
            rc = 1;
            break;
        }
    }

    return rc;
}

/* -------------------------------------------------------------------------- */
/* fileNameFromPattern                                                        */
/* prints info from "exifTable" of length "exifTableItemCount" to "stream".   */
/* if "tagTable" is not null, only tags from "tagTable" are printed.          */
/* "tagTableItemCount" specifies the length of "tagTable". verbose output     */
/* can be toggled.                                                            */
/* -------------------------------------------------------------------------- */

long int fileNameFromPattern(char **fileName, char *pattern, char *oldFileName,
                             struct exifItem *exifTable,
                             int exifTableItemCount) {
    long int i = 0;
    long int rc = 0;

    char *subPatternEnd = "";
    char *fileNameNew = NULL;
    char *subFileName = NULL;
    char *subPattern = NULL;

    if (pattern == NULL) return EXIF_ERR_PATTERN;
    if (strlen(pattern) == 0) return EXIF_ERR_PATTERN;

    /* initialize filename */

    if ((rc = sprintf_wr(fileName, "")) != 0) return rc;

    /* analyze pattern char by char */

    for (i = 0; i < strlen(pattern); i++) {
        /* character is start of sub pattern */

        if (strncmp(pattern + i, "[", 1) == 0) {
            if ((subPatternEnd = strchr(pattern + i, ']')) == NULL)
                return EXIF_ERR_PATTERN;

            snprintf_wr(&subPattern, subPatternEnd - pattern - i, "%s",
                        pattern + i + 1);

            if ((rc = parseSubPattern(&subFileName, subPattern, oldFileName,
                                      exifTable, exifTableItemCount)))
                return rc;

            if ((rc = sprintf_wr(&fileNameNew, "%s%s", *fileName,
                                 subFileName)) < 0)
                return rc;

            i = i + subPatternEnd - pattern - i;
        }

        /* character is regular character */

        else {
            if ((rc = sprintf_wr(&fileNameNew, "%s%c", *fileName, pattern[i])) <
                0)
                return rc;
        }

        /* update filename */

        *fileName = fileNameNew;
    }

    return 0;
}

/* -------------------------------------------------------------------------- */
/* parseSubPattern                                                            */
/* analyzes the sub pattern between the square brackets of a pattern and      */
/* parses the tag data. returns 0 if successful or a negative value           */
/* otherwise.                                                                 */
/* -------------------------------------------------------------------------- */

static long int parseSubPattern(char **subFileName, char *subPattern,
                                char *oldFileName, struct exifItem *exifTable,
                                int exifTableItemCount) {
    long int rc = 0;

    char *colonPos = NULL;
    char *semicolPos = NULL;
    char *parsedTagId = NULL;

    char *from = NULL;
    char *to = NULL;
    long int fromPos = 0;
    long int toPos = 0;

    char *tagData = NULL;
    static struct exifItem *exifTag = NULL;

    /* extract tag id */

    if ((semicolPos = strchr(subPattern, ';')) == NULL) {
        if ((rc = sprintf_wr(&parsedTagId, "%s", subPattern)) < 0) return rc;
    } else {
        if ((rc = snprintf_wr(&parsedTagId, semicolPos - subPattern + 1, "%s",
                              subPattern)) < 0)
            return rc;
    }

    /* extract from and to positions */

    if (semicolPos != NULL) {
        if ((colonPos = strchr(semicolPos, ':')) == NULL)
            return EXIF_ERR_PATTERN;

        if ((rc = snprintf_wr(&from, colonPos - semicolPos, "%s",
                              semicolPos + 1)) < 0)
            return rc;

        fromPos = atol(from);

        if ((rc = snprintf_wr(&to, subPattern + strlen(subPattern) - colonPos,
                              "%s", colonPos + 1)) < 0)
            return rc;

        toPos = atol(to);
    }

    /* get tag data (starting with special cases) */

    if (strcmp(parsedTagId, "OldFileName") == 0)
        tagData = oldFileName;
    else {
        if ((exifTag = findTagByName(exifTable, exifTableItemCount,
                                     parsedTagId)) == NULL) {
            return EXIF_ERR_PATTERN_NOMATCH;
        }

        if ((tagData = parseTagData(exifTag)) == NULL)
            return EXIF_ERR_PATTERN_NOMATCH;
    }

    /* modify from and to */

    if (fromPos < 1) fromPos = 1;
    if (toPos < 1) toPos = strlen(tagData);

    if (fromPos > strlen(tagData) || toPos > strlen(tagData))
        return EXIF_ERR_PATTERN;

    if (fromPos > toPos) return EXIF_ERR_PATTERN_NOMATCH;

    /* create subfilename */

    if ((rc = snprintf_wr(subFileName, toPos - fromPos + 2, "%s",
                          tagData + fromPos - 1)) < 0)
        return rc;

    return 0;
}

/* -------------------------------------------------------------------------- */
