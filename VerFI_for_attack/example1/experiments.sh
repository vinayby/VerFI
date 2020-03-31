#! /bin/sh
#
# experiments.sh
# Copyright (C) 2020 Vinay B. Y. Kumar <vinayby@iitb.ac.in>
#
# Distributed under terms of the - license.
#
if [ $# -lt 4 ]
then 
    echo Usage ./$0 N_THREADS N_SIMS \(cm\|o\) \(sa0\|sa1\|toggle\) [N_FAULTS_default_is_2]
    exit
fi

N_FAULTS=2
if [ "x$5" != "x" ]
then
  N_FAULTS=$5
fi
echo $N_FAULTS 

THREADS=$1
NUM_SIMULATIONS=$2
FAULT_TYPE=stuck_at_0
if [ "x$4" == "xsa0" ] ; then 
FAULT_TYPE=stuck_at_0
fi
if [ "x$4" == "xsa1" ] ; then 
FAULT_TYPE=stuck_at_1
fi
if [ "x$4" == "xtoggle" ] ; then 
FAULT_TYPE=toggle
fi
#fault_injection_type
mkdir -p /tmp/{out,cm_,o_}


SIG_TAP_AT_CYCLE=30


###############################
## O_ ## 
###############################
function do_o {
  RSEED=11
  cat o_/sim1.txt | sed "/max_no_of_threads/{n;s/.*/${THREADS}/}" | sed "/number_of_random_fault_injections/{n;s/.*/${NUM_SIMULATIONS}/}"  | sed "/fault_injection_type/{n;s/.*/${FAULT_TYPE}/}" | sed "/.*no_faults_per_*/{n;s/.*/${N_FAULTS}/}" |  sed "/select_clock_cycle_to_tap/{n;s/.*/${SIG_TAP_AT_CYCLE}/}" > /tmp/sim.txt
  make simo RSEED=${RSEED} O_SIM=/tmp/sim.txt 

  cd analysis_/

  rm -rf /tmp/o_/*
  OUTDIR=/tmp/out/o_threads_${THREADS}_seed_${RSEED}_nsimulations_${NUM_SIMULATIONS}_fault_${FAULT_TYPE}_nfaults_${N_FAULTS}
  rm -rf ${OUTDIR}

  ./ct_distrib.py o_
  cd -
  cp -a /tmp/o_/ ${OUTDIR}
  cp -a o_ ${OUTDIR}
  cp /tmp/sim.txt ${OUTDIR}/o_/sim1.txt
}
###############################
## CM_ ## 
###############################
function do_cm {
  RSEED=11
  cat cm_/sim1.txt | sed "/max_no_of_threads/{n;s/.*/${THREADS}/}" | sed "/number_of_random_fault_injections/{n;s/.*/${NUM_SIMULATIONS}/}" | sed "/fault_injection_type/{n;s/.*/${FAULT_TYPE}/}" | sed "/.*no_faults_per_*/{n;s/.*/${N_FAULTS}/}"  |  sed "/select_clock_cycle_to_tap/{n;s/.*/${SIG_TAP_AT_CYCLE}/}" > /tmp/sim.txt
  make simcm RSEED=${RSEED} CM_SIM=/tmp/sim.txt 

  cd analysis_/

  rm -rf /tmp/cm_/*
  OUTDIR=/tmp/out/cm_threads_${THREADS}_seed_${RSEED}_nsimulations_${NUM_SIMULATIONS}_fault_${FAULT_TYPE}_nfaults_${N_FAULTS}

  rm -rf ${OUTDIR}

  ./ct_distrib.py cm_
  cd -
  cp -a /tmp/cm_/ ${OUTDIR}
  cp -a cm_ ${OUTDIR}
  cp /tmp/sim.txt ${OUTDIR}/cm_/sim1.txt
}


if [ "x$3" == "x" ]
then
do_o
do_cm
fi

if [ "x$3" == "xo" ]
then 
  do_o
else 
  do_cm
fi 
