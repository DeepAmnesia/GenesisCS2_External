#pragma once
#include <iostream>
#include <Windows.h>
#include <vector>
#include <Tlhelp32.h>
#include <atlconv.h>
#define _is_invalid(v) if(v==NULL) return false
#define _is_invalid(v,n) if(v==NULL) return n

#define BLOCKMAXSIZE 409600



/*
	@Liv github.com/TKazer
*/

/// <summary>
/// 进程状态码
/// </summary>
enum StatusCode
{
	SUCCEED,
	FAILE_PROCESSID,
	FAILE_HPROCESS,
	FAILE_MODULE,
};

/// <summary>
/// 进程管理
/// </summary>
class ProcessManager 
{
private:

	bool   Attached = false;

public:

	HANDLE hProcess = 0;
	DWORD  ProcessID = 0;
	DWORD64  ModuleAddress = 0;

public:
	~ProcessManager()
	{
		//if (hProcess)
			//CloseHandle(hProcess);
	}
	BYTE* MemoryData;
	short Next[260];

	WORD GetSignatureArray(std::string Signature, WORD* SignatureArray)
	{
		int len = 0;
		WORD Length = Signature.length() / 3 + 1;

		for (int i = 0; i < Signature.length(); )
		{
			char num[2];
			num[0] = Signature[i++];
			num[1] = Signature[i++];
			i++;
			if (num[0] != '?' && num[1] != '?')
			{
				int sum = 0;
				WORD a[2];
				for (int i = 0; i < 2; i++)
				{
					if (num[i] >= '0' && num[i] <= '9')
						a[i] = num[i] - '0';
					else if (num[i] >= 'a' && num[i] <= 'z')
						a[i] = num[i] - 87;
					else if (num[i] >= 'A' && num[i] <= 'Z')
						a[i] = num[i] - 55;
				}
				sum = a[0] * 16 + a[1];
				SignatureArray[len++] = sum;
			}
			else
				SignatureArray[len++] = 256;
		}
		return Length;
	}

	void GetNext(short* next, WORD* Signature, WORD SignatureLength)
	{
		for (int i = 0; i < 260; i++)
			next[i] = -1;
		for (int i = 0; i < SignatureLength; i++)
			next[Signature[i]] = i;
	}

	void SearchMemoryBlock(HANDLE hProcess, WORD* Signature, WORD SignatureLength, DWORD64 StartAddress, DWORD size, std::vector<DWORD64>& ResultArray)
	{
		if (!ReadProcessMemory(hProcess, (LPCVOID)StartAddress, MemoryData, size, NULL))
			return;

		for (int i = 0, j, k; i < size;)
		{
			j = i; k = 0;

			for (; k < SignatureLength && j < size && (Signature[k] == MemoryData[j] || Signature[k] == 256); k++, j++);

			if (k == SignatureLength)
				ResultArray.push_back(StartAddress + i);

			if ((i + SignatureLength) >= size)
				return;

			int num = Next[MemoryData[i + SignatureLength]];
			if (num == -1)
				i += (SignatureLength - Next[256]);
			else
				i += (SignatureLength - num);
		}
	}
	/// <summary>
	/// 附加
	/// </summary>
	/// <param name="ProcessName">进程名</param>
	/// <returns>进程状态码</returns>
	StatusCode Attach(std::string ProcessName)
	{
		ProcessID = this->GetProcessID(ProcessName);
		_is_invalid(ProcessID, FAILE_PROCESSID);

		hProcess = OpenProcess(PROCESS_ALL_ACCESS | PROCESS_CREATE_THREAD, TRUE, ProcessID);
		_is_invalid(hProcess, FAILE_HPROCESS);

		ModuleAddress = reinterpret_cast<DWORD64>(this->GetProcessModuleHandle(ProcessName));
		_is_invalid(ModuleAddress, FAILE_MODULE);

		Attached = true;

		return SUCCEED;
	}

	/// <summary>
	/// 取消附加
	/// </summary>
	void Detach()
	{
		if (hProcess)
			CloseHandle(hProcess);
		hProcess = 0;
		ProcessID = 0;
		ModuleAddress = 0;
		Attached = false;
	}

	/// <summary>
	/// 判断进程是否激活状态
	/// </summary>
	/// <returns>是否激活状态</returns>
	bool IsActive()
	{
		if (!Attached)
			return false;
		DWORD ExitCode{};
		GetExitCodeProcess(hProcess, &ExitCode);
		return ExitCode == STILL_ACTIVE;
	}

	/// <summary>
	/// 读取进程内存
	/// </summary>
	/// <typeparam name="ReadType">读取类型</typeparam>
	/// <param name="Address">读取地址</param>
	/// <param name="Value">返回数据</param>
	/// <param name="Size">读取大小</param>
	/// <returns>是否读取成功</returns>
	template <typename ReadType>
	bool ReadMemory(DWORD64 Address, ReadType& Value, int Size)
	{
		_is_invalid(hProcess,false);
		_is_invalid(ProcessID, false);

		if (ReadProcessMemory(hProcess, reinterpret_cast<LPCVOID>(Address), &Value, Size, 0))
			return true;
		return false;
	}

	template <typename ReadType>
	bool ReadMemory(DWORD64 Address, ReadType& Value)
	{
		_is_invalid(hProcess, false);
		_is_invalid(ProcessID, false);

		if (ReadProcessMemory(hProcess, reinterpret_cast<LPCVOID>(Address), &Value, sizeof(ReadType), 0))
			return true;
		return false;
	}

	/// <summary>
	/// 写入进程内存
	/// </summary>
	/// <typeparam name="ReadType">写入类型</typeparam>
	/// <param name="Address">写入地址</param>
	/// <param name="Value">写入数据</param>
	/// <param name="Size">写入大小</param>
	/// <returns>是否写入成功</returns>
	template <typename ReadType>
	bool WriteMemory(DWORD64 Address, ReadType& Value, int Size)
	{
		_is_invalid(hProcess, false);
		_is_invalid(ProcessID, false);

		if (WriteProcessMemory(hProcess, reinterpret_cast<LPCVOID>(Address), &Value, Size, 0))
			return true;
		return false;
	}

	template <typename ReadType>
	bool WriteMemory(DWORD64 Address, ReadType& Value)
	{
		_is_invalid(hProcess, false);
		_is_invalid(ProcessID, false);

		if (WriteProcessMemory(hProcess, reinterpret_cast<LPVOID>(Address), &Value, sizeof(ReadType), 0))
			return true;
		return false;
	}


	inline std::vector<DWORD64> SearchMemory(std::string Signature, DWORD64 StartAddress, DWORD64 EndAddress)
	{
		MemoryData = new BYTE[BLOCKMAXSIZE];
		int i = 0;
		unsigned long BlockSize;
		MEMORY_BASIC_INFORMATION mbi;
		WORD SignatureLength = Signature.length() / 3 + 1;
		WORD* SignatureArray = new WORD[SignatureLength];
		std::vector<DWORD64>ResultArray;
		GetSignatureArray(Signature, SignatureArray);
		GetNext(Next, SignatureArray, SignatureLength);

		while (VirtualQueryEx(hProcess, (LPCVOID)StartAddress, &mbi, sizeof(mbi)) != 0)
		{
			i = 0;
			BlockSize = mbi.RegionSize;
			while (BlockSize >= BLOCKMAXSIZE)
			{
				SearchMemoryBlock(hProcess, SignatureArray, SignatureLength, StartAddress + (BLOCKMAXSIZE * i), BLOCKMAXSIZE, ResultArray);
				BlockSize -= BLOCKMAXSIZE; i++;
			}
			SearchMemoryBlock(hProcess, SignatureArray, SignatureLength, StartAddress + (BLOCKMAXSIZE * i), BlockSize, ResultArray);

			StartAddress += mbi.RegionSize;

			if (EndAddress != 0 && StartAddress > EndAddress)
				return ResultArray;
		}
		delete[] MemoryData;
		delete[] SignatureArray;
		return ResultArray;
	}

	DWORD64 TraceAddress(DWORD64 BaseAddress, std::vector<DWORD> Offsets)
	{
		_is_invalid(hProcess,0);
		_is_invalid(ProcessID,0);
		DWORD64 Address = 0;

		if (Offsets.size() == 0)
			return BaseAddress;

		if (!ReadMemory<DWORD64>(BaseAddress, Address))
			return 0;
	
		for (int i = 0; i < Offsets.size() - 1; i++)
		{
			if (!ReadMemory<DWORD64>(Address + Offsets[i], Address))
				return 0;
		}
		return Address == 0 ? 0 : Address + Offsets[Offsets.size() - 1];
	}

public:

	DWORD GetProcessID(std::string ProcessName)
	{
		PROCESSENTRY32 ProcessInfoPE;
		ProcessInfoPE.dwSize = sizeof(PROCESSENTRY32);
		HANDLE hSnapshot = CreateToolhelp32Snapshot(15, 0);
		Process32First(hSnapshot, &ProcessInfoPE);
		USES_CONVERSION;
		do {
			if (strcmp(W2A(ProcessInfoPE.szExeFile), ProcessName.c_str()) == 0)
			{
				CloseHandle(hSnapshot);
				return ProcessInfoPE.th32ProcessID;
			}
		} while (Process32Next(hSnapshot, &ProcessInfoPE));
		CloseHandle(hSnapshot);
		return 0;
	}

	HMODULE GetProcessModuleHandle(std::string ModuleName)
	{
		MODULEENTRY32 ModuleInfoPE;
		ModuleInfoPE.dwSize = sizeof(MODULEENTRY32);
		HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, this->ProcessID);
		Module32First(hSnapshot, &ModuleInfoPE);
		USES_CONVERSION;
		do {
			if (strcmp(W2A(ModuleInfoPE.szModule), ModuleName.c_str()) == 0)
			{
				CloseHandle(hSnapshot);
				return ModuleInfoPE.hModule;
			}
		} while (Module32Next(hSnapshot, &ModuleInfoPE));
		CloseHandle(hSnapshot);
		return 0;
	}

};

inline ProcessManager ProcessMgr;