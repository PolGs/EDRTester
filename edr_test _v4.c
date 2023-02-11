#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include <urlmon.h>
#pragma comment(lib, "urlmon.lib")

#define REG_KEY "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run"
#define REG_NAME "MyScript"

void print_commands() {
    printf("Possible commands:\n");
    printf("  1 or boot - create registry key to run the script on startup\n");
    printf("  2 or copy - copy the script to the same folder as copy.exe\n");
    printf("  3 or download - download a file from a URL defined in the script and store in the same folder as the script\n");
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

void copy_to_folder() {
    char source_path[MAX_PATH];
    char dest_path[MAX_PATH];
    GetModuleFileName(NULL, source_path, MAX_PATH);
    dest_path[]= source_path[];
    strcpy(strrchr(source_path, '\\') + 1, "tester.exe");
    printf(source_path);
    sprintf(dest_path, "%s\\copy.exe", strrchr(dest_path, '\\'));
    printf(dest_path);
    if (CopyFile(source_path, dest_path, FALSE)) {
        printf("Copied the script to the same folder as copy.exe.\n");
    } else {
        printf("Failed to copy the script to the same folder as copy.exe.\n");
    }
}

void download_file() {
    char url[] = "http://example.com/file.txt";
    char dest_path[MAX_PATH];
    GetModuleFileName(NULL, dest_path, MAX_PATH);
    sprintf(strrchr(dest_path, '\\') + 1, "file.txt");
    if (URLDownloadToFile(NULL, url, dest_path, 0, NULL) == S_OK) {
        printf("Downloaded a file from a URL and stored in the same folder as the script.\n");
    } else {
        printf("Failed to download a file from a URL.\n");
    }
}

int main() {
    char input[256];
    while (1) {
        print_commands();
        printf("Enter a command number: ");
        fgets(input, sizeof(input), stdin);
        input[strcspn(input, "\r\n")] = 0; // remove newline
        if (strcmp(input, "1") == 0 || strcmp(input, "boot") == 0) {
            create_registry_key();
        } else if (strcmp(input, "2") == 0 || strcmp(input, "copy") == 0) {
            copy_to_folder();
        } else if (strcmp(input, "3") == 0 || strcmp(input, "download") == 0) {
            download_file();
        } else {
            printf("Invalid command.\n");
        }
    }
    return 0;
}
