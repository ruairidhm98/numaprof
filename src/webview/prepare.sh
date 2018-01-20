#!/bin/bash
######################################################
#            PROJECT  : numaprof                     #
#            VERSION  : 0.0.0-dev                    #
#            DATE     : 07/2017                      #
#            AUTHOR   : Valat Sébastien  - CERN      #
#            LICENSE  : CeCILL-C                     #
######################################################

######################################################
export PATH=./node_modules/.bin/:$PATH
SOURCE_DIR=$PWD

######################################################
if [ ! -z "$1" ]
then
	cp bower.json "$1"
	cd "$1"
fi

######################################################
if ! which pip
then
	echo "Python 'pip' command required to fetch the webview server dependencies, please install before proceeding or download a release archive instead of using master branch"
	exit 1
fi

######################################################
if ! which npm
then
	echo "Nodejs 'npm' command required to fetch the webview client dependencies, please install 'nodejs' before proceeding or download a release archive instead of using master branch"
	exit 1
fi

######################################################
set -e
set -x
pip install -t deps --no-compile flask flask_httpauth Flask-Cache htpasswd flask-htpasswd
npm install bower
bower install
set +x
if [ ! -z "$1" ]; then
	echo > deps-loaded
fi