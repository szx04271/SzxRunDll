/************************************
*  Fast Dll Runner V3.0 for CPP17   *
*         Author: szx0427           *
*         Date: 2026/03/01          *
************************************/

#pragma once

#ifndef SZXRUNDLL_NO_WINDOWS_H
#include <Windows.h>
#endif
#include <tchar.h>
#include <string>
#include <map>

namespace FastRunDll
{
	enum CallingConventions
	{
		Stdcall = 0,
		Cdecl = 1,
		Fastcall = 2
	};

	class Win32Exception : public std::exception
	{
	private:
		const char* msg_;
		DWORD code_;

	public:
		Win32Exception(const char* msg, DWORD code)
			: msg_(msg), code_(code)
		{ }

		char const* what() const override
		{
			return msg_;
		}

		DWORD code() const
		{
			return code_;
		}
	};

	class RunDll
	{
	protected:
		static inline const std::string _callingConvSymbols[3] = { "@@YG","@@YA","@@YI" };
		static inline std::map<std::string, std::string> _typeSymbolMap;
		static void _InitTypeSymbolMap() {
			if (!_typeSymbolMap.empty())
				return;
			_typeSymbolMap = {
				{ "void", "X" },
				{ "char", "D" },
				{ "unsigned char", "E" },
				{ "short", "F" },
				{ "unsigned short", "G" },
				{ "int", "H" },
				{ "unsigned int", "I" },
				{ "long", "J" },
				{ "unsigned long", "K" },
				{ "__int64", "_J" },
				{ "unsigned __int64", "_K" },
				{ "float", "M" },
				{ "double", "N" },
				{ "bool", "_N" },
				{ "*", "PA" },
				{ "const *", "PB" },
				{ "2 *", "0" },
				{ "2 const *", "1" }
			};
		}

	protected:
		HMODULE m_hModule;
		template <class _ReturnType, class _FxnType, class... _ParamTypes>
		_ReturnType _basicCallDllFunc2(LPCSTR lpFxnName, const _ParamTypes&... args) {
			if (!m_hModule) throw std::logic_error("DLL not loaded");
			if (!lpFxnName) throw std::invalid_argument("Function name is null");
			_FxnType _Myfxn = (_FxnType)::GetProcAddress(m_hModule, lpFxnName);
			if (!_Myfxn) throw std::logic_error("Procedure not found");
			return _Myfxn(args...);
		}

	public:
		RunDll(LPCTSTR lpDllName = nullptr)
			: m_hModule(nullptr) {
			if (lpDllName)
				m_hModule = LoadLibrary(lpDllName);
		}

		virtual ~RunDll() noexcept {
			UnloadDll();
		}

		void LoadDll(LPCTSTR lpDllName) {
			HMODULE hMod = LoadLibrary(lpDllName);
			if (hMod)
			{
				UnloadDll();
				m_hModule = hMod;
			}
			else
			{
				throw Win32Exception("LoadLibrary error", GetLastError());
			}
		}

		UINT UnloadDll() noexcept {
			if (!m_hModule)
			{
				return 0;
			}
			auto freeSuccess = FreeLibrary(m_hModule);
			if (freeSuccess)
			{
				m_hModule = nullptr;
				return 0;
			}
			return GetLastError();
		}

		HMODULE GetHmodule() const // ++ v2.2
		{
			return m_hModule;
		}

		// 暂时不知如何正确复制HMODULE句柄
		RunDll& operator=(const RunDll& other) = delete;

		RunDll& operator=(RunDll&& other) noexcept
		{
			m_hModule = other.m_hModule;
			other.m_hModule = nullptr;
			return *this;
		}

		RunDll(const RunDll& other) = delete;

		RunDll(RunDll&& other) noexcept
			: m_hModule(other.m_hModule)
		{
			other.m_hModule = nullptr;
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
			if (!m_hModule) throw std::logic_error("DLL not loaded");
			return (_ReturnType(__stdcall*)(_ParamTypes...))::GetProcAddress(m_hModule, lpProcName);
		}

		template <class _ReturnType = void, class... _ParamTypes>
		auto GetProcAddress_cdecl(LPCSTR lpProcName) {
			if (!m_hModule) throw std::logic_error("DLL not loaded");
			return (_ReturnType(__cdecl*)(_ParamTypes...))::GetProcAddress(m_hModule, lpProcName);
		}

		template <class _ReturnType = void, class... _ParamTypes>
		auto GetProcAddress_fastcall(LPCSTR lpProcName) {
			if (!m_hModule) throw std::logic_error("DLL not loaded");
			return (_ReturnType(__fastcall*)(_ParamTypes...))::GetProcAddress(m_hModule, lpProcName);
		}

		template <class _ReturnType = void, class... _ParamTypes>
		auto GetProcAddress_thiscall(LPCSTR lpProcName) {
			if (!m_hModule) throw std::logic_error("DLL not loaded");
			return (_ReturnType(__thiscall*)(_ParamTypes...))::GetProcAddress(m_hModule, lpProcName);
		}

		template <class _ReturnType = void, class... _ParamTypes>
		static std::string BuildCppDecoratedName(const std::string& sFxnName, CallingConventions cc = Cdecl) {
			_InitTypeSymbolMap();
			std::string ret = "?" + sFxnName + _callingConvSymbols[(int)cc];
			const char* szTypes[] = { typeid(_ReturnType).name(), typeid(_ParamTypes).name()... };
			int nCount = 1 + sizeof...(_ParamTypes);
			std::string tmp, par;
			int pos1, pos2, sum = 0;
			for (int i = 0; i < nCount; i++)
			{
				tmp = szTypes[i];

				// This version doesn't support struct/enum/reference
				if (!(tmp.find("struct") == tmp.npos && tmp.find("enum") == tmp.npos && tmp.find("&") == tmp.npos))
				{
					throw std::logic_error("struct/enum/reference is not supported");
				}

				if (!(tmp.find('[') == tmp.npos))
				{
					throw std::logic_error("No arrays allowed; use pointers instead");
				}

				if ((pos1 = tmp.find(" *")) != tmp.npos)
				{
					if ((pos2 = tmp.find(" const *")) != tmp.npos)
					{
						if (i >= 1 && tmp == szTypes[i - 1])
							par += _typeSymbolMap["2 const *"];
						else
							par += _typeSymbolMap["const *"] + _typeSymbolMap[tmp.substr(0, pos2)];
					}
					else
					{
						if (i >= 1 && tmp == szTypes[i - 1])
							par += _typeSymbolMap["2 *"];
						else
							par += _typeSymbolMap["*"] + _typeSymbolMap[tmp.substr(0, pos1)];
					}
				}
				else
					par += _typeSymbolMap[tmp];
			}
			if (par.length() == 1)
				par += "XZ";
			else
				par += "@Z";
			ret += par;
			return ret;
		}
	};
}
