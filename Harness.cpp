#include <Windows.h>
#include <stdio.h>
#include <conio.h>
#include <iostream>

typedef void(*pFunction1)(int);
typedef void(*pFunction2)(wchar_t*);
typedef void(*pFunction3)(wchar_t*,int,int,int,int,int,int,int,int,int,int,int);
typedef void(*pFunction4)(int,wchar_t*);
pFunction1 Function1;
pFunction2 Function2;
pFunction3 Function3;
pFunction4 Function4;

wchar_t tempPath[MAX_PATH];
wchar_t TempPdfToBeCreated[520] = {0};
HMODULE hDll = NULL;
DWORD Result = 0;

DWORD unkonwnadd;
DWORD arr[3];
int UselessValue = 1094795585;
unsigned int thispointer;
unsigned int vtable;

wchar_t FilePath[7760] = {0};
wchar_t *fileName = L"Converted.pdf";
wchar_t *printerinfo = L"Foxit Reader Printer Version 9.7.1.2227";
wchar_t *printerName = L"Foxit Reader PDF Printer";
wchar_t *language = L"en-US";	

extern "C" __declspec(dllexport)  int main(int argc, char** argv);
extern "C" __declspec(dllexport)  int fuzzme(char *);

typedef unsigned int (__stdcall *CreateFXPDFConvertor)(void);
typedef void (*FunctionPointer)();
typedef void (*FunctionPointer2)(void*);

CreateFXPDFConvertor obj = NULL;

void dealloccallbackfunc(void* mem)
{
	//No code because we don't allocate any new memory
}

void alloccallbackfunc()
{
	__asm
	{
		mov eax, ecx	//ECX is the Heap Address of FilePath Variable, EAX is the retun value, and it is expected by the Convertor Plugin. I think foxit developers add this to tackle our fuzzing effort.	
	};
}

typedef struct{
	FunctionPointer2 funcpointer1;
	FunctionPointer funcpointer2;
}MyUnkownType;

wchar_t* charToWChar(const char* text)
{
    size_t size = strlen(text) + 1;
    wchar_t* wa = (wchar_t*)malloc(sizeof(wchar_t) * size);
    mbstowcs(wa, text, size);
    return wa;
}

int fuzzme(char *Path)
{
	thispointer = (unsigned int)(obj)();			
	vtable = *(unsigned int*)thispointer;
	//printf("\n\t[x] vtable.................................0x%04x",  vtable);
	Function3 = (pFunction3)(*(unsigned int*)(vtable));
	Function1 = (pFunction1)(*(unsigned int*)(vtable + 4 * 1));
	Function2 = (pFunction2)(*(unsigned int*)(vtable + 4 * 2));
	Function4 = (pFunction4)(*(unsigned int*)(vtable + 4 * 3));
	//printf("\n\t[x] Calling 0x%04x......................",  Function4);
	__asm
	{
		mov ecx,thispointer	
		mov edx,vtable
		push language
		push 2			
		call Function4
		mov Result,eax
	}	
	if(Result>=0)
	{
		//printf("Successful!\n\t[x] Calling 0x%04x......................",  Function1);
		__asm
		{
			mov ecx,thispointer	
			mov edx,vtable
			push 2			
			call Function1
			mov Result,eax
		}		
		if(Result>=0)
		{
			//printf("Successful!\n\t[x] Calling 0x%04x......................",  Function2);
			__asm
			{
				mov ecx,thispointer	
				mov edx,vtable
				lea edi,printerName
				push edi			
				call Function2
				mov Result,eax
			}			
			if(Result>=0)
			{	
				//printf("Successful!\n\t[x] Calling 0x%04x......................",  Function3);
				wchar_t * filepath = charToWChar(Path);
				memcpy(FilePath + 0x1624/2,filepath,wcslen(filepath)*2);			
				__asm
				{				
					mov esi,thispointer	
					mov edx,Function3
					mov eax,[esi]
					mov edx,[eax]
					push 0
					push 0	
					push UselessValue
					push UselessValue
					push UselessValue
					push UselessValue
					push UselessValue
					push UselessValue
					push UselessValue
					push UselessValue
					lea ecx,unkonwnadd
					push ecx
					lea ecx,FilePath
					push ecx
					mov ecx,esi					
					call edx
				}
			}
			else
			{
				//printf("Unsuccessful!\n");
				return 1;
			}
		}
		else
		{
			//printf("Unsuccessful!\n");
			return 1;
		}
	}
	else
	{
		//printf("Unsuccessful!\n");
		return 1;
	}
	return 0;
}

int main(int argc, char** argv)
{
	if(argc < 2)
	{
		printf("Usage %s: <input file>\n",argv[0]);
		return 1;
	}	
	wcscpy(tempPath,L"C:\\Temp\\");//Converted.pdf Destination
			
	memcpy(TempPdfToBeCreated,tempPath,wcslen(tempPath)*2);
	memcpy(TempPdfToBeCreated + wcslen(tempPath),fileName,wcslen(fileName)*2);
	//printf("\t[x] Temp Path -> %ls\n",TempPdfToBeCreated);
	
	//printf("\t[x] Loading ConvertToPDF_x86.dll...........");
	hDll = LoadLibraryA("ConvertToPDF_x86.dll");

	if(hDll > 0)
	{		
		//printf("Successful!\n\t[x] Calling CreateFXPDFConvertor...........");		
		obj =  reinterpret_cast<CreateFXPDFConvertor>(GetProcAddress(hDll, "CreateFXPDFConvertor"));
		if(obj != NULL)
		{
			//printf("Successful!");
			MyUnkownType tt;
			tt.funcpointer1 = &dealloccallbackfunc;
			tt.funcpointer2 = &alloccallbackfunc;
			MyUnkownType *t2 = &tt;	
            		arr[0] = DWORD(t2);
			arr[1] = 0XB; //Image file will be converted
			arr[2] = 0;	
			memcpy(FilePath,arr,sizeof(arr));
			memcpy(FilePath + 0xb68/2,printerinfo,wcslen(printerinfo)*2);
			memcpy(FilePath + 0x182C/2,TempPdfToBeCreated,sizeof(TempPdfToBeCreated));	
							
			fuzzme(argv[1]);	
		}
	}
	return 0;
}
