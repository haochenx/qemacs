#!/bin/bash -e

function prompt() {
  read -p ">> Continue? [yN] " -n 1 -r c
  seq 2 | xargs -I.n1 echo
  if [[ ! $c =~ ^[Yy]$ ]]; then exit 2; fi
}

make xq.gz

which wrangler || (echo 'You have to have wrangler installed. We can install it for you using `npm install -g wrangler`' && prompt && npm install -g wrangler)
wrangler r2 object put kxc-sh-objs/xq.bc52-darwin-aarch64.exe.gz --file=xq.gz
