#!/bin/bash
export DEBIAN_FRONTEND=noninteractive
export LANG=C.UTF-8
export LC_ALL=C.UTF-8
apt-get update -q -y
apt-get install -y --no-install-recommends bash gcc g++ make perl libfontconfig-dev libx11-dev libxmu-dev libxaw7-dev build-essential
