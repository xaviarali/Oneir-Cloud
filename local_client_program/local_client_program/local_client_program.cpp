// oneir_local_client.cpp : Defines the entry point for the console application.
//

// header files
#include "stdafx.h"
#include <io.h>
#include<iostream>
#include <stdio.h>

#include<string.h>
#include<time.h>
#include<winsock2.h>
#include<ws2tcpip.h>
#include <windows.h>
#include <process.h>
#include <Tlhelp32.h>
#include <winbase.h>
#pragma comment(lib,"Ws2_32.lib")
#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT "80"
#define SESSION_ID "test@oneir.com"
#define telnet98 "C:\\oneirV1.0\\Dev3-Cenedex.etx"
#define terminal "C:\\oneirV1.0\\terminal.etx"

using namespace std;
// global variable
int choice = 0;
// Macros
#define _ETNAPI_H
#define ETNBUFSIZ       0x2000
HWND erik98 = NULL;

// aliases for function pointers
typedef VOID(CALLBACK* ETNDATAPROC)(HWND, LPBYTE, DWORD);
typedef BOOL(__stdcall *etnEnableInput)(HWND hSession, BOOL bEnable);
typedef BOOL(__stdcall *etnIsSession)(HWND hSession);
typedef HWND(__stdcall *etnObtainHandle)(LPCTSTR lpSessionName, DWORD dwMilliseconds);
typedef BOOL(__stdcall *etnReceiveData)(HWND hSession, LPBYTE lpData, LPDWORD lpcbData, DWORD dwMilliseconds);
typedef BOOL(__stdcall *etnRegisterCallback)(HWND hSession, ETNDATAPROC lpDataProc);
typedef BOOL(__stdcall *etnReleaseHandle)(HWND hSession);
typedef BOOL(__stdcall *etnSendData)(HWND hSession, LPBYTE lpData, DWORD cbData);
typedef BOOL(__stdcall *etnSetDelay)(DWORD dwMilliseconds);

// loads the .DLL provided by Erik's Telnet98
static HANDLE GetEtnApi(void)
{
	static HANDLE hDLL = NULL;
#if _WIN64
	if (!hDLL)
		hDLL = LoadLibrary(L"ETNAPI64.DLL");
#else
	if (!hDLL)
		hDLL = LoadLibrary(L"ETNAPI32.DLL");
#endif
	if (!hDLL) {
		MessageBox(NULL, L"Failed to load ETNAPI", (LPCWSTR)__FILE__, MB_ICONSTOP);
		exit(1);
	} // if
	return hDLL;
} // GetEtnApi


  // wrapper function for etnEnableInput which is loaded from the .DLL file
BOOL WINAPI xetnEnableInput(HWND hSession, BOOL bEnable)
{
	//static BOOL(STDAPICALLTYPE *etnEnableInput)(HWND, BOOL) = NULL;
	static etnEnableInput x = NULL;
	if (!x)
		x = (etnEnableInput)GetProcAddress((HMODULE)GetEtnApi(), "etnEnableInput");
	if (x)
		return x(hSession, bEnable);
	return FALSE;
} // etnEnableInput


  // wrapper function for etnIsSession which is loaded from the .DLL file
BOOL WINAPI xetnIsSession(HWND hSession)
{
	//static BOOL(STDAPICALLTYPE *etnIsSession)(HWND) = NULL;
	static etnIsSession x = NULL;
	if (!x)
		x = (etnIsSession)GetProcAddress((HMODULE)GetEtnApi(), "etnIsSession");
	if (x)
		return x(hSession);
	return FALSE;
} // etnIsSession


  // wrapper function for etnObtainHandle which is loaded from the .DLL file
HWND WINAPI xetnObtainHandle(LPCTSTR lpSessionName, DWORD dwMilliseconds)
{
	//static HWND(STDAPICALLTYPE *etnObtainHandle)(LPCTSTR, DWORD) = NULL;
	static etnObtainHandle x = NULL;
	if (!x)
		x = (etnObtainHandle)GetProcAddress((HMODULE)GetEtnApi(), "etnObtainHandle");
	if (x)
		return x(lpSessionName, dwMilliseconds);
	return NULL;
} // etnObtainHandle


  // wrapper function for etnReceiveData which is loaded from the .DLL file
BOOL WINAPI xetnReceiveData(HWND hSession, LPBYTE lpData, LPDWORD lpcbData, DWORD dwMilliseconds)
{
	//static BOOL(STDAPICALLTYPE *etnReceiveData)(HWND, LPBYTE, LPDWORD, DWORD) = NULL;
	static etnReceiveData x = NULL;
	if (!x)
		x = (etnReceiveData)GetProcAddress((HMODULE)GetEtnApi(), "etnReceiveData");
	if (x)
		return x(hSession, lpData, lpcbData, dwMilliseconds);
	return FALSE;
} // etnReceiveData


  // wrapper function for etnRegisterCallback which is loaded from the .DLL file
BOOL WINAPI xetnRegisterCallback(HWND hSession, ETNDATAPROC lpDataProc)
{
	//static BOOL(STDAPICALLTYPE *etnRegisterCallback)(HWND, ETNDATAPROC) = NULL;
	static etnRegisterCallback x = NULL;
	if (!x)
		x = (etnRegisterCallback)GetProcAddress((HMODULE)GetEtnApi(), "etnRegisterCallback");
	if (x)
		return x(hSession, lpDataProc);
	return FALSE;
} // etnRegisterCallback


  // wrapper function for etnReleaseHandle which is loaded from the .DLL file
BOOL WINAPI xetnReleaseHandle(HWND hSession)
{
	//static BOOL(STDAPICALLTYPE *etnReleaseHandle)(HWND) = NULL;
	static etnReleaseHandle x = NULL;
	if (!x)
		x = (etnReleaseHandle)GetProcAddress((HMODULE)GetEtnApi(), "etnSetDelay");
	if (x)
		return x(hSession);
	return FALSE;
} // etnReleaseHandle


  // wrapper function for etnSendData which is loaded from the .DLL file
BOOL WINAPI xetnSendData(HWND hSession, LPBYTE lpData, DWORD cbData)
{
	//static BOOL(STDAPICALLTYPE *etnSendData)(HWND, LPBYTE, DWORD) = NULL;
	static etnSendData x = NULL;
	if (!x)
		x = (etnSendData)GetProcAddress((HMODULE)GetEtnApi(), "etnSendData");
	if (x)
		return x(hSession, lpData, cbData);
	return FALSE;
} // etnSendData


  // wrapper function for etnSetDelay which is loaded from the .DLL file
VOID WINAPI xetnSetDelay(DWORD dwMilliseconds)
{
	//static VOID(STDAPICALLTYPE *etnSetDelay)(DWORD) = NULL;
	static etnSetDelay x = NULL;
	if (!x)
		x = (etnSetDelay)GetProcAddress((HMODULE)GetEtnApi(), "etnSetDelay");
	if (x)
		x(dwMilliseconds);
} // etnSetDelay

  /*
  *  temporary function which extracts a character from a char* and store it into global variable 'choice'
  */
void check(char* s)
{

	if (s[377] == '"' && s[379] == '"')
	{
		printf("Command Recieved from the Cloud.\n");
		choice = (int)(s[378] - '0');
	}
}
/*
* client which sends a session Id to the server and recives a JSON data in responce
*/
int client()
{
	WSADATA wsaData;
	SOCKET ConnectSocket = INVALID_SOCKET;
	struct addrinfo *result = NULL,
		*ptr = NULL,
		hints;
	char sendbuf[100];
	// hard-coded HTTP GET REQUEST
	sprintf_s(sendbuf, "GET /oneir.php?q=%s HTTP/1.1\r\nHost:social50.website\r\n\r\n", SESSION_ID);
	// buffer to store data
	char recvbuf[DEFAULT_BUFLEN];
	int iResult;
	int recvbuflen = DEFAULT_BUFLEN;



	// Initialize Winsock
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0) {
		printf("WSAStartup failed with error: %d\n", iResult);
		return 1;
	}

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

	// Resolve the server address and port
	iResult = getaddrinfo("social50.website", "80", &hints, &result);
	if (iResult != 0) {
		printf("Error in resolving getaddrinfo failed with error: %d\n", iResult);
		WSACleanup();
		return 1;
	}

	// Attempt to connect to an address until one succeeds
	for (ptr = result; ptr != NULL; ptr = ptr->ai_next) {

		// Create a SOCKET for connecting to server
		ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype,
			ptr->ai_protocol);
		if (ConnectSocket == INVALID_SOCKET) {
			printf("socket failed with error: %ld\n", WSAGetLastError());
			WSACleanup();
			return 1;
		}

		// Connect to server.
		iResult = connect(ConnectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
		if (iResult == SOCKET_ERROR) {
			closesocket(ConnectSocket);
			ConnectSocket = INVALID_SOCKET;
			continue;
		}
		break;
	}

	freeaddrinfo(result);

	if (ConnectSocket == INVALID_SOCKET) {
		printf("Unable to connect to server!\n");
		WSACleanup();
		return 1;
	}

	// Send an initial buffer
	iResult = send(ConnectSocket, sendbuf, (int)strlen(sendbuf), 0);
	if (iResult == SOCKET_ERROR) {
		printf("send failed with error: %d\n", WSAGetLastError());
		closesocket(ConnectSocket);
		WSACleanup();
		return 1;
	}



	// shutdown the connection since no more data will be sent
	iResult = shutdown(ConnectSocket, SD_SEND);
	if (iResult == SOCKET_ERROR) {
		printf("shutdown failed with error: %d\n", WSAGetLastError());
		closesocket(ConnectSocket);
		WSACleanup();
		return 1;
	}

	// Receive until the peer closes the connection
	do {

		iResult = recv(ConnectSocket, recvbuf, recvbuflen, 0); recvbuf[recvbuflen - 1] = '\0'; check(recvbuf); break;
		if (iResult > 0)
			printf("Bytes received: %d\n", iResult);
		else if (iResult == 0)
			printf("Connection closed\n");
		else
			printf("recv failed with error: %d\n", WSAGetLastError());

	} while (iResult > 0);
	// cleanup
	closesocket(ConnectSocket);
	//WSACleanup();

	return 0;
}

bool sendDataToTelnet98()
{
	HWND    hSession;
	DWORD   cbData;
	char    buf[ETNBUFSIZ];

	/*
	* increase this value if your application
	* is too slow to fetch all incoming data
	*/
	etnSetDelay(10);

	/*
	*  Hard Coded Data which is being sent to the telnet98 session
	*/
	if (hSession = xetnObtainHandle(NULL, (DWORD)1000)) {
		xetnEnableInput(hSession, FALSE);
		strncpy_s(buf, sizeof(buf), "1", 1);
		char arsd[] = "./runwin ./vigilant 2>$USER.log COMP=tutorial DATE=021231 PASS=DEMO DV=HO PROG=ARSD\r";
		char insd[] = "./runwin ./vigilant 2>$USER.log COMP=tutorial DATE=021231 PASS=DEMO DV=HO PROG=INSD\r";
		switch (choice)
		{
		case 1:
			xetnSendData(hSession, (LPBYTE)"1", (DWORD)strlen(buf));
			xetnSendData(hSession, (LPBYTE)"D", (DWORD)strlen(buf));
			xetnSendData(hSession, (LPBYTE)"E", (DWORD)strlen(buf));
			xetnSendData(hSession, (LPBYTE)"M", (DWORD)strlen(buf));
			xetnSendData(hSession, (LPBYTE)"O", (DWORD)strlen(buf));
			xetnSendData(hSession, (LPBYTE)"\r", (DWORD)strlen(buf));
			break;
		case 2:
			xetnSendData(hSession, (LPBYTE)"1", (DWORD)strlen(buf));
			xetnSendData(hSession, (LPBYTE)"D", (DWORD)strlen(buf));
			xetnSendData(hSession, (LPBYTE)"E", (DWORD)strlen(buf));
			xetnSendData(hSession, (LPBYTE)"M", (DWORD)strlen(buf));
			xetnSendData(hSession, (LPBYTE)"O", (DWORD)strlen(buf));
			xetnSendData(hSession, (LPBYTE)"\r", (DWORD)strlen(buf));
			xetnSendData(hSession, (LPBYTE)"4", (DWORD)strlen(buf));
			xetnSendData(hSession, (LPBYTE)"4", (DWORD)strlen(buf));
			break;

		case 3:
			xetnSendData(hSession, (LPBYTE)"1", (DWORD)strlen(buf));
			xetnSendData(hSession, (LPBYTE)"D", (DWORD)strlen(buf));
			xetnSendData(hSession, (LPBYTE)"E", (DWORD)strlen(buf));
			xetnSendData(hSession, (LPBYTE)"M", (DWORD)strlen(buf));
			xetnSendData(hSession, (LPBYTE)"O", (DWORD)strlen(buf));
			xetnSendData(hSession, (LPBYTE)"\r", (DWORD)strlen(buf));
			xetnSendData(hSession, (LPBYTE)"5", (DWORD)strlen(buf));
			xetnSendData(hSession, (LPBYTE)"4", (DWORD)strlen(buf));
			break;


		case 4:
			xetnSendData(hSession, (LPBYTE)"1", (DWORD)strlen(buf));
			xetnSendData(hSession, (LPBYTE)"D", (DWORD)strlen(buf));
			xetnSendData(hSession, (LPBYTE)"E", (DWORD)strlen(buf));
			xetnSendData(hSession, (LPBYTE)"M", (DWORD)strlen(buf));
			xetnSendData(hSession, (LPBYTE)"O", (DWORD)strlen(buf));
			xetnSendData(hSession, (LPBYTE)"\r", (DWORD)strlen(buf));
			xetnSendData(hSession, (LPBYTE)"5", (DWORD)strlen(buf));
			break;
		case 5:
			xetnSendData(hSession, (LPBYTE)arsd, (DWORD)strlen(arsd));
			break;

		case 6:
			xetnSendData(hSession, (LPBYTE)insd, (DWORD)strlen(insd));
			break;
		}

		xetnEnableInput(hSession, TRUE);
		return true;
	} // if
	printf("No Telnet98 Running!\n");
	return false;
}

// check whether telnet98 session is running
bool isTelnetReady()
{
	if (xetnObtainHandle(NULL, (DWORD)10000) == NULL) return 0;
	return 1;
}

// wait telnet to load , time may vary with the system
void wait(unsigned timeout)
{
	timeout += clock();
	while (clock() < timeout) continue;
}

// Get the horizontal and vertical screen sizes in pixel
void GetDesktopResolution(int& horizontal, int& vertical)
{
	RECT desktop;
	// Get a handle to the desktop window
	const HWND hDesktop = GetDesktopWindow();
	// Get the size of screen to the variable desktop
	GetWindowRect(hDesktop, &desktop);
	// The top left corner will have coordinates (0,0)
	// and the bottom right corner will have coordinates
	// (horizontal, vertical)
	horizontal = desktop.right;
	vertical = desktop.bottom;
}

void resizeApps(int v, int h)
{
	//HWND chrome = GetWindow(chromeWindow,GW_HWNDNEXT);
	//tn98FrameClass
	//MozillaWindowClass
	//Chrome_WidgetWin_1
	// try 0x0200 instead of 0x0400  SWP_NOZORDER
	// for google chrome
	HWND Window = NULL;

	Window = FindWindowEx(0, Window, L"MozillaWindowClass", 0);
	if (Window != NULL)
		SetWindowPos(Window, 0, 0, 0, (h / 8)+ (h / 16), v - 50, SWP_NOZORDER);
	// for telnet window
	Window = NULL;
	//Window = FindWindowEx(0, Window, L"tn98FrameClass", 0);
	Window = xetnObtainHandle(NULL, (DWORD)1000);
	if (Window != NULL)
		SetWindowPos(Window, 0, (h / 8) + (h / 16), 0,h-((h/8)+(h/16)), v - 50, SWP_NOZORDER);
}

int main(void)
{
	int vert, horz, taskbar_sz;
	GetDesktopResolution(horz, vert);

	//system("C:\\oneir\\Dev3-Cenedex.etx");
	printf("\n\t\t\tSession Id: %s\n", SESSION_ID);
	while (true)
	{
		resizeApps(vert, horz);
		client();
		//end of server
		if (choice != 0)
		{   
			if(choice == 5 || choice == 6) system(terminal);
			else system(telnet98);

			resizeApps(vert, horz);

			printf("Waiting For Telnet To load\n");
			while (!isTelnetReady());
			wait(3000);
			system("COLOR 2");
			printf("Time Ended For Telnet To load\n");
			sendDataToTelnet98();
			choice = 0;
			erik98 = NULL;
		}
	}
	return 0;
} // main

