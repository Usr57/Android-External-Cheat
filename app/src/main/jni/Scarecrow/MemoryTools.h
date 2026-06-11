#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <dirent.h>
#include <string.h>
#include <errno.h>
#include <pthread.h>
using namespace std;
struct MAPS
{
	long int addr;
	long int taddr;
	int type;
	struct MAPS *next;
};
struct RESULT
{
	long int addr;
	struct RESULT *next;
};
struct FREEZE
{
	long int addr;
	char *value;
	int type;
	struct FREEZE *next;
};
#define MODE_ROOT "MODE_ROOT"
#define MODE_NO_ROOT "MODE_NO_ROOT"
#define TRUE 1
#define FALSE 0
#define LEN sizeof(struct MAPS)
#define FRE sizeof(struct FREEZE)
typedef int BOOLEAN;
typedef struct MAPS *PMAPS;
typedef struct RESULT *PRES;
typedef struct FREEZE *PFREEZE;
typedef int TYPE;
typedef int RANGE;
typedef int COUNT;
typedef int COLOR;
typedef long int OFFSET;
typedef long int ADDRESS;
typedef char PACKAGENAME;
typedef int64_t QWORD;
typedef int32_t DWORD;
typedef int16_t WORD;
typedef int8_t BYTE;
typedef float FLOAT;
typedef double DOUBLE;
enum type
{
	TYPE_DWORD,
	TYPE_FLOAT,
	TYPE_DOUBLE,
	TYPE_WORD,
	TYPE_BYTE,
	TYPE_QWORD,
};
enum Range
{
	ALL,
	B_BAD,
	V,
	C_ALLOC,
	C_BSS,
	CODE_APP,
	C_DATA,
	C_HEAP,
	JAVA_HEAP,
	A_ANONMYOUS,
	CODE_SYSTEM,
	STACK,
	ASHMEM
};
enum Color
{
	COLOR_SILVERY,
	COLOR_RED,
	COLOR_GREEN,
	COLOR_YELLOW,
	COLOR_DARK_BLUE,
	COLOR_PINK,
	COLOR_SKY_BLUE,
	COLOR_WHITE
};
PMAPS Res = NULL;
PFREEZE Pfreeze = NULL;
PFREEZE pEnd = NULL;
PFREEZE pNew = NULL;
int FreezeCount = 0;
int Freeze = 0;
pthread_t pth;
char bm[64];
long int delay = 30000;
int handle;
int ResCount = 0;
int gs = 0;
int MemorySearchRange = 0;
int ms = 0;
int SetTextColor(int color);
int getPID(char bm[64]);
void initXMemoryTools(char *argv[], char *bm, char *mode);
int SetSearchRange(int type);
PMAPS readmaps(int type);
PMAPS readmaps_all();
PMAPS readmaps_bad();
PMAPS readmaps_v();
PMAPS readmaps_c_alloc();
PMAPS readmaps_c_bss();
PMAPS readmaps_code_app();
PMAPS readmaps_c_data();
PMAPS readmaps_c_heap();
PMAPS readmaps_java_heap();
PMAPS readmaps_a_anonmyous();
PMAPS readmaps_code_system();
PMAPS readmaps_stack();
PMAPS readmaps_ashmem();
void BaseAddressSearch(char *value, int type, long int BaseAddr);
PMAPS BaseAddressSearch_DWORD(DWORD value, long int BaseAddr, PMAPS pMap);
PMAPS BaseAddressSearch_FLOAT(FLOAT value, long int BaseAddr, PMAPS pMap);
PMAPS BaseAddressSearch_DOUBLE(DOUBLE value, long int BaseAddr, PMAPS pMap);
PMAPS BaseAddressSearch_WORD(WORD value, long int BaseAddr, PMAPS pMap);
PMAPS BaseAddressSearch_BYTE(BYTE value, long int BaseAddr, PMAPS pMap);
PMAPS BaseAddressSearch_QWORD(QWORD value, long int BaseAddr, PMAPS pMap);
void RangeMemorySearch(char *from_value, char *to_value, int type);
PMAPS RangeMemorySearch_DWORD(DWORD from_value, DWORD to_value, PMAPS pMap);
PMAPS RangeMemorySearch_FLOAT(FLOAT from_value, FLOAT to_value, PMAPS pMap);
PMAPS RangeMemorySearch_DOUBLE(DOUBLE from_value, DOUBLE to_value, PMAPS pMap);
PMAPS RangeMemorySearch_WORD(WORD from_value, WORD to_value, PMAPS pMap);
PMAPS RangeMemorySearch_BYTE(BYTE from_value, BYTE to_value, PMAPS pMap);
PMAPS RangeMemorySearch_QWORD(QWORD from_value, QWORD to_value, PMAPS pMap);
void MemorySearch(char *value, int TYPE);
PMAPS MemorySearch_DWORD(DWORD value, PMAPS pMap);
PMAPS MemorySearch_FLOAT(FLOAT value, PMAPS pMap);
PMAPS MemorySearch_DOUBLE(DOUBLE value, PMAPS pMap);
PMAPS MemorySearch_WORD(WORD value, PMAPS pMap);
PMAPS MemorySearch_BYTE(BYTE value, PMAPS pMap);
PMAPS MemorySearch_QWORD(QWORD value, PMAPS pMap);
void MemoryOffset(char *value, long int offset, int type);
PMAPS MemoryOffset_DWORD(DWORD value, long int offset, PMAPS pBuff);
PMAPS MemoryOffset_FLOAT(FLOAT value, long int offset, PMAPS pBuff);
PMAPS MemoryOffset_DOUBLE(DOUBLE value, long int offset, PMAPS pBuff);
PMAPS MemoryOffset_WORD(WORD value, long int offset, PMAPS pBuff);
PMAPS MemoryOffset_BYTE(BYTE value, long int offset, PMAPS pBuff);
PMAPS MemoryOffset_QWORD(QWORD value, long int offset, PMAPS pBuff);
void MemoryOffset(char *value, const char *offset_str, int type);
PMAPS MemoryOffsetRange_DWORD(DWORD value, long int offset_start, long int offset_end, PMAPS pBuff);
PMAPS MemoryOffsetRange_FLOAT(FLOAT value, long int offset_start, long int offset_end, PMAPS pBuff);
PMAPS MemoryOffsetRange_DOUBLE(DOUBLE value, long int offset_start, long int offset_end, PMAPS pBuff);
PMAPS MemoryOffsetRange_WORD(WORD value, long int offset_start, long int offset_end, PMAPS pBuff);
PMAPS MemoryOffsetRange_BYTE(BYTE value, long int offset_start, long int offset_end, PMAPS pBuff);
PMAPS MemoryOffsetRange_QWORD(QWORD value, long int offset_start, long int offset_end, PMAPS pBuff);
void RangeMemoryOffset(char *from_value, char *to_value, long int offset, int type);
PMAPS RangeMemoryOffset_DWORD(DWORD from_value, DWORD to_value, long int offset, PMAPS pBuff);
PMAPS RangeMemoryOffset_FLOAT(FLOAT from_value, FLOAT to_value, long int offset, PMAPS pBuff);
PMAPS RangeMemoryOffset_DOUBLE(DOUBLE from_value, DOUBLE to_value, long int offset, PMAPS pBuff);
PMAPS RangeMemoryOffset_WORD(WORD from_value, WORD to_value, long int offset, PMAPS pBuff);
PMAPS RangeMemoryOffset_BYTE(BYTE from_value, BYTE to_value, long int offset, PMAPS pBuff);
PMAPS RangeMemoryOffset_QWORD(QWORD from_value, QWORD to_value, long int offset, PMAPS pBuff);
void MemoryWrite(char *value, long int offset, int type);
void MemoryWriteRelative(const char *expr, long int offset, int type);
int MemoryWrite_DWORD(DWORD value, PMAPS pBuff, long int offset);
int MemoryWrite_FLOAT(FLOAT value, PMAPS pBuff, long int offset);
int MemoryWrite_DOUBLE(DOUBLE value, PMAPS pBuff, long int offset);
int MemoryWrite_WORD(WORD value, PMAPS pBuff, long int offset);
int MemoryWrite_BYTE(BYTE value, PMAPS pBuff, long int offset);
int MemoryWrite_QWORD(QWORD value, PMAPS pBuff, long int offset);
int WriteAddress(long int addr, char *value, int type);
int WriteAddress_DWORD(ADDRESS addr, DWORD value);
int WriteAddress_FLOAT(ADDRESS addr, FLOAT value);
int WriteAddress_DOUBLE(ADDRESS addr, DOUBLE value);
int WriteAddress_WORD(ADDRESS addr, WORD value);
int WriteAddress_BYTE(ADDRESS addr, BYTE value);
int WriteAddress_QWORD(ADDRESS addr, QWORD value);
void PrintResults();
void ClearResults();
void ClearMaps(PMAPS pMap);
int GetResultCount();
int isapkinstalled(char *bm);
int isapkrunning(char *bm);
int killprocess(char *bm);
char GetProcessState(char *bm);
int killGG();
int killXs();
int killLOL();
int uninstallapk(char *bm);
int installapk(char *lj);
int rebootsystem();
int leechermati();
int leechermati2();
int PutDate();
int GetDate(char *date);
PMAPS GetResults();
int AddFreezeItem_All(char *Value, int type, long int offset);
int AddFreezeItem_All_Relative(const char *expr, int type, long int offset);
int AddFreezeItem(long int addr, char *value, int type, long int offset);
int AddFreezeItem_DWORD(long int addr, char *value);
int AddFreezeItem_FLOAT(long int addr, char *value);
int AddFreezeItem_DOUBLE(long int addr, char *value);
int AddFreezeItem_WORD(long int addr, char *value);
int AddFreezeItem_BYTE(long int addr, char *value);
int AddFreezeItem_QWORD(long int addr, char *value);
int RemoveFreezeItem(long int addr);
int RemoveFreezeItem_All();
int StartFreeze();
int StopFreeze();
int StartFreezeThread();
int StopFreezeThread();
int SetFreezeDelay(long int De);
int PrintFreezeItems();
char *GetAddressValue(ADDRESS addr, int type);
DWORD GetAddressValue_DWORD(ADDRESS addr);
FLOAT GetAddressValue_FLOAT(ADDRESS addr);
DOUBLE GetAddressValue_DOUBLE(ADDRESS addr);
WORD GetAddressValue_WORD(ADDRESS addr);
BYTE GetAddressValue_BYTE(ADDRESS addr);
QWORD GetAddressValue_QWORD(ADDRESS addr);
char *getRightText(char *dst, char *src, int n);
bool getMask(long int addr, char *def);
void MaskSearch(char *def, int type);
bool snowpd(long int addr);
void initXMemoryTools(char *b, char *mode)
{
	strcpy(bm, b);
	if (strcmp(mode, MODE_ROOT) == 0)
	{
		if (getuid() != 0)
		{
			system("echo starting");			
		}
	}
	pid_t pid = getPID(b);
	if (pid == 0)
	{
		printf("\033[31;1m");
		puts("Gagal mendapatkan proses!");
	}
	char lj[64];
	sprintf(lj, "/proc/%d/mem", pid);
	handle = open(lj, O_RDWR);
	if (handle == -1)
	{
		printf("\033[31;1m");
		perror("FATAL ERROR: Gagal mendapatkan memory (/proc/PID/mem)"); 
	}
    printf("\033[32;1mSUCCESS: Память процесса успешно открыта! Handle: %d\n", handle);
	lseek(handle, 0, SEEK_SET);
}
int GetResultCount()
{
	return gs;
}
bool snowpd(long int a2)
{
	return a2 <= 7999 && a2 > 31;
}
DWORD GetAddressValue_DWORD(ADDRESS addr)
{
	DWORD buf;
	pread64(handle, &buf, sizeof(DWORD), addr);
	return buf;
}
FLOAT GetAddressValue_FLOAT(ADDRESS addr)
{
	FLOAT buf;
	pread64(handle, &buf, sizeof(FLOAT), addr);
	return buf;
}
DOUBLE GetAddressValue_DOUBLE(ADDRESS addr)
{
	DOUBLE buf;
	pread64(handle, &buf, sizeof(DOUBLE), addr);
	return buf;
}
WORD GetAddressValue_WORD(ADDRESS addr)
{
	WORD buf;
	pread64(handle, &buf, sizeof(WORD), addr);
	return buf;
}
BYTE GetAddressValue_BYTE(ADDRESS addr)
{
	BYTE buf;
	pread64(handle, &buf, sizeof(BYTE), addr);
	return buf;
}
QWORD GetAddressValue_QWORD(ADDRESS addr)
{
	QWORD buf;
	pread64(handle, &buf, sizeof(QWORD), addr);
	return buf;
}
char *GetAddressValue(ADDRESS addr, int type)
{
	char *str = (char *)malloc(sizeof(char));
	DWORD i;
	FLOAT f;
	DOUBLE d;
	WORD w;
	BYTE b;
	QWORD q;
	switch (type)
	{
	case TYPE_DWORD:
		i = GetAddressValue_DWORD(addr);
		sprintf(str, "%d", i);
		break;
	case TYPE_FLOAT:
		f = GetAddressValue_FLOAT(addr);
		sprintf(str, "%e", f);
		break;
	case TYPE_DOUBLE:
		d = GetAddressValue_DOUBLE(addr);
		sprintf(str, "%e", d);
		break;
	case TYPE_WORD:
		w = GetAddressValue_WORD(addr);
		sprintf(str, "%d", w);
		break;
	case TYPE_BYTE:
		b = GetAddressValue_BYTE(addr);
		sprintf(str, "%d", b);
		break;
	case TYPE_QWORD:
		q = GetAddressValue_QWORD(addr);
		sprintf(str, "%ld", q);
		break;
	default:
		sprintf(str, "%s", "");
		printf("\033[32;1m你选择了一个未知类型!\n");
		break;
	}
	return str;
}
char *getRightText(char *dst, char *src, int n)
{
	char *p = src;
	char *q = dst;
	int len = strlen(src);
	if (n > len)
		n = len;
	p += (len - n);
	while (*(q++) = *(p++));
	return dst;
}
bool getMask(long int addr, char *def)
{
	char *str = (char *)malloc(sizeof(char));
	sprintf(str, "%lX", addr);
	char *dest;
	char *s = getRightText(dest, str, strlen(def));
	if (strcmp(s, def) == 0)
	{
		return true;
	}
	return false;
}
void MaskSearch(char *def, int type)
{
	PMAPS e, n;
	e = n = (PMAPS) malloc(LEN);
	PMAPS pBuff = n;
	int iCount = 0;
	PMAPS pTemp = Res;
	for (int i = 0; i < ResCount; i++)
	{
		long int addr = pTemp->addr;
		bool value = getMask(addr, def);
		if (value == true)
		{
			iCount++;
			n->addr = addr;
			n->type = type;
			if (iCount == 1)
			{
				n->next = NULL;
				e = n;
				pBuff = n;
			}
			else
			{
				n->next = NULL;
				e->next = n;
				e = n;
			}
			n = (PMAPS) malloc(LEN);
		}
		pTemp = pTemp->next;
	}
	ResCount = iCount;
	gs = iCount;
	Res = pBuff;
}
int SetTextColor(COLOR color)
{
	switch (color)
	{
	case COLOR_SILVERY:
		printf("\033[30;1m");
		break;
	case COLOR_RED:
		printf("\033[31;1m");
		break;
	case COLOR_GREEN:
		printf("\033[32;1m");
		break;
	case COLOR_YELLOW:
		printf("\033[33;1m");
		break;
	case COLOR_DARK_BLUE:
		printf("\033[34;1m");
		break;
	case COLOR_PINK:
		printf("\033[35;1m");
		break;
	case COLOR_SKY_BLUE:
		printf("\033[36;1m");
		break;
	case COLOR_WHITE:
		printf("\033[37;1m");
		break;
	default:
		printf("\033[37;1m");
		break;
	}
	return 0;
}
long int GetModuleBase(char *module, char *flags, int MapSize)
{
	PMAPS pHead = NULL;
	PMAPS pNew = NULL;
	PMAPS pEnd = NULL;
	pEnd = pNew = (PMAPS) malloc(MapSize);
	FILE *fp;
	int flag = 1;
	char lj[64], buff[256];
	int pid = getPID(bm);
	sprintf(lj, "/proc/%d/maps", pid);
	fp = fopen(lj, "r");
	if (fp == NULL)
	{
		printf("\033[31;1m");
		perror("FATAL ERROR: Не удалось открыть /proc/PID/maps"); 
		return 0;
	}
    printf("\033[32;1mDEBUG: /proc/PID/maps успешно открыт.\n");
	while (!feof(fp))
	{
		fgets(buff, sizeof(buff), fp);
		if (strstr(buff, flags) != NULL && !feof(fp) && strstr(buff, module))
		{
			sscanf(buff, "%lx-%lx", &pNew->addr, &pNew->taddr);
			flag = 1;
		}
		else
		{
			flag = 0;
		}
		if (flag == 1)
		{
			pNew->next = NULL;
			pEnd = pNew;
			pHead = pNew;
			pNew = (PMAPS) malloc(MapSize);
		}
	}
	free(pNew);
	fclose(fp);
	return pHead->addr;
}
int getPID(PACKAGENAME * PackageName)
{
	DIR *dir = NULL;
	struct dirent *ptr = NULL;
	FILE *fp = NULL;
	char filepath[256];
	char filetext[128] = {0}; 
    int found_pid = 0; 
	dir = opendir("/proc");
	if (NULL == dir)
	{
        perror("ERROR: Не удалось открыть /proc");
		return 0;
	}
	while ((ptr = readdir(dir)) != NULL)
	{
		if ((strcmp(ptr->d_name, ".") == 0) || (strcmp(ptr->d_name, "..") == 0))
			continue;
		if (ptr->d_type != DT_DIR)
			continue;         
        memset(filepath, 0, sizeof(filepath));
        memset(filetext, 0, sizeof(filetext));
		sprintf(filepath, "/proc/%s/cmdline", ptr->d_name);
		fp = fopen(filepath, "r");
		if (NULL != fp)
		{
			if (fgets(filetext, sizeof(filetext), fp) != NULL)
            {
				if (strcmp(filetext, PackageName) == 0)
				{
                    found_pid = atoi(ptr->d_name);
					fclose(fp);
					break; 
				}
            }
			fclose(fp);
		}
	}
	closedir(dir);
	return found_pid;
}
void PrintResults()
{
	PMAPS temp = Res;
	const char *type = (char *)malloc(sizeof(char));
	for (int i = 0; i < ResCount; i++)
	{
		switch (temp->type)
		{
		case TYPE_DWORD:
			type = "DWORD";
			break;
		case TYPE_FLOAT:
			type = "FLOAT";
			break;
		case TYPE_DOUBLE:
			type = "DOUBLE";
			break;
		case TYPE_WORD:
			type = "WORD";
			break;
		case TYPE_BYTE:
			type = "BYTE";
			break;
		case TYPE_QWORD:
			type = "QWORD";
			break;
		default:
			type = "";
			break;
		}
		printf("\e[37;1mAddr:\e[32;1m0x%lX  \e[37;1mType:\e[36;1m%s\n", temp->addr, type);
		temp = temp->next;
	}
}
void ClearResults()
{
	PMAPS pHead = Res;
	PMAPS pTemp = pHead;
	int i;
	for (i = 0; i < ResCount; i++)
	{
		pTemp = pHead;
		pHead = pHead->next;
		free(pTemp);
	}
}
void BaseAddressSearch(char *value, TYPE type, ADDRESS BaseAddr)
{
	PMAPS pHead = NULL;
	PMAPS pMap = NULL;
	gs = 0;
	switch (MemorySearchRange)
	{
	case ALL:
		pMap = readmaps(ALL);
		break;
	case B_BAD:
		pMap = readmaps(B_BAD);
		break;
	case V:
		pMap = readmaps(V);
		break;
	case C_ALLOC:
		pMap = readmaps(C_ALLOC);
		break;
	case C_BSS:
		pMap = readmaps(C_BSS);
		break;
	case CODE_APP:
		pMap = readmaps(CODE_APP);
		break;
	case C_DATA:
		pMap = readmaps(C_DATA);
		break;
	case C_HEAP:
		pMap = readmaps(C_HEAP);
		break;
	case JAVA_HEAP:
		pMap = readmaps(JAVA_HEAP);
		break;
	case A_ANONMYOUS:
		pMap = readmaps(A_ANONMYOUS);
		break;
	case CODE_SYSTEM:
		pMap = readmaps(CODE_SYSTEM);
		break;
	case STACK:
		pMap = readmaps(STACK);
		break;
	case ASHMEM:
		pMap = readmaps(ASHMEM);
		break;
	default:
		printf("\033[32;1mYou Select A NULL Type!\n");
		break;
	}
	if (pMap == NULL)
	{
		puts("map error");
		return;
	}
	switch (type)
	{
	case TYPE_DWORD:
		pHead = BaseAddressSearch_DWORD(atoi(value), BaseAddr, pMap);
		break;
	case TYPE_FLOAT:
		pHead = BaseAddressSearch_FLOAT(atof(value), BaseAddr, pMap);
		break;
	case TYPE_DOUBLE:
		pHead = BaseAddressSearch_DOUBLE(atof(value), BaseAddr, pMap);
		break;
	case TYPE_WORD:
		pHead = BaseAddressSearch_WORD(atoi(value), BaseAddr, pMap);
		break;
	case TYPE_BYTE:
		pHead = BaseAddressSearch_BYTE(atoi(value), BaseAddr, pMap);
		break;
	case TYPE_QWORD:
		pHead = BaseAddressSearch_QWORD(atoi(value), BaseAddr, pMap);
		break;
	default:
		printf("\033[32;1mYou Select A NULL Type!\n");
		break;
	}
	if (pHead == NULL)
	{
		puts("搜索内存失败!");
		return (void)0;
	}
	ResCount = gs;
	Res = pHead;
}
PMAPS BaseAddressSearch_DWORD(DWORD value, ADDRESS BaseAddr, PMAPS pMap)
{
	PMAPS e, n;
	e = n = (PMAPS) malloc(LEN);
	PMAPS pBuff = n;
	int iCount = 0;
	long int c, ADDR;
	void *BUF[8];
	PMAPS pTemp = pMap;
	while (pTemp != NULL)
	{
		c = (pTemp->taddr - pTemp->addr) / 4096;
		for (int j = 0; j < c; j++)
		{
			ADDR = pTemp->addr + j * 4096 + BaseAddr;
			pread64(handle, BUF, 8, ADDR);
			if (*(DWORD *) & BUF[0] == value)
			{
				iCount++;
				gs += 1;
				n->addr = ADDR;
				n->type = TYPE_DWORD;
				if (iCount == 1)
				{
					n->next = NULL;
					e = n;
					pBuff = n;
				}
				else
				{
					n->next = NULL;
					e->next = n;
					e = n;
				}
				n = (PMAPS) malloc(LEN);
			}
		}
		pTemp = pTemp->next;
	}
	return pBuff;
}
PMAPS BaseAddressSearch_DOUBLE(DOUBLE value, ADDRESS BaseAddr, PMAPS pMap)
{
	PMAPS e, n;
	e = n = (PMAPS) malloc(LEN);
	PMAPS pBuff = n;
	int iCount = 0;
	long int c, ADDR;
	void *BUF[8];
	PMAPS pTemp = pMap;
	while (pTemp != NULL)
	{
		c = (pTemp->taddr - pTemp->addr) / 4096;
		for (int j = 0; j < c; j++)
		{
			ADDR = pTemp->addr + j * 4096 + BaseAddr;
			pread64(handle, BUF, 8, ADDR);
			if (*(DOUBLE *) & BUF[0] == value)
			{
				iCount++;
				gs += 1;
				n->addr = ADDR;
				n->type = TYPE_DOUBLE;
				if (iCount == 1)
				{
					n->next = NULL;
					e = n;
					pBuff = n;
				}
				else
				{
					n->next = NULL;
					e->next = n;
					e = n;
				}
				n = (PMAPS) malloc(LEN);
			}
		}
		pTemp = pTemp->next;
	}
	return pBuff;
}
PMAPS BaseAddressSearch_FLOAT(FLOAT value, ADDRESS BaseAddr, PMAPS pMap)
{
	PMAPS e, n;
	e = n = (PMAPS) malloc(LEN);
	PMAPS pBuff = n;
	long int c, ADDR;
	int iCount = 0;
	void *BUF[8];
	PMAPS pTemp = pMap;
	while (pTemp != NULL)
	{
		c = (pTemp->taddr - pTemp->addr) / 4096;
		for (int j = 0; j < c; j++)
		{
			ADDR = pTemp->addr + j * 4096 + BaseAddr;
			pread64(handle, BUF, 8, ADDR);
			if (*(FLOAT *) & BUF[0] == value)
			{
				iCount++;
				gs += 1;
				n->addr = ADDR;
				n->type = TYPE_FLOAT;
				if (iCount == 1)
				{
					n->next = NULL;
					e = n;
					pBuff = n;
				}
				else
				{
					n->next = NULL;
					e->next = n;
					e = n;
				}
				n = (PMAPS) malloc(LEN);
			}
		}
		pTemp = pTemp->next;
	}
	return pBuff;
}
PMAPS BaseAddressSearch_WORD(WORD value, ADDRESS BaseAddr, PMAPS pMap)
{
	PMAPS e, n;
	e = n = (PMAPS) malloc(LEN);
	PMAPS pBuff = n;
	long int c, ADDR;
	int iCount = 0;
	void *BUF[8];
	PMAPS pTemp = pMap;
	while (pTemp != NULL)
	{
		c = (pTemp->taddr - pTemp->addr) / 4096;
		for (int j = 0; j < c; j++)
		{
			ADDR = pTemp->addr + j * 4096 + BaseAddr;
			pread64(handle, BUF, 8, ADDR);
			if (*(WORD *) & BUF[0] == value)
			{
				iCount++;
				gs += 1;
				n->addr = ADDR;
				n->type = TYPE_WORD;
				if (iCount == 1)
				{
					n->next = NULL;
					e = n;
					pBuff = n;
				}
				else
				{
					n->next = NULL;
					e->next = n;
					e = n;
				}
				n = (PMAPS) malloc(LEN);
			}
		}
		pTemp = pTemp->next;
	}
	return pBuff;
}
PMAPS BaseAddressSearch_QWORD(QWORD value, ADDRESS BaseAddr, PMAPS pMap)
{
	PMAPS e, n;
	e = n = (PMAPS) malloc(LEN);
	PMAPS pBuff = n;
	long int c, ADDR;
	int iCount = 0;
	void *BUF[8];
	PMAPS pTemp = pMap;
	while (pTemp != NULL)
	{
		c = (pTemp->taddr - pTemp->addr) / 4096;
		for (int j = 0; j < c; j++)
		{
			ADDR = pTemp->addr + j * 4096 + BaseAddr;
			pread64(handle, BUF, 8, ADDR);
			if (*(QWORD *) & BUF[0] == value)
			{
				iCount++;
				gs += 1;
				n->addr = ADDR;
				n->type = TYPE_QWORD;
				if (iCount == 1)
				{
					n->next = NULL;
					e = n;
					pBuff = n;
				}
				else
				{
					n->next = NULL;
					e->next = n;
					e = n;
				}
				n = (PMAPS) malloc(LEN);
			}
		}
		pTemp = pTemp->next;
	}
	return pBuff;
}
PMAPS BaseAddressSearch_BYTE(BYTE value, ADDRESS BaseAddr, PMAPS pMap)
{
	PMAPS e, n;
	e = n = (PMAPS) malloc(LEN);
	PMAPS pBuff = n;
	long int c, ADDR;
	int iCount = 0;
	void *BUF[8];
	PMAPS pTemp = pMap;
	while (pTemp != NULL)
	{
		c = (pTemp->taddr - pTemp->addr) / 4096;
		for (int j = 0; j < c; j++)
		{
			ADDR = pTemp->addr + j * 4096 + BaseAddr;
			pread64(handle, BUF, 8, ADDR);
			if (*(BYTE *) & BUF[0] == value)
			{
				iCount++;
				gs += 1;
				n->addr = ADDR;
				n->type = TYPE_BYTE;
				if (iCount == 1)
				{
					n->next = NULL;
					e = n;
					pBuff = n;
				}
				else
				{
					n->next = NULL;
					e->next = n;
					e = n;
				}
				n = (PMAPS) malloc(LEN);
			}
		}
		pTemp = pTemp->next;
	}
	return pBuff;
}
void RangeMemorySearch(char *from_value, char *to_value, TYPE type)
{
	PMAPS pHead = NULL;
	PMAPS pMap = NULL;
	gs = 0;
	switch (MemorySearchRange)
	{
	case ALL:
		pMap = readmaps(ALL);
		break;
	case B_BAD:
		pMap = readmaps(B_BAD);
		break;
	case V:
		pMap = readmaps(V);
		break;
	case C_ALLOC:
		pMap = readmaps(C_ALLOC);
		break;
	case C_BSS:
		pMap = readmaps(C_BSS);
		break;
	case CODE_APP:
		pMap = readmaps(CODE_APP);
		break;
	case C_DATA:
		pMap = readmaps(C_DATA);
		break;
	case C_HEAP:
		pMap = readmaps(C_HEAP);
		break;
	case JAVA_HEAP:
		pMap = readmaps(JAVA_HEAP);
		break;
	case A_ANONMYOUS:
		pMap = readmaps(A_ANONMYOUS);
		break;
	case CODE_SYSTEM:
		pMap = readmaps(CODE_SYSTEM);
		break;
	case STACK:
		pMap = readmaps(STACK);
		break;
	case ASHMEM:
		pMap = readmaps(ASHMEM);
		break;
	default:
		printf("\033[32;1mYou Select A NULL Type!\n");
		break;
	}
	if (pMap == NULL)
	{
		puts("map error");
		return;
	}
	switch (type)
	{
	case TYPE_DWORD:
		if (atoi(from_value) > atoi(to_value))
			pHead = RangeMemorySearch_DWORD(atoi(to_value), atoi(from_value), pMap);
		else
			pHead = RangeMemorySearch_DWORD(atoi(from_value), atoi(to_value), pMap);
		break;
	case TYPE_FLOAT:
		if (atof(from_value) > atof(to_value))
			pHead = RangeMemorySearch_FLOAT(atof(to_value), atof(from_value), pMap);
		else
			pHead = RangeMemorySearch_FLOAT(atof(from_value), atof(to_value), pMap);
		break;
	case TYPE_DOUBLE:
		if (atof(from_value) > atof(to_value))
			pHead = RangeMemorySearch_DOUBLE(atof(to_value), atof(from_value), pMap);
		else
			pHead = RangeMemorySearch_DOUBLE(atof(from_value), atof(to_value), pMap);
		break;
	case TYPE_WORD:
		if (atoi(from_value) > atoi(to_value))
			pHead = RangeMemorySearch_WORD(atoi(to_value), atoi(from_value), pMap);
		else
			pHead = RangeMemorySearch_WORD(atoi(from_value), atoi(to_value), pMap);
		break;
	case TYPE_BYTE:
		if (atoi(from_value) > atoi(to_value))
			pHead = RangeMemorySearch_BYTE(atoi(to_value), atoi(from_value), pMap);
		else
			pHead = RangeMemorySearch_BYTE(atoi(from_value), atoi(to_value), pMap);
		break;
	case TYPE_QWORD:
		if (atoi(from_value) > atoi(to_value))
			pHead = RangeMemorySearch_QWORD(atoi(to_value), atoi(from_value), pMap);
		else
			pHead = RangeMemorySearch_QWORD(atoi(from_value), atoi(to_value), pMap);
		break;
	default:
		printf("\033[32;1mYou Select A NULL Type!\n");
		break;
	}
	if (pHead == NULL)
	{
		puts("RANGE搜索内存失败!");
		return;
	}
	ResCount = gs;
	Res = pHead;
}
PMAPS RangeMemorySearch_DWORD(DWORD from_value, DWORD to_value, PMAPS pMap)
{
	pid_t pid = getPID(bm);
	if (pid == 0)
	{
		puts("获取进程失败!");
		return NULL;
	}
	PMAPS pTemp = NULL;
	pTemp = pMap;
	PMAPS n, e;
	e = n = (PMAPS) malloc(LEN);
	PMAPS pBuff;
	pBuff = n;
	int iCount = 0;
	int c;
	DWORD buff[1024] = { 0 };
	memset(buff, 0, 4);
	while (pTemp != NULL)
	{
		c = (pTemp->taddr - pTemp->addr) / 4096;
		for (int j = 0; j < c; j++)
		{
			pread64(handle, buff, 0x1000, pTemp->addr + j * 4096);
			for (int i = 0; i < 1024; i++)
			{
				if (buff[i] >= from_value && buff[i] <= to_value)
				{
					iCount++;
					gs += 1;
					n->addr = (pTemp->addr) + (j * 4096) + (i * 4);
					n->type = TYPE_DWORD;
					if (iCount == 1)
					{
						n->next = NULL;
						e = n;
						pBuff = n;
					}
					else
					{
						n->next = NULL;
						e->next = n;
						e = n;
					}
					n = (PMAPS) malloc(LEN);
				}
			}
		}
		pTemp = pTemp->next;
	}
	free(n);
	return pBuff;
}
PMAPS RangeMemorySearch_FLOAT(FLOAT from_value, FLOAT to_value, PMAPS pMap)
{
	pid_t pid = getPID(bm);
	if (pid == 0)
	{
		puts("获取进程失败!");
		return NULL;
	}
	PMAPS pTemp = NULL;
	pTemp = pMap;
	PMAPS n, e;
	e = n = (PMAPS) malloc(LEN);
	PMAPS pBuff;
	pBuff = n;
	int iCount = 0;
	int c;
	FLOAT buff[1024] = { 0 };
	while (pTemp->next != NULL)
	{
		c = (pTemp->taddr - pTemp->addr) / 4096;
		for (int j = 0; j < c; j += 1)
		{
			pread64(handle, buff, 0x1000, pTemp->addr + (j * 4096));
			for (int i = 0; i < 1024; i += 1)
			{
				if (buff[i] >= from_value && buff[i] <= to_value)
				{				
					iCount++;
					gs += 1;
					n->addr = (pTemp->addr) + (j * 4096) + (i * 4);
					n->type = TYPE_FLOAT;
					if (iCount == 1)
					{
						n->next = NULL;
						e = n;
						pBuff = n;
					}
					else
					{
						n->next = NULL;
						e->next = n;
						e = n;
					}
					n = (PMAPS) malloc(LEN);
				}
			}
		}
		pTemp = pTemp->next;
	}
	free(n);
	return pBuff;
}
PMAPS RangeMemorySearch_DOUBLE(DOUBLE from_value, DOUBLE to_value, PMAPS pMap)
{
	pid_t pid = getPID(bm);
	if (pid == 0)
	{
		puts("获取进程失败!");
		return NULL;
	}
	PMAPS pTemp = NULL;
	pTemp = pMap;
	PMAPS n, e;
	e = n = (PMAPS) malloc(LEN);
	PMAPS pBuff;
	pBuff = n;
	int iCount = 0;
	int c;
	DOUBLE buff[1024] = { 0 };
	while (pTemp->next != NULL)
	{
		c = (pTemp->taddr - pTemp->addr) / 4096;
		for (int j = 0; j < c; j += 1)
		{
			pread64(handle, buff, 0x1000, pTemp->addr + (j * 4096));
			for (int i = 0; i < 1024; i += 1)
			{
				if (buff[i] >= from_value && buff[i] <= to_value)
				{				
					iCount++;
					gs += 1;
					n->addr = (pTemp->addr) + (j * 4096) + (i * 4);
					n->type = TYPE_DOUBLE;
					if (iCount == 1)
					{
						n->next = NULL;
						e = n;
						pBuff = n;
					}
					else
					{
						n->next = NULL;
						e->next = n;
						e = n;
					}
					n = (PMAPS) malloc(LEN);
				}
			}
		}
		pTemp = pTemp->next;
	}
	free(n);
	return pBuff;
}
PMAPS RangeMemorySearch_WORD(WORD from_value, WORD to_value, PMAPS pMap)
{
	pid_t pid = getPID(bm);
	if (pid == 0)
	{
		puts("获取进程失败!");
		return NULL;
	}
	PMAPS pTemp = NULL;
	pTemp = pMap;
	PMAPS n, e;
	e = n = (PMAPS) malloc(LEN);
	PMAPS pBuff;
	pBuff = n;
	int iCount = 0;
	int c;
	WORD buff[1024] = { 0 };
	while (pTemp->next != NULL)
	{
		c = (pTemp->taddr - pTemp->addr) / 4096;
		for (int j = 0; j < c; j += 1)
		{
			pread64(handle, buff, 0x800, pTemp->addr + (j * 4096));
			for (int i = 0; i < 1024; i += 1)
			{
				if (buff[i] >= from_value && buff[i] <= to_value)
				{				
					iCount++;
					gs += 1;
					n->addr = (pTemp->addr) + (j * 4096) + (i * 4);
					n->type = TYPE_WORD;
					if (iCount == 1)
					{
						n->next = NULL;
						e = n;
						pBuff = n;
					}
					else
					{
						n->next = NULL;
						e->next = n;
						e = n;
					}
					n = (PMAPS) malloc(LEN);
				}
			}
		}
		pTemp = pTemp->next;
	}
	free(n);
	return pBuff;
}
PMAPS RangeMemorySearch_BYTE(BYTE from_value, BYTE to_value, PMAPS pMap)
{
	pid_t pid = getPID(bm);
	if (pid == 0)
	{
		puts("获取进程失败!");
		return NULL;
	}
	PMAPS pTemp = NULL;
	pTemp = pMap;
	PMAPS n, e;
	e = n = (PMAPS) malloc(LEN);
	PMAPS pBuff;
	pBuff = n;
	int iCount = 0;
	int c;
	BYTE buff[1024] = { 0 };
	while (pTemp->next != NULL)
	{
		c = (pTemp->taddr - pTemp->addr) / 4096;
		for (int j = 0; j < c; j += 1)
		{
			pread64(handle, buff, 0x400, pTemp->addr + (j * 4096));
			for (int i = 0; i < 1024; i += 1)
			{
				if (buff[i] >= from_value && buff[i] <= to_value)
				{
					iCount++;
					gs += 1;
					n->addr = (pTemp->addr) + (j * 4096) + (i * 4);
					n->type = TYPE_BYTE;
					if (iCount == 1)
					{
						n->next = NULL;
						e = n;
						pBuff = n;
					}
					else
					{
						n->next = NULL;
						e->next = n;
						e = n;
					}
					n = (PMAPS) malloc(LEN);
				}
			}
		}
		pTemp = pTemp->next;
	}
	free(n);
	return pBuff;
}
PMAPS RangeMemorySearch_QWORD(QWORD from_value, QWORD to_value, PMAPS pMap)
{
	pid_t pid = getPID(bm);
	if (pid == 0)
	{
		puts("获取进程失败!");
		return NULL;
	}
	PMAPS pTemp = NULL;
	pTemp = pMap;
	PMAPS n, e;
	e = n = (PMAPS) malloc(LEN);
	PMAPS pBuff;
	pBuff = n;
	int iCount = 0;
	int c;
	QWORD buff[1024] = { 0 };
	while (pTemp->next != NULL)
	{
		c = (pTemp->taddr - pTemp->addr) / 4096;
		for (int j = 0; j < c; j += 1)
		{
			pread64(handle, buff, 0x1000, pTemp->addr + (j * 4096));
			for (int i = 0; i < 1024; i += 1)
			{
				if (buff[i] >= from_value && buff[i] <= to_value)
				{				
					iCount++;
					gs += 1;
					n->addr = (pTemp->addr) + (j * 4096) + (i * 4);
					n->type = TYPE_QWORD;
					if (iCount == 1)
					{
						n->next = NULL;
						e = n;
						pBuff = n;
					}
					else
					{
						n->next = NULL;
						e->next = n;
						e = n;
					}
					n = (PMAPS) malloc(LEN);
				}
			}
		}
		pTemp = pTemp->next;
	}
	free(n);
	return pBuff;
}
void MemorySearch(char *value, TYPE type)
{
if (strchr(value, '~')) {
        char from_val[64], to_val[64];
        sscanf(value, "%63[^~]~%63s", from_val, to_val);
        RangeMemorySearch(from_val, to_val, type);
        return;
    }
	PMAPS pHead = NULL;
	PMAPS pMap = NULL;
	gs = 0;
	switch (MemorySearchRange)
	{
	case ALL:
		pMap = readmaps(ALL);
		break;
	case B_BAD:
		pMap = readmaps(B_BAD);
		break;
	case V:
		pMap = readmaps(V);
		break;
	case C_ALLOC:
		pMap = readmaps(C_ALLOC);
		break;
	case C_BSS:
		pMap = readmaps(C_BSS);
		break;
	case CODE_APP:
		pMap = readmaps(CODE_APP);
		break;
	case C_DATA:
		pMap = readmaps(C_DATA);
		break;
	case C_HEAP:
		pMap = readmaps(C_HEAP);
		break;
	case JAVA_HEAP:
		pMap = readmaps(JAVA_HEAP);
		break;
	case A_ANONMYOUS:
		pMap = readmaps(A_ANONMYOUS);
		break;
	case CODE_SYSTEM:
		pMap = readmaps(CODE_SYSTEM);
		break;
	case STACK:
		pMap = readmaps(STACK);
		break;
	case ASHMEM:
		pMap = readmaps(ASHMEM);
		break;
	default:
		printf("\033[32;1mYou Select A NULL Type!\n");
		break;
	}
	if (pMap == NULL)
	{
		puts("map error");
		return (void)0;
	}
	switch (type)
	{
	case TYPE_DWORD:
		pHead = MemorySearch_DWORD(atoi(value), pMap);
		break;
	case TYPE_FLOAT:
		pHead = MemorySearch_FLOAT(atof(value), pMap);
		break;
	case TYPE_DOUBLE:
		pHead = MemorySearch_DOUBLE(atof(value), pMap);
		break;
	case TYPE_WORD:
		pHead = MemorySearch_WORD(atoi(value), pMap);
		break;
	case TYPE_BYTE:
		pHead = MemorySearch_BYTE(atoi(value), pMap);
		break;
	case TYPE_QWORD:
		pHead = MemorySearch_QWORD(atoi(value), pMap);
		break;
	default:
		printf("\033[32;1mYou Select A NULL Type!\n");
		break;
	}
	if (pHead == NULL)
	{
		puts("搜索内存失败!");
		return;
	}
	ResCount = gs;
	Res = pHead;
}
PMAPS MemorySearch_DWORD(DWORD value, PMAPS pMap)
{
	pid_t pid = getPID(bm);
	if (pid == 0)
	{
		puts("获取进程失败!");
		return NULL;
	}
	PMAPS pTemp = NULL;
	pTemp = pMap;
	PMAPS n, e;
	e = n = (PMAPS) malloc(LEN);
	PMAPS pBuff;
	pBuff = n;
	int iCount = 0;
	int c;
	DWORD buff[1024] = { 0 };
	while (pTemp != NULL)
	{
		c = (pTemp->taddr - pTemp->addr) / 4096;
		for (int j = 0; j < c; j++)
		{
			pread64(handle, buff, 0x1000, pTemp->addr + j * 4096);
			for (int i = 0; i < 1024; i++)
			{
				if (buff[i] == value)
				{
					iCount++;
					gs += 1;
					n->addr = (pTemp->addr) + (j * 4096) + (i * 4);
					n->type = TYPE_DWORD;
					if (iCount == 1)
					{
						n->next = NULL;
						e = n;
						pBuff = n;
					}
					else
					{
						n->next = NULL;
						e->next = n;
						e = n;
					}
					n = (PMAPS) malloc(LEN);
				}
			}
		}
		pTemp = pTemp->next;
	}
	free(n);
	return pBuff;
}
PMAPS MemorySearch_FLOAT(FLOAT value, PMAPS pMap)
{
	pid_t pid = getPID(bm);
	if (pid == 0)
	{
		puts("获取进程失败!");
		return NULL;
	}
	PMAPS pTemp = NULL;
	pTemp = pMap;
	PMAPS n, e;
	e = n = (PMAPS) malloc(LEN);
	PMAPS pBuff;
	pBuff = n;
	int iCount = 0;
	int c;
	FLOAT buff[1024] = { 0 };
	while (pTemp->next != NULL)
	{
		c = (pTemp->taddr - pTemp->addr) / 4096;
		for (int j = 0; j < c; j += 1)
		{
			pread64(handle, buff, 0x1000, pTemp->addr + (j * 4096));
			for (int i = 0; i < 1024; i += 1)
			{
				if (buff[i] == value)
				{
					iCount++;
					gs += 1;
					n->addr = (pTemp->addr) + (j * 4096) + (i * 4);
					n->type = TYPE_FLOAT;
					if (iCount == 1)
					{
						n->next = NULL;
						e = n;
						pBuff = n;
					}
					else
					{
						n->next = NULL;
						e->next = n;
						e = n;
					}
					n = (PMAPS) malloc(LEN);
				}
			}
		}
		pTemp = pTemp->next;
	}
	free(n);
	return pBuff;
}
PMAPS MemorySearch_DOUBLE(DOUBLE value, PMAPS pMap)
{
	pid_t pid = getPID(bm);
	if (pid == 0)
	{
		puts("获取进程失败!");
		return NULL;
	}
	PMAPS pTemp = NULL;
	pTemp = pMap;
	PMAPS n, e;
	e = n = (PMAPS) malloc(LEN);
	PMAPS pBuff;
	pBuff = n;
	int iCount = 0;
	int c;
	DOUBLE buff[1024] = { 0 };
	while (pTemp->next != NULL)
	{
		c = (pTemp->taddr - pTemp->addr) / 4096;
		for (int j = 0; j < c; j += 1)
		{
			pread64(handle, buff, 0x1000, pTemp->addr + (j * 4096));
			for (int i = 0; i < 1024; i += 1)
			{
				if (buff[i] == value)
				{
					iCount++;
					gs += 1;
					n->addr = (pTemp->addr) + (j * 4096) + (i * 4);
					n->type = TYPE_DOUBLE;
					if (iCount == 1)
					{
						n->next = NULL;
						e = n;
						pBuff = n;
					}
					else
					{
						n->next = NULL;
						e->next = n;
						e = n;
					}
					n = (PMAPS) malloc(LEN);
				}
			}
		}
		pTemp = pTemp->next;
	}
	free(n);
	return pBuff;
}
PMAPS MemorySearch_WORD(WORD value, PMAPS pMap)
{
	pid_t pid = getPID(bm);
	if (pid == 0)
	{
		puts("获取进程失败!");
		return NULL;
	}
	PMAPS pTemp = NULL;
	pTemp = pMap;
	PMAPS n, e;
	e = n = (PMAPS) malloc(LEN);
	PMAPS pBuff;
	pBuff = n;
	int iCount = 0;
	int c;
	WORD buff[1024] = { 0 };
	while (pTemp->next != NULL)
	{
		c = (pTemp->taddr - pTemp->addr) / 4096;
		for (int j = 0; j < c; j += 1)
		{
			pread64(handle, buff, 0x800, pTemp->addr + (j * 4096));
			n->type = TYPE_WORD;
			for (int i = 0; i < 1024; i += 1)
			{
				if (buff[i] == value)
				{
					iCount++;
					gs += 1;
					n->addr = (pTemp->addr) + (j * 4096) + (i * 4);
					if (iCount == 1)
					{
						n->next = NULL;
						e = n;
						pBuff = n;
					}
					else
					{
						n->next = NULL;
						e->next = n;
						e = n;
					}
					n = (PMAPS) malloc(LEN);
				}
			}
		}
		pTemp = pTemp->next;
	}
	free(n);
	return pBuff;
}
PMAPS MemorySearch_BYTE(BYTE value, PMAPS pMap)
{
	pid_t pid = getPID(bm);
	if (pid == 0)
	{
		puts("获取进程失败!");
		return NULL;
	}
	PMAPS pTemp = NULL;
	pTemp = pMap;
	PMAPS n, e;
	e = n = (PMAPS) malloc(LEN);
	PMAPS pBuff;
	pBuff = n;
	int iCount = 0;
	int c;
	BYTE buff[1024] = { 0 };
	while (pTemp->next != NULL)
	{
		c = (pTemp->taddr - pTemp->addr) / 4096;
		for (int j = 0; j < c; j += 1)
		{
			pread64(handle, buff, 0x400, pTemp->addr + (j * 4096));
			for (int i = 0; i < 1024; i += 1)
			{
				if (buff[i] == value)
				{
					iCount++;
					gs += 1;
					n->addr = (pTemp->addr) + (j * 4096) + (i * 4);
					n->type = TYPE_BYTE;
					if (iCount == 1)
					{
						n->next = NULL;
						e = n;
						pBuff = n;
					}
					else
					{
						n->next = NULL;
						e->next = n;
						e = n;
					}
					n = (PMAPS) malloc(LEN);
				}
			}
		}
		pTemp = pTemp->next;
	}
	free(n);
	return pBuff;
}
PMAPS MemorySearch_QWORD(QWORD value, PMAPS pMap)
{
	pid_t pid = getPID(bm);
	if (pid == 0)
	{
		puts("获取进程失败!");
		return NULL;
	}
	PMAPS pTemp = NULL;
	pTemp = pMap;
	PMAPS n, e;
	e = n = (PMAPS) malloc(LEN);
	PMAPS pBuff;
	pBuff = n;
	int iCount = 0;
	int c;
	QWORD buff[1024] = { 0 };
	while (pTemp->next != NULL)
	{
		c = (pTemp->taddr - pTemp->addr) / 4096;
		for (int j = 0; j < c; j += 1)
		{
			pread64(handle, buff, 0x1000, pTemp->addr + (j * 4096));
			for (int i = 0; i < 1024; i += 1)
			{
				if (buff[i] == value)
				{
					iCount++;
					gs += 1;
					n->addr = (pTemp->addr) + (j * 4096) + (i * 4);
					n->type = TYPE_QWORD;
					if (iCount == 1)
					{
						n->next = NULL;
						e = n;
						pBuff = n;
					}
					else
					{
						n->next = NULL;
						e->next = n;
						e = n;
					}
					n = (PMAPS) malloc(LEN);
				}
			}
		}
		pTemp = pTemp->next;
	}
	free(n);
	return pBuff;
}
void MemoryOffset(char *value, OFFSET offset, TYPE type)
{
if (strchr(value, '~')) {
        char from_val[64], to_val[64];
        sscanf(value, "%63[^~]~%63s", from_val, to_val);
        RangeMemoryOffset(from_val, to_val, offset, type);
        return;
    }
	PMAPS pHead = NULL;
	gs = 0;
	switch (type)
	{
	case TYPE_DWORD:
		pHead = MemoryOffset_DWORD(atoi(value), offset, Res);
		break;
	case TYPE_FLOAT:
		pHead = MemoryOffset_FLOAT(atof(value), offset, Res);
		break;
	case TYPE_DOUBLE:
		pHead = MemoryOffset_DOUBLE(atof(value), offset, Res);
		break;
	case TYPE_WORD:
		pHead = MemoryOffset_WORD(atoi(value), offset, Res);
		break;
	case TYPE_BYTE:
		pHead = MemoryOffset_BYTE(atoi(value), offset, Res);
		break;
	case TYPE_QWORD:
		pHead = MemoryOffset_QWORD(atoi(value), offset, Res);
		break;
	default:
		printf("\033[32;1mYou Select A NULL Type!\n");
		break;
	}
	if (pHead == NULL)
	{
		puts("偏移失败!");
		return (void)0;
	}
	ResCount = gs;
	ClearResults();
	Res = pHead;
}
void MemoryOffset(char *value, const char *offset_str, int type)
{
    long int offset_start = 0, offset_end = 0;
    if (strchr(offset_str, '~')) {
        sscanf(offset_str, "%ld~%ld", &offset_start, &offset_end);
    } else {
        offset_start = offset_end = atol(offset_str);
    }
    PMAPS pHead = NULL;
    gs = 0;
    switch (type)
    {
    case TYPE_DWORD:
        pHead = MemoryOffsetRange_DWORD(atoi(value), offset_start, offset_end, Res);
        break;
    case TYPE_FLOAT:
        pHead = MemoryOffsetRange_FLOAT(atof(value), offset_start, offset_end, Res);
        break;
    case TYPE_DOUBLE:
        pHead = MemoryOffsetRange_DOUBLE(atof(value), offset_start, offset_end, Res);
        break;
    case TYPE_WORD:
        pHead = MemoryOffsetRange_WORD(atoi(value), offset_start, offset_end, Res);
        break;
    case TYPE_BYTE:
        pHead = MemoryOffsetRange_BYTE(atoi(value), offset_start, offset_end, Res);
        break;
    case TYPE_QWORD:
        pHead = MemoryOffsetRange_QWORD(atoll(value), offset_start, offset_end, Res);
        break;
    default:
        printf("\033[32;1mYou Select A NULL Type!\n");
        break;
    }
    if (pHead == NULL)
    {
        puts("Смещение не найдено!");
        return;
    }
    ResCount = gs;
    ClearResults();
    Res = pHead;
}
PMAPS MemoryOffset_DWORD(DWORD value, OFFSET offset, PMAPS pBuff)
{
	PMAPS pEnd = NULL;
	PMAPS pNew = NULL;
	PMAPS pTemp = pBuff;
	PMAPS BUFF = NULL;
	pEnd = pNew = (PMAPS) malloc(LEN);
	BUFF = pNew;
	int iCount = 0;
	long int all;
	DWORD *buf = (DWORD *) malloc(sizeof(DWORD));
	int jg;
	while (pTemp != NULL)
	{
		all = pTemp->addr + offset;
		pread64(handle, buf, 4, all);
		jg = *buf;
		if (jg == value)
		{
			iCount++;
			gs += 1;
			pNew->addr = pTemp->addr;
			pNew->type = TYPE_DWORD;
			if (iCount == 1)
			{
				pNew->next = NULL;
				pEnd = pNew;
				BUFF = pNew;
			}
			else
			{
				pNew->next = NULL;
				pEnd->next = pNew;
				pEnd = pNew;
			}
			pNew = (PMAPS) malloc(LEN);
			if (ResCount == 1)
			{
				free(pNew);
				close(handle);
				return BUFF;
			}
		}
		pTemp = pTemp->next;
	}
	free(pNew);
	return BUFF;
}
PMAPS MemoryOffsetRange_DWORD(DWORD value, long int offset_start, long int offset_end, PMAPS pBuff)
{
    PMAPS pEnd = NULL, pNew = NULL, pTemp = pBuff, BUFF = NULL;
    pEnd = pNew = (PMAPS) malloc(LEN);
    BUFF = pNew;
    int iCount = 0;
    DWORD buf;
    while (pTemp != NULL)
    {
        bool found = false;
        for (long int off = offset_start; off <= offset_end; off++)
        {
            pread64(handle, &buf, sizeof(DWORD), pTemp->addr + off);
            if (buf == value)
            {
                found = true;
                break; 
            }
        }
        if (found)
        {
            iCount++;
            gs += 1;
            pNew->addr = pTemp->addr;
            pNew->type = TYPE_DWORD;
            if (iCount == 1) {
                pNew->next = NULL;
                pEnd = pNew;
                BUFF = pNew;
            } else {
                pNew->next = NULL;
                pEnd->next = pNew;
                pEnd = pNew;
            }
            pNew = (PMAPS) malloc(LEN);           
            if (ResCount == 1) {
                free(pNew);
                return BUFF;
            }
        }
        pTemp = pTemp->next;
    }
    free(pNew);
    return BUFF;
}
PMAPS MemoryOffset_FLOAT(FLOAT value, OFFSET offset, PMAPS pBuff)
{
	PMAPS pEnd = NULL;
	PMAPS pNew = NULL;
	PMAPS pTemp = pBuff;
	PMAPS BUFF = NULL;
	pEnd = pNew = (PMAPS) malloc(LEN);
	BUFF = pNew;
	int iCount = 0;
	long int all;
	FLOAT *buf = (FLOAT *) malloc(sizeof(FLOAT));
	FLOAT jg;
	while (pTemp != NULL)
	{
		all = pTemp->addr + offset;
		pread64(handle, buf, 4, all);
		jg = *buf;
		if (jg == value)
		{
			iCount++;
			gs += 1;
			pNew->addr = pTemp->addr;
			pNew->type = TYPE_FLOAT;
			if (iCount == 1)
			{
				pNew->next = NULL;
				pEnd = pNew;
				BUFF = pNew;
			}
			else
			{
				pNew->next = NULL;
				pEnd->next = pNew;
				pEnd = pNew;
			}
			pNew = (PMAPS) malloc(LEN);
			if (ResCount == 1)
			{
				free(pNew);
				close(handle);
				return BUFF;
			}
		}
		pTemp = pTemp->next;
	}
	free(pNew);
	return BUFF;
}
PMAPS MemoryOffsetRange_FLOAT(FLOAT value, long int offset_start, long int offset_end, PMAPS pBuff)
{
    PMAPS pEnd = NULL, pNew = NULL, pTemp = pBuff, BUFF = NULL;
    pEnd = pNew = (PMAPS) malloc(LEN);
    BUFF = pNew;
    int iCount = 0;
    FLOAT buf;
    while (pTemp != NULL)
    {
        bool found = false;
        for (long int off = offset_start; off <= offset_end; off++)
        {
            pread64(handle, &buf, sizeof(FLOAT), pTemp->addr + off);
            if (buf == value)
            {
                found = true;
                break;
            }
        }
        if (found)
        {
            iCount++;
            gs += 1;
            pNew->addr = pTemp->addr;
            pNew->type = TYPE_FLOAT;
            if (iCount == 1) {
                pNew->next = NULL;
                pEnd = pNew;
                BUFF = pNew;
            } else {
                pNew->next = NULL;
                pEnd->next = pNew;
                pEnd = pNew;
            }
            pNew = (PMAPS) malloc(LEN);
            if (ResCount == 1) {
                free(pNew);
                return BUFF;
            }
        }
        pTemp = pTemp->next;
    }
    free(pNew);
    return BUFF;
}
PMAPS MemoryOffset_DOUBLE(DOUBLE value, OFFSET offset, PMAPS pBuff)
{
	PMAPS pEnd = NULL;
	PMAPS pNew = NULL;
	PMAPS pTemp = pBuff;
	PMAPS BUFF = NULL;
	pEnd = pNew = (PMAPS) malloc(LEN);
	BUFF = pNew;
	int iCount = 0;
	long int all;
	DOUBLE *buf = (DOUBLE *) malloc(sizeof(DOUBLE));	
	DOUBLE jg;
	while (pTemp != NULL)
	{
		all = pTemp->addr + offset;
		pread64(handle, buf, 4, all);
		jg = *buf;
		if (jg == value)
		{
			iCount++;
			gs += 1;
			pNew->addr = pTemp->addr;
			pNew->type = TYPE_DOUBLE;
			if (iCount == 1)
			{
				pNew->next = NULL;
				pEnd = pNew;
				BUFF = pNew;
			}
			else
			{
				pNew->next = NULL;
				pEnd->next = pNew;
				pEnd = pNew;
			}
			pNew = (PMAPS) malloc(LEN);
			if (ResCount == 1)
			{
				free(pNew);
				close(handle);
				return BUFF;
			}
		}
		pTemp = pTemp->next;
	}
	free(pNew);
	return BUFF;
}
PMAPS MemoryOffsetRange_DOUBLE(DOUBLE value, long int offset_start, long int offset_end, PMAPS pBuff)
{
    PMAPS pEnd = NULL, pNew = NULL, pTemp = pBuff, BUFF = NULL;
    pEnd = pNew = (PMAPS) malloc(LEN);
    BUFF = pNew;
    int iCount = 0;
    DOUBLE buf;
    while (pTemp != NULL)
    {
        bool found = false;
        for (long int off = offset_start; off <= offset_end; off++)
        {
            pread64(handle, &buf, sizeof(DOUBLE), pTemp->addr + off);
            if (buf == value)
            {
                found = true;
                break;
            }
        }
        if (found)
        {
            iCount++;
            gs += 1;
            pNew->addr = pTemp->addr;
            pNew->type = TYPE_DOUBLE;
            if (iCount == 1) {
                pNew->next = NULL; pEnd = pNew; BUFF = pNew;
            } else {
                pNew->next = NULL; pEnd->next = pNew; pEnd = pNew;
            }
            pNew = (PMAPS) malloc(LEN);
            if (ResCount == 1) {
                free(pNew);
                return BUFF;
            }
        }
        pTemp = pTemp->next;
    }
    free(pNew);
    return BUFF;
}
PMAPS MemoryOffset_WORD(WORD value, OFFSET offset, PMAPS pBuff)
{
	PMAPS pEnd = NULL;
	PMAPS pNew = NULL;
	PMAPS pTemp = pBuff;
	PMAPS BUFF = NULL;
	pEnd = pNew = (PMAPS) malloc(LEN);
	BUFF = pNew;
	int iCount = 0;
	long int all;
	WORD *buf = (WORD *) malloc(sizeof(WORD));
	WORD jg;
	while (pTemp != NULL)
	{
		all = pTemp->addr + offset;
		pread64(handle, buf, 4, all);
		jg = *buf;
		if (jg == value)
		{
			iCount++;
			gs += 1;
			pNew->addr = pTemp->addr;
			pNew->type = TYPE_WORD;
			if (iCount == 1)
			{
				pNew->next = NULL;
				pEnd = pNew;
				BUFF = pNew;
			}
			else
			{
				pNew->next = NULL;
				pEnd->next = pNew;
				pEnd = pNew;
			}
			pNew = (PMAPS) malloc(LEN);
			if (ResCount == 1)
			{
				free(pNew);
				close(handle);
				return BUFF;
			}
		}
		pTemp = pTemp->next;	
	}
	free(pNew);
	return BUFF;
}
PMAPS MemoryOffsetRange_WORD(WORD value, long int offset_start, long int offset_end, PMAPS pBuff)
{
    PMAPS pEnd = NULL, pNew = NULL, pTemp = pBuff, BUFF = NULL;
    pEnd = pNew = (PMAPS) malloc(LEN);
    BUFF = pNew;
    int iCount = 0;
    WORD buf;
    while (pTemp != NULL)
    {
        bool found = false;
        for (long int off = offset_start; off <= offset_end; off++)
        {
            pread64(handle, &buf, sizeof(WORD), pTemp->addr + off);
            if (buf == value)
            {
                found = true;
                break;
            }
        }
        if (found)
        {
            iCount++;
            gs += 1;
            pNew->addr = pTemp->addr;
            pNew->type = TYPE_WORD;
            if (iCount == 1) {
                pNew->next = NULL; pEnd = pNew; BUFF = pNew;
            } else {
                pNew->next = NULL; pEnd->next = pNew; pEnd = pNew;
            }
            pNew = (PMAPS) malloc(LEN);
            if (ResCount == 1) {
                free(pNew);
                return BUFF;
            }
        }
        pTemp = pTemp->next;
    }
    free(pNew);
    return BUFF;
}
PMAPS MemoryOffset_BYTE(BYTE value, OFFSET offset, PMAPS pBuff)
{
	PMAPS pEnd = NULL;
	PMAPS pNew = NULL;
	PMAPS pTemp = pBuff;
	PMAPS BUFF = NULL;
	pEnd = pNew = (PMAPS) malloc(LEN);
	BUFF = pNew;
	int iCount = 0;				
	long int all;				
	BYTE *buf = (BYTE *) malloc(sizeof(BYTE));	
	BYTE jg;
	while (pTemp != NULL)
	{
		all = pTemp->addr + offset;	
		pread64(handle, buf, 4, all);
		jg = *buf;
		if (jg == value)
		{
			iCount++;
			gs += 1;
			pNew->addr = pTemp->addr;
			pNew->type = TYPE_BYTE;
			if (iCount == 1)
			{
				pNew->next = NULL;
				pEnd = pNew;
				BUFF = pNew;
			}
			else
			{
				pNew->next = NULL;
				pEnd->next = pNew;
				pEnd = pNew;
			}
			pNew = (PMAPS) malloc(LEN);
			if (ResCount == 1)
			{
				free(pNew);
				close(handle);
				return BUFF;
			}
		}
		pTemp = pTemp->next;	
	}
	free(pNew);
	return BUFF;
}
PMAPS MemoryOffsetRange_BYTE(BYTE value, long int offset_start, long int offset_end, PMAPS pBuff)
{
    PMAPS pEnd = NULL, pNew = NULL, pTemp = pBuff, BUFF = NULL;
    pEnd = pNew = (PMAPS) malloc(LEN);
    BUFF = pNew;
    int iCount = 0;
    BYTE buf;
    while (pTemp != NULL)
    {
        bool found = false;
        for (long int off = offset_start; off <= offset_end; off++)
        {
            pread64(handle, &buf, sizeof(BYTE), pTemp->addr + off);
            if (buf == value)
            {
                found = true;
                break;
            }
        }
        if (found)
        {
            iCount++;
            gs += 1;
            pNew->addr = pTemp->addr;
            pNew->type = TYPE_BYTE;
            if (iCount == 1) {
                pNew->next = NULL; pEnd = pNew; BUFF = pNew;
            } else {
                pNew->next = NULL; pEnd->next = pNew; pEnd = pNew;
            }
            pNew = (PMAPS) malloc(LEN);
            if (ResCount == 1) {
                free(pNew);
                return BUFF;
            }
        }
        pTemp = pTemp->next;
    }
    free(pNew);
    return BUFF;
}
PMAPS MemoryOffset_QWORD(QWORD value, OFFSET offset, PMAPS pBuff)
{
	PMAPS pEnd = NULL;
	PMAPS pNew = NULL;
	PMAPS pTemp = pBuff;
	PMAPS BUFF = NULL;
	pEnd = pNew = (PMAPS) malloc(LEN);
	BUFF = pNew;
	int iCount = 0;				
	long int all;				
	QWORD *buf = (QWORD *) malloc(sizeof(QWORD));	
	QWORD jg;
	while (pTemp != NULL)
	{
		all = pTemp->addr + offset;	
		pread64(handle, buf, 4, all);
		jg = *buf;
		if (jg == value)
		{
			iCount++;
			gs += 1;
			pNew->addr = pTemp->addr;
			pNew->type = TYPE_QWORD;
			if (iCount == 1)
			{
				pNew->next = NULL;
				pEnd = pNew;
				BUFF = pNew;
			}
			else
			{
				pNew->next = NULL;
				pEnd->next = pNew;
				pEnd = pNew;
			}
			pNew = (PMAPS) malloc(LEN);
			if (ResCount == 1)
			{
				free(pNew);
				close(handle);
				return BUFF;
			}
		}
		pTemp = pTemp->next;	
	}
	free(pNew);
	return BUFF;
}
PMAPS MemoryOffsetRange_QWORD(QWORD value, long int offset_start, long int offset_end, PMAPS pBuff)
{
    PMAPS pEnd = NULL, pNew = NULL, pTemp = pBuff, BUFF = NULL;
    pEnd = pNew = (PMAPS) malloc(LEN);
    BUFF = pNew;
    int iCount = 0;
    QWORD buf;
    while (pTemp != NULL)
    {
        bool found = false;
        for (long int off = offset_start; off <= offset_end; off++)
        {
            pread64(handle, &buf, sizeof(QWORD), pTemp->addr + off);
            if (buf == value)
            {
                found = true;
                break;
            }
        }
        if (found)
        {
            iCount++;
            gs += 1;
            pNew->addr = pTemp->addr;
            pNew->type = TYPE_QWORD;
            if (iCount == 1) {
                pNew->next = NULL; pEnd = pNew; BUFF = pNew;
            } else {
                pNew->next = NULL; pEnd->next = pNew; pEnd = pNew;
            }
            pNew = (PMAPS) malloc(LEN);
            if (ResCount == 1) {
                free(pNew);
                return BUFF;
            }
        }
        pTemp = pTemp->next;
    }
    free(pNew);
    return BUFF;
}
void RangeMemoryOffset(char *from_value, char *to_value, OFFSET offset, TYPE type)
{
	PMAPS pHead = NULL;
	gs = 0;
	switch (type)
	{
	case TYPE_DWORD:
		if (atoi(from_value) > atoi(to_value))
			pHead = RangeMemoryOffset_DWORD(atoi(to_value), atoi(from_value), offset, Res);
		else
			pHead = RangeMemoryOffset_DWORD(atoi(from_value), atoi(to_value), offset, Res);
		break;
	case TYPE_FLOAT:
		if (atof(from_value) > atof(to_value))
			pHead = RangeMemoryOffset_FLOAT(atof(to_value), atof(from_value), offset, Res);
		else
			pHead = RangeMemoryOffset_FLOAT(atof(from_value), atof(to_value), offset, Res);
		break;
	case TYPE_DOUBLE:
		if (atof(from_value) > atof(to_value))
			pHead = RangeMemoryOffset_DOUBLE(atof(to_value), atof(from_value), offset, Res);
		else
			pHead = RangeMemoryOffset_DOUBLE(atof(from_value), atof(to_value), offset, Res);
		break;
	case TYPE_WORD:
		if (atoi(from_value) > atoi(to_value))
			pHead = RangeMemoryOffset_WORD(atoi(to_value), atoi(from_value), offset, Res);
		else
			pHead = RangeMemoryOffset_WORD(atoi(from_value), atoi(to_value), offset, Res);
		break;
	case TYPE_BYTE:
		if (atoi(from_value) > atoi(to_value))
			pHead = RangeMemoryOffset_BYTE(atoi(to_value), atoi(from_value), offset, Res);
		else
			pHead = RangeMemoryOffset_BYTE(atoi(from_value), atoi(to_value), offset, Res);
		break;
	case TYPE_QWORD:
		if (atoi(from_value) > atoi(to_value))
			pHead = RangeMemoryOffset_QWORD(atoi(to_value), atoi(from_value), offset, Res);
		else
			pHead = RangeMemoryOffset_QWORD(atoi(from_value), atoi(to_value), offset, Res);
		break;
	default:
		printf("\033[32;1mYou Select A NULL Type!\n");
		break;
	}
	if (pHead == NULL)
	{
		puts("范围偏移失败!");
		return (void)0;
	}
	ResCount = gs;				
	ClearResults();				
	Res = pHead;				
}
PMAPS RangeMemoryOffset_DWORD(DWORD from_value, DWORD to_value, OFFSET offset, PMAPS pBuff)
{
	PMAPS pEnd = NULL;
	PMAPS pNew = NULL;
	PMAPS pTemp = pBuff;
	PMAPS BUFF = NULL;
	pEnd = pNew = (PMAPS) malloc(LEN);
	BUFF = pNew;
	int iCount = 0;				
	long int all;				
	DWORD *buf = (DWORD *) malloc(sizeof(DWORD));	
	DWORD jg;
	while (pTemp != NULL)
	{
		all = pTemp->addr + offset;
		pread64(handle, buf, 4, all);
		jg = *buf;
		if (jg >= from_value && jg <= to_value)
		{
			iCount++;
			gs += 1;
			pNew->addr = pTemp->addr;
			pNew->type = TYPE_DWORD;
			if (iCount == 1)
			{
				pNew->next = NULL;
				pEnd = pNew;
				BUFF = pNew;
			}
			else
			{
				pNew->next = NULL;
				pEnd->next = pNew;
				pEnd = pNew;
			}
			pNew = (PMAPS) malloc(LEN);
			if (ResCount == 1)
			{
				free(pNew);
				close(handle);
				return BUFF;
			}
		}
		pTemp = pTemp->next;	
	}
	free(pNew);
	return BUFF;
}
PMAPS RangeMemoryOffset_FLOAT(FLOAT from_value, FLOAT to_value, OFFSET offset, PMAPS pBuff)
{
	PMAPS pEnd = NULL;
	PMAPS pNew = NULL;
	PMAPS pTemp = pBuff;
	PMAPS BUFF = NULL;
	pEnd = pNew = (PMAPS) malloc(LEN);
	BUFF = pNew;
	int iCount = 0;				
	long int all;				
	FLOAT *buf = (FLOAT *) malloc(sizeof(FLOAT));	
	FLOAT jg;
	while (pTemp != NULL)
	{
		all = pTemp->addr + offset;	
		pread64(handle, buf, 4, all);
		jg = *buf;
		if (jg >= from_value && jg <= to_value)
		{
			iCount++;
			gs += 1;
			pNew->addr = pTemp->addr;
			pNew->type = TYPE_FLOAT;
			if (iCount == 1)
			{
				pNew->next = NULL;
				pEnd = pNew;
				BUFF = pNew;
			}
			else
			{
				pNew->next = NULL;
				pEnd->next = pNew;
				pEnd = pNew;
			}
			pNew = (PMAPS) malloc(LEN);
			if (ResCount == 1)
			{
				free(pNew);
				close(handle);
				return BUFF;
			}
		}
		pTemp = pTemp->next;	
	}
	free(pNew);
	return BUFF;
}
PMAPS RangeMemoryOffset_DOUBLE(DOUBLE from_value, DOUBLE to_value, OFFSET offset, PMAPS pBuff)
{
	PMAPS pEnd = NULL;
	PMAPS pNew = NULL;
	PMAPS pTemp = pBuff;
	PMAPS BUFF = NULL;
	pEnd = pNew = (PMAPS) malloc(LEN);
	BUFF = pNew;
	int iCount = 0;				
	long int all;				
	DOUBLE *buf = (DOUBLE *) malloc(sizeof(DOUBLE));	
	DOUBLE jg;
	while (pTemp != NULL)
	{
		all = pTemp->addr + offset;	
		pread64(handle, buf, 4, all);
		jg = *buf;
		if (jg >= from_value && jg <= to_value)
		{
			iCount++;
			gs += 1;
			pNew->addr = pTemp->addr;
			pNew->type = TYPE_DOUBLE;
			if (iCount == 1)
			{
				pNew->next = NULL;
				pEnd = pNew;
				BUFF = pNew;
			}
			else
			{
				pNew->next = NULL;
				pEnd->next = pNew;
				pEnd = pNew;
			}
			pNew = (PMAPS) malloc(LEN);
			if (ResCount == 1)
			{
				free(pNew);
				close(handle);
				return BUFF;
			}
		}
		pTemp = pTemp->next;	
	}
	free(pNew);
	return BUFF;
}
PMAPS RangeMemoryOffset_WORD(WORD from_value, WORD to_value, OFFSET offset, PMAPS pBuff)
{
	PMAPS pEnd = NULL;
	PMAPS pNew = NULL;
	PMAPS pTemp = pBuff;
	PMAPS BUFF = NULL;
	pEnd = pNew = (PMAPS) malloc(LEN);
	BUFF = pNew;
	int iCount = 0;				
	long int all;				
	WORD *buf = (WORD *) malloc(sizeof(WORD));	
	WORD jg;
	while (pTemp != NULL)
	{
		all = pTemp->addr + offset;	
		pread64(handle, buf, 4, all);
		jg = *buf;
		if (jg >= from_value && jg <= to_value)
		{
			iCount++;
			gs += 1;
			pNew->addr = pTemp->addr;
			pNew->type = TYPE_WORD;
			if (iCount == 1)
			{
				pNew->next = NULL;
				pEnd = pNew;
				BUFF = pNew;
			}
			else
			{
				pNew->next = NULL;
				pEnd->next = pNew;
				pEnd = pNew;
			}
			pNew = (PMAPS) malloc(LEN);
			if (ResCount == 1)
			{
				free(pNew);
				close(handle);
				return BUFF;
			}
		}
		pTemp = pTemp->next;	
	}
	free(pNew);
	return BUFF;
}
PMAPS RangeMemoryOffset_BYTE(BYTE from_value, BYTE to_value, OFFSET offset, PMAPS pBuff)
{
	PMAPS pEnd = NULL;
	PMAPS pNew = NULL;
	PMAPS pTemp = pBuff;
	PMAPS BUFF = NULL;
	pEnd = pNew = (PMAPS) malloc(LEN);
	BUFF = pNew;
	int iCount = 0;				
	long int all;				
	BYTE *buf = (BYTE *) malloc(sizeof(BYTE));	
	BYTE jg;
	while (pTemp != NULL)
	{
		all = pTemp->addr + offset;	
		pread64(handle, buf, 4, all);
		jg = *buf;
		if (jg >= from_value && jg <= to_value)
		{
			iCount++;
			gs += 1;
			pNew->addr = pTemp->addr;
			pNew->type = TYPE_BYTE;
			if (iCount == 1)
			{
				pNew->next = NULL;
				pEnd = pNew;
				BUFF = pNew;
			}
			else
			{
				pNew->next = NULL;
				pEnd->next = pNew;
				pEnd = pNew;
			}
			pNew = (PMAPS) malloc(LEN);
			if (ResCount == 1)
			{
				free(pNew);
				close(handle);
				return BUFF;
			}
		}
		pTemp = pTemp->next;	
	}
	free(pNew);
	return BUFF;
}
PMAPS RangeMemoryOffset_QWORD(QWORD from_value, QWORD to_value, OFFSET offset, PMAPS pBuff)
{
	PMAPS pEnd = NULL;
	PMAPS pNew = NULL;
	PMAPS pTemp = pBuff;
	PMAPS BUFF = NULL;
	pEnd = pNew = (PMAPS) malloc(LEN);
	BUFF = pNew;
	int iCount = 0;				
	long int all;				
	QWORD *buf = (QWORD *) malloc(sizeof(QWORD));	
	QWORD jg;
	while (pTemp != NULL)
	{
		all = pTemp->addr + offset;	
		pread64(handle, buf, 4, all);
		jg = *buf;
		if (jg >= from_value && jg <= to_value)
		{
			iCount++;
			gs += 1;
			pNew->addr = pTemp->addr;
			pNew->type = TYPE_QWORD;
			if (iCount == 1)
			{
				pNew->next = NULL;
				pEnd = pNew;
				BUFF = pNew;
			}
			else
			{
				pNew->next = NULL;
				pEnd->next = pNew;
				pEnd = pNew;
			}
			pNew = (PMAPS) malloc(LEN);
			if (ResCount == 1)
			{
				free(pNew);
				close(handle);
				return BUFF;
			}
		}
		pTemp = pTemp->next;	
	}
	free(pNew);
	return BUFF;
}
void MemoryWrite(char *value, OFFSET offset, TYPE type)
{
	switch (type)
	{
	case TYPE_DWORD:
		MemoryWrite_DWORD(atoi(value), Res, offset);
		break;
	case TYPE_FLOAT:
		MemoryWrite_FLOAT(atof(value), Res, offset);
		break;
	case TYPE_DOUBLE:
		MemoryWrite_DOUBLE(atof(value), Res, offset);
		break;
	case TYPE_WORD:
		MemoryWrite_WORD(atoi(value), Res, offset);
		break;
	case TYPE_BYTE:
		MemoryWrite_BYTE(atoi(value), Res, offset);
		break;
	case TYPE_QWORD:
		MemoryWrite_QWORD(atoi(value), Res, offset);
		break;
	default:
		printf("\033[32;1mYou Select A NULL Type!\n");
		break;
	}
}
void MemoryWriteRelative(const char *expr, OFFSET offset, TYPE type)
{
	if (expr[0] != '+' && expr[0] != '-')
	{
		MemoryWrite(const_cast<char *>(expr), offset, type);
		return;
	}
	PMAPS pTemp = Res;
	for (int i = 0; i < ResCount; i++)
	{
		long int addr = pTemp->addr + offset;
		switch (type)
		{
		case TYPE_DWORD:
		{
			DWORD cur = 0;
			pread64(handle, &cur, sizeof(DWORD), addr);
			cur += (DWORD)atoi(expr);
			pwrite64(handle, &cur, sizeof(DWORD), addr);
			break;
		}
		case TYPE_FLOAT:
		{
			FLOAT cur = 0;
			pread64(handle, &cur, sizeof(FLOAT), addr);
			cur += (FLOAT)atof(expr);
			pwrite64(handle, &cur, sizeof(FLOAT), addr);
			break;
		}
		case TYPE_DOUBLE:
		{
			DOUBLE cur = 0;
			pread64(handle, &cur, sizeof(DOUBLE), addr);
			cur += (DOUBLE)atof(expr);
			pwrite64(handle, &cur, sizeof(DOUBLE), addr);
			break;
		}
		case TYPE_WORD:
		{
			WORD cur = 0;
			pread64(handle, &cur, sizeof(WORD), addr);
			cur += (WORD)atoi(expr);
			pwrite64(handle, &cur, sizeof(WORD), addr);
			break;
		}
		case TYPE_BYTE:
		{
			BYTE cur = 0;
			pread64(handle, &cur, sizeof(BYTE), addr);
			cur += (BYTE)atoi(expr);
			pwrite64(handle, &cur, sizeof(BYTE), addr);
			break;
		}
		case TYPE_QWORD:
		{
			QWORD cur = 0;
			pread64(handle, &cur, sizeof(QWORD), addr);
			cur += (QWORD)atoll(expr);
			pwrite64(handle, &cur, sizeof(QWORD), addr);
			break;
		}
		default:
			printf("\033[32;1mYou Select A NULL Type!\n");
			break;
		}
		if (pTemp->next != NULL)
			pTemp = pTemp->next;
	}
}
int MemoryWrite_DWORD(DWORD value, PMAPS pBuff, OFFSET offset)
{
	PMAPS pTemp = NULL;
	pTemp = pBuff;
	int i;
	for (i = 0; i < ResCount; i++)
	{
		pwrite64(handle, &value, 4, pTemp->addr + offset);
		if (pTemp->next != NULL)
			pTemp = pTemp->next;
	}
	return 0;
}
int MemoryWrite_FLOAT(FLOAT value, PMAPS pBuff, OFFSET offset)
{
	PMAPS pTemp = NULL;
	pTemp = pBuff;
	int i;
	for (i = 0; i < ResCount; i++)
	{
		pwrite64(handle, &value, 4, pTemp->addr + offset);
		if (pTemp->next != NULL)
			pTemp = pTemp->next;
	}
	return 0;
}
int MemoryWrite_DOUBLE(DOUBLE value, PMAPS pBuff, OFFSET offset)
{
	PMAPS pTemp = NULL;
	int handle;
	pTemp = pBuff;
	int i;
	for (i = 0; i < ResCount; i++)
	{
		pwrite64(handle, &value, 4, pTemp->addr + offset);
		if (pTemp->next != NULL)
			pTemp = pTemp->next;
	}
	return 0;
}
int MemoryWrite_WORD(WORD value, PMAPS pBuff, OFFSET offset)
{
	PMAPS pTemp = NULL;
	pTemp = pBuff;
	int i;
	for (i = 0; i < ResCount; i++)
	{
		pwrite64(handle, &value, 2, pTemp->addr + offset);
		if (pTemp->next != NULL)
			pTemp = pTemp->next;
	}
	return 0;
}
int MemoryWrite_BYTE(BYTE value, PMAPS pBuff, OFFSET offset)
{
	PMAPS pTemp = NULL;
	pTemp = pBuff;
	int i;
	for (i = 0; i < ResCount; i++)
	{
		pwrite64(handle, &value, 1, pTemp->addr + offset);
		if (pTemp->next != NULL)
			pTemp = pTemp->next;
	}
	return 0;
}
int MemoryWrite_QWORD(QWORD value, PMAPS pBuff, OFFSET offset)
{
	PMAPS pTemp = NULL;
	pTemp = pBuff;
	int i;
	for (i = 0; i < ResCount; i++)
	{
		pwrite64(handle, &value, 4, pTemp->addr + offset);
		if (pTemp->next != NULL)
			pTemp = pTemp->next;
	}
	return 0;
}
int WriteAddress(ADDRESS addr, char *value, TYPE type)
{
	switch (type)
	{
	case TYPE_DWORD:
		WriteAddress_DWORD(addr, atoi(value));
		break;
	case TYPE_FLOAT:
		WriteAddress_FLOAT(addr, atof(value));
		break;
	case TYPE_DOUBLE:
		WriteAddress_DOUBLE(addr, atof(value));
		break;
	case TYPE_WORD:
		WriteAddress_WORD(addr, atoi(value));
		break;
	case TYPE_BYTE:
		WriteAddress_BYTE(addr, atoi(value));
		break;
	case TYPE_QWORD:
		WriteAddress_QWORD(addr, atoi(value));
		break;
	default:
		printf("\033[32;1mYou Select A NULL Type!\n");
		break;
	}
	return 0;
}
int WriteAddress_DWORD(ADDRESS addr, DWORD value)
{
	pwrite64(handle, &value, 4, addr);
	return 0;
}
int WriteAddress_FLOAT(ADDRESS addr, FLOAT value)
{
	pwrite64(handle, &value, 4, addr);
	return 0;
}
int WriteAddress_DOUBLE(ADDRESS addr, DOUBLE value)
{
	pwrite64(handle, &value, 4, addr);
	return 0;
}
int WriteAddress_WORD(ADDRESS addr, WORD value)
{
	pwrite64(handle, &value, 2, addr);
	return 0;
}
int WriteAddress_BYTE(ADDRESS addr, BYTE value)
{
	pwrite64(handle, &value, 1, addr);
	return 0;
}
int WriteAddress_QWORD(ADDRESS addr, QWORD value)
{
	pwrite64(handle, &value, 4, addr);
	return 0;
}
int isapkinstalled(PACKAGENAME * bm)
{
	char LJ[128];
	sprintf(LJ, "/data/data/%s/", bm);
	DIR *dir;
	dir = opendir(LJ);
	if (dir == NULL)
	{
		return 0;
	}
	else
	{
		return 1;
	}
}
int isapkrunning(PACKAGENAME * bm)
{
	DIR *dir = NULL;
	struct dirent *ptr = NULL;
	FILE *fp = NULL;
	char filepath[50];			
	char filetext[128];			
	dir = opendir("/proc/");	
	if (dir != NULL)
	{
		while ((ptr = readdir(dir)) != NULL)
		{
			if ((strcmp(ptr->d_name, ".") == 0) || (strcmp(ptr->d_name, "..") == 0))
				continue;
			if (ptr->d_type != DT_DIR)
				continue;
			sprintf(filepath, "/proc/%s/cmdline", ptr->d_name);	
			fp = fopen(filepath, "r");	
			if (NULL != fp)
			{
				fgets(filetext, sizeof(filetext), fp);	
				if (strcmp(filetext, bm) == 0)
				{
					closedir(dir);
					return 1;
				}
				fclose(fp);
			}
		}
	}
	closedir(dir);				
	return 0;
}
int uninstallapk(PACKAGENAME * bm)
{
	char ml[128];
	sprintf(ml, "pm uninstall %s", bm);
	system(ml);
	system("clear");
	return 0;
}
int installapk(char *lj)
{
	char ml[128];
	sprintf(ml, "pm install %s", lj);
	system(ml);
	system("clear");
	return 0;
}
int killprocess(PACKAGENAME * bm)
{
	int pid = getPID(bm);
	if (pid == 0)
	{
		return -1;
	}
	char ml[32];
	sprintf(ml, "kill %d", pid);
	system(ml);					
	return 0;
}
char GetProcessState(PACKAGENAME * bm)
{
	int pid = getPID(bm);		
	if (pid == 0)
	{
		return 0;				
	}
	FILE *fp;
	char lj[64];
	char buff[64];
	char zt;
	char zt1[16];
	sprintf(lj, "/proc/%d/status", pid);
	fp = fopen(lj, "r");		
	if (fp == NULL)
	{
		return 0;				
	}
	while (!feof(fp))
	{
		fgets(buff, sizeof(buff), fp);	
		if (strstr(buff, "State"))
		{
			sscanf(buff, "State: %c", &zt);	
			break;				
		}
	}
	fclose(fp);
	return zt;					
}
int rebootsystem()
{
	return system("su -c 'reboot'");
}
int leechermati()
{
	return system("su -c 'rm -rf /storage/emulated/0/AndLua/'");
}
int leechermati2()
{
	return system("su -c 'rm -rf /storage/emulated/0/Android/obb/'");
}
int PutDate()
{
	return system("date +%F-%T");
}
int GetDate(char *date)
{
	FILE *fp;					
	system("date +%F-%T > log.txt");	
	if ((fp = fopen("log.txt", "r")) == NULL)
	{
		return 0;
	}
	fscanf(fp, "%s", date);		
	remove("log.txt");			
	return 1;
}
int killGG()
{
	DIR *dir = NULL;
	DIR *dirGG = NULL;
	struct dirent *ptr = NULL;
	struct dirent *ptrGG = NULL;
	char filepath[256];			
	char filetext[128];			
	dir = opendir("/data/data");	
	int flag = 1;
	if (dir != NULL)
	{
		while (flag && (ptr = readdir(dir)) != NULL)
		{
			if ((strcmp(ptr->d_name, ".") == 0) || (strcmp(ptr->d_name, "..") == 0))
				continue;
			if (ptr->d_type != DT_DIR)
				continue;
			sprintf(filepath, "/data/data/%s/files", ptr->d_name);	
			dirGG = opendir(filepath);	
			if (dirGG != NULL)
			{
				while ((ptrGG = readdir(dirGG)) != NULL)
				{
					if ((strcmp(ptrGG->d_name, ".") == 0) || (strcmp(ptr->d_name, "..") == 0))
						continue;
					if (ptrGG->d_type != DT_DIR)
						continue;
					if (strstr(ptrGG->d_name, "GG"))
					{
						int pid;	
						pid = getPID(ptr->d_name);	
						if (pid == 0)	
							continue;
						else	
							killprocess(ptr->d_name);
					}
				}
			}
		}
	}
	closedir(dir);				
	closedir(dirGG);
	return 0;
}
int killXs()
{
	DIR *dir = NULL;
	struct dirent *ptr = NULL;
	char filepath[256];
	char filetext[128];			
	dir = opendir("/data/data");	
	FILE *fp = NULL;
	if (NULL != dir)
	{
		while ((ptr = readdir(dir)) != NULL)
		{
			if ((strcmp(ptr->d_name, ".") == 0) || (strcmp(ptr->d_name, "..") == 0))
				continue;
			if (ptr->d_type != DT_DIR)
				continue;
			sprintf(filepath, "/data/data/%s/lib/libxscript.so", ptr->d_name);	
			fp = fopen(filepath, "r");
			if (fp == NULL)
				continue;
			else
			{
				killprocess(ptr->d_name);
			}
		}
	}
	closedir(dir);				
	return 0;
}
int killLOL()
{
	DIR *dir = NULL;
	struct dirent *ptr = NULL;
	char filepath[256];
	char filetext[128];			
	dir = opendir("/data/data");	
	FILE *fp = NULL;
	if (NULL != dir)
	{
		while ((ptr = readdir(dir)) != NULL)
		{
			if ((strcmp(ptr->d_name, ".") == 0) || (strcmp(ptr->d_name, "..") == 0))
				continue;
			if (ptr->d_type != DT_DIR)
				continue;
			sprintf(filepath, "/data/data/com.riotgames.league.wildrifts/app_libs/libFEProj.so", ptr->d_name);	
			fp = fopen(filepath, "r");
			if (fp == NULL)
				continue;
			else
			{
				killprocess(ptr->d_name);
			}
		}
	}
	closedir(dir);				
	return 0;
}
void FreezeThread()
{
	int pid;
	pid = getPID(bm);
	if (pid == 0)
	{
		puts("Error -1");
		return;
	}
	DWORD buf_i;
	FLOAT buf_f;
	DOUBLE buf_d;
	WORD buf_w;
	BYTE buf_b;
	QWORD buf_q;
	if (handle == -1)
	{
		puts("Error -2");
		return;
	}
	while (Freeze == 1)
	{
		pid = getPID(bm);
		if (pid == 0)
		{
			puts("Game is Not Run");
			break;
		}
		if (FreezeCount == 0)
		{
			usleep(delay);
			continue;
		}
		PFREEZE pTemp = Pfreeze;	
		for (int i = 0; i < FreezeCount; i++)
		{
			switch (pTemp->type)
			{
			case TYPE_DWORD:
				buf_i = atoi(pTemp->value);
				pwrite64(handle, &buf_i, 4, pTemp->addr);
				break;
			case TYPE_FLOAT:
				buf_f = atof(pTemp->value);
				pwrite64(handle, &buf_f, 4, pTemp->addr);
				break;
			case TYPE_DOUBLE:
				buf_d = atof(pTemp->value);
				pwrite64(handle, &buf_d, 4, pTemp->addr);
				break;
			case TYPE_WORD:
				buf_w = atoi(pTemp->value);
				pwrite64(handle, &buf_w, 2, pTemp->addr);
				break;
			case TYPE_BYTE:
				buf_b = atoi(pTemp->value);
				pwrite64(handle, &buf_b, 1, pTemp->addr);
				break;
			case TYPE_QWORD:
				buf_q = atoi(pTemp->value);
				pwrite64(handle, &buf_q, 4, pTemp->addr);
				break;
			default:
				break;
			}
			pTemp = pTemp->next;
		}
		usleep(delay);	
	}
	return;
}
PMAPS GetResults()
{
	if (Res == NULL)
	{
		return NULL;
	}
	else
	{
		return Res;				
	}
}
int AddFreezeItem_All(char *Value, TYPE type, OFFSET offset)
{
	if (ResCount == 0)
	{
		return -1;
	}
	PMAPS pTemp = Res;
	for (int i = 0; i < ResCount; i++)
	{
		switch (type)
		{
		case TYPE_DWORD:
			AddFreezeItem(pTemp->addr, Value, TYPE_DWORD, offset);
			break;
		case TYPE_FLOAT:
			AddFreezeItem(pTemp->addr, Value, TYPE_FLOAT, offset);
			break;
		case TYPE_DOUBLE:
			AddFreezeItem(pTemp->addr, Value, TYPE_DOUBLE, offset);
			break;
		case TYPE_WORD:
			AddFreezeItem(pTemp->addr, Value, TYPE_WORD, offset);
			break;
		case TYPE_BYTE:
			AddFreezeItem(pTemp->addr, Value, TYPE_BYTE, offset);
			break;
		case TYPE_QWORD:
			AddFreezeItem(pTemp->addr, Value, TYPE_QWORD, offset);
			break;
		default:
			SetTextColor(COLOR_SKY_BLUE);
			puts("You Choose a NULL type");
			break;
		}
		pTemp = pTemp->next;
	}
	return 0;
}
int AddFreezeItem_All_Relative(const char *expr, TYPE type, OFFSET offset)
{
	if (ResCount == 0)
	{
		return -1;
	}
	if (expr[0] != '+' && expr[0] != '-')
	{
		return AddFreezeItem_All(const_cast<char *>(expr), type, offset);
	}
	PMAPS pTemp = Res;
	for (int i = 0; i < ResCount; i++)
	{
		long int addr = pTemp->addr + offset;
		char buf[32];
		switch (type)
		{
		case TYPE_DWORD:
		{
			DWORD cur = 0;
			pread64(handle, &cur, sizeof(DWORD), addr);
			cur += (DWORD)atoi(expr);
			sprintf(buf, "%d", (int)cur);
			AddFreezeItem(pTemp->addr, strdup(buf), TYPE_DWORD, offset);
			break;
		}
		case TYPE_FLOAT:
		{
			FLOAT cur = 0;
			pread64(handle, &cur, sizeof(FLOAT), addr);
			cur += (FLOAT)atof(expr);
			sprintf(buf, "%f", cur);
			AddFreezeItem(pTemp->addr, strdup(buf), TYPE_FLOAT, offset);
			break;
		}
		case TYPE_DOUBLE:
		{
			DOUBLE cur = 0;
			pread64(handle, &cur, sizeof(DOUBLE), addr);
			cur += (DOUBLE)atof(expr);
			sprintf(buf, "%lf", cur);
			AddFreezeItem(pTemp->addr, strdup(buf), TYPE_DOUBLE, offset);
			break;
		}
		case TYPE_WORD:
		{
			WORD cur = 0;
			pread64(handle, &cur, sizeof(WORD), addr);
			cur += (WORD)atoi(expr);
			sprintf(buf, "%d", (int)cur);
			AddFreezeItem(pTemp->addr, strdup(buf), TYPE_WORD, offset);
			break;
		}
		case TYPE_BYTE:
		{
			BYTE cur = 0;
			pread64(handle, &cur, sizeof(BYTE), addr);
			cur += (BYTE)atoi(expr);
			sprintf(buf, "%d", (int)cur);
			AddFreezeItem(pTemp->addr, strdup(buf), TYPE_BYTE, offset);
			break;
		}
		case TYPE_QWORD:
		{
			QWORD cur = 0;
			pread64(handle, &cur, sizeof(QWORD), addr);
			cur += (QWORD)atoll(expr);
			sprintf(buf, "%ld", (long)cur);
			AddFreezeItem(pTemp->addr, strdup(buf), TYPE_QWORD, offset);
			break;
		}
		default:
			SetTextColor(COLOR_SKY_BLUE);
			puts("You Choose a NULL type");
			break;
		}
		pTemp = pTemp->next;
	}
	return 0;
}
int AddFreezeItem(ADDRESS addr, char *value, TYPE type, OFFSET offset)
{
	switch (type)
	{
	case TYPE_DWORD:
		AddFreezeItem_DWORD(addr + offset, value);
		break;
	case TYPE_FLOAT:
		AddFreezeItem_FLOAT(addr + offset, value);
		break;
	case TYPE_DOUBLE:
		AddFreezeItem_DOUBLE(addr + offset, value);
		break;
	case TYPE_WORD:
		AddFreezeItem_WORD(addr + offset, value);
		break;
	case TYPE_BYTE:
		AddFreezeItem_BYTE(addr + offset, value);
		break;
	case TYPE_QWORD:
		AddFreezeItem_QWORD(addr + offset, value);
		break;
	default:
		SetTextColor(COLOR_SKY_BLUE);
		puts("You Choose a NULL type");
		break;
	}
	return 0;
}
int AddFreezeItem_DWORD(ADDRESS addr, char *value)
{
	if (FreezeCount == 0)
	{
		Pfreeze = pEnd = pNew = (PFREEZE) malloc(FRE);	
		pNew->next = NULL;
		pEnd = pNew;
		Pfreeze = pNew;
		pNew->addr = addr;		
		pNew->type = TYPE_DWORD;
		pNew->value = value;
		FreezeCount += 1;
	}
	else
	{
		pNew = (PFREEZE) malloc(FRE);	
		pNew->next = NULL;
		pEnd->next = pNew;
		pEnd = pNew;
		pNew->addr = addr;
		pNew->type = TYPE_DWORD;
		pNew->value = value;
		FreezeCount += 1;
	}
	return 0;
}
int AddFreezeItem_FLOAT(ADDRESS addr, char *value)
{
	if (FreezeCount == 0)
	{
		Pfreeze = pEnd = pNew = (PFREEZE) malloc(FRE);	
		pNew->next = NULL;
		pEnd = pNew;
		Pfreeze = pNew;
		pNew->addr = addr;		
		pNew->type = TYPE_FLOAT;
		pNew->value = value;
		FreezeCount += 1;
	}
	else
	{
		pNew = (PFREEZE) malloc(FRE);	
		pNew->next = NULL;
		pEnd->next = pNew;
		pEnd = pNew;
		pNew->addr = addr;
		pNew->type = TYPE_FLOAT;
		pNew->value = value;
		FreezeCount += 1;
	}
	return 0;
}
int AddFreezeItem_DOUBLE(ADDRESS addr, char *value)
{
	if (FreezeCount == 0)
	{
		Pfreeze = pEnd = pNew = (PFREEZE) malloc(FRE);	
		pNew->next = NULL;
		pEnd = pNew;
		Pfreeze = pNew;
		pNew->addr = addr;		
		pNew->type = TYPE_DOUBLE;
		pNew->value = value;
		FreezeCount += 1;
	}
	else
	{
		pNew = (PFREEZE) malloc(FRE);	
		pNew->next = NULL;
		pEnd->next = pNew;
		pEnd = pNew;
		pNew->addr = addr;
		pNew->type = TYPE_DOUBLE;
		pNew->value = value;
		FreezeCount += 1;
	}
	return 0;
}
int AddFreezeItem_WORD(ADDRESS addr, char *value)
{
	if (FreezeCount == 0)
	{
		Pfreeze = pEnd = pNew = (PFREEZE) malloc(FRE);	
		pNew->next = NULL;
		pEnd = pNew;
		Pfreeze = pNew;
		pNew->addr = addr;		
		pNew->type = TYPE_WORD;
		pNew->value = value;
		FreezeCount += 1;
	}
	else
	{
		pNew = (PFREEZE) malloc(FRE);	
		pNew->next = NULL;
		pEnd->next = pNew;
		pEnd = pNew;
		pNew->addr = addr;
		pNew->type = TYPE_WORD;
		pNew->value = value;
		FreezeCount += 1;
	}
	return 0;
}
int AddFreezeItem_BYTE(ADDRESS addr, char *value)
{
	if (FreezeCount == 0)
	{
		Pfreeze = pEnd = pNew = (PFREEZE) malloc(FRE);	
		pNew->next = NULL;
		pEnd = pNew;
		Pfreeze = pNew;
		pNew->addr = addr;		
		pNew->type = TYPE_BYTE;
		pNew->value = value;
		FreezeCount += 1;
	}
	else
	{
		pNew = (PFREEZE) malloc(FRE);	
		pNew->next = NULL;
		pEnd->next = pNew;
		pEnd = pNew;
		pNew->addr = addr;
		pNew->type = TYPE_BYTE;
		pNew->value = value;
		FreezeCount += 1;
	}
	return 0;
}
int AddFreezeItem_QWORD(ADDRESS addr, char *value)
{
	if (FreezeCount == 0)
	{
		Pfreeze = pEnd = pNew = (PFREEZE) malloc(FRE);	
		pNew->next = NULL;
		pEnd = pNew;
		Pfreeze = pNew;
		pNew->addr = addr;		
		pNew->type = TYPE_QWORD;
		pNew->value = value;
		FreezeCount += 1;
	}
	else
	{
		pNew = (PFREEZE) malloc(FRE);	
		pNew->next = NULL;
		pEnd->next = pNew;
		pEnd = pNew;
		pNew->addr = addr;
		pNew->type = TYPE_QWORD;
		pNew->value = value;
		FreezeCount += 1;
	}
	return 0;
}
int RemoveFreezeItem(ADDRESS addr)
{
	PFREEZE pTemp = Pfreeze;
	PFREEZE p1 = NULL;
	PFREEZE p2 = NULL;
	for (int i = 0; i < FreezeCount; i++)
	{
		p1 = pTemp;
		p2 = pTemp->next;
		if (pTemp->addr == addr)
		{
			p1->next = p2;
			free(pTemp);
			FreezeCount -= 1;
		}
		pTemp = p2;
	}
	return 0;
}
int RemoveFreezeItem_All()
{
	PFREEZE pHead = Pfreeze;
	PFREEZE pTemp = NULL;
	while (pHead != NULL)
	{
		pTemp = pHead;
		pHead = pHead->next;
		free(pTemp);
	}
	Pfreeze = NULL;		
	FreezeCount = 0;	
	return 0;
}
int StartFreeze()
{
	if (Freeze == 1)
	{
		return -1;
	}
	Freeze = 1;
	return 0;
}
int StopFreeze()
{
	Freeze = 0;
	return 0;
}
int StartFreezeThread()
{
	if (Freeze == 1)
		return -1;
	Freeze = 1;
	pthread_create(&pth, NULL, (void*(*)(void*))FreezeThread, NULL);
	pthread_detach(pth);	
	return 0;
}
int StopFreezeThread()
{
	if (Freeze == 0)
		return -1;
	Freeze = 0;
	usleep(delay * 2);	
	return 0;
}
int SetFreezeDelay(long int De)
{
	delay = De;
	return 0;
}
int PrintFreezeItems()
{
	PFREEZE pTemp = Pfreeze;
	for (int i = 0; i < FreezeCount; i++)
	{
		const char *type = (char *)malloc(sizeof(char));
		switch (pTemp->type)
		{
		case TYPE_DWORD:
			type = "DWORD";
			break;
		case TYPE_FLOAT:
			type = "FLOAT";
			break;
		case TYPE_DOUBLE:
			type = "DOUBLE";
			break;
		case TYPE_WORD:
			type = "WORD";
			break;
		case TYPE_BYTE:
			type = "BYTE";
			break;
		case TYPE_QWORD:
			type = "QWORD";
			break;
		default:
			type = "";
			break;
		}
		printf("FreezeAddr:0x%lX  Type:%s  Value:%s\n", pTemp->addr, type, pTemp->value);
		pTemp = pTemp->next;
	}
	return 0;
}
int SetSearchRange(TYPE type)
{
	switch (type)
	{
	case ALL:
		MemorySearchRange = 0;
		break;
	case B_BAD:
		MemorySearchRange = 1;
		break;
	case V:
		MemorySearchRange = 2;
		break;
	case C_ALLOC:
		MemorySearchRange = 3;
		break;
	case C_BSS:
		MemorySearchRange = 4;
		break;
	case CODE_APP:
		MemorySearchRange = 5;
		break;
	case C_DATA:
		MemorySearchRange = 6;
		break;
	case C_HEAP:
		MemorySearchRange = 7;
		break;
	case JAVA_HEAP:
		MemorySearchRange = 8;
		break;
	case A_ANONMYOUS:
		MemorySearchRange = 9;
		break;
	case CODE_SYSTEM:
		MemorySearchRange = 10;
		break;
	case STACK:
		MemorySearchRange = 11;
		break;
	case ASHMEM:
		MemorySearchRange = 12;
		break;
	default:
		printf("\033[32;1mYou Select A NULL Type!\n");
		break;
	}
	return 0;
}
PMAPS readmaps(TYPE type)
{
	PMAPS pMap = NULL;
	switch (type)
	{
	case ALL:
		pMap = readmaps_all();
		break;
	case B_BAD:
		pMap = readmaps_bad();
		break;
	case V:
		pMap = readmaps_v();
		break;
	case C_ALLOC:
		pMap = readmaps_c_alloc();
		break;
	case C_BSS:
		pMap = readmaps_c_bss();
		break;
	case CODE_APP:
		pMap = readmaps_code_app();
		break;
	case C_DATA:
		pMap = readmaps_c_data();
		break;
	case C_HEAP:
		pMap = readmaps_c_heap();
		break;
	case JAVA_HEAP:
		pMap = readmaps_java_heap();
		break;
	case A_ANONMYOUS:
		pMap = readmaps_a_anonmyous();
		break;
	case CODE_SYSTEM:
		pMap = readmaps_code_system();
		break;
	case STACK:
		pMap = readmaps_stack();
		break;
	case ASHMEM:
		pMap = readmaps_ashmem();
		break;
	default:
		printf("\033[32;1mYou Select A NULL Type!\n");
		break;
	}
	if (pMap == NULL)
	{
		return 0;
	}
	return pMap;
}
PMAPS readmaps_all()
{
	PMAPS pHead = NULL;
	PMAPS pNew;
	PMAPS pEnd;
	pEnd = pNew = (PMAPS) malloc(LEN);
	FILE *fp;
	int i = 0, flag = 1;
	char lj[64], buff[256];
	int pid = getPID(bm);
	sprintf(lj, "/proc/%d/maps", pid);
	fp = fopen(lj, "r");
	if (fp == NULL)
	{
		puts("内存读取失败!");
		return NULL;
	}
	while (!feof(fp))
	{
		fgets(buff, sizeof(buff), fp);	
		if (strstr(buff, "rw") != NULL && !feof(fp))
		{
			sscanf(buff, "%lx-%lx", &pNew->addr, &pNew->taddr);
			flag = 1;
		}
		else
		{
			flag = 0;
		}
		if (flag == 1)
		{
			i++;
			if (i == 1)
			{
				pNew->next = NULL;
				pEnd = pNew;
				pHead = pNew;
			}
			else
			{
				pNew->next = NULL;
				pEnd->next = pNew;
				pEnd = pNew;
			}
			pNew = (PMAPS) malloc(LEN);	
		}
	}
	free(pNew);					
	fclose(fp);					
	return pHead;
}
PMAPS readmaps_bad()
{
	PMAPS pHead = NULL;
	PMAPS pNew = NULL;
	PMAPS pEnd = NULL;
	pEnd = pNew = (PMAPS) malloc(LEN);
	FILE *fp;
	int i = 0, flag = 1;
	char lj[64], buff[256];
	int pid = getPID(bm);
	sprintf(lj, "/proc/%d/maps", pid);
	fp = fopen(lj, "r");
	if (fp == NULL)
	{
		puts("内存读取失败!");
		return NULL;
	}
	while (!feof(fp))
	{
		fgets(buff, sizeof(buff), fp);
		if (strstr(buff, "rw") != NULL && !feof(fp) && strstr(buff, "kgsl-3d0"))
		{
			sscanf(buff, "%lx-%lx", &pNew->addr, &pNew->taddr);
			flag = 1;
		}
		else
		{
			flag = 0;
		}
		if (flag == 1)
		{
			i++;
			if (i == 1)
			{
				pNew->next = NULL;
				pEnd = pNew;
				pHead = pNew;
			}
			else
			{
				pNew->next = NULL;
				pEnd->next = pNew;
				pEnd = pNew;
			}
			pNew = (PMAPS) malloc(LEN);
		}
	}
	free(pNew);
	fclose(fp);
	return pHead;
}
PMAPS readmaps_v()
{
	PMAPS pHead = NULL;
	PMAPS pNew = NULL;
	PMAPS pEnd = NULL;
	pEnd = pNew = (PMAPS) malloc(LEN);
	FILE *fp;
	int i = 0, flag = 1;
	char lj[64], buff[256];
	int pid = getPID(bm);
	sprintf(lj, "/proc/%d/maps", pid);
	fp = fopen(lj, "r");
	if (fp == NULL)
	{
		puts("分析失败");
		return NULL;
	}
	while (!feof(fp))
	{
		fgets(buff, sizeof(buff), fp);
		if (strstr(buff, "rw") != NULL && !feof(fp) && strstr(buff, "/dev/kgsl-3d0"))
		{
			sscanf(buff, "%lx-%lx", &pNew->addr, &pNew->taddr);
			flag = 1;
		}
		else
		{
			flag = 0;
		}
		if (flag == 1)
		{
			i++;
			if (i == 1)
			{
				pNew->next = NULL;
				pEnd = pNew;
				pHead = pNew;
			}
			else
			{
				pNew->next = NULL;
				pEnd->next = pNew;
				pEnd = pNew;
			}
			pNew = (PMAPS) malloc(LEN);
		}
	}
	free(pNew);
	fclose(fp);
	return pHead;
}
PMAPS readmaps_c_alloc()
{
	PMAPS pHead = NULL;
	PMAPS pNew = NULL;
	PMAPS pEnd = NULL;
	pEnd = pNew = (PMAPS) malloc(LEN);
	FILE *fp;
	int i = 0, flag = 1;
	char lj[64], buff[256];
	int pid = getPID(bm);
	sprintf(lj, "/proc/%d/maps", pid);
	fp = fopen(lj, "r");
	if (fp == NULL)
	{
		puts("内存读取失败!");
		return NULL;
	}
	while (!feof(fp))
	{
		fgets(buff, sizeof(buff), fp);
		if (strstr(buff, "rw") != NULL && !feof(fp) && strstr(buff, "[anon:libc_malloc]"))
		{
			sscanf(buff, "%lx-%lx", &pNew->addr, &pNew->taddr);
			flag = 1;
		}
		else
		{
			flag = 0;
		}
		if (flag == 1)
		{
			i++;
			if (i == 1)
			{
				pNew->next = NULL;
				pEnd = pNew;
				pHead = pNew;
			}
			else
			{
				pNew->next = NULL;
				pEnd->next = pNew;
				pEnd = pNew;
			}
			pNew = (PMAPS) malloc(LEN);
		}
	}
	free(pNew);
	fclose(fp);
	return pHead;
}
PMAPS readmaps_c_bss()
{
	PMAPS pHead = NULL;
	PMAPS pNew = NULL;
	PMAPS pEnd = NULL;
	pEnd = pNew = (PMAPS) malloc(LEN);
	FILE *fp;
	int i = 0, flag = 1;
	char lj[64], buff[256];
	int pid = getPID(bm);
	sprintf(lj, "/proc/%d/maps", pid);
	fp = fopen(lj, "r");
	if (fp == NULL)
	{
		puts("内存读取失败!");
		return NULL;
	}
	while (!feof(fp))
	{
		fgets(buff, sizeof(buff), fp);
		if (strstr(buff, "rw") != NULL && !feof(fp) && strstr(buff, "[anon:.bss]"))
		{
			sscanf(buff, "%lx-%lx", &pNew->addr, &pNew->taddr);
			flag = 1;
		}
		else
		{
			flag = 0;
		}
		if (flag == 1)
		{
			i++;
			if (i == 1)
			{
				pNew->next = NULL;
				pEnd = pNew;
				pHead = pNew;
			}
			else
			{
				pNew->next = NULL;
				pEnd->next = pNew;
				pEnd = pNew;
			}
			pNew = (PMAPS) malloc(LEN);
		}
	}
	free(pNew);
	fclose(fp);
	return pHead;
}
PMAPS readmaps_code_app()
{
	PMAPS pHead = NULL;
	PMAPS pNew = NULL;
	PMAPS pEnd = NULL;
	pEnd = pNew = (PMAPS) malloc(LEN);
	FILE *fp;
	int i = 0, flag = 1;
	char lj[64], buff[256];
	int pid = getPID(bm);
	sprintf(lj, "/proc/%d/maps", pid);
	fp = fopen(lj, "r");
	if (fp == NULL)
	{
		puts("内存读取失败!");
		return NULL;
	}
	while (!feof(fp))
	{
		fgets(buff, sizeof(buff), fp);
		if (strstr(buff, "r-xp") != NULL && !feof(fp) && strstr(buff, "/data/app/"))
		{
			sscanf(buff, "%lx-%lx", &pNew->addr, &pNew->taddr);
			flag = 1;
		}
		else
		{
			flag = 0;
		}
		if (flag == 1)
		{
			i++;
			if (i == 1)
			{
				pNew->next = NULL;
				pEnd = pNew;
				pHead = pNew;
			}
			else
			{
				pNew->next = NULL;
				pEnd->next = pNew;
				pEnd = pNew;
			}
			pNew = (PMAPS) malloc(LEN);
		}
	}
	free(pNew);
	fclose(fp);
	return pHead;
}
PMAPS readmaps_c_data()
{
    PMAPS pHead = NULL;
    PMAPS pNew = NULL;
    PMAPS pEnd = NULL;
    FILE *fp;
    char lj[64], buff[512];
    char perms[16];
    int pid = getPID(bm);
    if (pid == 0) return NULL;
    sprintf(lj, "/proc/%d/maps", pid);
    fp = fopen(lj, "r");
    if (fp == NULL)
    {
        puts("内存读取失败!");
        return NULL;
    }
    while (fgets(buff, sizeof(buff), fp) != NULL)
    {
        if (strstr(buff, bm) != NULL)
        {
            long start_addr = 0;
            long end_addr = 0;
            long file_offset = 0;
            if (sscanf(buff, "%lx-%lx %15s %lx", &start_addr, &end_addr, perms, &file_offset) == 4)
            {
                if (file_offset == 0)
                {
                    pNew = (PMAPS) malloc(LEN);
                    if (pNew == NULL) break;
                    pNew->addr = start_addr;
                    pNew->taddr = end_addr;
                    pNew->next = NULL;
                    if (pHead == NULL)
                    {
                        pHead = pNew;
                        pEnd = pNew;
                    }
                    else
                    {
                        pEnd->next = pNew;
                        pEnd = pNew;
                    }
                }
            }
        }
    }
    fclose(fp);
    return pHead;
}
PMAPS readmaps_c_heap()
{
	PMAPS pHead = NULL;
	PMAPS pNew = NULL;
	PMAPS pEnd = NULL;
	pEnd = pNew = (PMAPS) malloc(LEN);
	FILE *fp;
	int i = 0, flag = 1;
	char lj[64], buff[256];
	int pid = getPID(bm);
	sprintf(lj, "/proc/%d/maps", pid);
	fp = fopen(lj, "r");
	if (fp == NULL)
	{
		puts("内存读取失败!");
		return NULL;
	}
	while (!feof(fp))
	{
		fgets(buff, sizeof(buff), fp);
		if (strstr(buff, "rw") != NULL && !feof(fp) && strstr(buff, "[heap]"))
		{
			sscanf(buff, "%lx-%lx", &pNew->addr, &pNew->taddr);
			flag = 1;
		}
		else
		{
			flag = 0;
		}
		if (flag == 1)
		{
			i++;
			if (i == 1)
			{
				pNew->next = NULL;
				pEnd = pNew;
				pHead = pNew;
			}
			else
			{
				pNew->next = NULL;
				pEnd->next = pNew;
				pEnd = pNew;
			}
			pNew = (PMAPS) malloc(LEN);
		}
	}
	free(pNew);
	fclose(fp);
	return pHead;
}
PMAPS readmaps_java_heap()
{
	PMAPS pHead = NULL;
	PMAPS pNew = NULL;
	PMAPS pEnd = NULL;
	pEnd = pNew = (PMAPS) malloc(LEN);
	FILE *fp;
	int i = 0, flag = 1;
	char lj[64], buff[256];
	int pid = getPID(bm);
	sprintf(lj, "/proc/%d/maps", pid);
	fp = fopen(lj, "r");
	if (fp == NULL)
	{
		puts("内存读取失败!");
		return NULL;
	}
	while (!feof(fp))
	{
		fgets(buff, sizeof(buff), fp);
		if (strstr(buff, "rw") != NULL && !feof(fp) && strstr(buff, "/dev/ashmem/"))
		{
			sscanf(buff, "%lx-%lx", &pNew->addr, &pNew->taddr);
			flag = 1;
		}
		else
		{
			flag = 0;
		}
		if (flag == 1)
		{
			i++;
			if (i == 1)
			{
				pNew->next = NULL;
				pEnd = pNew;
				pHead = pNew;
			}
			else
			{
				pNew->next = NULL;
				pEnd->next = pNew;
				pEnd = pNew;
			}
			pNew = (PMAPS) malloc(LEN);
		}
	}
	free(pNew);
	fclose(fp);
	return pHead;
}
PMAPS readmaps_a_anonmyous()
{
    PMAPS pHead = NULL;
    PMAPS pEnd = NULL;
    FILE *fp;
    char lj[64], buff[256];
    int pid = getPID(bm);
    int count = 0;
    sprintf(lj, "/proc/%d/maps", pid);
    fp = fopen(lj, "r");
    if (fp == NULL)
    {
        return NULL;
    }
    while (fgets(buff, sizeof(buff), fp))
    {    
        if (strstr(buff, "rw-p") != NULL && strchr(buff, '/') == NULL && strchr(buff, '[') == NULL)
        {
            unsigned long start, end;
            sscanf(buff, "%lx-%lx", &start, &end);        
            long size = end - start;
              if (size < 150000) { 
              continue;
              }
            unsigned long mid_addr = start + (end - start) / 1;
            PMAPS pNew = (PMAPS)malloc(LEN);
            if (pNew == NULL) break;
            pNew->addr = start;
            pNew->taddr = mid_addr; 
            pNew->next = NULL;
            if (pHead == NULL)
            {
                pHead = pNew;
                pEnd = pNew;
            }
            else
            {
                pEnd->next = pNew;
                pEnd = pNew;
            }
            count++;
        }
    }
    fclose(fp);
    return pHead;
}
PMAPS readmaps_code_system()
{
	PMAPS pHead = NULL;
	PMAPS pNew = NULL;
	PMAPS pEnd = NULL;
	pEnd = pNew = (PMAPS) malloc(LEN);
	FILE *fp;
	int i = 0, flag = 1;
	char lj[64], buff[256];
	int pid = getPID(bm);
	sprintf(lj, "/proc/%d/maps", pid);
	fp = fopen(lj, "r");
	if (fp == NULL)
	{
		puts("内存读取失败!");
		return NULL;
	}
	while (!feof(fp))
	{
		fgets(buff, sizeof(buff), fp);
		if (strstr(buff, "rw") != NULL && !feof(fp) && strstr(buff, "/system"))
		{
			sscanf(buff, "%lx-%lx", &pNew->addr, &pNew->taddr);
			flag = 1;
		}
		else
		{
			flag = 0;
		}
		if (flag == 1)
		{
			i++;
			if (i == 1)
			{
				pNew->next = NULL;
				pEnd = pNew;
				pHead = pNew;
			}
			else
			{
				pNew->next = NULL;
				pEnd->next = pNew;
				pEnd = pNew;
			}
			pNew = (PMAPS) malloc(LEN);
		}
	}
	free(pNew);
	fclose(fp);
	return pHead;
}
PMAPS readmaps_stack()
{
	PMAPS pHead = NULL;
	PMAPS pNew = NULL;
	PMAPS pEnd = NULL;
	pEnd = pNew = (PMAPS) malloc(LEN);
	FILE *fp;
	int i = 0, flag = 1;
	char lj[64], buff[256];
	int pid = getPID(bm);
	sprintf(lj, "/proc/%d/maps", pid);
	fp = fopen(lj, "r");
	if (fp == NULL)
	{
		puts("内存读取失败!");
		return NULL;
	}
	while (!feof(fp))
	{
		fgets(buff, sizeof(buff), fp);
		if (strstr(buff, "rw") != NULL && !feof(fp) && strstr(buff, "[stack]"))
		{
			sscanf(buff, "%lx-%lx", &pNew->addr, &pNew->taddr);
			flag = 1;
		}
		else
		{
			flag = 0;
		}
		if (flag == 1)
		{
			i++;
			if (i == 1)
			{
				pNew->next = NULL;
				pEnd = pNew;
				pHead = pNew;
			}
			else
			{
				pNew->next = NULL;
				pEnd->next = pNew;
				pEnd = pNew;
			}
			pNew = (PMAPS) malloc(LEN);
		}
	}
	free(pNew);
	fclose(fp);
	return pHead;
}
PMAPS readmaps_ashmem()
{
	PMAPS pHead = NULL;
	PMAPS pNew = NULL;
	PMAPS pEnd = NULL;
	pEnd = pNew = (PMAPS) malloc(LEN);
	FILE *fp;
	int i = 0, flag = 1;
	char lj[64], buff[256];
	int pid = getPID(bm);
	sprintf(lj, "/proc/%d/maps", pid);
	fp = fopen(lj, "r");
	if (fp == NULL)
	{
		puts("内存读取失败!");
		return NULL;
	}
	while (!feof(fp))
	{
		fgets(buff, sizeof(buff), fp);
		if (strstr(buff, "rw") != NULL && !feof(fp) && strstr(buff, "/dev/ashmem/")
			&& !strstr(buff, "dalvik"))
		{
			sscanf(buff, "%lx-%lx", &pNew->addr, &pNew->taddr);
			flag = 1;
		}
		else
		{
			flag = 0;
		}
		if (flag == 1)
		{
			i++;
			if (i == 1)
			{
				pNew->next = NULL;
				pEnd = pNew;
				pHead = pNew;
			}
			else
			{
				pNew->next = NULL;
				pEnd->next = pNew;
				pEnd = pNew;
			}
			pNew = (PMAPS) malloc(LEN);
		}
	}
	free(pNew);
	fclose(fp);
	return pHead;
}
