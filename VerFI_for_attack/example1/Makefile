#
# Makefile
#

sim: 
	../source/verfi_a -lf Library.txt -ln NANG45 -df design.v -mn Cipher -cf cells.rpt -sf sim.txt -uf Summary.txt -rf Result.txt


gen:
	make -C PRESENT_cipher/ 
	cp PRESENT_cipher/design.v .
	cp PRESENT_cipher/reports/cells.rpt .



# vim:ft=make
#
