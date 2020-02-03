/* -------------------------------------------------------------------------- */

#include "exifextras.h"

/* -------------------------------------------------------------------------- */
// TODO
/* -------------------------------------------------------------------------- */

long int printExifInfo ( FILE *stream,
                         struct exifItem *exifTable,
                         int exifTableItemCount,
                         char **tagTable,
                         int tagTableItemCount,
                         int verbose ) {
                   
    long int i = 0;
    long int j = 0;

    char *tagID = NULL;
    char *tagData = NULL;
                         
    for ( i = 0; i < exifTableItemCount; i++ ) {
        
        tagID = parseTagID ( &exifTable[i] );
        tagData = parseTagData ( &exifTable[i] );

        if ( stream == NULL )
            continue;

        if ( tagTable != NULL && tagID != NULL ) {
        
            for ( j = 0; j < tagTableItemCount; j++ ) {
                if ( strcmp ( tagTable[j], tagID ) == 0 )
                    break;
            }
            
            if ( j == tagTableItemCount )
                continue;
       
        } else if ( tagTable != NULL && tagID == NULL ) {
            continue;
        }
        
        if ( ( tagID = parseTagID ( &exifTable[i] ) ) == NULL ) {
            fprintf ( stream, "[unknown]" );
            fprintf ( stream, "%*c",
                      36 - 9, ' ');
        } else {
            fprintf ( stream, "%s",
                  parseTagID ( &exifTable[i] ) );
            fprintf ( stream, "%*c",
                      36 - strlen ( parseTagID ( &exifTable[i] ) ), ' ');
        }

        if ( ( tagData = parseTagData ( &exifTable[i] ) ) == NULL ) {
            fprintf ( stream, "= [unknown]\n" );
        } else {
            fprintf ( stream, "= %s\n",
                      parseTagData ( &exifTable[i] ) ) ;
        }
                     
         if ( verbose ) {    
             fprintf( stream, "\ttagNo         = %ld\n",
                      i+1 );     
             fprintf( stream, "\texifFormat    = %ld\n",
                      exifTable[i].exifFormat ); 
             fprintf( stream, "\tifdID         = %ld\n",
                      exifTable[i].ifdID );                       
             fprintf( stream, "\ttagPos        = %ld\n",
                      exifTable[i].tagPos );
             fprintf( stream, "\ttagID         = 0x%04x\n",
                      exifTable[i].tagID );    
             fprintf( stream, "\ttagType       = %s\n",
                      parseTagType ( &exifTable[i] ) );
             fprintf( stream, "\ttagTypeSize   = %ld\n",
                      exifTable[i].tagType );
             fprintf( stream, "\ttagCount      = %ld\n",
                      exifTable[i].tagCount );
             fprintf( stream, "\ttagDataPos    = %ld\n",
                      exifTable[i].tagDataPos );
        }    

    } 
                         
}

/* -------------------------------------------------------------------------- */
// TODO
/* -------------------------------------------------------------------------- */

long int printExifCsv  ( FILE *stream,
                         struct exifItem *exifTable,
                         int exifTableItemCount,
                         char **tagTable,
                         int tagTableItemCount,
                         int verbose ) {
                   
    long int i = 0;

    char *tagData = NULL;
    static struct exifItem *exifTag = NULL;

    for ( i = 0; i < tagTableItemCount; i++ ) {
            
        if ( ( exifTag = findTagByName ( exifTable,
                                         exifTableItemCount,
                                         tagTable[i] ) ) == NULL ) {

            fprintf ( stream, "n/a," );

        } else {

            if ( ( tagData = parseTagData ( exifTag ) ) != NULL)
                fprintf ( stream, "%s,", tagData );
            else
                fprintf ( stream, "n/a" );

        }

    }

    fprintf ( stream, "\n" );

    return 0;

}

/* -------------------------------------------------------------------------- */
