#!/bin/bash

set -eu

javac --release 8 WorldCupLineupVis.java
zip tester.jar *.class
rm *.class
