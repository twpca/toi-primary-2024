#/usr/bin/bash

readonly SANS_VER="2.004R"
readonly SERIF_VER="2.001R"
readonly SANS_TAR="sans-${SANS_VER}.tar.gz"
readonly SERIF_TAR="serif-${SERIF_VER}.tar.gz"

curl -L "https://github.com/adobe-fonts/source-han-sans/archive/${SANS_VER}.tar.gz" -o "${SANS_TAR}"
curl -L "https://github.com/adobe-fonts/source-han-serif/archive/${SERIF_VER}.tar.gz" -o "${SERIF_TAR}"

mkdir -p font/otc
mkdir -p font/sans
mkdir -p font/serif

tar zxvf "${SANS_TAR}" -C font/otc --wildcards "source-han-sans-${SANS_VER}/OTC/*.ttc" --transform='s/.*\///'
tar zxvf "${SANS_TAR}" -C font/sans --wildcards "source-han-sans-${SANS_VER}/OTF/Japanese/*.otf" --transform='s/.*\///'
tar zxvf "${SANS_TAR}" -C font/sans --wildcards "source-han-sans-${SANS_VER}/OTF/TraditionalChinese/*.otf" --transform='s/.*\///'

tar zxvf "${SERIF_TAR}" -C font/otc --wildcards "source-han-serif-${SERIF_VER}/OTC/*.ttc" --transform='s/.*\///'
tar zxvf "${SERIF_TAR}" -C font/serif --wildcards "source-han-serif-${SERIF_VER}/OTF/Japanese/*.otf" --transform='s/.*\///'
tar zxvf "${SERIF_TAR}" -C font/serif --wildcards "source-han-serif-${SERIF_VER}/OTF/TraditionalChinese/*.otf" --transform='s/.*\///'

rm "${SANS_TAR}" "${SERIF_TAR}"
