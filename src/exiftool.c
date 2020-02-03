/* -------------------------------------------------------------------------- */

#include "exiftool.h"

//TODO: prettify exifparser.c and exifextras.c
//TODO: add rename task
//TODO: add modify time/date task
//TODO: write unit tests

/* -------------------------------------------------------------------------- */
/* main                                                                       */
/* -------------------------------------------------------------------------- */

int main(int argc,
         char *argv[])
{

    int rc = 0;

    /* process */

    if ((rc = processArgs(argc, argv)) < 0)
        return rc;

    return 0;
}

/* -------------------------------------------------------------------------- */
/* processArgs                                                                */
/* main processor of the exif tool. returns 0 if successful or a negative     */
/* value otherwise.                                                           */
/* -------------------------------------------------------------------------- */

static long int processArgs(int argc,
                            char *argv[])
{

    int rc = 0;
    int i = 0;
    int task = 0;
    long int fileCount = 0;
    long int tagCount = 0;
    struct options opt = {0, 0};
    char **fileTable = NULL;
    char **tagTable = NULL;

    /* check if there are any arguments */

    if (argc <= 1)
    {
        fprintf(stderr, "exiftool: missing arguments\n");
        fprintf(stderr, "Try 'exiftool help' for more information.\n");
        return ERR_NO_ARG;
    }

    /* get task */

    if ((task = getTask(argv[1])) < 0)
    {
        fprintf(stderr, "exiftool: invalid task\n");
        fprintf(stderr, "Try 'exiftool help' for more information.\n");
        return task;
    }

    /* get options */

    if ((rc = getOptions(argc, argv, &opt)) < 0)
    {
        fprintf(stderr, "exiftool: invalid option\n");
        fprintf(stderr, "Try 'exiftool help' for more information.\n");
        return rc;
    }

    /* get tag list */

    if ((tagCount = getTagList(argc, argv, &tagTable)) < 0)
    {
        fprintf(stderr, "exiftool: invalid tag\n");
        fprintf(stderr, "Try 'exiftool help' for more information.\n");
        return rc;
    }

    /* get file list */

    if ((fileCount = getFileList(argc, argv,
                                 &fileTable, opt.recursive)) < 0)
    {
        fprintf(stderr, "exiftool: error processing file list\n");
        fprintf(stderr, "Try 'exiftool help' for more information.\n");
        return fileCount;
    }

    /* execute tasks */

    if (task == TASK_HELP)
    {

        taskHelp(stdout);
    }

    else if (task == TASK_PRINT)
    {

        if ((rc = taskPrint(stdout, &opt, fileTable, fileCount,
                            tagTable, tagCount)) < 0)
            return rc;
    }

    else if (task == TASK_CSV)
    {

        if ((rc = taskCsv(stdout, &opt, fileTable, fileCount,
                          tagTable, tagCount)) < 0)
            return rc;
    }

    else if (task == TASK_GPS)
    {

        if ((rc = taskGps(stdout, &opt, fileTable, fileCount)) < 0)
            return rc;
    }

    return 0;
}

/* -------------------------------------------------------------------------- */
/* getTask                                                                    */
/* returns the task determined by a command line argument "arg". returns      */
/* a negative value in case of an error.                                      */
/* -------------------------------------------------------------------------- */

static long int getTask(char *arg)
{

    int task = 0;

    if (strcmp("help", arg) == 0 || strcmp("--help", arg) == 0 || strcmp("-h", arg) == 0)
        task = TASK_HELP;
    else if (strcmp("print", arg) == 0)
        task = TASK_PRINT;
    else if (strcmp("gps", arg) == 0)
        task = TASK_GPS;
    else if (strcmp("csv", arg) == 0)
        task = TASK_CSV;
    else
        return ERR_ARG_INVALID;

    return task;
}

/* -------------------------------------------------------------------------- */
/* getOptions                                                                 */
/* checks "argc" command line arguments "argv" for options and writes them    */
/* to the "opt" struct. returns a negative value in case of an error.         */
/* -------------------------------------------------------------------------- */

static long int getOptions(int argc,
                           char *argv[],
                           struct options *opt)
{

    int i = 0;

    for (i = 2; i < argc; i++)
    {

        if (strncmp("-", argv[i], 1) != 0)
            continue;

        if (strcmp("-r", argv[i]) == 0)
            (*opt).recursive = 1;
        else if (strcmp("-v", argv[i]) == 0)
            (*opt).verbose = 1;
        else if (strncmp("-d=", argv[i], 3) == 0)
        {
            (*opt).debug = atoi(argv[i] + 3);
            debug = (*opt).debug;
        }
        else
            return ERR_OPT_INVALID;
    }

    return 0;
}

/* -------------------------------------------------------------------------- */
/* allocateTagTable                                                           */
/* allocates memory for a new file item in an existing or new "fileTable"     */
/* if "fileTable" is an existing table "fileTableItemCount" should be greater */
/* than zero. the function allocates one more table item. returns 0 if        */
/* successful, otherwise a value smaller than 0 is returned.                  */
/* -------------------------------------------------------------------------- */

static long int allocateTagTable(char ***tagTable,
                                 int tagTableItemCount)
{

    if (tagTableItemCount == 0)
    {

        if ((*tagTable =
                 (char **)malloc(sizeof(char *))) == NULL)
            return ERR_MALLOC;
    }
    else
    {

        if ((*tagTable =
                 (char **)realloc(*tagTable,
                                  sizeof(char *) * (tagTableItemCount + 1))) == NULL)
            return ERR_MALLOC;
    }

    return 0;
}

/* -------------------------------------------------------------------------- */
/* getTagList                                                                 */
/* checks "argc" command line arguments "argv" for tags and writes them       */
/* to the tag table. returns a negative value in case of an error.            */
/* -------------------------------------------------------------------------- */

static long int getTagList(int argc,
                           char *argv[],
                           char ***tagTable)
{

    long int i = 0;
    long int rc = 0;
    long int count = 0;
    long int tagNameLength = 0;

    for (i = 2; i < argc; i++)
    {

        if (strncmp("+", argv[i], 1) != 0)
            continue;

        if ((rc = allocateTagTable(tagTable, count)) < 0)
            return rc;

        tagNameLength = strlen(argv[i]) - 1;

        if (((*tagTable)[count] = (char *)malloc(tagNameLength + 1)) == NULL)
            return ERR_MALLOC;

        if (strcpy((*tagTable)[count],
                   argv[i] + 1) == NULL)
            return ERR_MEMCPY;

        count++;
    }

    return count;
}

/* -------------------------------------------------------------------------- */
/* allocateFileTable                                                          */
/* allocates memory for a new file item in an existing or new "fileTable"     */
/* if "fileTable" is an existing table "fileTableItemCount" should be greater */
/* than zero. the function allocates one more table item. returns 0 if        */
/* successful, otherwise a value smaller than 0 is returned.                  */
/* -------------------------------------------------------------------------- */

static long int allocateFileTable(char ***fileTable,
                                  int fileTableItemCount)
{

    if (fileTableItemCount == 0)
    {

        if ((*fileTable =
                 (char **)malloc(sizeof(char *))) == NULL)
            return ERR_MALLOC;
    }
    else
    {

        if ((*fileTable =
                 (char **)realloc(*fileTable,
                                  sizeof(char *) * (fileTableItemCount + 1))) == NULL)
            return ERR_MALLOC;
    }

    return 0;
}

/* -------------------------------------------------------------------------- */
/* addFileToFileTable                                                         */
/* adds a file or directory of name "fileName" to the file table "fileTable"  */
/* if "fileTable" is an existing table "fileTableItemCount" should be greater */
/* than zero. if "recursive" is true, this function is a recursive function   */
/* and adds directories. returns the number of items added or 0 otherwise.    */
/* -------------------------------------------------------------------------- */

static long int addFileToFileTable(char *fileName,
                                   char ***fileTable,
                                   long int fileTableItemCount,
                                   int recursive)
{

    long int rc = 0;
    long int count = 0;

    struct dirent *dirEntry;
    struct stat fileStat;

    char *relFileName = NULL;

    DIR *dir;

    /* stat file */

    if (stat(fileName, &fileStat) < 0)
        return ERR_FILESTAT;

    /* regular file */

    if (S_ISREG(fileStat.st_mode))
    {

        if ((rc = allocateFileTable(fileTable,
                                    fileTableItemCount)) < 0)
            return rc;

        if (((*fileTable)[fileTableItemCount] = (char *)malloc(strlen(fileName) + 1)) == NULL)
            return ERR_MALLOC;

        if (strcpy((*fileTable)[fileTableItemCount],
                   fileName) == NULL)
            return ERR_MEMCPY;

        count = 1;
    }
    else if (S_ISDIR(fileStat.st_mode) && recursive == 1)
    {

        if ((dir = opendir(fileName)) == NULL)
            return ERR_DIROPEN;

        while ((dirEntry = readdir(dir)) != NULL)
        {

            if (strncmp(dirEntry->d_name, ".", 1) == 0)
                continue;

            if ((relFileName = (char *)malloc(strlen(fileName) + strlen(dirEntry->d_name) + 2)) == NULL)
                return ERR_MALLOC;

            sprintf(relFileName, "%s/%s", fileName, dirEntry->d_name);

            if ((rc = addFileToFileTable(relFileName,
                                         fileTable,
                                         fileTableItemCount + count,
                                         recursive)) < 0)
                return rc;

            free(relFileName);

            count = count + rc;
        }

        closedir(dir);
    }

    return count;
}

/* -------------------------------------------------------------------------- */
/* getFileList                                                                */
/* checks "argc" command line arguments "argv" for files and writes them      */
/* to the "fileTable". set recursive to true to search directories.           */
/* returns a negative value in case of an error.                              */
/* -------------------------------------------------------------------------- */

static long int getFileList(int argc,
                            char *argv[],
                            char ***fileTable,
                            int recursive)
{

    long int i = 0;
    long int rc = 0;
    long int count = 0;
    long int fileNameLength = 0;

    for (i = 2; i < argc; i++)
    {

        if (strncmp("-", argv[i], 1) == 0 || strncmp("+", argv[i], 1) == 0)
            continue;

        if ((rc = addFileToFileTable(argv[i],
                                     fileTable,
                                     count,
                                     recursive)) < 0)
            return rc;

        count = count + rc;
    }

    return count;
}

/* -------------------------------------------------------------------------- */
/* taskHelp                                                                   */
/* prints help message.                                                       */
/* -------------------------------------------------------------------------- */

static void taskHelp(FILE *stream)
{

    fprintf(stream, "Usage: exiftool <operation> [<tags>] [<options>] ");
    fprintf(stream, "<files>\n\n");

    fprintf(stream, "Operations\n");
    fprintf(stream, "  help              Print this help screen\n");
    fprintf(stream, "  print             Print exif information\n");
    fprintf(stream, "  csv               Print specified tag(s) as csv\n");
    fprintf(stream, "  gps               Print gps coordinates\n\n");

    fprintf(stream, "Options\n");
    fprintf(stream, "  -r                Search directories recursively\n");
    fprintf(stream, "                    Default is off\n");
    fprintf(stream, "  -v                Turn on verbose mode\n");
    fprintf(stream, "                    Default is off\n");
    fprintf(stream, "  -d=x              Turn on debug mode to level x\n");
    fprintf(stream, "                    Default is zero\n\n");

    fprintf(stream, "Tags\n");
    fprintf(stream, "  +[tag]            Specifies which tags to print\n\n");

    fprintf(stream, "Examples\n");
    fprintf(stream, "  $ exiftool print test.jpg\n");
    fprintf(stream, "  Prints all exif information in test.jpg\n\n");
    fprintf(stream, "  $ exiftool print +Model +Make test.jpg\n");
    fprintf(stream, "  Prints the tags 'Model' and 'Make' only\n\n");
    fprintf(stream, "  $ exiftool gps +Model +Make *.jpg > test.csv\n");
    fprintf(stream, "  Prints the tags 'Model' and 'Make' to a csv\n\n");
    fprintf(stream, "  $ exiftool gps test.jpg\n");
    fprintf(stream, "  Prints the gps information in test.jpg\n");
}

/* -------------------------------------------------------------------------- */
/* taskPrint                                                                  */
/* prints the exif table. returns 0 if successful or a negative value         */
/* otherwise.                                                                 */
/* -------------------------------------------------------------------------- */

static long int taskPrint(FILE *stream,
                          struct options *opt,
                          char **fileTable,
                          long int fileTableItemCount,
                          char **tagTable,
                          long int tagTableItemCount)
{

    long int i = 0;
    long int rc = 0;
    long int exifTableItemCount = 0;
    struct exifItem *exifTable = NULL;

    for (i = 0; i < fileTableItemCount; i++)
    {

        if ((exifTableItemCount = extractExifInfo(fileTable[i],
                                                  &exifTable)) < 0)
        {
            fprintf(stderr, "exiftool: exiflib error %ld\n",
                    exifTableItemCount);
            return exifTableItemCount;
        }

        if ((rc = printExifInfo(stream,
                                exifTable,
                                exifTableItemCount,
                                tagTable,
                                tagTableItemCount,
                                (*opt).verbose)) < 0)
        {
            fprintf(stderr, "exiftool: exifparser error %ld\n",
                    rc);
            return rc;
        }

        free(exifTable);
    }

    return 0;
}

/* -------------------------------------------------------------------------- */
/* taskCsv                                                                    */
/* prints exif information in a csv format. returns 0 if successful or a      */
/* negative value otherwise.                                                  */
/* -------------------------------------------------------------------------- */

static long int taskCsv(FILE *stream,
                        struct options *opt,
                        char **fileTable,
                        long int fileTableItemCount,
                        char **tagTable,
                        long int tagTableItemCount)
{

    long int i = 0;
    long int j = 0;
    long int rc = 0;
    long int exifTableItemCount = 0;
    struct exifItem *exifTable = NULL;

    static struct exifItem *exifTag = NULL;

    /* print header */

    fprintf(stream, "Filename,");
    for (i = 0; i < tagTableItemCount; i++)
        fprintf(stream, "%s,", tagTable[i]);
    fprintf(stream, "\n");

    /* loop file table */

    for (i = 0; i < fileTableItemCount; i++)
    {

        fprintf(stream, "%s,", fileTable[i]);

        if ((exifTableItemCount = extractExifInfo(fileTable[i],
                                                  &exifTable)) < 0)
        {
            fprintf(stream, "\n");
            continue;
        }

        if ((rc = printExifCsv(stream,
                               exifTable,
                               exifTableItemCount,
                               tagTable,
                               tagTableItemCount,
                               (*opt).verbose)) < 0)
        {
            fprintf(stderr, "exiftool: exifparser error %ld\n",
                    rc);
            return rc;
        }

        free(exifTable);
    }

    return 0;
}

/* -------------------------------------------------------------------------- */
/* taskGps                                                                    */
/* prints gps information. returns 0 if successful or a negative value        */
/* otherwise.                                                                 */
/* -------------------------------------------------------------------------- */

static long int taskGps(FILE *stream,
                        struct options *opt,
                        char **fileTable,
                        long int fileTableItemCount)
{

    long int i = 0;
    long int rc = 0;
    long int exifTableItemCount = 0;
    struct exifItem *exifTable = NULL;
    char *gps = NULL;

    for (i = 0; i < fileTableItemCount; i++)
    {

        if ((exifTableItemCount = extractExifInfo(fileTable[i],
                                                  &exifTable)) < 0)
        {
            fprintf(stream, "no gps\n");
            continue;
        }

        if ((gps = parseSpecialGPS(exifTable,
                                   exifTableItemCount)) != NULL)
            fprintf(stream, "%s\n", gps);
        else
            fprintf(stream, "no gps\n");

        free(exifTable);
    }

    return 0;
}

/* -------------------------------------------------------------------------- */