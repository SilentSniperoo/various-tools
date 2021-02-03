# various-tools

A mishmash of several small applets for automation I needed at random moments.

## CustomCommandLine.cpp

- For all those times when you don't want to open in read only mode by default.
- Reads a config file to start another program with preset arguments as if it had been triggered from the command line.
- Generates a stubbed config file and explains usage when no config file exists.

## File2Big.cpp

- Sometimes you can't fit a file into a flash drive or version control system.
- Splits files larger than 1GB into as many files as necessary in order to guarantee all files are less than 1GB.
- Combines files into a single file.

## UbuntuMinecraftServerSetup.sh

- Server setup can be such a chore and this way you don't have to look up how each time.
- Meant to be run on a Ubuntu VM in VirtualBox.

## find_duplicates.py

- Quickly identifies partial and exact duplicate files using hashing, sequence matching, and data feature comparisons.
- Caches files to reduce cache misses in iterating over the search set.

## convert2ogg.bat

- REQUIRES THAT VLC IS INSTALLED
- Many game engines accept both unnecessarily large ".wav" and much more lean ".ogg" audio file formats
- Converts all files of certain extensions to the ".ogg" format.

## spaces2underscores.bat

- Spaces are bad. On the command line, they can also be tricky. They break many a program as inputs.
- The renaming script, "spaces2underscores.bat" is used as a prerequisite for scripts like "convert2ogg.bat", which uses VLC, which requires that paths never include spaces.

## recursive_copy.bat

- A simple script to copy folders recursively on the command line.
- Used by other scripts like "spaces2underscores.bat" as a convenience (in that specific case to preserve the originals being renamed).
