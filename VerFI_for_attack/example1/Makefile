#
# Makefile
#
ifndef O_SIM 
O_SIM=o_/sim1.txt
endif 

ifndef CM_SIM 
CM_SIM=cm_/sim1.txt
endif 

simo: 
ifdef RSEED
	../source/verfi_a -lf Library.txt -ln NANG45 -df PRESENT_cipher/o_design.v -mn Cipher -cf PRESENT_cipher/reports/o_cells.rpt -sf $(O_SIM) -uf o_/Summary1.txt -rf o_/Result1.txt -sd $(RSEED)
else
	../source/verfi_a -lf Library.txt -ln NANG45 -df PRESENT_cipher/o_design.v -mn Cipher -cf PRESENT_cipher/reports/o_cells.rpt -sf $(O_SIM) -uf o_/Summary1.txt -rf o_/Result1.txt 
endif 

simcm: 
ifdef RSEED
	../source/verfi_a -lf Library.txt -ln NANG45 -df PRESENT_cipher/cm_design.v -mn Cipher -cf PRESENT_cipher/reports/cm_cells.rpt -sf $(CM_SIM) -uf cm_/Summary1.txt -rf cm_/Result1.txt -sd $(RSEED)
else
	../source/verfi_a -lf Library.txt -ln NANG45 -df PRESENT_cipher/cm_design.v -mn Cipher -cf PRESENT_cipher/reports/cm_cells.rpt -sf $(CM_SIM) -uf cm_/Summary1.txt -rf cm_/Result1.txt
endif 


gen:
	make -C PRESENT_cipher/ 



# vim:ft=make
#
