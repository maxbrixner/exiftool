# exiftool

a simple and lightweight exif tool.

## Usage

exiftool <operation> [<tags>] [<options>] <files>

Operations
  help              Print this help screen
  print             Print exif information
  csv               Print specified tag(s) as csv
  gps               Print gps coordinates

Options
  -r                Search directories recursively
                    Default is off
  -v                Turn on verbose mode
                    Default is off
  -d=x              Turn on debug mode to level x
                    Default is zero

Tags
  +[tag]            Specifies which tags to print

Examples
  $ exiftool print test.jpg
  Prints all exif information in test.jpg

  $ exiftool print +Model +Make test.jpg
  Prints the tags 'Model' and 'Make' only

  $ exiftool gps +Model +Make *.jpg > test.csv
  Prints the tags 'Model' and 'Make' to a csv

  $ exiftool gps test.jpg
  Prints the gps information in test.jpg
