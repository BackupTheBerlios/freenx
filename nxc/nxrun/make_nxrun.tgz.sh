#!/bin/sh


Usage()
{
  echo "Usage: $0 <release version> <lib version>"
  echo "  Example: $0 1.2.2 16"
  exit 1;
}

if [ $# -ne 2 ]; then
	Usage
fi

if test -e Makefile; then
  make distclean
  rm -f Makefile
fi

# Remove cvs merged files
rm -f .#*
rm -rf CVS

# Remove tmp/bak files
rm -f *~

# Create version .h
echo "Creating version $1-$2 ..."

FILE_VERSION_H="NXRunVersion.h"

if test -e $FILE_VERSION_H; then
  rm -f $FILE_VERSION_H
fi

echo "#ifndef NX_RUN_VERSION_H" >>$FILE_VERSION_H
echo "#define NX_RUN_VERSION_H" >>$FILE_VERSION_H
echo "" >>$FILE_VERSION_H
echo "const char *NX_RUN_VERSION = \"$1-$2\";" >>$FILE_VERSION_H
echo "" >>$FILE_VERSION_H
echo "#endif" >>$FILE_VERSION_H
echo "" >>$FILE_VERSION_H

# Making tar
cd ..

echo "Executing: tar -zcvf nxrun-$1-$2.tar.gz nxrun/"
tar -zcvf nxrun-$1-$2.tar.gz nxrun/ >/dev/null

if [ $? != 0 ]; then
  echo "Error Occurred."
  exit 1
else
  echo "Done."
  exit 0
fi

