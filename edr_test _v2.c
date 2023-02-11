#include <stdio.h>
#include <windows.h>

int main() {
    char input[100];
    char scriptPath[MAX_PATH];
    char sys32Path[MAX_PATH];
    HKEY hKey;
    LONG regStatus;

    // Step 1: Output possible commands
    printf("Possible commands:\n");
    printf("  boot - create regkey that makes the script run on startup\n");
    printf("  copy - copy script to system32\n");
    printf("Enter command: ");

    // Step 2: Read input from keyboard
    fgets(input, 100, stdin);

    // Remove newline character from input
    if (input[strlen(input)-1] == '\n') {
        input[strlen(input)-1] = '\0';
    }

    // Step 3: If input is boot, create regkey that makes the script run on startup
    if (strcmp(input, "boot") == 0) {
        GetModuleFileName(NULL, scriptPath, MAX_PATH);
        regStatus = RegCreateKeyEx(HKEY_CURRENT_USER, "Software\\Microsoft\\Windows\\CurrentVersion\\Run", 0, NULL, 0, KEY_WRITE, NULL, &hKey, NULL);
        if (regStatus == ERROR_SUCCESS) {
            RegSetValueEx(hKey, "MyScript", 0, REG_SZ, (BYTE*)scriptPath, strlen(scriptPath) + 1);
            printf("Regkey created successfully\n");
        } else {
            printf("Error creating regkey: %d\n", regStatus);
        }
    }

    // Step 4: If input is copy, copy script to system32
    else if (strcmp(input, "copy") == 0) {
        GetModuleFileName(NULL, scriptPath, MAX_PATH);
        GetSystemDirectory(sys32Path, MAX_PATH);
        strcat(sys32Path, "\\MyScript.exe");
        if (CopyFile(scriptPath, sys32Path, FALSE)) {
            printf("Script copied successfully\n");
        } else {
            printf("Error copying script: %d\n", GetLastError());
        }
    }

    // Step 5: Report any errors that happened
    else {
        printf("Invalid command\n");
    }

    // Step 6: Go back to step 1
    return main();
}
