#include <stdio.h>
#include "pin.H"

FILE * trace;

// ANALYSIS ROUTINES 
VOID RecordMemRead(VOID * ip, VOID * addr, UINT32 size) {
    fprintf(trace,"%p: R %p %u\n", ip, addr, size);
}

VOID RecordMemWrite(VOID * ip, VOID * addr, UINT32 size) {
    fprintf(trace,"%p: W %p %u\n", ip, addr, size);
}

// INSTRUMENTATION ROUTINE 
VOID Instruction(INS ins, VOID *v) {
    if (INS_IsMemoryRead(ins)) {
        INS_InsertPredicatedCall(
            ins, IPOINT_BEFORE, (AFUNPTR)RecordMemRead,
            IARG_INST_PTR, IARG_MEMORYREAD_EA, IARG_MEMORYREAD_SIZE, IARG_END);
    }
    if (INS_IsMemoryWrite(ins)) {
        INS_InsertPredicatedCall(
            ins, IPOINT_BEFORE, (AFUNPTR)RecordMemWrite,
            IARG_INST_PTR, IARG_MEMORYWRITE_EA, IARG_MEMORYWRITE_SIZE, IARG_END);
    }
}

VOID Fini(INT32 code, VOID *v) {
    fprintf(trace, "#eof\n");
    fclose(trace);
}

int main(int argc, char *argv[]) {
    if (PIN_Init(argc, argv)) return -1;

    trace = fopen("trace_2024CSB1118.out", "w");
    fprintf(trace, "# Memory Access Trace\n");
    fprintf(trace, "# Format: [IP]: [R/W] [Address] [Size]\n");

    INS_AddInstrumentFunction(Instruction, 0);
    PIN_AddFiniFunction(Fini, 0);
    PIN_StartProgram();

    return 0;
}