#! /bin/sh
for module in libhpcalcs; do
echo Updating "$module"
cd "$module"; rm m4/*.m4; autoreconf -f -i || exit 1; cd -
done
