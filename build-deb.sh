#!/bin/bash
set -e

NAME=conducteo
VERSION=5.0.0

export DEBEMAIL="clement.marcel@nwanda.fr"
export DEBFULLNAME="C. Marcel"

cd ..

echo "Generate source tarball"
git -C ${NAME}-${VERSION} archive \
  --format=tar.gz \
  --prefix=${NAME}-${VERSION}/ \
  HEAD \
  -- . ':(exclude)debian' ':(exclude)install/windows' \
  > ${NAME}_${VERSION}.orig.tar.gz

echo "Build Debian"
cd ${NAME}-${VERSION}
debuild -us -uc

echo "Done."
