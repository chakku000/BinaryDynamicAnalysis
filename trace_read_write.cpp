/*
 * プログラムの
 */
#include "pin.H"
#include <iostream>

VOID AnalysisMemRead(ADDRINT addr)
{
    std::cout << "READ  " << std::hex << addr << std::endl;
}

VOID AnalysisMemWrite(ADDRINT addr)
{
    std::cout << "WRITE " << std::hex << addr << std::endl;
}

VOID Trace(TRACE trace , VOID *v)
{
    for(BBL bbl = TRACE_BblHead(trace);BBL_Valid(bbl);bbl = BBL_Next(bbl))
    {
        for(INS ins = BBL_InsHead(bbl); INS_Valid(ins); ins = INS_Next(ins))
        {
            UINT32 operandsCount = INS_MemoryOperandCount(ins); // オペランドの数

            for(UINT32 memOp = 0; memOp < operandsCount ; memOp++)
            {
                if(INS_MemoryOperandIsRead(ins,memOp))
                {
                    INS_InsertPredicatedCall(
                            ins,IPOINT_BEFORE,(AFUNPTR)AnalysisMemRead,
                            IARG_MEMORYOP_EA,memOp,
                            IARG_END);
                }
                if(INS_MemoryOperandIsWritten(ins,memOp))
                {
                    INS_InsertPredicatedCall(
                            ins,IPOINT_BEFORE,(AFUNPTR)AnalysisMemWrite,
                            IARG_MEMORYOP_EA,memOp,
                            IARG_END);
                }
            }
        }
    }
}

int main(int argc,char** argv){
    // Initialization
    PIN_InitSymbols();
    PIN_Init(argc,argv);

    // Implement Trace
    TRACE_AddInstrumentFunction(Trace,0);

    // Start the Program never returns
    PIN_StartProgram();
    return 0;
}
