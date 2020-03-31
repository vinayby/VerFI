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
#include "SimulationTools.h"
#include <cstdlib>
#include <cstring>
#include "time.h"
#include "omp.h"
int MakeCircuitDepth(SignalStruct** Signals, int NumberOfSignals, CellTypeStruct** CellTypes, CellStruct** Cells,
	int* Gates, int NumberOfGates, short &MaxDepth, int** &CellsInDepth, int* &NumberOfCellsInDepth)
{
	int  i;
	int  InputIndex;
	int  OutputIndex;
	int  SignalIndex;
	int   GateIndex;
	int   CellIndex;
	short DepthIndex;
	char  all_have_depth;

	DepthIndex = 0;

	do {
		all_have_depth = 1;

		for (SignalIndex = 0;SignalIndex < NumberOfSignals;SignalIndex++)
		{
			if (Signals[SignalIndex]->Depth == DepthIndex)
			{
				for (InputIndex = 0;InputIndex < Signals[SignalIndex]->NumberOfInputs;InputIndex++)
				{
					CellIndex = Signals[SignalIndex]->Inputs[InputIndex];

					if (CellTypes[Cells[CellIndex]->Type]->GateOrReg == CellType_Gate)
					{
						for (i = 0;i < Cells[CellIndex]->NumberOfInputs;i++)
							if (Signals[Cells[CellIndex]->Inputs[i]]->Depth == -1)
								break;

						if (i >= Cells[CellIndex]->NumberOfInputs) // all have depth
						{
							Cells[CellIndex]->Depth = DepthIndex + 1;

							for (OutputIndex = 0;OutputIndex < Cells[CellIndex]->NumberOfOutputs;OutputIndex++)
								if (Cells[CellIndex]->Outputs[OutputIndex] != -1)
									Signals[Cells[CellIndex]->Outputs[OutputIndex]]->Depth = DepthIndex + 1;
						}
					}
				}

				all_have_depth = 0;
			}
		}

		DepthIndex++;
	} while (!all_have_depth);

	MaxDepth = DepthIndex;
	CellsInDepth = (int **)malloc((MaxDepth + 1) * sizeof(int *));
	NumberOfCellsInDepth = (int *)calloc(MaxDepth + 1, sizeof(int));

	for (GateIndex = 0;GateIndex < NumberOfGates;GateIndex++)
		NumberOfCellsInDepth[Cells[Gates[GateIndex]]->Depth]++;

	for (DepthIndex = 1;DepthIndex <= MaxDepth;DepthIndex++)
	{
		CellsInDepth[DepthIndex] = (int *)malloc(NumberOfCellsInDepth[DepthIndex] * sizeof(int));
		NumberOfCellsInDepth[DepthIndex] = 0; // temporary to be used as index in the next loop
	}

	for (GateIndex = 0;GateIndex < NumberOfGates;GateIndex++)
	{
		DepthIndex = Cells[Gates[GateIndex]]->Depth;
		CellsInDepth[DepthIndex][NumberOfCellsInDepth[DepthIndex]] = Gates[GateIndex];
		NumberOfCellsInDepth[DepthIndex]++;
	}

	for (SignalIndex = 0;SignalIndex < NumberOfSignals;SignalIndex++)
		if ((Signals[SignalIndex]->Output != -1) & (Signals[SignalIndex]->Depth == -1))
			break;

	if (SignalIndex < NumberOfSignals)
	{
		printf("the depth of signal ""%s"" could not be identified\n", Signals[SignalIndex]->Name);
		return 1;
	}

	return 0;
}

//***************************************************************************************


int RunSimulation(SignalStruct** Signals, int ClockSignal, int Max_No_ClockCycles,
	int InitialSim_NumberOfClockCycles, int InitialSim_NumberOfInputs,
	int** InitialSim_Inputs, char** InitialSim_Values,
	CellStruct** Cells, int* Regs, int NumberOfRegs,
	short MaxDepth, int** CellsInDepth, int* NumberOfCellsInDepth, CellTypeStruct** CellTypes,
	int* EndSimCondition_Signals, char* EndSimCondition_Values,
	int EndSimCondition_NumberOfSignals, int EndSim_NumberOfWaitCycles,
	int* SignalValues, int* SignalValuesAt1SelectCycle, int SelectCycleToTap, int* RegValues, char*** Faults)
{
	int  i;
	int  InputIndex;
	int  OutputIndex;
	int  SignalIndex;
	int  RegIndex;
	int  DepthIndex;
	int  CellIndex;
	int  ClockCycle;
	int  v;
	int  Value;
	int  NumberOfWaitedClockCycles = -1;

	for (ClockCycle = 0;ClockCycle < Max_No_ClockCycles;ClockCycle++)
	{
		SignalValues[ClockSignal] = 1;

		// ----------- evaluate the registers

		for (RegIndex = 0;RegIndex < NumberOfRegs;RegIndex++)
		{
			v = 0;
			for (InputIndex = 0;InputIndex < Cells[Regs[RegIndex]]->NumberOfInputs;InputIndex++)
				v |= SignalValues[Cells[Regs[RegIndex]]->Inputs[InputIndex]] << InputIndex;

			for (OutputIndex = 0;OutputIndex < Cells[Regs[RegIndex]]->NumberOfOutputs;OutputIndex++)
				v |= RegValues[Cells[Regs[RegIndex]]->RegValueIndexes[OutputIndex]] << (Cells[Regs[RegIndex]]->NumberOfInputs + OutputIndex);

			for (OutputIndex = 0;OutputIndex < Cells[Regs[RegIndex]]->NumberOfOutputs;OutputIndex++)
			{
				Value = CellTypes[Cells[Regs[RegIndex]]->Type]->Tables[OutputIndex][v];
				Value ^= Faults[FaultInjection_toggle][ClockCycle][Regs[RegIndex]];
				Value |= Faults[FaultInjection_stuck_at_1][ClockCycle][Regs[RegIndex]];
				Value &= !Faults[FaultInjection_stuck_at_0][ClockCycle][Regs[RegIndex]];
				RegValues[Cells[Regs[RegIndex]]->RegValueIndexes[OutputIndex]] = Value;
			}
		}

		// ----------- applying the initial inputs

		if (ClockCycle < InitialSim_NumberOfClockCycles)
			for (InputIndex = 0;InputIndex < InitialSim_NumberOfInputs;InputIndex++)
				SignalValues[InitialSim_Inputs[ClockCycle][InputIndex]] = InitialSim_Values[ClockCycle][InputIndex];

		// ----------- applying the register outputs to the output signals

		for (RegIndex = 0;RegIndex < NumberOfRegs;RegIndex++)
			for (OutputIndex = 0;OutputIndex < Cells[Regs[RegIndex]]->NumberOfOutputs;OutputIndex++)
				if (Cells[Regs[RegIndex]]->Outputs[OutputIndex] != -1) {
					SignalValues[Cells[Regs[RegIndex]]->Outputs[OutputIndex]] = RegValues[Cells[Regs[RegIndex]]->RegValueIndexes[OutputIndex]];

          if (ClockCycle == SelectCycleToTap) {
					  SignalValuesAt1SelectCycle[Cells[Regs[RegIndex]]->Outputs[OutputIndex]] = SignalValues[Cells[Regs[RegIndex]]->Outputs[OutputIndex]];
          }
        }

		// ----------- evaluate the circuits :D

		for (DepthIndex = 1;DepthIndex <= MaxDepth;DepthIndex++)
		{
			for (i = 0;i < NumberOfCellsInDepth[DepthIndex];i++)
			{
				CellIndex = CellsInDepth[DepthIndex][i];

				v = 0;
				for (InputIndex = 0;InputIndex < Cells[CellIndex]->NumberOfInputs;InputIndex++)
					v |= SignalValues[Cells[CellIndex]->Inputs[InputIndex]] << InputIndex;

				for (OutputIndex = 0;OutputIndex < Cells[CellIndex]->NumberOfOutputs;OutputIndex++)
					if (Cells[CellIndex]->Outputs[OutputIndex] != -1)
					{
						Value = CellTypes[Cells[CellIndex]->Type]->Tables[OutputIndex][v];
						Value ^= Faults[FaultInjection_toggle][ClockCycle][CellIndex];
						Value |= Faults[FaultInjection_stuck_at_1][ClockCycle][CellIndex];
						Value &= !Faults[FaultInjection_stuck_at_0][ClockCycle][CellIndex];
						SignalValues[Cells[CellIndex]->Outputs[OutputIndex]] = Value;

            if (ClockCycle == SelectCycleToTap) {
              SignalValuesAt1SelectCycle[Cells[CellIndex]->Outputs[OutputIndex]] = Value;
            }
					}
			}
		}

		SignalValues[ClockSignal] = 0;

		// re-evaluate (we don't need it in this design since it works only at possitive edge of the clock and does not have a latch
		//
		//
		//

		// ----------- check the conditions to terminate the simulation

		if (ClockCycle > InitialSim_NumberOfClockCycles)
		{
			if (NumberOfWaitedClockCycles == -1)
			{
				for (SignalIndex = 0;SignalIndex < EndSimCondition_NumberOfSignals;SignalIndex++)
					if (SignalValues[EndSimCondition_Signals[SignalIndex]] != EndSimCondition_Values[SignalIndex]){
						break;
          }

				if (SignalIndex >= EndSimCondition_NumberOfSignals)
					NumberOfWaitedClockCycles = 0;
			}
			else
				NumberOfWaitedClockCycles++;

			if (NumberOfWaitedClockCycles >= EndSim_NumberOfWaitCycles)
				break;
		}
	}

	return (ClockCycle);
}

//***************************************************************************************

int MakeSelectedOutputs(char** EndSim_OutputNames, int* EndSim_Outputs_IndexL, int* EndSim_Outputs_IndexH,
	int EndSim_NumberOfOutputBlocks, SignalStruct** Signals, int NumberOfSignals,
	int** &EndSim_OutputsInBlock, int* &EndSim_NumberOfOutputsInBlock)
{
	char *Str1 = (char *)malloc(Max_Name_Length * sizeof(char));
	int  j;
	int  OutputIndex;
	int  SignalIndex;
	int  IndexH, IndexL;

	EndSim_OutputsInBlock = (int**)malloc(EndSim_NumberOfOutputBlocks * sizeof(int*));
	EndSim_NumberOfOutputsInBlock = (int*)malloc(EndSim_NumberOfOutputBlocks * sizeof(int));

	for (OutputIndex = 0;OutputIndex < EndSim_NumberOfOutputBlocks;OutputIndex++)
	{
		EndSim_NumberOfOutputsInBlock[OutputIndex] = (EndSim_Outputs_IndexH[OutputIndex] - EndSim_Outputs_IndexL[OutputIndex] + 1);
		EndSim_OutputsInBlock[OutputIndex] = (int *)malloc(EndSim_NumberOfOutputsInBlock[OutputIndex] * sizeof(int));

		IndexL = EndSim_Outputs_IndexL[OutputIndex];
		IndexH = EndSim_Outputs_IndexH[OutputIndex];

		for (j = IndexL;j <= IndexH;j++)
		{
			if (IndexL != -1)
				sprintf(Str1, "%s[%d]", EndSim_OutputNames[OutputIndex], j);
			else
				sprintf(Str1, "%s", EndSim_OutputNames[OutputIndex]);

			for (SignalIndex = 0;SignalIndex < NumberOfSignals;SignalIndex++)
				if (!strcmp(Signals[SignalIndex]->Name, Str1))
					break;

			if (SignalIndex >= NumberOfSignals)
			{
				printf("simulation: signal ""%s"" as output signal not found", Str1);
				free(Str1);
				return 1;
			}

			EndSim_OutputsInBlock[OutputIndex][j - IndexL] = SignalIndex;
		}
	}

	free(Str1);
	return 0;
}


// Note: Replicates MakeSelectedOutputs function as-is, and retains EndSim_ prefix for convenience
// i.e., read signals with SelectedSignalsToTap_ prefix
int MakeSelectedSignalsToTap(char** EndSim_OutputNames, int* EndSim_Outputs_IndexL, int* EndSim_Outputs_IndexH,
	int EndSim_NumberOfOutputBlocks, SignalStruct** Signals, int NumberOfSignals,
	int** &EndSim_OutputsInBlock, int* &EndSim_NumberOfOutputsInBlock)
{
	char *Str1 = (char *)malloc(Max_Name_Length * sizeof(char));
	int  j;
	int  OutputIndex;
	int  SignalIndex;
	int  IndexH, IndexL;

	EndSim_OutputsInBlock = (int**)malloc(EndSim_NumberOfOutputBlocks * sizeof(int*));
	EndSim_NumberOfOutputsInBlock = (int*)malloc(EndSim_NumberOfOutputBlocks * sizeof(int));

	for (OutputIndex = 0;OutputIndex < EndSim_NumberOfOutputBlocks;OutputIndex++)
	{
		EndSim_NumberOfOutputsInBlock[OutputIndex] = (EndSim_Outputs_IndexH[OutputIndex] - EndSim_Outputs_IndexL[OutputIndex] + 1);
		EndSim_OutputsInBlock[OutputIndex] = (int *)malloc(EndSim_NumberOfOutputsInBlock[OutputIndex] * sizeof(int));

		IndexL = EndSim_Outputs_IndexL[OutputIndex];
		IndexH = EndSim_Outputs_IndexH[OutputIndex];

		for (j = IndexL;j <= IndexH;j++)
		{
			if (IndexL != -1)
				sprintf(Str1, "%s[%d]", EndSim_OutputNames[OutputIndex], j);
			else
				sprintf(Str1, "%s", EndSim_OutputNames[OutputIndex]);

			for (SignalIndex = 0;SignalIndex < NumberOfSignals;SignalIndex++)
				if (!strcmp(Signals[SignalIndex]->Name, Str1))
					break;

			if (SignalIndex >= NumberOfSignals)
			{
				printf("simulation: signal ""%s"" specified for tapping, not found", Str1);
				free(Str1);
				return 1;
			}

			EndSim_OutputsInBlock[OutputIndex][j - IndexL] = SignalIndex;
		}
	}

	free(Str1);
	return 0;
}

//***************************************************************************************

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
	char** SignalsToTap_OutputNames, int* SignalsToTap_Outputs_IndexL, int* SignalsToTap_Outputs_IndexH,
	char* SignalsToTap_Outputs_Base, int SignalsToTap_NumberOfOutputBlocks,
	int** SignalsToTap_OutputsInBlock, int* SignalsToTap_NumberOfOutputsInBlock,
	SimulationResultStruct* &SimulationResults, int &NumberOfSimulations, int SelectCycleToTap, int RndSeed)
{
	int  CellIndex;
	int  *FaultAllowedCells;
	int  NumberOfFaultAllowedCells;
	int  ClockCycle;
	int  ClockCycleIndex;
	int  ClockCycleFaultFree;
	int  ClockCycleFaulty;
	int  **SignalValues = NULL;
	int  **SignalValuesAt1SelectCycle = NULL;
	int  **RegValues = NULL;
	int  **RandomInputValues = NULL;
	char ****Faults = NULL;
	int  ***FaultFreeOutputValues = NULL;
	int  ThreadIndex;
	int  SimulationIndex;
	int  SimulationCounter;
	int  RangeNumberOfFaultsPerCycle;
	int  RangeNumberOfFaultsPerRun;
	int  *DetectedCounter;
	int  *NondetectedCounter;
	int  *IneffectiveCounter;
	int  *RunTimeOverCounter;
	FILE *SummaryFile;
	int  TotalDetected;
	int  TotalNondetected;
	int  TotalIneffective;
	int  TotalRunTimeOver;
	int  LocalIndex;
	int	 InputIndex;
	int	 OutputIndex;
	int	 BlockIndex;
	int  i, j, k;
	int  NumberOfInjectedFaults;
	int  NumberOfFaultsInCycle;
	int  SelectedNumberOfInjectedFaults;
	char *Seeded;
	int	 MaxTargetClockCycle;
	int  MinTargetClockCycle;
	clock_t begin;

	NumberOfFaultAllowedCells = 0;
	for (CellIndex = 0;CellIndex < NumberOfCells;CellIndex++)
		if (Cells[CellIndex]->FaultAllowed)
			NumberOfFaultAllowedCells++;

	FaultAllowedCells = (int*)malloc(NumberOfFaultAllowedCells * sizeof(int));
	NumberOfFaultAllowedCells = 0;
	for (CellIndex = 0;CellIndex < NumberOfCells;CellIndex++)
		if (Cells[CellIndex]->FaultAllowed)
			FaultAllowedCells[NumberOfFaultAllowedCells++] = CellIndex;

	SignalValues = (int **)malloc(Max_no_of_Threads * sizeof(int *));
	SignalValuesAt1SelectCycle = (int **)malloc(Max_no_of_Threads * sizeof(int *));
	RegValues = (int **)malloc(Max_no_of_Threads * sizeof(int *));
	RandomInputValues = (int **)malloc(Max_no_of_Threads * sizeof(int *));
	Faults = (char ****)malloc(Max_no_of_Threads * sizeof(char ***));
	FaultFreeOutputValues = (int ***)malloc(Max_no_of_Threads * sizeof(int **));
	DetectedCounter = (int *)calloc(Max_no_of_Threads, sizeof(int));
	NondetectedCounter = (int *)calloc(Max_no_of_Threads, sizeof(int));
	IneffectiveCounter = (int *)calloc(Max_no_of_Threads, sizeof(int));
	RunTimeOverCounter = (int *)calloc(Max_no_of_Threads, sizeof(int));
	Seeded = (char *)calloc(Max_no_of_Threads, sizeof(char));

	for (ThreadIndex = 0;ThreadIndex < Max_no_of_Threads;ThreadIndex++)
	{
		SignalValues[ThreadIndex] = (int *)calloc(NumberOfSignals, sizeof(int));
		SignalValuesAt1SelectCycle[ThreadIndex] = (int *)calloc(NumberOfSignals, sizeof(int));

		RegValues[ThreadIndex] = (int *)calloc(NumberOfRegValues, sizeof(int));
		RandomInputValues[ThreadIndex] = (int *)malloc(NumberOfRandomInputs * sizeof(int));
		Faults[ThreadIndex] = (char ***)malloc(NumberOfFaultInjectionTypes * sizeof(char **));
		SignalValues[ThreadIndex][1] = 1;    // constant 1'b1
		SignalValuesAt1SelectCycle[ThreadIndex][1] = 1;    // constant 1'b1
		FaultFreeOutputValues[ThreadIndex] = (int**)malloc(EndSim_NumberOfOutputBlocks * sizeof(int*));

		for (i = 0;i < NumberOfFaultInjectionTypes;i++)
		{
			Faults[ThreadIndex][i] = (char **)malloc(Max_No_ClockCycles * sizeof(char *));

			for (ClockCycle = 0;ClockCycle < Max_No_ClockCycles;ClockCycle++)
				Faults[ThreadIndex][i][ClockCycle] = (char *)calloc(NumberOfCells, sizeof(char));
		}

		for (BlockIndex = 0;BlockIndex < EndSim_NumberOfOutputBlocks;BlockIndex++)
			FaultFreeOutputValues[ThreadIndex][BlockIndex] = (int*)malloc(EndSim_NumberOfOutputsInBlock[BlockIndex] * sizeof(int));
	}

	MaxTargetClockCycle = TargetClockCycles[0];
	MinTargetClockCycle = TargetClockCycles[0];

	for (ClockCycleIndex = 1;ClockCycleIndex < NumberOfTargetClockCycles;ClockCycleIndex++)
	{
		if (MaxTargetClockCycle < TargetClockCycles[ClockCycleIndex])
			MaxTargetClockCycle = TargetClockCycles[ClockCycleIndex];

		if (MinTargetClockCycle > TargetClockCycles[ClockCycleIndex])
			MinTargetClockCycle = TargetClockCycles[ClockCycleIndex];
	}

	NumberOfSimulations = NumberOfSimulationsInFile;
	if (NumberOfSimulations > 600000000L)
	{
		printf("Number of simulations %d is over the threshold", NumberOfSimulations);

		return 1;
	}

	printf("Number of simulations: %d\r\n", NumberOfSimulations);

	SimulationResults = (SimulationResultStruct *)malloc(NumberOfSimulations * sizeof(SimulationResultStruct));

	omp_set_num_threads(Max_no_of_Threads);
	RangeNumberOfFaultsPerCycle = MaxNumberOfFaultsPerCycle - MinNumberOfFaultsPerCycle + 1;
	RangeNumberOfFaultsPerRun = MaxNumberOfFaultsPerRun - MinNumberOfFaultsPerRun + 1;
	SimulationCounter = 0;
	SummaryFile = fopen(SummaryFileName, "wt");
	begin = clock();

  if (RndSeed != -1) {
    srand(int(RndSeed)); 
  }

	#pragma omp parallel for schedule(guided) private(ThreadIndex, ClockCycleIndex, ClockCycle, ClockCycleFaultFree, ClockCycleFaulty, i, j, k, LocalIndex, InputIndex, OutputIndex, SelectedNumberOfInjectedFaults, NumberOfInjectedFaults, NumberOfFaultsInCycle, TotalDetected, TotalNondetected, TotalIneffective, TotalRunTimeOver)
	for (SimulationIndex = 0;SimulationIndex < NumberOfSimulations; SimulationIndex++)
	{
		ThreadIndex = omp_get_thread_num();
		if (!Seeded[ThreadIndex])
		{
      if (RndSeed == -1) {
        srand(int(time(NULL)) ^ ThreadIndex); 
      } 	
      Seeded[ThreadIndex] = 1;
		}
		SimulationResults[SimulationIndex].TaregtCells = (int *)malloc(MaxNumberOfFaultsPerRun * sizeof(int));
		SimulationResults[SimulationIndex].TaregtClockCycles = (int *)malloc(MaxNumberOfFaultsPerRun * sizeof(int));

		NumberOfInjectedFaults = 0;
		SelectedNumberOfInjectedFaults = MinNumberOfFaultsPerRun + (rand() % RangeNumberOfFaultsPerRun);

		while (NumberOfInjectedFaults < SelectedNumberOfInjectedFaults)
		{
			do {
				ClockCycleIndex = rand() % NumberOfTargetClockCycles;
				ClockCycle = TargetClockCycles[ClockCycleIndex];

				for (j = 0;j < NumberOfInjectedFaults;j++)
					if (SimulationResults[SimulationIndex].TaregtClockCycles[j] == ClockCycle)
						break;
			} while (j < NumberOfInjectedFaults);

			NumberOfFaultsInCycle = MinNumberOfFaultsPerCycle + (rand() % RangeNumberOfFaultsPerCycle);

			for (i = 0;(i < NumberOfFaultsInCycle) & (NumberOfInjectedFaults < MaxNumberOfFaultsPerRun);i++)
			{
				SimulationResults[SimulationIndex].TaregtCells[NumberOfInjectedFaults] = rand() % NumberOfCells;

				if (Cells[SimulationResults[SimulationIndex].TaregtCells[NumberOfInjectedFaults]]->FaultAllowed)
				{
					SimulationResults[SimulationIndex].TaregtClockCycles[NumberOfInjectedFaults] = ClockCycle;

					for (j = 0;j < i;j++)
						if (SimulationResults[SimulationIndex].TaregtCells[NumberOfInjectedFaults] ==
							SimulationResults[SimulationIndex].TaregtCells[NumberOfInjectedFaults - j - 1])
							break;

					if (j < i)
						i--;
					else
						NumberOfInjectedFaults++;
				}
				else
					i--;
			}
		}

		SimulationResults[SimulationIndex].Valid = 1;
		SimulationResults[SimulationIndex].NumberOfInjectedFaults = NumberOfInjectedFaults;
    
#if defined(ALL_SIMS_USE_SAME_RND_DECISIONS)
    //TEMP hack:  override with first selection of fault locations
    if (SimulationIndex > 0) {
      for(unsigned e = 0; e < SimulationResults[0].NumberOfInjectedFaults; e++) { // ^^ some alignment issue segfault
        SimulationResults[SimulationIndex].TaregtCells[e] = SimulationResults[0].TaregtCells[e];
        SimulationResults[SimulationIndex].TaregtClockCycles[e] = SimulationResults[0].TaregtClockCycles[e];
      }
		  SimulationResults[SimulationIndex].NumberOfInjectedFaults = SimulationResults[0].NumberOfInjectedFaults;
    }
#endif
		for (InputIndex = 0;InputIndex < NumberOfRandomInputs;InputIndex++) {
			SignalValues[ThreadIndex][RandomInputs[InputIndex]] = rand() & 1;
			SignalValuesAt1SelectCycle[ThreadIndex][RandomInputs[InputIndex]] = SignalValues[ThreadIndex][RandomInputs[InputIndex]];
    }

		ClockCycleFaultFree = RunSimulation(Signals, ClockSignal, Max_No_ClockCycles,
			InitialSim_NumberOfClockCycles, InitialSim_NumberOfInputs,
			InitialSim_Inputs, InitialSim_Values,
			Cells, Regs, NumberOfRegs,
			MaxDepth, CellsInDepth, NumberOfCellsInDepth, CellTypes,
			EndSimCondition_Signals, EndSimCondition_Values,
			EndSimCondition_NumberOfSignals, EndSim_NumberOfWaitCycles,
			//SignalValues[ThreadIndex],SignalValuesAt1SelectCycle[ThreadIndex], SelectCycleToTap, RegValues[ThreadIndex], Faults[ThreadIndex]);
			SignalValues[ThreadIndex],SignalValuesAt1SelectCycle[ThreadIndex], SelectCycleToTap, RegValues[ThreadIndex], Faults[ThreadIndex]);

		for (BlockIndex = 0;BlockIndex < EndSim_NumberOfOutputBlocks;BlockIndex++)
			for (OutputIndex = 0;OutputIndex < EndSim_NumberOfOutputsInBlock[BlockIndex];OutputIndex++)
				FaultFreeOutputValues[ThreadIndex][BlockIndex][OutputIndex] = SignalValues[ThreadIndex][EndSim_OutputsInBlock[BlockIndex][OutputIndex]];

		for (i = 0;i < NumberOfInjectedFaults;i++)
			Faults[ThreadIndex][FaultInjectionType][SimulationResults[SimulationIndex].TaregtClockCycles[i]][SimulationResults[SimulationIndex].TaregtCells[i]] = 1;

    //NB: fault-free/faulty SignalValuesAt1SelectCycle

		ClockCycleFaulty = RunSimulation(Signals, ClockSignal, Max_No_ClockCycles,
			InitialSim_NumberOfClockCycles, InitialSim_NumberOfInputs,
			InitialSim_Inputs, InitialSim_Values,
			Cells, Regs, NumberOfRegs,
			MaxDepth, CellsInDepth, NumberOfCellsInDepth, CellTypes,
			EndSimCondition_Signals, EndSimCondition_Values,
			EndSimCondition_NumberOfSignals, EndSim_NumberOfWaitCycles,
			//SignalValues[ThreadIndex], SignalValuesAt1SelectCycle[ThreadIndex], SelectCycleToTap, RegValues[ThreadIndex], Faults[ThreadIndex]);
			SignalValues[ThreadIndex], SignalValuesAt1SelectCycle[ThreadIndex], -1, RegValues[ThreadIndex], Faults[ThreadIndex]);

		CheckResults(ClockCycleFaultFree, ClockCycleFaulty, Max_No_ClockCycles,
			EndSim_OutputNames, EndSim_Outputs_IndexL, EndSim_Outputs_IndexH,
			EndSim_Outputs_Base, EndSim_NumberOfOutputBlocks,
			EndSim_OutputsInBlock, EndSim_NumberOfOutputsInBlock,

			SignalsToTap_OutputNames, SignalsToTap_Outputs_IndexL, SignalsToTap_Outputs_IndexH,
			SignalsToTap_Outputs_Base, SignalsToTap_NumberOfOutputBlocks,
			SignalsToTap_OutputsInBlock, SignalsToTap_NumberOfOutputsInBlock,
			
      Signals, NumberOfSignals, FaultFreeOutputValues[ThreadIndex],
			SignalValues[ThreadIndex], SignalValuesAt1SelectCycle[ThreadIndex], SimulationResults[SimulationIndex],
			NumberOfRandomInputs, RandomInputs, IneffectiveCounter[ThreadIndex],
			NondetectedCounter[ThreadIndex], DetectedCounter[ThreadIndex],
			RunTimeOverCounter[ThreadIndex]);

		for (i = 0;i < NumberOfInjectedFaults;i++)
			Faults[ThreadIndex][FaultInjectionType][SimulationResults[SimulationIndex].TaregtClockCycles[i]][SimulationResults[SimulationIndex].TaregtCells[i]] = 0;

		#pragma omp atomic
		SimulationCounter++;

		if ((SimulationCounter & 0x7ff) == 0x7ff)
		{
			TotalDetected = 0;
			TotalNondetected = 0;
			TotalIneffective = 0;
			TotalRunTimeOver = 0;

			for (i = 0; i < Max_no_of_Threads; i++)
			{
				TotalDetected += DetectedCounter[i];
				TotalNondetected += NondetectedCounter[i];
				TotalIneffective += IneffectiveCounter[i];
				TotalRunTimeOver += RunTimeOverCounter[i];
			}

			int    elapsed_secs = int(double(clock() - begin) / CLOCKS_PER_SEC);
			char   Str1[200];

			sprintf(Str1, "%04d:%02d Total: %d  Ineffective: %d  Detected: %d  Non-detected: %d  RunTimeOver: %d \r\n", elapsed_secs / 60, elapsed_secs % 60,
				SimulationCounter, TotalIneffective, TotalDetected, TotalNondetected, TotalRunTimeOver);

			printf(Str1);
			fprintf(SummaryFile, Str1);
		}
	}

	TotalDetected = 0;
	TotalNondetected = 0;
	TotalIneffective = 0;
	TotalRunTimeOver = 0;

	for (i = 0; i < Max_no_of_Threads; i++)
	{
		TotalDetected += DetectedCounter[i];
		TotalNondetected += NondetectedCounter[i];
		TotalIneffective += IneffectiveCounter[i];
		TotalRunTimeOver += RunTimeOverCounter[i];
	}

	int    elapsed_secs = int(double(clock() - begin) / CLOCKS_PER_SEC);
	char   Str1[200];

  float pi_ = (1.0*TotalIneffective)/(TotalDetected+TotalNondetected); // Fault Ineffectivity Rate 
  sprintf(Str1, "%04d:%02d Total: %d  Ineffective: %d  Detected: %d  Non-detected: %d  RunTimeOver: %d   FaultIneffRate: %f\r\n", elapsed_secs / 60, elapsed_secs % 60,
		SimulationCounter, TotalIneffective, TotalDetected, TotalNondetected, TotalRunTimeOver, pi_);

	printf(Str1);
	fprintf(SummaryFile, Str1);

	fclose(SummaryFile);
	return 0;
}



void CheckResults(int ClockCycleFaultFree, int ClockCycleFaulty, int Max_No_ClockCycles,
	char** EndSim_OutputNames, int* EndSim_Outputs_IndexL, int* EndSim_Outputs_IndexH,
	char* EndSim_Outputs_Base, int EndSim_NumberOfOutputBlocks,
	int** EndSim_OutputsInBlock, int* EndSim_NumberOfOutputsInBlock,

	char** SignalsToTap_OutputNames, int* SignalsToTap_Outputs_IndexL, int* SignalsToTap_Outputs_IndexH,
	char* SignalsToTap_Outputs_Base, int SignalsToTap_NumberOfOutputBlocks,
	int** SignalsToTap_OutputsInBlock, int* SignalsToTap_NumberOfOutputsInBlock,
	
  SignalStruct** Signals, int NumberOfSignals, int** FaultFreeOutputValues,
	int* SignalValues, int *SignalValuesAt1SelectCycle, SimulationResultStruct &SimulationResult,
	int NumberOfRandomInputs, int* RandomInputs,
	int &IneffectiveCounter, int &NondetectedCounter, int &DetectedCounter, int &RunTimeOverCounter)
{
	char *Str1 = (char *)malloc(2*Max_Name_Length * sizeof(char));
	char *Str2 = (char *)malloc(Max_Name_Length * sizeof(char));
	char *Str3 = (char *)malloc(Max_Name_Length * sizeof(char));
	char *Str4 = (char *)malloc(Max_Name_Length * sizeof(char));
	int  i, j;
	int  BlockIndex;
	int  OutputIndex;
	int  SignalIndex;
	int  IndexH, IndexL;
	int  v;
	char Check;

	if ((ClockCycleFaultFree >= Max_No_ClockCycles) | (ClockCycleFaulty >= Max_No_ClockCycles))
	{
		SimulationResult.Result = Result_RunTimeOver;
		RunTimeOverCounter++;
	}
	else
	{
		Check = 0;
		for (BlockIndex = 0;BlockIndex < EndSim_NumberOfOutputBlocks;BlockIndex++)
			for (OutputIndex = 0;OutputIndex < EndSim_NumberOfOutputsInBlock[BlockIndex];OutputIndex++)
				if (SignalValues[EndSim_OutputsInBlock[BlockIndex][OutputIndex]] != FaultFreeOutputValues[BlockIndex][OutputIndex])
				{
					Check = 1;
					BlockIndex = EndSim_NumberOfOutputBlocks;
					break;
				}

		if (Check == 0)      // fault was ineffective
		{
			SimulationResult.Result = Result_Ineffective;
			IneffectiveCounter++;
		}
		else
		{
			for (BlockIndex = 0; BlockIndex < EndSim_NumberOfOutputBlocks; BlockIndex++)
				for (OutputIndex = 0; OutputIndex < EndSim_NumberOfOutputsInBlock[BlockIndex]; OutputIndex++)
					if (SignalValues[EndSim_OutputsInBlock[BlockIndex][OutputIndex]] != 0)
					{
						Check = 2;
						BlockIndex = EndSim_NumberOfOutputBlocks;
						break;
					}

			if (Check == 1) // fault was detected
			{
				SimulationResult.Result = Result_Detected;
				DetectedCounter++;
			}
			else                 // fault was not detected
			{
				SimulationResult.Result = Result_Nondetected;
				NondetectedCounter++;
			}
		}
	}

	//--- inputs

	Str2[0] = 0;
	v = 0;
	for (j = 0;j < NumberOfRandomInputs;j++)
	{
		i = j % 4;
		v |= SignalValues[RandomInputs[j]] << i;

		if ((i == 3) | (j == (NumberOfRandomInputs - 1)))
		{
			sprintf(Str3, "%x%s", v, Str2);
			strcpy(Str2, Str3);
			v = 0;
		}
	}

	strcpy(Str1, "input: ");
	strcat(Str1, Str2);

	//---  fault-free outputs

	Str2[0] = 0;
	for (BlockIndex = 0;BlockIndex < EndSim_NumberOfOutputBlocks;BlockIndex++)
	{
		strcat(Str2, " ");
		strcat(Str2, EndSim_OutputNames[BlockIndex]);
		strcat(Str2, ": ");

		sprintf(Str2, "%s%d'%c", Str2, EndSim_NumberOfOutputsInBlock[BlockIndex],
			(EndSim_Outputs_Base[BlockIndex] == 16) ? 'h' : 'b');

		Str3[0] = 0;
		v = 0;
		for (OutputIndex = 0;OutputIndex < EndSim_NumberOfOutputsInBlock[BlockIndex];OutputIndex++)
		{
			if (EndSim_Outputs_Base[BlockIndex] == 16)
			{
				i = OutputIndex % 4;
				v |= FaultFreeOutputValues[BlockIndex][OutputIndex] << i;

				if ((i == 3) | (OutputIndex == (EndSim_NumberOfOutputsInBlock[BlockIndex] - 1)))
				{
					sprintf(Str4, "%x%s", v, Str3);
					strcpy(Str3, Str4);
					v = 0;
				}
			}
			else if (EndSim_Outputs_Base[BlockIndex] == 2)
			{
				sprintf(Str4, "%d%s", FaultFreeOutputValues[BlockIndex][OutputIndex], Str3);
				strcpy(Str3, Str4);
			}
		}

		strcat(Str2, Str3);
	}

	strcat(Str1, " (fault-free)");
	strcat(Str1, Str2);

	//---  faulty outputs

	Str2[0] = 0;
	for (BlockIndex = 0;BlockIndex < EndSim_NumberOfOutputBlocks;BlockIndex++)
	{
		strcat(Str2, " ");
		strcat(Str2, EndSim_OutputNames[BlockIndex]);
		strcat(Str2, ": ");

		sprintf(Str2, "%s%d'%c", Str2, EndSim_NumberOfOutputsInBlock[BlockIndex],
			(EndSim_Outputs_Base[BlockIndex] == 16) ? 'h' : 'b');

		Str3[0] = 0;
		v = 0;
		for (OutputIndex = 0;OutputIndex < EndSim_NumberOfOutputsInBlock[BlockIndex];OutputIndex++)
		{
			if (EndSim_Outputs_Base[BlockIndex] == 16)
			{
				i = OutputIndex % 4;
				v |= SignalValues[EndSim_OutputsInBlock[BlockIndex][OutputIndex]] << i;

				if ((i == 3) | (OutputIndex == (EndSim_NumberOfOutputsInBlock[BlockIndex] - 1)))
				{
					sprintf(Str4, "%x%s", v, Str3);
					strcpy(Str3, Str4);
					v = 0;
				}
			}
			else if (EndSim_Outputs_Base[BlockIndex] == 2)
			{
				sprintf(Str4, "%d%s", SignalValues[EndSim_OutputsInBlock[BlockIndex][OutputIndex]], Str3);
				strcpy(Str3, Str4);
			}
		}

		strcat(Str2, Str3);
	}

	strcat(Str1, " (faulty)");
	strcat(Str1, Str2);
	
  //---  faulty outputs, at a select clock cycle
#if 1
	Str2[0] = 0;
	for (BlockIndex = 0;BlockIndex < SignalsToTap_NumberOfOutputBlocks;BlockIndex++)
	{
		strcat(Str2, " ");
		strcat(Str2, SignalsToTap_OutputNames[BlockIndex]);
		strcat(Str2, ": ");

		sprintf(Str2, "%s%d'%c", Str2, SignalsToTap_NumberOfOutputsInBlock[BlockIndex],
			(SignalsToTap_Outputs_Base[BlockIndex] == 16) ? 'h' : 'b');

		Str3[0] = 0;
		v = 0;
		for (OutputIndex = 0;OutputIndex < SignalsToTap_NumberOfOutputsInBlock[BlockIndex];OutputIndex++)
		{
			if (SignalsToTap_Outputs_Base[BlockIndex] == 16)
			{
				i = OutputIndex % 4;
				v |= SignalValuesAt1SelectCycle[SignalsToTap_OutputsInBlock[BlockIndex][OutputIndex]] << i;
//         printf(" %d ", SignalsToTap_OutputsInBlock[BlockIndex][OutputIndex]  );

				if ((i == 3) | (OutputIndex == (SignalsToTap_NumberOfOutputsInBlock[BlockIndex] - 1)))
				{
					sprintf(Str4, "%x%s", v, Str3);
					strcpy(Str3, Str4);
					v = 0;
				}
			}
			else if (SignalsToTap_Outputs_Base[BlockIndex] == 2)
			{
				sprintf(Str4, "%d%s", SignalValuesAt1SelectCycle[SignalsToTap_OutputsInBlock[BlockIndex][OutputIndex]], Str3);
				strcpy(Str3, Str4);
			}
		}

		strcat(Str2, Str3);
//   printf("xx: %s\n", Str2);
  fflush(stdout);
	}
#endif
	strcat(Str1, " (faultySelectCC)");
	strcat(Str1, Str2);

	strcat(Str1, " ineffective: ");
	if (SimulationResult.Result == Result_Ineffective)
		strcat(Str1, "1\n");
	else if (SimulationResult.Result == Result_Detected)
		strcat(Str1, "0  D\n");
	else if (SimulationResult.Result == Result_Nondetected)
		strcat(Str1, "0  ND\n");
	else if (SimulationResult.Result == Result_RunTimeOver)
		strcat(Str1, "0  RTO\n");

	SimulationResult.Output = (char *)malloc((strlen(Str1)+1) * sizeof(char));
	strcpy(SimulationResult.Output, Str1);

	free(Str1);
	free(Str2);
	free(Str3);
	free(Str4);
	return;
}

//***************************************************************************************

void PrintResultInFile(char* ResultFileName, int NumberOfSimulations,
	SimulationResultStruct* SimulationResults)
{
	FILE *ResultFile = NULL;
	int  SimulationIndex;

	ResultFile = fopen(ResultFileName, "wt");

	for (SimulationIndex = 0;SimulationIndex < NumberOfSimulations; SimulationIndex++)
		if (SimulationResults[SimulationIndex].Valid == 1)
			fprintf(ResultFile, "%s", SimulationResults[SimulationIndex].Output);

	fclose(ResultFile);
}

//***************************************************************************************

//***************************************************************************************
