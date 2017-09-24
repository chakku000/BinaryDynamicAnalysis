/*
 * This program find the entrance to the main call and return of the main
 */
#include "pin.H"
#include <iostream>
#include <fstream>

VOID ReturnFromMain()
{
    std::cerr << "Return from main" << std::endl;
}

VOID Trace(TRACE trace, VOID *v)
{
    for(BBL bbl = TRACE_BblHead(trace); BBL_Valid(bbl); bbl = BBL_Next(bbl))
    {
        for(INS ins = BBL_InsHead(bbl); INS_Valid(ins); ins = INS_Next(ins))
        {
            if(INS_IsRet(ins))  // the instruction is "return"
            {
                RTN rtn = INS_Rtn(ins); // The Routine of the instruction
                if(RTN_Valid(rtn)){
                    std::string routine_name = RTN_Name(rtn);
                    if(routine_name == "main"){ // The routine is main
                        INS_InsertCall(ins,IPOINT_BEFORE,(AFUNPTR)ReturnFromMain,IARG_CALL_ORDER,CALL_ORDER_LAST,IARG_END);
                    }
                }
            }
        }
    }
}

// called when enter main
VOID MainEntrance()
{
    std::cerr << "Enter main" << std::endl;
}

VOID Image(IMG img,VOID *v)
{
    // Implement entrance to the main
    RTN rtn = RTN_FindByName(img,"main");
    if(RTN_Valid(rtn)){
        RTN_Open(rtn);
        RTN_InsertCall(rtn,IPOINT_BEFORE,(AFUNPTR)MainEntrance,IARG_CALL_ORDER,CALL_ORDER_FIRST,IARG_END);
        RTN_Close(rtn);
    }
}

int main(int argc,char** argv){
    // Initialization
    PIN_InitSymbols();
    PIN_Init(argc,argv);

    // Image Implementation
    IMG_AddInstrumentFunction(Image,0);

    // Implement Trace
    TRACE_AddInstrumentFunction(Trace,0);

    // Start the Program never returns
    PIN_StartProgram();
    return 0;
}
