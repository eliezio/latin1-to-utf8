latin1-to-utf8
==============

A utility to recode files from ISO-8859-1 (latin1) to UTF8 that supports mixed encodings and/or formats.

It's able to handle these two special cases:

1. The recoding can be applyed to delimited portions of the input. Useful for files that contains
both text and binary data, such as RCS files;
2. Preserves characters already converted to UTF-8. Handy when you have texts with mixed encoding.
Editing a file on different plataforms can led it to this state.

I wrote this utility because both [iconv](http://www.gnu.org/software/libiconv/) and [recode](http://recode.progiciels-bpi.ca/),
the two most widely used recoders, only supports plain-text files with an uniform encoding.

