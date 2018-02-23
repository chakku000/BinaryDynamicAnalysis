#include <iostream>
#include <vector>
#include "pin.H"

template<typename T> ostream& operator<<(ostream& os,const vector<T>& vec){ os << "["; for(const auto& v : vec){ os << v << ","; } os << "]"; return os; }

const int MAX_THREAD = 15;

// tls_keyを用いて、各スレッドのデータにアクセスできる
TLS_KEY tls_key = INVALID_TLS_KEY;

PIN_LOCK pinlock;

// スレッドローカルなデータ構造
struct ThreadLocalData{
    int id;
    std::vector<int> v;
    ThreadLocalData(int N,int id_) :id(id_),v(N,0) {}
    void inc(){ v[id]++; }
};

// スレッドローカルデータを取り出すメソッドをラップしておくと便利
ThreadLocalData* getTLS(THREADID threadid){
    return static_cast<ThreadLocalData*>(PIN_GetThreadData(tls_key,threadid));
}

// スレッドローカルデータをセットするメソッドをラップしておくと便利
bool setTLS(THREADID threadid,ThreadLocalData* data){
    return PIN_SetThreadData(tls_key,data,threadid);
}

VOID ThreadStart(THREADID threadid,CONTEXT* ctxt,INT32 flags,VOID* v){
    PIN_GetLock(&pinlock,threadid+1);
    std::cout << "Thread " << threadid << " Start" << std::endl;
    ThreadLocalData* tld = getTLS(threadid);
    if(tld == NULL){
        std::cout << "Thread " << threadid << " has NO DATA" << std::endl;
        ThreadLocalData* data = new ThreadLocalData(MAX_THREAD,threadid);
        if(!setTLS(threadid,data)){
            PIN_ReleaseLock(&pinlock);
            return;
        }
        data->inc();    // 自分のスレッド番号に対応するエントリだけインクリメント
    }
    PIN_ReleaseLock(&pinlock);
}

VOID ThreadFini(THREADID threadid,const CONTEXT* ctxt,INT32 code,VOID* v){
    PIN_GetLock(&pinlock,threadid+1);
    ThreadLocalData* data = getTLS(threadid);
    std::cout << "Thread " << threadid << " Finish" <<  " " << data->v << std::endl;
    PIN_ReleaseLock(&pinlock);
}

int main(int argc,char** argv){
    PIN_InitSymbols();
    if(PIN_Init(argc,argv)){
        std::cerr << "argument error" << std::endl;
        return -1;
    }

    // PINロックの初期化
    PIN_InitLock(&pinlock);

    // tls_keyの初期化
    tls_key = PIN_CreateThreadDataKey(NULL);
    if(tls_key == INVALID_TLS_KEY){
        std::cerr << "Faild PIN_CreateThreadDataKey" << std::endl;
        PIN_ExitProcess(1);
    }

    PIN_AddThreadStartFunction(ThreadStart,NULL);
    PIN_AddThreadFiniFunction(ThreadFini,NULL);
    PIN_StartProgram();
    return 0;
}
