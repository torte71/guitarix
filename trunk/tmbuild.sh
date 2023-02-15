#!/bin/bash

# pacman -S git base-devel mingw-w64-x86_64-gcc mingw-w64-x86_64-pkg-config mingw-w64-x86_64-meson mingw-w64-x86_64-libsigc++ mingw-w64-x86_64-lv2 mingw-w64-x86_64-eigen3 mingw-w64-x86_64-fftw mingw-w64-x86_64-libffi mingw-w64-x86_64-libsndfile

export PYTHONIOENCODING=utf-8
export PYTHONLEGACYWINDOWSSTDIO=utf-8
export WAF_NO_PREFORK=1


if [ ! -e .tmpatched ] ; then
  # unpack waf for patching
  ./waf --version
  patch -p1 < tm-04-ugly-Bdynamic-workaround.patch
  touch .tmpatched
fi

./waf configure -j 7 	\
  --check-cxx-compiler="g++" \
  --no-standalone	\
  --no-faust		\
  --includeconvolver	\
  --includeresampler	\
  --no-avahi		\
  --no-bluez		\
  --no-nsm		\
  --no-desktop-update	\
  --static-lib		\
  --ldflags="-shared -static \
	$(pkg-config --static --libs sndfile) \
	$(pkg-config --static --libs cairo) \
	-lfftw3f -liconv"	\
  --cxxflags="-g -Wall -DGSEAL_ENABLE -fpermissive -D_USE_MATH_DEFINES" \
  \
  && ./waf build \
  && ./waf install --destdir=$(pwd)/_bin \
  && find ./_bin/ -name *.ttl|xargs sed -i "s/X11UI/WindowsUI/"

