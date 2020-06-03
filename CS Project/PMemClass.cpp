#include "PMemHeader.h"

using namespace std;

PainMem::PainMem() {
	//Constructor For Class, Do Not Remove!
}

PainMem::~PainMem() {
	//De-Constructor
	//Clean Up! (Close Handle - Not Needed Anymore)
	CloseHandle(hProcess);
}





#pragma region Memory Functions

/* This Function Will Return A Handle To The Process So We Can Write & Read Memeory From The Process. */

bool PainMem::Module(LPSTR ModuleName, DWORD &output) {
	//Variables
	HANDLE hModule = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, dwPID); //Take A Module Snapshot Of The Process (Grab All Loaded Modules)
	MODULEENTRY32 mEntry; //Declare Module Entry Structure
	mEntry.dwSize = sizeof(mEntry); //Declare Structure Size And Populate It With Loaded Modules

									//Scan For Module By Name
	do
		if (!strcmp(mEntry.szModule, ModuleName))
		{
			CloseHandle(hModule);
			output = (DWORD)mEntry.modBaseAddr;
			return true;
		}
	while (Module32Next(hModule, &mEntry));

	
	return false;
}


bool PainMem::Process(char* ProcessName) {

	//Variables
	HANDLE hPID = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL); //Snapshot To View All Active Processes
	PROCESSENTRY32 ProcEntry;
	ProcEntry.dwSize = sizeof(ProcEntry); //Declare Structure Size And Populate It

										  //Loop Through All Running Processes To Find Process
	do
		if (!strcmp(ProcEntry.szExeFile, ProcessName))
		{
			//Store Process ID
			dwPID = ProcEntry.th32ProcessID;
			CloseHandle(hPID);

			//Give Our Handle All Access Rights 
			hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwPID);
			return true;
		}
	while (Process32Next(hPID, &ProcEntry));
	return false;
}


/* This Function Is Similiar To Cheat Engine's Code Injection Function, It's Able To Create JMP's
/* To A Codecave And Write New Memory. Untested CALL Command */



/* Returns The Base Address Of The Specified Module Inside The Target Process
/* e.g.[ Module("client.dll"); ]. */

#pragma endregion