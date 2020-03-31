#
# Makefile
#

.PHONY: netlist preprocess

all: preprocess

preprocess: netlist 
	./preprocess1.sh 'o'
	./preprocess1.sh 'cm'

netlist:
	mkdir -p logfiles
	mkdir -p netlist
	mkdir -p reports
	rm -f netlist/*
	dc_shell -f scripts/o_compile.tcl
	dc_shell -f scripts/cm_compile.tcl


clean:
	rm -rf logfiles netlist reports
	rm -rf WORK_autoread command.log default.svf


# vim:ft=make
#
