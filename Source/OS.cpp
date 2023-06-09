#include "OS.h"
#if _WIN32
//Include Windows Headers
#include <Windows.h>
#include <Shlobj.h>
#include <shobjidl.h> 
#include <map>
#endif

#if __linux__
//Currently no linux implementation needs to include anything
#endif

//https://www.cplusplus.com/forum/windows/74644/
std::string wstrtostr(const std::wstring& wstr)
{
#if _WIN32
	std::string strTo;
	char* szTo = new char[wstr.length() + 1];
	szTo[wstr.size()] = '\0';
	WideCharToMultiByte(CP_ACP, 0, wstr.c_str(), -1, szTo, (int)wstr.length(), NULL, NULL);
	strTo = szTo;
	delete[] szTo;
	return strTo;
#else
	return "Function not supported on Linux";
#endif
}

namespace OS
{
	bool ConsoleCanBeHidden = true;
}

void OS::SetConsoleCanBeHidden(bool NewConsoleCanBeHidden)
{
	ConsoleCanBeHidden = NewConsoleCanBeHidden;
}

#if _WIN32
void OS::SetConsoleWindowVisible(bool Visible)
{
	if (ConsoleCanBeHidden)
	{
		::ShowWindow(::GetConsoleWindow(), Visible ? SW_SHOW : SW_HIDE);
	}
}
#endif

#if __linux__
void OS::SetConsoleWindowVisible(bool Visible)
{
	//Clearing the Console is usually a bad idea on linux
}
#endif



#if _WIN32
std::string OS::ShowOpenFileDialog()
{
	std::string FilePath = "";
	HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED |
		COINIT_DISABLE_OLE1DDE);
	if (SUCCEEDED(hr))
	{
		IFileOpenDialog* pFileOpen;

		// Create the FileOpenDialog object.
		hr = CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_ALL,
			IID_IFileOpenDialog, reinterpret_cast<void**>(&pFileOpen));

		if (SUCCEEDED(hr))
		{
			// Show the Open dialog box.
			hr = pFileOpen->Show(NULL);
			// Get the file name from the dialog box.
			if (SUCCEEDED(hr))
			{
				IShellItem* pItem;
				hr = pFileOpen->GetResult(&pItem);
				if (SUCCEEDED(hr))
				{
					PWSTR pszFilePath;
					hr = pItem->GetDisplayName(SIGDN_FILESYSPATH, &pszFilePath);
					// Display the file name to the user.
					if (SUCCEEDED(hr))
					{
						FilePath = wstrtostr(pszFilePath);
						return FilePath;
					}
					pItem->Release();
				}
			}
			pFileOpen->Release();
		}
		CoUninitialize();
	}
	return FilePath;
}
#endif

#if __linux__
std::string OS::ShowOpenFileDialog()
{
	Log::Print("Creating an \"Open File-Dialog\" is not currently supported on Linux", Vector3(1, 1, 0));
	return std::string();
}
#endif



//Get the OS's name
#if _WIN64
std::string OS::GetOSString()
{
	int osver = 0;

	NTSTATUS(WINAPI * RtlGetVersion)(LPOSVERSIONINFOEXW);

	OSVERSIONINFOEXW osInfo;
	*(FARPROC*)&RtlGetVersion = GetProcAddress(GetModuleHandleA("ntdll"), "RtlGetVersion");

	if (NULL != RtlGetVersion)
	{
		osInfo.dwOSVersionInfoSize = sizeof(osInfo);
		RtlGetVersion(&osInfo);
		osver = osInfo.dwMajorVersion;
		return "64-bit Windows " + std::to_string(osver) + "." + std::to_string(osInfo.dwMinorVersion) + " (Build " + std::to_string(osInfo.dwBuildNumber) + ")";
	}
	return "64-bit Windows (Unknown version)";
}
#else
#if _WIN32
std::string OS::GetOSString()
{
	int osver = 0.0;

	NTSTATUS(WINAPI * RtlGetVersion)(LPOSVERSIONINFOEXW);

	OSVERSIONINFOEXW osInfo;

	*(FARPROC*)&RtlGetVersion = GetProcAddress(GetModuleHandleA("ntdll"), "RtlGetVersion");

	if (NULL != RtlGetVersion)
	{
		osInfo.dwOSVersionInfoSize = sizeof(osInfo);
		RtlGetVersion(&osInfo);
		osver = osInfo.dwMajorVersion;
		return "32-bit Windows " + std::to_string(osver) + "." + std::to_string(osInfo.dwMinorVersion) + " (Build " + std::to_string(osInfo.dwBuildNumber) + ")";
	}
	return "32-bit Windows (Unknown version)";
}
#endif
#endif

#if __linux__
std::string OS::GetOSString()
{
	return "Linux";
}
#endif

#if _WIN32
void OS::ClearConsoleWindow()
{
	system("CLS");
}
#endif

#if __linux__
void OS::ClearConsoleWindow()
{
	system("reset");
}
#endif

#if _WIN32

namespace OS
{
	std::map<EConsoleColor, WORD> WindowsColors =
	{
		std::pair(E_WHITE, FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_INTENSITY),
		std::pair(E_GRAY, FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED),
		std::pair(E_RED, FOREGROUND_RED | FOREGROUND_INTENSITY),
		std::pair(E_GREEN, FOREGROUND_GREEN | FOREGROUND_INTENSITY),
		std::pair(E_BLUE, FOREGROUND_BLUE | FOREGROUND_INTENSITY),
		std::pair(E_YELLOW, FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_INTENSITY),
	};
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
}

void OS::SetConsoleColor(EConsoleColor NewColor)
{
	SetConsoleTextAttribute(hConsole, WindowsColors[NewColor]);
}
#endif

#if __linux__
void OS::SetConsoleColor(EConsoleColor NewColor)
{
	//TODO: implement console colors on linux
}
#endif


#if _WIN32
void OS::OpenFile(std::string Path)
{
	system(("start " + Path).c_str());
}
#endif
#if __linux__
void OS::OpenFile(std::string Path)
{
	system(("xdg-open " + Path).c_str());
}
#endif