FILENAME_IN=netlist/present_encryptor_top.v # post dc_shell, std_cell level netlist/file.v
lno_start=`grep --color=no "module present_encryptor_top" ${FILENAME_IN}  -n|cut -f1 -d:`
lno_end=`grep --color=no -E  "wire[ ]*N1|wire[ ]*data_o_valid_*"                  ${FILENAME_IN}  -n|cut -f1 -d:`

OUTF=`mktemp`
{ sed -n "1,$(expr $lno_start - 1)p" $FILENAME_IN ; sed -n '1,$p' patch_design.v ; sed -n "${lno_end},\$p" $FILENAME_IN ; } > $OUTF && mv $OUTF design.v

perl -pi -e "s/\\\([a-zA-Z])/\1/g" design.v # remove \ before signal names
perl -pi -e "s/(.*_reg)(\[)([0-9]*)\]/\1_\3_/g" design.v
perl -pi -e "s/\] \)/\]\)/g" design.v 
perl -pi -e "s/(.*) (.*)\/(.*) \(/\1 \2_\3 \(/g" design.v # repeat multiple times if more depth is expected (A/B/C/U1)
perl -pi -e "s/(.*) (.*)\/(.*) \(/\1 \2_\3 \(/g" design.v # repeat multiple times if more depth is expected (A/B/C/U1)
# handle signals now with / 
perl -pi -e "s/([a-zA-Z0-9])\//\1_/g" design.v
perl -pi -e "s/([a-zA-Z0-9])\//\1_/g" design.v
perl -pi -e "s/([a-zA-Z0-9])\//\1_/g" design.v
# remove the two spaces synopsys adds between .PortA(U_sig_name  [3])
perl -pi -e "s/\(([a-zA-Z0-9_]*)  \[/\(\1\[/g" design.v 
# remove the space after signal name for those that had a \ prefixed earlier: e.g., .PortA(U_sig_name )
perl -pi -e "s/\(([a-zA-Z0-9_]*) \)/\(\1\)/g" design.v 
#../source/verfi_tool0 -lf Library.txt -ln NANG45 -df design.v -mn Cipher -cf cells.rpt -sf sim.txt -uf Summary.txt -rf Result.txt
