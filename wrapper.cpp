#include <iostream>
#include <cstdio>
#include <fstream>
#include <pthread.h>

#include "pin.H"

void sthread_create(int v){
    std::cout << "sthread_create : "  << v << std::endl;
}

// orgFuncptr : pthread_createへのポインタ
int Replace_PthreadCreate(CONTEXT* context, AFUNPTR orgFuncptr, pthread_t* thread, const pthread_attr_t* attr, void* fun_ptr,void* arg)
{
    int ret;

    std::pair<void*,int> vc;

    PIN_CallApplicationFunction(
        context,PIN_ThreadId(),CALLINGSTD_DEFAULT,
        orgFuncptr,NULL,
        PIN_PARG(int),&ret,
        PIN_PARG(pthread_t*),thread,
        PIN_PARG(pthread_attr_t*),attr,
        PIN_PARG(void*), sthread_create,
        PIN_PARG(void*),10,
        PIN_PARG_END());

    return ret;
}

/* ===================================================================== */
/* Image Instrumentation                                                 */
/* ===================================================================== */
VOID LockInstrumentation(IMG img,VOID *v)
{
    PROTO proto_func;   // 関数のプロトタイプ
    RTN rtn;            // 置換対象のルーチン

    // Replace pthread_create
    proto_func = PROTO_Allocate(PIN_PARG(int),CALLINGSTD_DEFAULT,
                                "pthread_create",PIN_PARG(pthread_t*),PIN_PARG(pthread_attr_t*),PIN_PARG(void*),PIN_PARG(void*),PIN_PARG_END());
    rtn = RTN_FindByName(img,"pthread_create");
    if(RTN_Valid(rtn)){
        RTN_ReplaceSignature(rtn,AFUNPTR(Replace_PthreadCreate),
                            IARG_PROTOTYPE,proto_func,          // 置換対象の関数情報
                            IARG_CONTEXT,
                            IARG_ORIG_FUNCPTR,
                            IARG_FUNCARG_ENTRYPOINT_VALUE,0,
                            IARG_END);
    }
}


/* ===================================================================== */
/* Print Help Message                                                    */
/* ===================================================================== */
INT32 Usage()
{
    std::cerr << "This tool counts the number of dynamic instructions executed" << endl;
    std::cerr << endl << KNOB_BASE::StringKnobSummary() << endl;
    return -1;
}

/* ===================================================================== */
/* Main                                                                  */
/* ===================================================================== */
/*   argc, argv are the entire command line: pin -t <toolname> -- ...    */
/* ===================================================================== */
int main(int argc,char* argv[])
{
    // Initalize Symbols
    PIN_InitSymbols();

    // Initialize pin
    if(PIN_Init(argc,argv)) return Usage();

    // Image Instrumentation
    IMG_AddInstrumentFunction(LockInstrumentation,0);

    // Start the program, never returns
    PIN_StartProgram();
    return 0;
}
