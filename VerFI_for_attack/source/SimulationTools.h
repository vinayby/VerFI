//////////////////////////////////////////////////////////////////////////////////
// COMPANY:		Ruhr University Bochum, Embedded Security
// AUTHOR:		Amir Moradi (for the paper: https://eprint.iacr.org/2019/1312 )
//////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2019, Amir Moradi
// All rights reserved.
//
// BSD-3-Clause License
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//     * Redistributions of source code must retain the above copyright
//       notice, this list of conditions and the following disclaimer.
//     * Redistributions in binary form must reproduce the above copyright
//       notice, this list of conditions and the following disclaimer in the
//       documentation and/or other materials provided with the distribution.
//     * Neither the name of the copyright holder, their organization nor the
//       names of its contributors may be used to endorse or promote products
//       derived from this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
// ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTERS BE LIABLE FOR ANY
// DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
// (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
// LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
// ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

//***************************************************************************************
#ifndef SIMULATIONTOOLS_H
#define SIMULATIONTOOLS_H
#include "curses.h"
#include "Definitions.h"
int MakeCircuitDepth(SignalStruct** Signals, int NumberOfSignals, CellTypeStruct** CellTypes, CellStruct** Cells,
	int* Gates, int NumberOfGates, short &MaxDepth, int** &CellsInDepth, int* &NumberOfCellsInDepth);


int RunSimulation(SignalStruct** Signals, int ClockSignal, int Max_No_ClockCycles,
	int InitialSim_NumberOfClockCycles, int InitialSim_NumberOfInputs,
	int** InitialSim_Inputs, char** InitialSim_Values,
	CellStruct** Cells, int* Regs, int NumberOfRegs,
	short MaxDepth, int** CellsInDepth, int* NumberOfCellsInDepth, CellTypeStruct** CellTypes,
	int* EndSimCondition_Signals, char* EndSimCondition_Values,
	int EndSimCondition_NumberOfSignals, int EndSim_NumberOfWaitCycles,
	int* SignalValues, int* RegValues, char*** Faults);

int MakeSelectedOutputs(char** EndSim_OutputNames, int* EndSim_Outputs_IndexL, int* EndSim_Outputs_IndexH,
	int EndSim_NumberOfOutputBlocks, SignalStruct** Signals, int NumberOfSignals,
	int** &EndSim_OutputsInBlock, int* &EndSim_NumberOfOutputsInBlock);


int RunFaultInjection(int Max_no_of_Threads, SignalStruct** Signals, int NumberOfSignals,
	int ClockSignal, int NumberOfRegValues, int Max_No_ClockCycles,
	CellStruct** Cells, int NumberOfCells,
	char FaultInjectionType, int NumberOfSimulationsInFile, int NumberOfTargetClockCycles, int* TargetClockCycles,
	int MaxNumberOfFaultsPerRun, int MinNumberOfFaultsPerRun, int MaxNumberOfFaultsPerCycle, int MinNumberOfFaultsPerCycle,
	int NumberOfRandomInputs, int* RandomInputs,
	char* SummaryFileName,
	int InitialSim_NumberOfClockCycles, int InitialSim_NumberOfInputs,
	int** InitialSim_Inputs, char** InitialSim_Values,
	int* Regs, int NumberOfRegs,
	short MaxDepth, int** CellsInDepth, int* NumberOfCellsInDepth, CellTypeStruct** CellTypes,
	int* EndSimCondition_Signals, char* EndSimCondition_Values,
	int EndSimCondition_NumberOfSignals, int EndSim_NumberOfWaitCycles,
	char** EndSim_OutputNames, int* EndSim_Outputs_IndexL, int* EndSim_Outputs_IndexH,
	char* EndSim_Outputs_Base, int EndSim_NumberOfOutputBlocks,
	int** EndSim_OutputsInBlock, int* EndSim_NumberOfOutputsInBlock,
	SimulationResultStruct* &SimulationResults, int &NumberOfSimulations);


void CheckResults(int ClockCycleFaultFree, int ClockCycleFaulty, int Max_No_ClockCycles,
	char** EndSim_OutputNames, int* EndSim_Outputs_IndexL, int* EndSim_Outputs_IndexH,
	char* EndSim_Outputs_Base, int EndSim_NumberOfOutputBlocks,
	int** EndSim_OutputsInBlock, int* EndSim_NumberOfOutputsInBlock,
	SignalStruct** Signals, int NumberOfSignals, int** FaultFreeOutputValues,
	int* SignalValues, SimulationResultStruct &SimulationResult,
	int NumberOfRandomInputs, int* RandomInputs,
	int &IneffectiveCounter, int &NondetectedCounter, int &DetectedCounter, int &RunTimeOverCounter);


void PrintResultInFile(char* ResultFileName, int NumberOfSimulations,
	SimulationResultStruct* SimulationResults);


//***************************************************************************************
#endif

//***************************************************************************************
