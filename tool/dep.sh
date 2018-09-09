#!/usr/bin/env sh

cd $(dirname $0)/..    #切换至根目录

mkdir -p lib     #create lib if not exist
cd lib           #切换至lib

# unit test: Unity
git clone   https://github.com/ThrowTheSwitch/Unity.git

# macro lib: p99, http://p99.gforge.inria.fr
#git clone   https://scm.gforge.inria.fr/anonscm/git/p99/p99.git

# implement c11 threads
#git clone   https://github.com/tinycthread/tinycthread.git
