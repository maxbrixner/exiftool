/* -------------------------------------------------------------------------- */

#include "exifparser.h"

/* -------------------------------------------------------------------------- */
//TODO
/* -------------------------------------------------------------------------- */

char *parseTagID ( struct exifItem *tag ) {

    int i = 0;
    char *tagName = NULL;
        
    for ( i = 0; i < LOOKUP_TAG_ID; i++ ) {
    
        if ( idLookupTable[i].tagID == (*tag).tagID ) {       
            tagName = idLookupTable[i].tagName;
            break;
        }
    
    }
    
    return tagName;

}

/* -------------------------------------------------------------------------- */
//TODO
/* -------------------------------------------------------------------------- */

char *parseTagType ( struct exifItem *tag ) {

    char *tagTypeName = NULL;

    switch ( (*tag).tagType ) {

        case 1:
            if ( sprintf_wr ( &tagTypeName, "BYTE" ) < 0 )
                return NULL;
        break;
    
        case 2:
            if ( sprintf_wr ( &tagTypeName, "ASCII" ) < 0 )
                return NULL;
        break;
        
        case 3:
            if ( sprintf_wr ( &tagTypeName, "SHORT" ) < 0 )
                return NULL;
        break;

        case 4:
            if ( sprintf_wr ( &tagTypeName, "LONG" ) < 0 )
                return NULL;
        break;

        case 5:
            if ( sprintf_wr ( &tagTypeName, "RATIONAL" ) < 0 )
                return NULL;
        break;
        
        case 6:
            if ( sprintf_wr ( &tagTypeName, "SBYTE" ) < 0 )
                return NULL;
        break;

        case 7:
            if ( sprintf_wr ( &tagTypeName, "UNDEFINED" ) < 0 )
                return NULL;
        break;
        
        case 8:
            if ( sprintf_wr ( &tagTypeName, "SSHORT" ) < 0 )
                return NULL;
        break;

        case 9:
            if ( sprintf_wr ( &tagTypeName, "SLONG" ) < 0 )
                return NULL;
        break;

        case 10:
            if ( sprintf_wr ( &tagTypeName, "SRATIONAL" ) < 0 )
                return NULL;
        break;
        
        case 11:
            if ( sprintf_wr ( &tagTypeName, "SINGLEF" ) < 0 )
                return NULL;
        break;
        
        case 12:
            if ( sprintf_wr ( &tagTypeName, "DOUBLEF" ) < 0 )
                return NULL;
        break;
        
        default:
        
        break;
    
    } 
    
    return tagTypeName;

}

/* -------------------------------------------------------------------------- */
//TODO
/* -------------------------------------------------------------------------- */

char *parseTagData ( struct exifItem *tag ) {

    //TODO add remaining parsers

    char *tagData = NULL;
    long int i = 0;
    
    size_t n = 0;
    long int length = 0;
    
    double fnumber = 0.0;
    long int ldnumber = 0;

    switch ( (*tag).tagType ) {

        case 1: // byte
            for ( i = 0; i < (*tag).tagCount; i++ ) {
                ldnumber = castUInt8 ( (*tag).tagData + i,
                                       (*tag).exifFormat );
                if ( ( n = sprintf_wr ( &tagData,
                                        length == 0 ? "%s%ld" : "%s | %ld",
                                        length == 0 ? "" : tagData,
                                        ldnumber ) ) < 0 )
                    return NULL;
                length += n;
            }       
            break;        
        
        case 2: //ascii
            if ( snprintf_wr ( &tagData,
                               (*tag).tagCount * (*tag).tagTypeSize, 
                               "%s", 
                               (*tag).tagData ) < 0 )
                return NULL;
            break;
        
        case 3: // short
            ldnumber = castUInt16 ( (*tag).tagData,
                                    (*tag).exifFormat );
            if ( sprintf_wr ( &tagData,
                              "%ld",
                              ldnumber ) < 0 )
                return NULL;
            break;
        
        case 4: // long
            ldnumber = castUInt32 ( (*tag).tagData,
                                    (*tag).exifFormat );
            if ( sprintf_wr ( &tagData,
                              "%ld",
                              ldnumber ) < 0 )
                return NULL;
            break;
        
        case 5: // rational
            for ( i = 0; i < (*tag).tagCount; i++ ) {
                ldnumber = castUInt32 ( (*tag).tagData + ( i * 8 ),
                                        (*tag).exifFormat );
                fnumber = (double) ldnumber;
                ldnumber = castUInt32 ( (*tag).tagData + ( i * 8 ) + 4,
                                        (*tag).exifFormat );
                fnumber = fnumber / (double) ldnumber;
                if ( ( n = sprintf_wr ( &tagData,
                                        length == 0 ? "%s%.4f" : "%s | %.4f",
                                        length == 0 ? "" : tagData,
                                        fnumber ) ) < 0 )
                    return NULL;    
                length += n;
            }
            break;
            
        case 10: // srational
            for ( i = 0; i < (*tag).tagCount; i++ ) {
                ldnumber = castInt32 ( (*tag).tagData + ( i * 8 ),
                                       (*tag).exifFormat );
                fnumber = (double) ldnumber;
                ldnumber = castInt32 ( (*tag).tagData + ( i * 8 ) + 4,
                                       (*tag).exifFormat );
                fnumber = fnumber / (double) ldnumber;
                if ( ( n = sprintf_wr ( &tagData,
                                        length == 0 ? "%s%.4f" : "%s | %.4f",
                                        length == 0 ? "" : tagData,
                                        fnumber ) ) < 0 )
                    return NULL;    
                length += n;
            }
            break;     
        
        default:
        
        break;
    
    }
    
    return tagData;

}

/* -------------------------------------------------------------------------- */
//TODO
/* -------------------------------------------------------------------------- */

char *parseSpecialGPS ( struct exifItem *exifTable,
                        long int exifTableItemCount ) {

    struct exifItem *tag = NULL;
    char *gps = NULL;
    
    double latitude = 0;
    double longitude = 0;
    
    if ( ( tag = findTagByName ( exifTable, 
                                 exifTableItemCount,
                                 "GPSLatitude" ) ) == NULL )
        return NULL;
        
    latitude = ( (double) castUInt32 ( (*tag).tagData + ( 0 * 8 ), 
                                       (*tag).exifFormat )
                        / castUInt32 ( (*tag).tagData + ( 0 * 8 ) + 4,
                                       (*tag).exifFormat ) )
             + ( (double) castUInt32 ( (*tag).tagData + ( 1 * 8 ),
                                        (*tag).exifFormat )
                        / castUInt32 ( (*tag).tagData + ( 1 * 8 ) + 4,
                                       (*tag).exifFormat ) ) / 60
             + ( (double) castUInt32 ( (*tag).tagData + ( 2 * 8 ),
                                       (*tag).exifFormat )
                        / castUInt32 ( (*tag).tagData + ( 2 * 8 ) + 4,
                                       (*tag).exifFormat ) ) / 3600;       

    if ( ( tag = findTagByName ( exifTable, 
                                 exifTableItemCount,
                                 "GPSLongitude" ) ) == NULL )
        return NULL;
        
    longitude = ( (double) castUInt32 ( (*tag).tagData + ( 0 * 8 ), 
                                        (*tag).exifFormat )
                         / castUInt32 ( (*tag).tagData + ( 0 * 8 ) + 4,
                                        (*tag).exifFormat ) )
              + ( (double) castUInt32 ( (*tag).tagData + ( 1 * 8 ),
                                         (*tag).exifFormat )
                         / castUInt32 ( (*tag).tagData + ( 1 * 8 ) + 4,
                                        (*tag).exifFormat ) ) / 60
              + ( (double) castUInt32 ( (*tag).tagData + ( 2 * 8 ),
                                        (*tag).exifFormat )
                         / castUInt32 ( (*tag).tagData + ( 2 * 8 ) + 4,
                                        (*tag).exifFormat ) ) / 3600;   
    
    if ( ( tag = findTagByName ( exifTable, 
                                 exifTableItemCount,
                                 "GPSLongitudeRef" ) ) == NULL )
        return NULL;
        
    if ( strcmp ( "W", parseTagData ( tag ) ) == 0 )
        longitude = 0 - longitude;
        
    if ( ( tag = findTagByName ( exifTable, 
                                 exifTableItemCount,
                                 "GPSLatitudeRef" ) ) == NULL )
        return NULL;
        
    if ( strcmp ( "S", parseTagData ( tag ) ) == 0 )
        latitude = 0 - latitude;    

    if ( sprintf_wr ( &gps,
                      "%.4f,%.4f",
                      latitude,
                      longitude ) < 0 )
        return NULL;
        
    return gps;

}

/* -------------------------------------------------------------------------- */
//TODO
/* -------------------------------------------------------------------------- */

struct exifItem *findTagByName ( struct exifItem *exifTable,
                                 int exifTableItemCount,
                                 char *tagName ) {
                         
    long int i = 0;
    
    struct exifItem *result = NULL;
    char *tagID = NULL;
    
    for ( i = 0; i < exifTableItemCount; i++) {
    
        if ( ( tagID = parseTagID ( &exifTable[i] ) ) == NULL )
            continue;

        if ( strcmp ( tagName, tagID ) == 0) {
            result = &exifTable[i];
            break;
        }
    
    }
    
    return result;
                                               
}                        

/* -------------------------------------------------------------------------- */
//TODO
/* -------------------------------------------------------------------------- */

static int sprintf_wr ( char **buf, 
                        char *fmt,
                        ...) {

    int length = 0;

    va_list va, va2;
    
    va_start ( va, fmt );
    va_copy ( va2, va );
    
    length = vsnprintf ( NULL, 0, fmt, va );
    
    va_end ( va );
  
    if( ( (*buf) = ( char * ) malloc ( length + 1 ) ) == NULL ) {
        va_end ( va2 );
        return EXIF_ERR_MALLOC;   
    }
    
    vsnprintf ( *buf, length + 1, fmt, va2 );
    
    va_end ( va2 );
    
    return length;

}

/* -------------------------------------------------------------------------- */
//TODO
/* -------------------------------------------------------------------------- */

static size_t snprintf_wr ( char **buf,
                            size_t n,
                            char *fmt,
                            ...) {

    va_list va;
    
    va_start ( va, fmt );
  
    if( ( (*buf) = ( char * ) malloc ( n ) ) == NULL ) {
        va_end ( va );
        return EXIF_ERR_MALLOC;   
    }   
    
    vsnprintf ( *buf, n, fmt, va );
    
    va_end ( va );
    
    return n;

}

/* -------------------------------------------------------------------------- */

