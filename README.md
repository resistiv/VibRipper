# VibRipper
VibRipper is a C++-based CLI program for unpacking and repacking Vib-Ribbon's PAK file format.
The focus of this program was to create an accurate yet flexible PAK handler for future Vib-Ribbon modding.

## Usage
``VibRipper { u <pakfile> [outdir] | r <indir> [tocfile] }``

Passing `u` allows a user to **u**npack a PAK file. Optionally, a user can define an output directory of their choosing. If not, the program will create its own within the same directory as the PAK file with ``_out`` appended. In either case, the program will also create a ``_TOC.txt`` file within the same directory as the PAK file, which describes the original **t**able **o**f **c**ontents structure of the PAK file, which can later be used for accurate repacking.

Passing `r` allows a user to **r**epack a PAK file from a directory. Optionally, a user can define a TOC file (``_TOC.txt`` file) to repack the directory with, maintaining the original PAK structure and PAK file name. If not provided with a TOC file, the program will repack the directory based on your OS's filesystem rules into its parent directory with ``.PAK`` appended.

Passing `h` displays a basic **h**elp message for the user.

## Remarks
I created a similar program to this one some months ago at the request of a Twitter user who reached out to me. However, there was the particular problem that some PAK files don't follow a constant ruleset when sorting their file names in the table of contents. This caused inconsistent and incompatible results. So, I created the basic text TOC file to preserve the order of contents when repacking. While this solution is a bit clunky, I found it was most effective instead of trying to hardcode bizarre rules for certain directories.

That aside, I decided to leave the raw folder repacking as part of the program in the rare case that the community deems it is viable for some scenarios.

## License
This project is licensed under the [GNU GPLv3](LICENSE).