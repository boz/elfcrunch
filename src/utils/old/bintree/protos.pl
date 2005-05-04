#!/usr/bin/perl -w
# print prototypes for functions in a c file.
# must be one-liners, sorry.

$func = "";
while( <> )
{
   m/^{/ && print $func,"\n";
   $func = "$_";
   $func =~ s/\).*/\);/g;
}

