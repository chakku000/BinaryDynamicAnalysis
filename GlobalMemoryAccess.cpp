#include <iostream>
#include <fstream>
#include "pin.H"


PIN_LOCK pinlock;

//std::ofstream outfile("access.txt");
bool onDetection[100];

VOID RecordMemRead(INS ins, ADDRINT addr){
    THREADID tid = PIN_ThreadId();
    if(!onDetection[tid]) return;
    //USIZE size = INS_MemoryReadSize(ins);
    PIN_GetLock(&pinlock,PIN_ThreadId()+1);
    //outfile << addr << " " << size << endl;
    //outfile << addr << endl;
    PIN_ReleaseLock(&pinlock);
}

VOID RecordMemWrite(INS ins, ADDRINT addr){
    THREADID tid = PIN_ThreadId();
    if(!onDetection[tid]) return;
    //USIZE size = INS_MemoryWriteSize(ins);
    PIN_GetLock(&pinlock,PIN_ThreadId()+1);
    //outfile << addr << " " << size << endl;
    //outfile << addr << endl;
    PIN_ReleaseLock(&pinlock);
}

VOID ExitMain(){
    onDetection[0] = false;
}

VOID Trace(TRACE trace,VOID *v){
    for(BBL bbl = TRACE_BblHead(trace); BBL_Valid(bbl); bbl = BBL_Next(bbl)){
        for(INS ins = BBL_InsHead(bbl); INS_Valid(ins); ins = INS_Next(ins)){
            UINT32 memOperands = INS_MemoryOperandCount(ins);
            for(UINT32 memOp=0;memOp<memOperands;memOp++){
                if(INS_MemoryOperandIsRead(ins,memOp) and !INS_IsStackRead(ins)){
                    INS_InsertPredicatedCall(
                            ins,IPOINT_BEFORE,(AFUNPTR)RecordMemRead,
                            IARG_INST_PTR,
                            IARG_MEMORYOP_EA,memOp,
                            IARG_END);
                }
                if(INS_MemoryOperandIsWritten(ins,memOp) and !INS_IsStackWrite(ins)){
                    INS_InsertPredicatedCall(
                            ins,IPOINT_BEFORE,(AFUNPTR)RecordMemWrite,
                            IARG_INST_PTR,
                            IARG_MEMORYOP_EA,memOp,
                            IARG_END);
                }
            }

            if(INS_IsRet(ins)){
                RTN rtn = INS_Rtn(ins);
                if(RTN_Valid(rtn)){
                    std::string rtn_name = RTN_Name(rtn);
                    if(rtn_name == "main"){
                        INS_InsertCall(ins,IPOINT_BEFORE,(AFUNPTR)ExitMain,IARG_CALL_ORDER,CALL_ORDER_LAST,IARG_END); 
                    }
                }
            }
        }
    }
}

VOID ThreadStart(THREADID tid,CONTEXT *ctxt, INT32 flags, VOID *v){
    if(tid == 0) return;
    onDetection[tid] = true;
}

VOID ThreadFini(THREADID tid,const CONTEXT *ctxt, INT32 code, VOID *v){
    if(tid == 0) return;
    onDetection[tid] = false;
}

// start implementation on thread 0
VOID MainEntrance(){
    onDetection[0] = true;
}

VOID FindMain(IMG img, VOID *v){
    RTN rtn = RTN_FindByName(img,"main");
    if(RTN_Valid(rtn)){
        RTN_Open(rtn);

        RTN_InsertCall(rtn,IPOINT_BEFORE,(AFUNPTR)MainEntrance,IARG_CALL_ORDER,CALL_ORDER_FIRST,IARG_END);

        RTN_Close(rtn);
    }
}

int main(int argc,char* argv[]){
    PIN_Init(argc,argv);
    PIN_InitSymbols();
    PIN_InitLock(&pinlock);

    //onDetection[0] = true;

    IMG_AddInstrumentFunction(FindMain,0);

    TRACE_AddInstrumentFunction(Trace,0);
    PIN_AddThreadStartFunction(ThreadStart,0);
    PIN_AddThreadFiniFunction(ThreadFini,0);

    PIN_StartProgram();
    //outfile.close();
    return 0;
}
