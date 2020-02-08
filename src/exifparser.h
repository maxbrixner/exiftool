#ifndef EXIFPARSER_H_INCLUDED
#define EXIFPARSER_H_INCLUDED

#include <dirent.h>
#include <errno.h>
#include <math.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#include "exiflib.h"

/* -------------------------------------------------------------------------- */
/* definitions                                                                */
/* -------------------------------------------------------------------------- */

#define LOOKUP_TAG_ID 146

/* -------------------------------------------------------------------------- */
/* structs                                                                    */
/* -------------------------------------------------------------------------- */

struct idLookupItem {
    long int tagID;
    char *tagName;
};

/* -------------------------------------------------------------------------- */
/* lookup table                                                               */
/* -------------------------------------------------------------------------- */

static struct idLookupItem idLookupTable[LOOKUP_TAG_ID] = {

    {0x0100, "ImageWidth"},
    {0x0101, "ImageLength"},
    {0x0102, "BitsPerSample"},
    {0x0103, "Compression"},
    {0x0106, "PhotometricInterpretation"},
    {0x010e, "ImageDescription"},
    {0x010f, "Make"},
    {0x0110, "Model"},
    {0x0111, "StripOffsets"},
    {0x0112, "Orientation"},
    {0x0115, "SamplesPerPixel"},
    {0x0116, "RowsPerStrip"},
    {0x0117, "StripByteCounts"},
    {0x011a, "XResolution"},
    {0x011b, "YResolution"},
    {0x011c, "PlanarConfiguration"},
    {0x0128, "ResolutionUnit"},
    {0x012d, "TransferFunction"},
    {0x0131, "Software"},
    {0x0132, "DateTime"},
    {0x013b, "Artist"},
    {0x013e, "WhitePoint"},
    {0x013f, "PrimaryChromaticities"},
    {0x0201, "JPEGInterchangeFormat"},
    {0x0202, "JPEGInterchangeFormatLength"},
    {0x0211, "YCbCrCoefficients"},
    {0x0212, "YCbCrSubSampling"},
    {0x0213, "YCbCrPositioning"},
    {0x0214, "ReferenceBlackWhite"},
    {0x8298, "Copyright"},
    {0x8769, "ExifIFDPointer"},
    {0x8825, "GPSInfoIFDPointer"},
    {0x829a, "ExposureTime"},
    {0x829d, "FNumber"},
    {0x8822, "ExposureProgram"},
    {0x8824, "SpectralSensitivity"},
    {0x8827, "PhotographicSensitivity"},
    {0x8828, "OECF"},
    {0x8830, "SensitivityType"},
    {0x8831, "StandardOutputSensitivity"},
    {0x8832, "RecommendedExposureIndex"},
    {0x8833, "ISOSpeed"},
    {0x8834, "ISOSpeedLatitudeyyy"},
    {0x8835, "ISOSpeedLatitudezzz"},
    {0x9000, "ExifVersion"},
    {0x9003, "DateTimeOriginal"},
    {0x9004, "DateTimeDigitized"},
    {0x9010, "OffsetTime"},
    {0x9011, "OffsetTimeOriginal"},
    {0x9012, "OffsetTimeDigitized"},
    {0x9101, "ComponentsConfiguration"},
    {0x9102, "CompressedBitsPerPixel"},
    {0x9201, "ShutterSpeedValue"},
    {0x9202, "ApertureValue"},
    {0x9203, "BrightnessValue"},
    {0x9204, "ExposureBiasValue"},
    {0x9205, "MaxApertureValue"},
    {0x9206, "SubjectDistance"},
    {0x9207, "MeteringMode"},
    {0x9208, "LightSource"},
    {0x9209, "Flash"},
    {0x920a, "FocalLength"},
    {0x9214, "SubjectArea"},
    {0x927c, "MakerNote"},
    {0x9286, "UserComment"},
    {0x9290, "SubSecTime"},
    {0x9291, "SubSecTimeOriginal"},
    {0x9292, "SubSecTimeDigitized"},
    {0x9400, "Temperature"},
    {0x9401, "Humidity"},
    {0x9402, "Pressure"},
    {0x9403, "WaterDepth"},
    {0x9404, "Acceleration"},
    {0x9405, "CameraElevationAngle"},
    {0xa000, "FlashpixVersion"},
    {0xa001, "ColorSpace"},
    {0xa002, "PixelXDimension"},
    {0xa003, "PixelYDimension"},
    {0xa004, "RelatedSoundFile"},
    {0xa005, "Interoperability IFD Pointer"},
    {0xa20b, "FlashEnergy"},
    {0xa20c, "SpatialFrequencyResponse"},
    {0xa20e, "FocalPlaneXResolution"},
    {0xa20f, "FocalPlaneYResolution"},
    {0xa210, "FocalPlaneResolutionUnit"},
    {0xa214, "SubjectLocation"},
    {0xa215, "ExposureIndex"},
    {0xa217, "SensingMethod"},
    {0xa300, "FileSource"},
    {0xa301, "SceneType"},
    {0xa302, "CFAPattern"},
    {0xa401, "CustomRendered"},
    {0xa402, "ExposureMode"},
    {0xa403, "WhiteBalance"},
    {0xa404, "DigitalZoomRatio"},
    {0xa405, "FocalLengthIn35mmFilm"},
    {0xa406, "SceneCaptureType"},
    {0xa407, "GainControl"},
    {0xa408, "Contrast"},
    {0xa409, "Saturation"},
    {0xa40a, "Sharpness"},
    {0xa40b, "DeviceSettingDescription"},
    {0xa40c, "SubjectDistanceRange"},
    {0xa420, "ImageUniqueID"},
    {0xa430, "CameraOwnerName"},
    {0xa431, "BodySerialNumber"},
    {0xa432, "LensSpecification"},
    {0xa433, "LensMake"},
    {0xa434, "LensModel"},
    {0xa435, "LensSerialNumber"},
    {0xa460, "CompositeImage"},
    {0xa461, "SourceImageNumberOfCompositeImage"},
    {0xa462, "SourceExposureTimesOfCompositeImage"},
    {0xa500, "Gamma"},
    {0x0000, "GPSVersionID"},
    {0x0001, "GPSLatitudeRef"},
    {0x0002, "GPSLatitude"},
    {0x0003, "GPSLongitudeRef"},
    {0x0004, "GPSLongitude"},
    {0x0005, "GPSAltitudeRef"},
    {0x0006, "GPSAltitude"},
    {0x0007, "GPSTimeStamp"},
    {0x0008, "GPSSatellites"},
    {0x0009, "GPSStatus"},
    {0x000a, "GPSMeasureMode"},
    {0x000b, "GPSDOP"},
    {0x000c, "GPSSpeedRef"},
    {0x000d, "GPSSpeed"},
    {0x000e, "GPSTrackRef"},
    {0x000f, "GPSTrack"},
    {0x0010, "GPSImgDirectionRef"},
    {0x0011, "GPSImgDirection"},
    {0x0012, "GPSMapDatum"},
    {0x0013, "GPSDestLatitudeRef"},
    {0x0014, "GPSDestLatitude"},
    {0x0015, "GPSDestLongitudeRef"},
    {0x0016, "GPSDestLongitude"},
    {0x0017, "GPSDestBearingRef"},
    {0x0018, "GPSDestBearing"},
    {0x0019, "GPSDestDistanceRef"},
    {0x001a, "GPSDestDistance"},
    {0x001b, "GPSProcessingMethod"},
    {0x001c, "GPSAreaInformation"},
    {0x001d, "GPSDateStamp"},
    {0x001e, "GPSDifferential"},
    {0x001f, "GPSHPositioningError"}

};

/* -------------------------------------------------------------------------- */
/* public functions                                                           */
/* -------------------------------------------------------------------------- */

char *parseTagID(struct exifItem *tag);

char *parseTagType(struct exifItem *tag);

char *parseTagData(struct exifItem *tag);

char *parseSpecialGPS(struct exifItem *exifTable, long int exifTableItemCount);

struct exifItem *findTagByName(struct exifItem *exifTable,
                               int exifTableItemCount, char *tagName);

int sprintf_wr(char **buf, char *fmt, ...);

size_t snprintf_wr(char **buf, size_t n, char *fmt, ...);

/* -------------------------------------------------------------------------- */

#endif

/* -------------------------------------------------------------------------- */
