#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <urlmon.h>

#pragma comment(lib, "urlmon.lib")

void create_regkey() {
    HKEY hkey;
    if (RegOpenKeyEx(HKEY_CURRENT_USER, "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run", 0, KEY_WRITE, &hkey) == ERROR_SUCCESS) {
        char script_path[MAX_PATH];
        GetModuleFileName(NULL, script_path, MAX_PATH);
        if (RegSetValueEx(hkey, "MyScript", 0, REG_SZ, (BYTE*)script_path, strlen(script_path) + 1) == ERROR_SUCCESS) {
            printf("Added the script to startup.\n");
        } else {
            printf("Failed to add the script to startup.\n");
        }
        RegCloseKey(hkey);
    } else {
        printf("Failed to open registry key.\n");
    }
}

void copy_to_folder() {
    char source_path[MAX_PATH];
    char dest_path[MAX_PATH];
    GetModuleFileName(NULL, source_path, MAX_PATH);
    char* filename = strrchr(source_path, '\\') + 1;
    strcpy(filename, "copy.exe");
    sprintf(dest_path, "%s\\%s", strrchr(source_path, '\\'), filename);
    if (CopyFile(source_path, dest_path, FALSE)) {
        printf("Copied the script to the same folder as copy.exe.\n");
        FILE* fp = fopen("test.txt", "w");
        if (fp != NULL) {
            fclose(fp);
            printf("Created test.txt.\n");
        } else {
            printf("Failed to create test.txt.\n");
        }
    } else {
        printf("Failed to copy the script to the same folder as copy.exe.\n");
    }
}

void download_file() {
    char url[] = "https://example.com/file.txt";
    char dest_path[MAX_PATH];
    GetModuleFileName(NULL, dest_path, MAX_PATH);
    strcpy(strrchr(dest_path, '\\') + 1, "file.txt");
    if (URLDownloadToFile(NULL, url, dest_path, 0, NULL) == S_OK) {
        printf("Downloaded file from %s to %s.\n", url, dest_path);
    } else {
        printf("Failed to download file from %s.\n", url);
    }
}

int main() {
    int command;
    do {
        printf("Possible commands:\n1. Boot\n2. Copy\n3. Download\n4. Exit\n");
        printf("Enter a command number: ");
        scanf("%d", &command);
        switch (command) {
            case 1:
                create_regkey();
                break;
            case 2:
                copy_to_folder();
                break;
            case 3:
                download_file();
                break;
            case 4:
                return 0;
            default:
                printf("Invalid command number. Please try again.\n");
        }
    } while (1);
}
