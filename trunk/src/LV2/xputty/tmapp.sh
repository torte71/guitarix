#!/bin/bash

CMD="g++ -g -Iheader -Iresources -Iheader/widgets -Ixdgmime -L../../../build/src/LV2/xputty \
  -o tmapp.exe tmapp.c \
  -static \
  -lxcairo \
  -lfftw3f \
  $(pkg-config --static --cflags --libs cairo) \
  -liconv \
"

echo $CMD
$CMD
