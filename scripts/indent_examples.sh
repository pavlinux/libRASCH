#!/bin/sh
#
# based on the settings used for the Linux source
#
# options used:
#
# kr   K&R is the base
# bl   braces on own line
# bl0  do not indent braces 
# sob  swallow optional blank lines
# l65  line width is 65
# ss   on one-line for and while statments, force a blank before the semicolon.
# ncs  no space after cast operator
# nut  replace tab's with space 
indent -kr -bl -bli0 -sob -l65 -ss -ncs -nut "$@"
