#include <Windows.h>
#include <iostream>
#include <TlHelp32.h>
#include <fstream>
#include <string>
#ifndef _C_MEMORY_
#define _C_MEMORY_

class PMemory
{
private:
	HANDLE _process;
	DWORD _pID;
	DWORD _procSize;

public:
	PMemory();
	~PMemory();

	bool Attach(const std::string& pName);
	void DeAttach();


	virtual DWORD Module(LPSTR ModuleName);

	HANDLE GetProcess() const;
	DWORD GetProcessID() const;
	DWORD GetProcessSize() const;

	bool Read(DWORD dwAddress, LPVOID lpBuffer, DWORD dwSize) const;
	bool Write(DWORD dwAddress, LPCVOID lpBuffer, DWORD dwSize) const;

	template<typename T>
	T Read(DWORD dwAddress, const T& tDefault = T())
	{
		T tRet;
		if (!Read(dwAddress, &tRet, sizeof(T)))
			return tDefault;

		return tRet;
	}
	template<typename T>
	T ReadXOR(DWORD dwAddress, const T& tDefault = T())
	{
		int tRet;
		if (!Read(dwAddress, &tRet, sizeof(T)))
			return tDefault;

		tRet ^= dwAddress - 0x30;
		return reinterpret_cast< T& >(tRet);
	}
	template<typename T>
	bool Write(DWORD dwAddress, const T& tValue)
	{
		return Write(dwAddress, &tValue, sizeof(T));
	}
};
extern PMemory* g_cMemory;
#endif

#pragma region ClickFunction

#pragma endregion