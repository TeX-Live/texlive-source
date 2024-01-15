#!/bin/sh
yum update -y
yum install -y centos-release-scl
yum install -y devtoolset-9 fontconfig-devel libX11-devel libXmu-devel libXaw-devel
