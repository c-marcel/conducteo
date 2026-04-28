#!/bin/bash
set -e

NAME=conducteo
VERSION=5.0.1

export DEBEMAIL="clement.marcel@nwanda.fr"
export DEBFULLNAME="C. Marcel"

cd ..

echo "Generate source tarball"
git -C ${NAME}-${VERSION} archive \
  --format=tar.gz \
  --prefix=${NAME}-${VERSION}/ \
  v${VERSION} \
  -- . ':(exclude)debian' ':(exclude)install/windows' \
  > ${NAME}_${VERSION}.orig.tar.gz

echo "Build Debian"
cd ${NAME}-${VERSION}

if [ "$1" = "source" ]; then
    debuild -S -us -uc
else
    debuild -us -uc
fi

echo "Done."
