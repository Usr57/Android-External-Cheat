#include "Scarecrow/Server.hpp"
	
function(ExampleOn) {
	char* pkg = const_cast<char*>(processGame.c_str());
    initXMemoryTools(pkg, MODE_ROOT);
    SetSearchRange(A_ANONMYOUS);
    MemorySearch(OBF("1000"), TYPE_DWORD);
	MemoryWrite("2000", 0, TYPE_DWORD);
    ClearResults();
    return 0;
}

function(ExampleOff) {
	char* pkg = const_cast<char*>(processGame.c_str());
    initXMemoryTools(pkg, MODE_ROOT);
    SetSearchRange(A_ANONMYOUS);
    MemorySearch("2000", TYPE_DWORD);
    MemoryWrite("1000", 0, TYPE_DWORD);
    ClearResults();
    return 0;
}

function(ExampleFreeze) {
    char* pkg = const_cast<char*>(processGame.c_str());
    initXMemoryToolsOld(pkg, MODE_ROOT); // Old для работы с rx-p через /mem
    SetSearchRange(CODE_APP);
    MemorySearch("1", TYPE_DWORD);
    MemoryOffset("2", 4, TYPE_DWORD);
    MemoryOffset("3", 8, TYPE_DWORD);
    MemoryOffset("0", -4, TYPE_DWORD); 

    AddFreezeItem_All("5577", TYPE_DWORD, 0);
    AddFreezeItem_All("6688", TYPE_DWORD, -4);
    StartFreezeThread();
    ClearResults();
    return 0;
	// gg: 0;1;2;3::13 запись на 6688;5577;2;3 первые 2 на фриз
}

function(ExampleFreezeOff) {
	StopFreezeThread(); 
    RemoveFreezeItem_All();
    SetSearchRange(CODE_APP);
    MemorySearch("5577", TYPE_DWORD);
	MemoryWrite("1", 0, TYPE_DWORD);
    ClearResults();
	
	MemorySearch("6688", TYPE_DWORD);
	MemoryWrite("0", 0, TYPE_DWORD);
	
    ClearResults();
    return 0;
}

function(ExampleAdd) {
	char* pkg = const_cast<char*>(processGame.c_str());
    initXMemoryTools(pkg, MODE_ROOT);
    SetSearchRange(A_ANONMYOUS);
    MemorySearch("1000", TYPE_DWORD);
    MemoryWriteRelative("+1", 0, TYPE_DWORD); // Добавить к значению, а не заменить
    ClearResults();
    return 0;
}



function(ExampleInaccurate) {
    char* pkg = const_cast<char*>(processGame.c_str());
    initXMemoryTools(pkg, MODE_ROOT);
    SetSearchRange(A_ANONMYOUS);
    MemorySearch("100~120", TYPE_FLOAT); // Ищем от 100 до 120
    MemoryOffset("2~3", 4, TYPE_FLOAT); // Рядом значения от 2 до 3
    
    AddFreezeItem_All_Relative("-5", TYPE_FLOAT, 0); // От каждого значения из поиска -5 и фриз
    StartFreezeThread();
    ClearResults();
    return 0;
	// gg: 100~120;2~3::5 Dword добавить к значению а не заменить "-5" + заморозить
}

function(HexPatchOn) {
    char* pkg = const_cast<char*>(processGame.c_str());
    return hexPatch(pkg, "0x2f696dc", "RET", "0", "libil2cpp.so"); // Offset, replace asm / hex, indent, libname
}


function(HexPatchOff) {
    char* pkg = const_cast<char*>(processGame.c_str());
    return resPatch(pkg, "0x2f696dc", "0", "libil2cpp.so");
}

function(HookMethodOn) {
    char* pkg = const_cast<char*>(processGame.c_str());
    return hookMethod(pkg, "0x8b38b8", "0x1043040", "libil2cpp.so");
}

function(HookMethodOff) {
    char* pkg = const_cast<char*>(processGame.c_str());
    return resPatch(pkg, "0x8b38b8", "0", "libil2cpp.so");
}

function(HookFieldOn) {
    char* pkg = const_cast<char*>(processGame.c_str());
    return hookField(pkg, "0xC4D4B0", "10.5", "0x88", "libil2cpp.so");
}

function(HookFieldOff) {
    char* pkg = const_cast<char*>(processGame.c_str());
    return resPatch(pkg, "0xC4D4B0", "0", "libil2cpp.so");
}

    /*
    TYPE_DWORD
	TYPE_FLOAT
	TYPE_DOUBLE
	TYPE_WORD
	TYPE_BYTE
	TYPE_QWORD

	ALL
	B_BAD
	V
	C_ALLOC
	C_BSS
	CODE_APP
	C_DATA
	C_HEAP
	JAVA_HEAP
	A_ANONMYOUS
	CODE_SYSTEM
	STACK
	ASHMEM
    */
