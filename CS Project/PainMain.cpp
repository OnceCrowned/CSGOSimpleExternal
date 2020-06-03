#include <Windows.h>
#include <iostream>
#include <string>
#include <math.h>
#include <TlHelp32.h>
#include "PineappleMemory.h"
#include "PainOffsets.h"
#include <cstdint>
#include <stdint.h>
#include <vector>
#include <windows.h>
#include <windows.h>
#include <tlhelp32.h>
#include <iostream>
#include <string>
#include "Math.h"
#define PI 3.14159265358979
#pragma region Funcs
float DistanceFormula(float enemyxcoord, float myxcoord, float enemyycoord, float myycoord, float enemyzcoord, float myzcoord);
void PineappleRadar();
void InitializeOnMatch();
void doRadar(DWORD ent, bool check);
void shutOffRadar();
#pragma endregion
float enemyCoords[3];
DWORD Engine;
DWORD Client;

bool TriggerActive = false;

float myPosition[3];
float x;
float y;
float z;

float enemysPositions[3];
float enemy_x;
float enemy_y;
float enemy_z;
float Distance;


DWORD LocalBase;
int orCrossHairID;
int isInCross;
DWORD entInCH;
int MyTeamID;
int LifeState;
int EnemyTeam;
bool TrigActive;
bool NoCheatsActive = true;
int CheatsActive = 0;
PMemory mem;

//const double calculateDistance(const Vector3& vec1, const Vector3& vec2);
/*
struct Vector3 {
	double x, y, z;

	Vector3(double _x, double _y, double _z)
		: x(_x), y(_y), z(_z) {
	}
};
*/
void GrabbingEnemyCoords();
void GetMyCoords();


void FunTapping();
void Startup();
int main()
{
	Startup();
	if (!mem.Attach("csgo.exe"))
		std::cout << "Can't find Process\n";
	while (1)
	{
		
		switch (CheatsActive)
		{
		case 0:
			//cout << "Main Menu" << endl; 
			//system("cls");
			if (GetAsyncKeyState(0x45))
				CheatsActive++;
			else if (GetAsyncKeyState(0x43))
				CheatsActive += 2;
			else if (GetAsyncKeyState(VK_DELETE))
				exit(0);
			else if (GetAsyncKeyState(VK_F9))
				CheatsActive += 3;

			
			Sleep(1);
			break;
		case 1:
			cout << "Trig starting" << endl;
			FunTapping();
			if (!GetAsyncKeyState(0x45))
				CheatsActive--;
			else if (GetAsyncKeyState(VK_DELETE))
				exit(0);
			break;
		case 2:
			Sleep(10);
			//cout << "Checking Positions :)" << endl;
			PineappleRadar();
			if (!GetAsyncKeyState(0x43))
			{
				shutOffRadar();
				CheatsActive -= 2;
			}
			 if (GetAsyncKeyState(VK_DELETE))
				exit(0);
			break;
		case 3:
			InitializeOnMatch();
			CheatsActive -= 3;
		default:
			printf("Switch Statement Broken, out of set domain\n");
			break;
		}


		if (GetAsyncKeyState(VK_DELETE))
			exit(0);
		/*
		while (1)
		{
			FunTapping();
			GetMyCoords();
			if (!TriggerActive)
			{
				GrabbingEnemyCoords();
				//float theDistance = DistanceFormula(enemy_x, x, enemy_y, y, enemy_z, z);
				//std::cout << "Distance to enemy: " << theDistance << std::endl;
			}
			Sleep(1);



		}
		*/
		Sleep(1);
	}

}

void Startup()
{
	SetConsoleTitle("Pineapples are a cool fruit, right?");
	mem.Attach("csgo.exe");
	Engine = mem.Module("engine.dll");
	Client = mem.Module("client_panorama.dll");




	while (!mem.Attach("csgo.exe"))
	{
		system("cls");

		std::cout << "Searching for CS:GO..." << std::endl;;
		Sleep(5000);
		std::cout << "CS:GO Not Found!" << std::endl;
		Sleep(4000);
	}



}

void InitializeOnMatch()
{

}
void GetMyCoords()
{
	float myPosition[3];
	DWORD LocalBase = mem.Read<DWORD>(Client + offsets::LocalPlayer);
	int MyTeamID = mem.Read<int>(LocalBase + offsets::dwTeam);
	myPosition[0] = mem.Read<float>(LocalBase + offsets::m_vecOrigin);
	myPosition[1] = mem.Read<float>(LocalBase + offsets::m_vecOrigin + 0x4);
	myPosition[2] = mem.Read<float>(LocalBase + offsets::m_vecOrigin + 0x8);

	x = myPosition[0];
	y = myPosition[1];
	z = myPosition[2];

}

void GrabbingEnemyCoords()
{
	float EnemyPos[3];

	for (int i = 0; i < 32; i++)
	{

		uintptr_t CEntityList = mem.Read<uintptr_t>(Client + offsets::EntityList + (i * 0x10));

		DWORD Health = mem.Read<int>(CEntityList + offsets::Health);
		DWORD TeamNum = mem.Read<int>(CEntityList + offsets::dwTeam);
		DWORD Dormant = mem.Read<int>(CEntityList + offsets::Dormant);



		if (CEntityList != 0 && Health > 0 && TeamNum != MyTeamID && TeamNum != 2)
		{
			//std::cout << CEntityList << std::endl;
			EnemyPos[0] = mem.Read<float>(CEntityList + offsets::m_vecOrigin);
			EnemyPos[1] = mem.Read<float>(CEntityList + offsets::m_vecOrigin + 0x4);
			EnemyPos[2] = mem.Read<float>(CEntityList + offsets::m_vecOrigin + 0x8);

			enemy_x = EnemyPos[0];
			enemy_y = EnemyPos[1];
			enemy_z = EnemyPos[2];

			Distance = DistanceFormula(enemy_x, x, enemy_y, y, enemy_z, z);
			std::cout << "Distance to Enemy Number" << i+1 << ": " << Distance << endl;
			
		}
		Sleep(1);
	}
}


void FunTapping()
{
	LocalBase = mem.Read<DWORD>(Client + offsets::LocalPlayer);
	orCrossHairID = mem.Read<int>(LocalBase + offsets::CrosshairId);
	MyTeamID = mem.Read<int>(LocalBase + offsets::dwTeam);
	isInCross = mem.Read<int>(LocalBase + (offsets::CrosshairId - 1));
	entInCH = mem.Read<DWORD>(Client + offsets::EntityList + (orCrossHairID - 1) * 0x10);

	LifeState = mem.Read<int>(entInCH + offsets::Health);
	EnemyTeam = mem.Read<int>(entInCH + offsets::dwTeam);

	if (LifeState > 0 && EnemyTeam != MyTeamID) 
	{
		//std::cout << "Working";
		//TriggerActive = true;
	//	std::cout << "Working 1 test" << std::endl;


				Sleep(50);
				mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);
				Sleep(1);
				mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
				Sleep(1);
				//TrigActive = false;
			//	NoCheatsActive = true;
	}
}



float DistanceFormula(float enemyxcoord, float myxcoord, float enemyycoord, float myycoord, float enemyzcoord, float myzcoord)
{

		float x = pow(enemyxcoord - myxcoord, 2);
		float y = pow(enemyycoord - myycoord, 2);
		float z = pow(enemyzcoord - myzcoord, 2);

		return sqrt(x + y + z);

}

void PineappleRadar()
{

	for (int i = 1; i < 65; i++)
	{
		DWORD myEntity =  mem.Read<DWORD>(Client + offsets::dwEntityList + i * 0x10);

		doRadar(myEntity, true);
	}

	Sleep(5);
}
struct myGlow
{
	DWORD dwEntity = 0;
	float r = 0;
	float g = 0;
	float b = 0;
	float a = 0;
	char pad_0014[4];
	float flUnknown = 0.f;
	float flBloomAmount = 0;
	float flLocalPlayerIsZeroPointThree = 0.f;
	bool bRenderWhenOccluded = false;
	bool bRenderWhenUnoccluded = false;
	bool bFullBloomRender = false;
	char pad_0027[1];
	int nFullBloomTestStencilValue = 0; // Needs to be zero.
	int nGlowStyle = 0;
	int nSplitScreenSlot = -1; // Should be -1.
	int nNextFreeSlot = 0;
}; myGlow glow;


void doRadar(DWORD ent, bool check)
{
			mem.Write<bool>(ent + offsets::bSpot, check);
			
}

void shutOffRadar()
{
	for (int i = 1; i < 65; i++)
	{
		DWORD shutOffEntity = mem.Read<DWORD>(Client + offsets::dwEntityList + i * 0x10);

		doRadar(shutOffEntity, false);
	}
}
/*
void NickGlow(float r, float g, float b)
{
	mem.Write<float>()
}
*/
/*
DWORD findClosestEnemy()
{
	vector<float> w2sHead;

	DWORD pToAim = NULL;
	double lowestDist = 999;

	DWORD myBase = mem.Read<DWORD>(Client + offsets::LocalPlayer);

	for (int i = 1; i < 65; i++)
	{
		auto base = mem.Read<DWORD>(Client + offsets::EntityList + i * 0x10);

		int enemyTeam = mem.Read<int>(base + offsets::dwTeam);
		int myTeam = mem.Read<int>(myBase + offsets::dwTeam);

		int enemyHealth = mem.Read<int>(base + offsets::Health);

		if (enemyTeam != myTeam && enemyHealth > 0)
		{
			WorldToScreen
		}
	}
}
*/