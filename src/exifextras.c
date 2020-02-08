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

    char *charSearch = "";
    char *fileNameNew = NULL;
    char *subPattern = NULL;

    if (pattern == NULL) return EXIF_ERR_PATTERN;
    if (strlen(pattern) <= 0) return EXIF_ERR_PATTERN;

    printf("pattern: \"%s\"\n", pattern);

    if ((rc = sprintf_wr(fileName, "")) != 0) return rc;

    for (i = 0; i < strlen(pattern); i++) {
        printf("[%ld]\"%c\"\n", i, pattern[i]);

        /* character is start of sub pattern */

        if (strncmp(pattern + i, "[", 1) == 0) {
            if ((charSearch = strchr(pattern + i, ']')) == NULL)
                return EXIF_ERR_PATTERN;

            snprintf_wr(&subPattern, charSearch - pattern - i, "%s",
                        pattern + i + 1);

            // TODO: add sub pattern analyzer

            if ((rc = sprintf_wr(&fileNameNew, "%s%s", *fileName, "TODO")) < 0)
                return rc;

            printf("\tsub pattern \"%s\"\n", subPattern);
            i = i + charSearch - pattern - i;
        }

        /* character is regular character */

        else {
            printf("\tregular character \"%c\"\n", pattern[i]);

            if ((rc = sprintf_wr(&fileNameNew, "%s%c", *fileName, pattern[i])) <
                0)
                return rc;
        }

        /* update filename */

        printf("\tfileNameNew: \"%s\"\n", fileNameNew);
        *fileName = fileNameNew;
    }

    printf("result: \"%s\"\n", *fileName);

    return 0;
}

/* -------------------------------------------------------------------------- */
