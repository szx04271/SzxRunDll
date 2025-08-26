/************************************
*  Fast Dll Runner V2.3 for CPP11+  *
*         Author: szx0427           *
*         Date: 2021/08/03          *
************************************/

#pragma once

#ifndef SZXRUNDLL_NO_WINDOWS_H
#include <Windows.h>
#endif
#include <tchar.h>
#include <cassert>
#include <string>
#include <map>

class CSzxRunDll2
{
public: // Public data types
	enum CallingConventions
	{
		Stdcall,
		Cdecl,
		Fastcall
	};

protected:
	static const std::string m_Prefix[3];
	static std::map<std::string, std::string> m_map;
	static void _InitMap() {
		if (m_map.size() > 0)
			return;
		m_map["void"] = "X";
		m_map["char"] = "D";
		m_map["unsigned char"] = "E";
		m_map["short"] = "F";
		m_map["unsigned short"] = "G";
		m_map["int"] = "H";
		m_map["unsigned int"] = "I";
		m_map["long"] = "J";
		m_map["unsigned long"] = "K";
		m_map["__int64"] = "_J";
		m_map["unsigned __int64"] = "_K";
		m_map["float"] = "M";
		m_map["double"] = "N";
		m_map["bool"] = "_N";
		m_map["*"] = "PA";
		m_map["const *"] = "PB";
		m_map["2 *"] = "0";
		m_map["2 const *"] = "1";
	}

protected: // Protected fields and methods
	HMODULE m_hModule;
	template <class _ReturnType, class _FxnType, class... _ParamTypes>
	_ReturnType _basicCallDllFunc2(LPCSTR lpFxnName, const _ParamTypes&... args) {
		assert(m_hModule);
		assert(lpFxnName);
		_FxnType _Myfxn = (_FxnType)::GetProcAddress(m_hModule, lpFxnName);
		assert(_Myfxn);
		return _Myfxn(args...);
	}

public: // Public methods
	CSzxRunDll2(LPCTSTR lpDllName = nullptr)
		: m_hModule(NULL) {
		if (lpDllName)
			m_hModule = LoadLibrary(lpDllName);
		_InitMap();
	}
	virtual ~CSzxRunDll2() {
		UnloadDll();
	}
	UINT LoadDll(LPCTSTR lpDllName) {
		assert(lpDllName);
		HMODULE hMod = LoadLibrary(lpDllName);
		if (hMod)
		{
			if (m_hModule)
				UnloadDll();
			m_hModule = hMod;
		}
		return GetLastError();
	}
	UINT UnloadDll() {
		FreeLibrary(m_hModule);
		return GetLastError();
	}
	HMODULE GetHmodule() const // ++ v2.2
	{
		return m_hModule;
	}

	template <class _ReturnType = void, class... _ParamTypes>
	_ReturnType CallDllFunc2_stdcall(LPCSTR lpFxnName, const _ParamTypes &..._Params) {
		return _basicCallDllFunc2<_ReturnType, _ReturnType(__stdcall*)(const _ParamTypes ...), _ParamTypes...>
			(lpFxnName, _Params...);
	}
	template <class _ReturnType = void, class... _ParamTypes>
	_ReturnType CallDllFunc2_cdecl(LPCSTR lpFxnName, const _ParamTypes &..._Params) {
		return _basicCallDllFunc2<_ReturnType, _ReturnType(__cdecl*)(const _ParamTypes ...), _ParamTypes...>
			(lpFxnName, _Params...);
	}
	template <class _ReturnType = void, class... _ParamTypes>
	_ReturnType CallDllFunc2_fastcall(LPCSTR lpFxnName, const _ParamTypes &..._Params) {
		return _basicCallDllFunc2<_ReturnType, _ReturnType(__fastcall*)(const _ParamTypes ...), _ParamTypes...>
			(lpFxnName, _Params...);
	}
	template <class _ReturnType = void, class... _ParamTypes>
	_ReturnType CallDllFunc2_thiscall(LPCSTR lpFxnName, const _ParamTypes &..._Params) {
		return _basicCallDllFunc2<_ReturnType, _ReturnType(__thiscall*)(const _ParamTypes ...), _ParamTypes...>
			(lpFxnName, _Params...);
	}

	// GetProcAddress_XXXX: ++v2.3
	template <class _ReturnType = void, class... _ParamTypes>
	auto GetProcAddress_stdcall(LPCSTR lpProcName) {
		assert(m_hModule);
		return (_ReturnType(__stdcall*)(_ParamTypes...))::GetProcAddress(m_hModule, lpProcName);
	}

	template <class _ReturnType = void, class... _ParamTypes>
	auto GetProcAddress_cdecl(LPCSTR lpProcName) {
		assert(m_hModule);
		return (_ReturnType(__cdecl*)(_ParamTypes...))::GetProcAddress(m_hModule, lpProcName);
	}

	template <class _ReturnType = void, class... _ParamTypes>
	auto GetProcAddress_fastcall(LPCSTR lpProcName) {
		assert(m_hModule);
		return (_ReturnType(__fastcall*)(_ParamTypes...))::GetProcAddress(m_hModule, lpProcName);
	}

	template <class _ReturnType = void, class... _ParamTypes>
	auto GetProcAddress_thiscall(LPCSTR lpProcName) {
		assert(m_hModule);
		return (_ReturnType(__thiscall*)(_ParamTypes...))::GetProcAddress(m_hModule, lpProcName);
	}

	template <class _ReturnType = void, class... _ParamTypes>
	static std::string BuildCppDecoratedName(const std::string& sFxnName, CallingConventions cc = Cdecl) {
		_InitMap();
		std::string ret = "?" + sFxnName + m_Prefix[(int)cc];
		const char* szTypes[] = { typeid(_ReturnType).name(), typeid(_ParamTypes).name()... };
		int nCount = 1 + sizeof...(_ParamTypes);
		std::string tmp, par;
		int pos1, pos2, sum = 0;
		for (int i = 0; i < nCount; i++)
		{
			tmp = szTypes[i];
			// This version doesn't support struct/enum/reference
			assert(tmp.find("struct") == tmp.npos && tmp.find("enum") == tmp.npos && tmp.find("&") == tmp.npos);
			assert(tmp.find('[') == tmp.npos); // Array(x) Pointer(√)
			if ((pos1 = tmp.find(" *")) != tmp.npos)
			{
				if ((pos2 = tmp.find(" const *")) != tmp.npos)
				{
					if (i >= 1 && tmp == szTypes[i - 1])
						par += m_map["2 const *"];
					else
						par += m_map["const *"] + m_map[tmp.substr(0, pos2)];
				}
				else
				{
					if (i >= 1 && tmp == szTypes[i - 1])
						par += m_map["2 *"];
					else
						par += m_map["*"] + m_map[tmp.substr(0, pos1)];
				}
			}
			else
				par += m_map[tmp];
		}
		if (par.length() == 1)
			par += "XZ";
		else
			par += "@Z";
		ret += par;
		return ret;
	}
};

const std::string CSzxRunDll2::m_Prefix[3] = { "@@YG","@@YA","@@YI" };
std::map<std::string, std::string> CSzxRunDll2::m_map;