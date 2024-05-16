FROM debian:bullseye

RUN sed -i.bak -e "s%http://deb.debian.org/debian %http://ftp.jp.debian.org/debian %g" /etc/apt/sources.list
RUN apt-get update
RUN apt-get install -y curl wget perl lmodern gnupg2 python3 python3-pip python3-distutils libfontconfig1
RUN pip3 install toml jsonschema

# texlive
RUN wget http://ftp.jaist.ac.jp/pub/CTAN/systems/texlive/tlnet/install-tl-unx.tar.gz && \
    tar xzf install-tl-unx.tar.gz && \
    cd install-tl-* && \
    printf "%s\n" \
        "selected_scheme scheme-basic" \
        "tlpdbopt_install_docfiles 0" \
        "tlpdbopt_install_srcfiles 0" \
        > texlive.profile && \
    ./install-tl --profile=texlive.profile \
        --repository http://ftp.jaist.ac.jp/pub/CTAN/systems/texlive/tlnet/ \
        --logfile /tmp/install-tl.log && \
    cd .. && rm -rf install-tl-* install-tl-unx.tar.gz

# not able to expose the command result to docker env
# https://github.com/moby/moby/issues/29110
RUN export BINPATH=$(cat /tmp/install-tl.log | grep 'Most importantly, add' | cut -f 4 -d ' ') && \
    echo "Found texlive path: $BINPATH" && \
    ln -s $BINPATH/* /usr/local/bin
#ENV PATH "$PATH:/usr/local/texlive/bin"

# packages
RUN curl -fsSL https://www.preining.info/rsa.asc | tlmgr key add -
RUN tlmgr init-usertree
RUN tlmgr update --self --all
RUN tlmgr repository add http://contrib.texlive.info/current tlcontrib
RUN tlmgr pinning add tlcontrib '*'
RUN tlmgr install collection-basic collection-fontsrecommended collection-fontutils \
    collection-langcjk collection-langjapanese collection-langchinese \
    collection-latexrecommended collection-luatex libertine titlesec lipsum titling \
    needspace amsmath makecell tasks multirow \
    babel-russian

# link command installed with packages
RUN export BINPATH=$(cat /tmp/install-tl.log | grep 'Most importantly, add' | cut -f 4 -d ' ') && \
    echo "Found texlive path: $BINPATH" && \
    ln -sf $BINPATH/* /usr/local/bin

# fonts
RUN mkdir -p /usr/local/texlive/texmf-local/fonts/opentype/adobe/sourcehanserif/
RUN mkdir -p /usr/local/texlive/texmf-local/fonts/opentype/adobe/sourcehansans/
RUN mkdir -p /usr/local/texlive/texmf-local/fonts/opentype/adobe/sourcehan-otc/
COPY font/serif/* /usr/local/texlive/texmf-local/fonts/opentype/adobe/sourcehanserif/
COPY font/sans/* /usr/local/texlive/texmf-local/fonts/opentype/adobe/sourcehansans/
COPY font/otc/* /usr/local/texlive/texmf-local/fonts/opentype/adobe/sourcehan-otc/
RUN mktexlsr
RUN kanji-config-updmap-sys status

# pandoc
# 2.17 contains https://github.com/jgm/pandoc/issues/7713, may breaks latex table
# arch list: https://github.com/jgm/pandoc/blob/bff49852a5dd552d4319f05bc2c3c07a2c64d104/linux/make_artifacts.sh#L5
RUN VERSION="2.16.2" && \
    MACHINE=$(uname -m) && \
    case "$MACHINE" in \
      x86_64)  ARCH=amd64;; \
      i686)    ARCH=i386;; \
      i386)    ARCH=i386;; \
      aarch64) ARCH=arm64;; \
      armv6l | armv7l) ARCH=armhf;; \
      *)       ARCH=unknown;; \
    esac && \
    wget "https://github.com/jgm/pandoc/releases/download/$VERSION/pandoc-$VERSION-1-$ARCH.deb" && \
    dpkg -i "pandoc-$VERSION-1-$ARCH.deb" && \
    rm "pandoc-$VERSION-1-$ARCH.deb"

# more packages
RUN tlmgr install background xwatermark zhnumber catoptions everypage
RUN tlmgr install picture ltxkeys framed transparent

# dummy compile to speed up future builds
RUN mkdir -p /tmp/compile
COPY dummy.tex /tmp/compile
RUN cd /tmp/compile && \
    lualatex -halt-on-error dummy.tex && \
    lualatex -halt-on-error dummy.tex && \
    cd / && \
    rm -rf /tmp/compile

VOLUME /workdir
WORKDIR /workdir
