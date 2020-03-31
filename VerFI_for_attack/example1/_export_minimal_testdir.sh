#! /bin/sh
#


rm -rf self_contained_test/ 
mkdir -p self_contained_test/PRESENT_cipher/reports
cp PRESENT_cipher/reports/*_cell*rpt self_contained_test/PRESENT_cipher/reports
cp PRESENT_cipher/*_design.v self_contained_test/PRESENT_cipher/
cat Makefile | sed 's/\.\/source//' > self_contained_test/Makefile
cp -a -t self_contained_test/ experiments.sh Library.txt cm_/ o_/ analysis_/ 
cp -L -t self_contained_test ../source/verfi_a 
