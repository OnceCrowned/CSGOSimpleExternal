
#include "PineappleMemory.h"
int PineapplesAdding();
PMemory* g_cMemory = new PMemory();

PMemory::PMemory() : _process(nullptr), _pID(NULL), _procSize(NULL)
{

}

PMemory::~PMemory()
{
	DeAttach();
}

int PineapplesAdding(int x, int y)
{
	int c = x + y;
	return c;
}
bool PMemory::Attach(const std::string& pName)
{
	DeAttach();
	if (pName.empty())
	{
		printf("Process ID Not Matching Game\n");
		return false;
	}

	auto Handle = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);
	PROCESSENTRY32 pineappleEntry; pineappleEntry.dwSize = sizeof(pineappleEntry);
	do
	{
		if (!strcmp(pineappleEntry.szExeFile, pName.c_str()))
		{

			_pID = pineappleEntry.th32ProcessID;
			_process = OpenProcess(PROCESS_ALL_ACCESS, false, _pID);
			_procSize = pineappleEntry.dwSize;

			CloseHandle(Handle);
			return true;
		}
	} while (Process32Next(Handle, &pineappleEntry));
	return false;
}

void PMemory::DeAttach()
{
	if (_process)
		CloseHandle(_process);
	_process = nullptr;
	_pID = NULL;
	_procSize = NULL;
}

HANDLE PMemory::GetProcess() const
{
	return _process;
}
DWORD PMemory::GetProcessID() const
{
	return _pID;
}
DWORD PMemory::GetProcessSize() const
{
	return _procSize;
}

DWORD PMemory::Module(LPSTR ModuleName)
{
	HANDLE hModule = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, _pID);
	MODULEENTRY32 mEntry;
	mEntry.dwSize = sizeof(mEntry);

	do
		if (!strcmp(mEntry.szModule, ModuleName))
		{
			
			CloseHandle(hModule);
			return (DWORD)mEntry.modBaseAddr;
		}
	while (Module32Next(hModule, &mEntry));

	printf("Module Name Matching Game\n");
	std::cout << "\nCouldn't find client.dll, retrying...\n";
	return 0;
}
bool PMemory::Read(DWORD dwAddress, LPVOID lpBuffer, DWORD dwSize) const
{
	SIZE_T Out = NULL;
	return (ReadProcessMemory(_process, reinterpret_cast< LPVOID >(dwAddress), lpBuffer, dwSize, &Out) == TRUE);
}

bool PMemory::Write(DWORD dwAddress, LPCVOID lpBuffer, DWORD dwSize) const
{
	SIZE_T Out = NULL;
	if (dwAddress == NULL)
		return false;

	return (WriteProcessMemory(_process, reinterpret_cast< LPVOID >(dwAddress), lpBuffer, dwSize, &Out) == TRUE);
}
