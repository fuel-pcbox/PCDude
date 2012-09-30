#!/bin/sh

astyle --style=allman --indent=tab --recursive ./src/\*.\*

rm -Rv src/*.orig*

