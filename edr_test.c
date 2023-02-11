#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>

int main() {
    // Output possible commands
    printf("Possible commands: boot, copy\n");

    // Read input from keyboard
    char input[100];
    printf("Enter command: ");
    fgets(input, sizeof(input), stdin);

    // Remove newline character from input
    input[strcspn(input, "\n")] = 0;

    // Check if input is boot
    if (strcmp(input, "boot") == 0) {
        // Create regkey that makes the script run on startup
        HKEY hkey;
        RegOpenKeyEx(HKEY_CURRENT_USER, "Software\\Microsoft\\Windows\\CurrentVersion\\Run", 0, KEY_WRITE, &hkey);
        char path[MAX_PATH];
        GetModuleFileName(NULL, path, MAX_PATH);
        RegSetValueEx(hkey, "MyScript", 0, REG_SZ, (BYTE*)path, strlen(path) + 1);
        RegCloseKey(hkey);
        printf("Script added to startup.\n");
    }

    // Check if input is copy
    if (strcmp(input, "copy") == 0) {
        // Copy script to system32
        char sourcePath[MAX_PATH];
        GetModuleFileName(NULL, sourcePath, MAX_PATH);
        char destPath[MAX_PATH];
        sprintf(destPath, "%s\\%s", getenv("SYSTEMROOT"), "System32\\MyScript.exe");
        CopyFile(sourcePath, destPath, FALSE);
        printf("Script copied to System32.\n");
    }

    return 0;
}
