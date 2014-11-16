latin1-to-utf8
==============

A utility to recode files in ISO-8859-1 (latin1) to UTF-8 but without messing up with characters already encoded in UTF-8.
The conversion can be restricted to delimited portions of the input file.

I wrote this utility because both [iconv](http://www.gnu.org/software/libiconv/) and [recode](http://recode.progiciels-bpi.ca/), the two most widely used recoders, are unable to:

1. Properly handle mixed encoding files;
2. Properly handle mixed binary and text files.

Theses cases are very common when converting CVS repositories to GIT.
