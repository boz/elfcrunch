#!/bin/sh

elfcrunch -f $1 -M > $1.dot &&
dot -Tpng $1.dot > $1.png

