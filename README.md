latin1-to-utf8
==============

A utility to recoding files encoded in ISO-8859-1 (latin1) to UTF8 but without messing up with characters encoded in UTF8. The conversion can be restricted to well delimited portions of the input file.

I wrote this utility because both iconv and recode, the two most widely used recoders, are unable to:

1. Properly handle mixed encoding files;
2. Properly handle mixed binary and text files.

Theses cases are very common when converting CVS repositories to GIT.
