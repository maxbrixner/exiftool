# exiftool

A simple and lightweight exif tool.

## Usage
Use the exiftool from your command line like this.
```
$ exiftool <operation> [<tags>] [<options>] <files>
```
### Operations
| Operation | Description                            |
| --------- | -------------------------------------- |
| `help`    | Print a help screen                    |
| `print`   | Print exif information                 |
| `csv`     | Print specified tag(s) in a csv format |
| `gps`     | Print gps coordinates                  |
| `rename`  | Rename files based on a given pattern  |

### Options
| Option    | Description                            |
| --------- | -------------------------------------- |
| `-r`      | Search directories recursively         |
| `-v`      | Turn on verbose moe                    |
| `-d=x`    | Turn debug level to x                  |
| `-p=x`    | Use rename pattern x                   |
| `-s`      | Only simulate renaming files           |

### Rename patterns
`-p=[x;2:4]` Uses the letters 2 to 4 of the data contained in exif tag x.

## Examples

Print all the exif information contained in the file `test.jpg`.
```
$ exiftool print test.jpg 
```
Print only the exif information contained in the `Model` and `Make` tags of the file `test.jpg`.
```
$ exiftool print +Model +Make test.jpg
```
Create a csv file `test.csv` containing all the `Model` and `Make` tags of the jpg files in the current directory.
```
$ exiftool csv +Model +Make *.jpg > test.csv
```
Print the gps information of the file `test.jpg` in a nice, readable form.
```
$ exiftool gps test.jpg
```
Rename the file `test.jpg` to `cam_NIKON.jpg` or similiar, depending on the `Make` tag in the file.
```
$ exiftool rename -p="cam_[Make].jpg" test.jpg
```  
Rename the file `test.jpg` to `year_2020.jpg` or similiar, depending on the `Make` tag in the file.
```
$ exiftool rename -p="year_[DateTimeOriginal;1:4].jpg" test.jpg
```
Rename all jpg files in the current directory according the information given in the DateTimeOriginal tag, i.e. `year/month/day/hhmmss.jpg`.
```
$ exiftool rename -s -v -p="[DateTimeOriginal;1:4]/[DateTimeOriginal;6:7]/[DateTimeOriginal;9:10]/[DateTimeOriginal;12:13][DateTimeOriginal;15:16][DateTimeOriginal;18:19].jpg" *.jpg
```

## TODO

+ Add exif modify lib and tasks
+ Exif parser: add remaining parsers
+ Exif lib: the data of the maker note itself also contains an ifd
