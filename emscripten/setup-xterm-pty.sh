#!/bin/bash -xe

curl -sSL https://codeload.github.com/mame/xterm-pty/zip/89c920315117e8c87910576ba8769b22b3ea38a7 \
  | tar -x

mv xterm-pty-89c920315117e8c87910576ba8769b22b3ea38a7 xterm-pty
(cd xterm-pty && yarn && npx tsc && npx webpack)
