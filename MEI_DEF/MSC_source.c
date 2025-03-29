#include <stdio.h>
#include <stdlib.h>
#include <windows.h>

// Function to check if the program is run as administrator
int isRunAsAdmin() {
    BOOL isAdmin = FALSE;
    HANDLE tokenHandle = NULL;
    
    // Check if we can get the process token
    if (OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &tokenHandle)) {
        TOKEN_ELEVATION elevation;
        DWORD size;
        
        // Check the elevation level
        if (GetTokenInformation(tokenHandle, TokenElevation, &elevation, sizeof(elevation), &size)) {
            isAdmin = elevation.TokenIsElevated;
        }
        CloseHandle(tokenHandle);
    }
    return isAdmin;
}

// Function to relaunch the program with admin privileges
void relaunchAsAdmin() {
    char szPath[MAX_PATH];
    
    // Get the path of the executable
    if (GetModuleFileName(NULL, szPath, MAX_PATH)) {
        SHELLEXECUTEINFO sei = { sizeof(sei) };
        sei.lpVerb = "runas";       // Request admin privileges
        sei.lpFile = szPath;        // Path of the executable
        sei.hwnd = NULL;
        sei.nShow = SW_SHOWNORMAL;
        
        if (!ShellExecuteEx(&sei)) {
            printf("Failed to relaunch as administrator.\n");
        }
    }
}

// Function to display help menu
void displayHelp() {
    printf("Meisei System Checkup Help Menu\n");
    printf("==================================\n");
    printf("1. System File Checker (sfc /scannow) - Scans and repairs system files.\n");
    printf("2. Deployment Image Servicing (DISM) - Restores the health of the system image.\n");
    printf("3. System File Repair (chkdsk /f /r) - Repairs disk errors.\n");
    printf("4. Memory Diagnostic Tool - Checks for memory problems.\n");
    printf("5. Performance Monitor - Provides a detailed performance report.\n");
    printf("6. Disk Cleanup - Cleans up unnecessary files on your drive.\n");
    printf("7. Defragment and Optimize Drive - Defragments and optimizes your drives.\n");
    printf("==================================\n");
    printf("Select the number corresponding to the action you'd like to run or type 'help' for this menu.\n");
}

// Function to execute system checkup commands based on user selection
void runSystemCheckup(int choice) {
    switch (choice) {
        case 1:
            printf("Running System File Checker (sfc /scannow)...\n");
            system("sfc /scannow");
            break;
        case 2:
            printf("Running Deployment Image Servicing and Management (DISM)...\n");
            system("DISM /Online /Cleanup-Image /RestoreHealth");
            break;
        case 3:
            printf("Running System File Repair (chkdsk /f /r)...\n");
            system("chkdsk /f /r");
            break;
        case 4:
            printf("Running Memory Diagnostic Tool (mdsched.exe)...\n");
            system("mdsched.exe");
            break;
        case 5:
            printf("Running Performance Monitor (perfmon /report)...\n");
            system("perfmon /report");
            break;
        case 6:
            printf("Running Disk Cleanup (cleanmgr)...\n");
            system("cleanmgr");
            break;
        case 7:
            printf("Running Defragment and Optimize Drive (dfrgui)...\n");
            system("dfrgui");
            break;
        default:
            printf("Invalid choice. Please select a valid option.\n");
            break;
    }
}

int main() {
    int choice;

    // Check if user has admin privileges
    if (!isRunAsAdmin()) {
        printf("Program needs to be run as administrator. Attempting to restart...\n");
        relaunchAsAdmin();
        return 1;  // Exit the current instance, the new one will run with admin rights
    }

    printf("Meisei System Checkup - Sorry! Just had to make sure we had Administrator to ensure things wouldn't break!\n");
    
    // Display help menu on start
    displayHelp();

    // Menu loop
    while (1) {
        printf("\nEnter your choice (or type 'help' for the menu, 0 to exit): ");
        
        // Handle user input
        if (scanf("%d", &choice) != 1) {
            char buffer[10];
            scanf("%s", buffer);  // Clear invalid input
            if (strcmp(buffer, "help") == 0) {
                displayHelp();
            } else {
                printf("Invalid input. Please enter a number or 'help'.\n");
            }
            continue;
        }
        
        if (choice == 0) {
            printf("Exiting Meisei System Checkup.\n");
            break;
        }

        // Execute the selected option
        runSystemCheckup(choice);
    }

    return 0;
}
