#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Function to run a shell command and store its result in a file
void run_command(const char *command, const char *filename) {
    char full_command[512];
    snprintf(full_command, sizeof(full_command), "%s > %s", command, filename);
    system(full_command);
}

// Function to print the content of a file
void print_file(const char *filename) {
    FILE *file = fopen(filename, "r");
    char buffer[1024];

    if (file != NULL) {
        while (fgets(buffer, sizeof(buffer), file)) {
            printf("%s", buffer);
        }
        fclose(file);
    } else {
        printf("Unable to open file: %s\n", filename);
    }
}

// Help menu
void display_help() {
    printf("\n--- NETWORK DIAGNOSTIC TOOL HELP MENU ---\n");
    printf("Commands:\n");
    printf("  help               Display this help menu\n");
    printf("  ipconfig           Display network adapter details (IP Address, Subnet Mask, etc.)\n");
    printf("  ping [IP]          Ping a specific IP address\n");
    printf("  nslookup [DOMAIN]  Perform nslookup on a domain\n");
    printf("  tracert [IP]       Perform a traceroute to a specific IP address\n");
    printf("  interfaces         Display detailed network interface information\n");
    printf("  checkinternet      Check if there is an active internet connection\n");
    printf("  portscan [IP] [PORT] Scan a specific port on an IP\n");
    printf("  resetadapters      Reset network adapters\n");
    printf("  stats              Display connection statistics\n");
    printf("  firewallcheck      Check firewall settings\n");
    printf("  speedtest          Perform a basic download speed test\n");
    printf("  savereport         Save the diagnostic report to a file\n");
    printf("  exit               Quit the diagnostic tool\n");
}

// IPConfig function
void ipconfig() {
    printf("\n--- NETWORK ADAPTER DETAILS ---\n");
    run_command("ipconfig", "ipconfig_result.txt");
    print_file("ipconfig_result.txt");
}

// Ping test function
void ping_test(const char *ip) {
    char command[256];
    snprintf(command, sizeof(command), "ping %s", ip);
    run_command(command, "ping_result.txt");
    printf("\n--- PING RESULT ---\n");
    print_file("ping_result.txt");
}

// Nslookup function
void nslookup_test(const char *domain) {
    char command[256];
    snprintf(command, sizeof(command), "nslookup %s", domain);
    run_command(command, "nslookup_result.txt");
    printf("\n--- NSLOOKUP RESULT ---\n");
    print_file("nslookup_result.txt");
}

// Traceroute function
void tracert_test(const char *ip) {
    char command[256];
    snprintf(command, sizeof(command), "tracert %s", ip);
    run_command(command, "tracert_result.txt");
    printf("\n--- TRACEROUTE RESULT ---\n");
    print_file("tracert_result.txt");
}

// Display network interfaces
void display_interface_info() {
    printf("\n--- NETWORK INTERFACE INFORMATION ---\n");
    run_command("ipconfig /all", "interface_info.txt");
    print_file("interface_info.txt");
}

// Check internet connection
void check_internet_connection() {
    printf("\n--- CHECKING INTERNET CONNECTION ---\n");
    if (system("ping -n 1 8.8.8.8 > nul") == 0) {
        printf("Internet connection is working.\n");
    } else {
        printf("No internet connection detected.\n");
    }
}

// Port scanning function
void port_scan(const char *ip_address, int port) {
    char command[256];
    snprintf(command, sizeof(command), "nc -zv %s %d > port_scan_result.txt", ip_address, port);
    printf("\n--- PORT SCAN RESULT ---\n");
    run_command(command, "port_scan_result.txt");
    print_file("port_scan_result.txt");
}

// Reset network adapters
void reset_network_adapters() {
    printf("\n--- RESETTING NETWORK ADAPTERS ---\n");
    run_command("netsh int ip reset", "reset_adapters.txt");
    run_command("netsh winsock reset", "reset_adapters.txt");
    printf("Network adapters reset. Please restart your computer for changes to take effect.\n");
}

// Show connection statistics
void connection_stats() {
    printf("\n--- CONNECTION STATISTICS ---\n");
    run_command("netstat -e", "connection_stats.txt");
    print_file("connection_stats.txt");
}

// Check firewall status
void check_firewall() {
    printf("\n--- CHECKING FIREWALL STATUS ---\n");
    run_command("netsh advfirewall show allprofiles", "firewall_status.txt");
    print_file("firewall_status.txt");
}

// Basic speed test
void speed_test() {
    printf("\n--- BASIC SPEED TEST ---\n");
    printf("This will download a test file from a server to calculate speed...\n");
    run_command("wget -O test_file.zip http://ipv4.download.thinkbroadband.com/10MB.zip", "speed_test.txt");
    print_file("speed_test.txt");
}

// Save report to file
void save_report() {
    printf("\nSaving diagnostic report to network_diagnostic_report.txt...\n");
    run_command("type ipconfig_result.txt ping_result.txt nslookup_result.txt tracert_result.txt > network_diagnostic_report.txt", "network_diagnostic_report.txt");
}

int main() {
    char input[512];
    char command[50];
    char arg1[256];
    int arg2;

    printf("Welcome to the Network Diagnostic Tool\n");
    printf("Type 'help' to see available commands.\n");

    // Command loop
    while (1) {
        printf("\nnetwork_diagnoser> ");
        fgets(input, sizeof(input), stdin);
        input[strcspn(input, "\n")] = '\0';  // Remove trailing newline

        // Parse the command input
        int num_args = sscanf(input, "%s %s %d", command, arg1, &arg2);

        if (strcmp(command, "help") == 0) {
            display_help();
        } else if (strcmp(command, "ipconfig") == 0) {
            ipconfig();
        } else if (strcmp(command, "ping") == 0 && num_args >= 2) {
            ping_test(arg1);
        } else if (strcmp(command, "nslookup") == 0 && num_args >= 2) {
            nslookup_test(arg1);
        } else if (strcmp(command, "tracert") == 0 && num_args >= 2) {
            tracert_test(arg1);
        } else if (strcmp(command, "interfaces") == 0) {
            display_interface_info();
        } else if (strcmp(command, "checkinternet") == 0) {
            check_internet_connection();
        } else if (strcmp(command, "portscan") == 0 && num_args >= 3) {
            port_scan(arg1, arg2);
        } else if (strcmp(command, "resetadapters") == 0) {
            reset_network_adapters();
        } else if (strcmp(command, "stats") == 0) {
            connection_stats();
        } else if (strcmp(command, "firewallcheck") == 0) {
            check_firewall();
        } else if (strcmp(command, "speedtest") == 0) {
            speed_test();
        } else if (strcmp(command, "savereport") == 0) {
            save_report();
        } else if (strcmp(command, "exit") == 0) {
            printf("Exiting the Network Diagnostic Tool. Goodbye!\n");
            break;
        } else {
            printf("Unknown command. Type 'help' for a list of available commands.\n");
        }
    }

    return 0;
}
