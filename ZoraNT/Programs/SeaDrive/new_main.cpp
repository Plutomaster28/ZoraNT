#include <iostream>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <ctime>
#include <sstream>
#include <vector>
#include <string>
#include <algorithm>
#include <cstdlib>
#include <windows.h>
#include <stdexcept>

#define MAX_INPUT_SIZE 1024
#include <unordered_map>

namespace fs = std::filesystem;

const std::string executableDir = "MEI_DEF"; // Adjust this path if necessary

void flipper(const std::string& executable) {
    // Check if the directory exists
    if (!fs::exists(executableDir)) {
        // Attempt to create the directory
        try {
            fs::create_directories(executableDir);
            std::cerr << "Directory " << executableDir << " created successfully." << std::endl;
        } catch (const std::exception& e) {
            std::cerr << "Error creating directory: " << e.what() << std::endl;
            return;
        }
    }

    // Create the full path to the executable
    fs::path execPath = fs::path(executableDir) / (executable + ".exe"); // Include the .exe extension

    // Check if the executable exists and is a regular file
    if (fs::exists(execPath) && fs::is_regular_file(execPath)) {
        std::cout << "Switching to " << execPath.filename().string() << " mode..." << std::endl;

        // Execute the new shell
        int result = std::system(execPath.string().c_str());
        if (result != 0) {
            std::cerr << "Error starting " << execPath.filename().string() << std::endl;
        }
    } else {
        std::cerr << "Error: " << execPath.filename().string() << " is not a valid executable." << std::endl;
    }
}

/* Tetra section starts here */

const std::string installedPackagesDir = "installed_packages/"; // Ensure there is a trailing slash

// Function to install a package from a GitHub repository
void installPackage(const std::string& packageName, const std::string& gitLink) {
    std::cout << "Attempting to clone package from provided GitHub link..." << std::endl;

    std::string command = "git clone " + gitLink + " " + installedPackagesDir + packageName;

    int result = system(command.c_str());

    if (result == 0) {
        std::cout << "Successfully cloned package: " << packageName << " from GitHub." << std::endl;
    } else {
        std::cerr << "Error: Failed to clone package " << packageName << " from GitHub." << std::endl;
    }
}

// Function to uninstall a package
void uninstallPackage(const std::string& packageName) {
    fs::path installedPath = installedPackagesDir + packageName;

    try {
        if (fs::exists(installedPath)) {
            // Attempt to remove all files and directories within the package directory
            for (const auto& entry : fs::recursive_directory_iterator(installedPath)) {
                fs::permissions(entry, fs::perms::owner_all | fs::perms::group_all | fs::perms::others_all, fs::perm_options::add);
            }
            fs::remove_all(installedPath);
            std::cout << "Uninstalled package: " << packageName << std::endl;
        } else {
            std::cerr << "Error: Package '" << packageName << "' not installed." << std::endl;
        }
    } catch (const fs::filesystem_error& e) {
        std::cerr << "Error: Failed to uninstall package '" << packageName << "'. " << e.what() << std::endl;
    }
}

// Function to list available packages in the installed packages directory
void listInstalledPackages() {
    if (!fs::exists(installedPackagesDir) || fs::is_empty(installedPackagesDir)) {
        std::cout << "No packages installed." << std::endl;
        return;
    }

    std::cout << "Installed packages:\n";
    for (const auto& entry : fs::directory_iterator(installedPackagesDir)) {
        if (fs::is_directory(entry.path())) {
            std::cout << "- " << entry.path().filename().string() << std::endl;
        }
    }
}

// Function to handle the "tetra" command
void handleTetraCommand(const std::vector<std::string>& tokens) {
    if (tokens.size() < 2) {
        std::cout << "Error: Missing arguments. Usage: tetra <command> [<args>]" << std::endl;
        return;
    }

    const std::string& command = tokens[1];

    if (command == "install") {
        if (tokens.size() != 4) {
            std::cout << "Error: Usage: tetra install <package_name> <git_link>" << std::endl;
        } else {
            const std::string& packageName = tokens[2];
            const std::string& gitLink = tokens[3];
            installPackage(packageName, gitLink);
        }
    } else if (command == "uninstall") {
        if (tokens.size() != 3) {
            std::cout << "Error: Usage: tetra uninstall <package_name>" << std::endl;
        } else {
            const std::string& packageName = tokens[2];
            uninstallPackage(packageName);
        }
    } else if (command == "list") {
        listInstalledPackages();
    } else {
        std::cout << "Error: Unknown command. Usage: tetra <install|uninstall|list> [<args>]" << std::endl;
    }
}

/* Tetra functions ended above */

// Function to pull updates from the SeaDrive GitHub repository
void pullSeaDriveUpdates(const std::string& repoDir) {
    if (!fs::exists(repoDir)) {
        std::cerr << "Error: SeaDrive repository not found at " << repoDir << std::endl;
        return;
    }

    // Change the working directory to the repository path
    std::string command = "cd " + repoDir + " && git pull";

    std::cout << "Checking for updates in the SeaDrive repository..." << std::endl;
    
    // Run the git pull command
    int result = system(command.c_str());

    if (result == 0) {
        std::cout << "SeaDrive repository is up to date or updated successfully." << std::endl;
    } else {
        std::cerr << "Error: Failed to pull updates from SeaDrive repository." << std::endl;
    }
}

// Function to handle the "pull" command
void handlePullCommand(const std::vector<std::string>& tokens) {
    if (tokens.size() != 2) {
        std::cout << "Error: Invalid number of arguments. Usage: pull <SeaDrive_repo_path>" << std::endl;
        return;
    }

    const std::string& repoDir = tokens[1];  // Directory of SeaDrive repository
    pullSeaDriveUpdates(repoDir);
}

//split thingy

// Function to split a string into tokens based on a delimiter
std::vector<std::string> split(const std::string& str, char delimiter) {
    std::vector<std::string> tokens;
    std::stringstream ss(str);
    std::string token;
    while (std::getline(ss, token, delimiter)) {
        tokens.push_back(token);
    }
    return tokens;
}

void processCommand(const std::string& command);

// Function to display system information
void displaySystemInfo() {
    // Operating system version
#ifdef _WIN32
    std::cout << "\nOperating System: Windows" << std::endl;
#elif __APPLE__
    std::cout << "Operating System: macOS" << std::endl;
#elif __linux__
    std::cout << "Operating System: Linux" << std::endl;
#else
    std::cout << "Operating System: Something from Narnia I have ZERO clue what the hell this is" << std::endl;
#endif

    // Hardware specifications
    std::ifstream cpuinfo("/proc/cpuinfo");
    std::string line;
    if (cpuinfo.is_open()) {
        while (std::getline(cpuinfo, line)) {
            if (line.find("model name") != std::string::npos) {
                std::cout << "CPU: " << line.substr(line.find(":") + 2) << std::endl;
                break;
            }
        }
        cpuinfo.close();
    }
    else {
        std::cout << "CPU: Why it's the silly processor of course! >w<" << std::endl;
    }

    // Memory information
    std::ifstream meminfo("/proc/meminfo");
    if (meminfo.is_open()) {
        while (std::getline(meminfo, line)) {
            if (line.find("MemTotal") != std::string::npos) {
                std::cout << "Memory: " << line.substr(line.find(":") + 2) << std::endl;
                break;
            }
        }
        meminfo.close();
    }
    else {
        std::cout << "Memory: Infinite lmfao" << std::endl;
    }

    std::cout << "\n(C)Zora system is proprietary software. All Rights Reserved." << std::endl;
    std::cout << "1985 Meisei\n" << std::endl;

    std::cout << "\nCredits: \n" << std::endl;
    std::cout << R"(
..........:=====:.......-@@@@@@*=:.....-====-:........:%%=====%@+-..-*@#:..               ...:..#%+=
++++==--:::=====:......+@@@@@@@%:......=+*#@@@@@@@@@@@@@@#++#@@@*::+@#==:..               ..-%%.+@%=
...::-==+++++=:.......:+++++++=....:=++++++++++++++++++++++++++++++=::::::.               .:===-:++-
   ....  ......:=*#*=+%%%%%%#*:=#%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%#+::::::....         .:++++=:#%-
.                ...-#@@@@@%%@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@*-:::::....    ..-**++++-#@+
                  ..-%@@@*@@@@@@@@@@@@@@@@@@@@@@%%#******#%@@@@@@@@@@@@@@@%+:::::::.....=***++++-*@*
                  .:#@@%:+@@@@@@@@@@@@@%#+-......        .......-+#%@@@@@@@@@#-::--::::=****++++=*@#
               ...:%@@@#@@@@@@@@@@@%-...                           ...+@@@@@@@@@@@@%::=*****++++=*@#
#:...          ..:*@@@@@@@@@@@@%=:                                     ..-#@@@@@@@@%=-******++++=#@*
%%*....     ...::=@@@@@@@@@@%+..                                           .=@@@@@@%=---+****+++=%@+
%%%#....   ..:::-@@@@@@@@@%:.                                               .#@@@@@@+--====***++*@%-
%%%*.+=....::::-%@@@@@@@%:...                              ..                .:#@@@@%-==---=***%%+.
%%%*.+**+::::=#%@@@@@@%:.                                 ...  ...          ...:%@@@@%----===***%%:.
+%%*.=****=:-@@@@@@@@+.                                  ..-.. ....            .:%@@@@#--==--=*@%%:.
*#%*.=******=%@@@@@@%.                                  ..-:... ........:::.     :%@@@@*----===@%:::
@#%*.=***==---@@@@@+.   ....         ...:..           ...::.:.    ..              ..#@@@@+-----=%%-.
+@**.+**--====@@@@%:. ...=...  .. ..::....            ..-::.:.   ....             ..:%@@@@=----=%%=.
:*@*.+*==---=+@@@@#:. ..*++=:............        ......:::..:.  .....         ...  ...%@@@@+=--=%%=.
=:=%.++-===--*@@@@+. ..=++++++=:................:-=+++=::...:.  ..-..         .:.     -@@@@%===#%%:.
=:.:.*+---=--#@@@@:. .:++++++++++++++++++++++++++++++-::...::.  .::.          ..      .+@@@@#=#%%-..
.....*+==----@@@@#. ..++++++++++++++++++++++++++++++-::.. .:...:::..        ...:..    ..*@@@@@%#-...
.....*=-=---=@@@@-. ..++++++++++++++++++++++++++++-:::..  .:.:::....        ...:..    ..:++@@%#.....
.....#=-----#@@@#.  .++++++++++++++++++++++++++=::::...  ..:::...           ....:.      ...#%=......
.....#*-----@@@@=.. .+++++++++++++++++++++++=::::::............   ..............:........-+#@#++=-:.       <--- This is Pluto, the creator of Zora!
:....=%====+@@@@:.. .=+++++++++++++++++=--::::::...::::... ......:=+**############-.:::-===+%%%%%%%%            If you enjoyed Zora consider maybe joining our discord, it's always open!
::--.:*%+==*@@@%:   .::----===---::::::::::::.....::::::::...=####################+=++++++++%@=..::-                                            https://discord.gg/HrKUt4PX2J
%%%*.-=+%%+*@@@#.   .:::::::::::::::::::..... ...:::::::::::..+##########**+=-:.**++=------=*@%-..::           (Btw the system specs are a bit above just scroll up, I'll fix that sometime in the future)
==+=.-===*@**#%+.   .:::=::::..:-=++*########=..::::::::::::..=--:.........   ..*+====++++++*@@%#@+.                100% Organic C++!
:::......=%*-....   .:::*::-+################*..::...-==...         .         ..**++++++++++*@%::+#:            :3
###*.===+@*+#+++=====***#*###########*+=-::...    ..:--...         ....       ..**++++++++++*@%=-:::
###*.=++@**%%++++++++****=###**+=:...... ...                  .....:+=..      ..**++++++++++*@%+-:::
%%%#.#%*+*#%*+++++++===++=-........                   .......-+*+****+..      .-**++++++++++*@%*++++
@%%*.#@@@%%@*+++++===+====-..                 ...  ...:=+**++++++++++=..      .=**++++++++++%@%....:
@@@%.=+++++%*+=-+++++***-*-:.                .....:=*++=============...    .....=**++==+++++%@%=-::..
%%%*.=++++##*+++++++***+=*=:.. .:            ...:++===========+=-....    .....:***+++=+**#@%%+++++++
%%%*.=++###@%+++++++*-==++*::-=-:.               ..:-=====-:.....      ....:-:=***+*+===+*@#++++++++
%%%#.#%*++*#%*++++*++===++*++==++:..                     .......:-+#%@@%%%@*====+*#*+===++@%**+++++
%%%*.#@@@%%@*++++*++===++*++-==++*+-:....................::--+@@@%%%%*+=+#@+--==++**+***++#***#@%++
%%%*.=++++++%#++++*++===++*++===++*++=+==++==++==++==++==++==++==++==++==++==++==++==++==++==++==++==
%%%*.=+++++*@*++++++*+++++++*++++*@%%%%%@@@@@@@@@@@-::::::::::+@%*++++++++%%++**++++++++++++++++*#@
%%%*.=++++*%%*+++++++++++++++++++%@%+++++++++++*@%*=::::::::::=*%@#++++++++%%++**++++++++++++++++*#@
%%%*.=+++%%+++++*+++*+++++++***+#@%*+++++++++%@*--==-:::::::::=---=%%*++++++#%++++++++++++****++++++
%%%*.=++*@+++++++*+======+*+++++#@%++++*#%%%%+-------.::::::.:=-----=%%#**+++%%+++++++:-=====+++++++
%%%*.=++##++++++++=======+*++++++#@%@@#=--===:-------=..  ..---------=-------+++++++*====+++++***+++
%%%#.=+++%%+++++*************++++=-----------:----===+=....-+==+-----=-------++++++**#*******+++++++
%%%*.=++++%%+++++++++++++++++++++------------::--++=====--=====++=--==--------++++++++++++++++++++*%
%%%#.-====+##++++++++++++++++++--------------::--=+=======+====+=-=-=----------=+++++++++++++++++*#%
%%%#=========================-:::::::::::::::::-:-=----------:---:---::::::::::::-==================
%%%%%%%%%%%@@%%%%%%%%%%%%%%@+=================+==+##===*==*===*#===+===============*@%%%%%%%%%%%%%@@
%%%%%%%%%%%@@%%####%%%%%%%%#======================*#+==*==*==+##====================#%%%%%%%%%%%%%%@



)";
}

// Function to print current working directory
void printCurrentDirectory() {
    std::cout << "Current Directory: " << fs::current_path() << std::endl;
}

// Function to list files in the current directory
void listFiles() {
    fs::path currentPath = fs::current_path();
    std::cout << "Files in " << currentPath << ":\n";
    for (const auto& entry : fs::directory_iterator(currentPath)) {
        std::cout << " - " << entry.path().filename().string() << std::endl;
    }
}

// Function to change the current directory
void changeDirectory(const std::string& dir) {
    fs::path newPath = fs::current_path() / dir;
    if (fs::exists(newPath) && fs::is_directory(newPath)) {
        fs::current_path(newPath);
        std::cout << "Changed directory to: " << fs::current_path() << std::endl;
    } else {
        std::cout << "Directory does not exist: " << newPath << std::endl;
    }
}

// Function to create a directory
void createDirectory(const std::string& dirName) {
    fs::path dirPath = fs::current_path() / dirName;
    if (fs::create_directory(dirPath)) {
        std::cout << "Directory created: " << dirPath << std::endl;
    } else {
        std::cout << "Error creating directory: " << dirPath << std::endl;
    }
}

// Function to remove a directory
void removeDirectory(const std::string& dirName) {
    fs::path dirPath = fs::current_path() / dirName;
    if (fs::remove(dirPath)) {
        std::cout << "Directory removed: " << dirPath << std::endl;
    } else {
        std::cout << "Error removing directory: " << dirPath << std::endl;
    }
}

// Function to create a new file
void createFile(const std::string& filename) {
    fs::path filePath = fs::current_path() / filename;
    std::ofstream file(filePath);
    if (file) {
        std::cout << "File created: " << filePath << std::endl;
        file.close();
    } else {
        std::cout << "Error creating file: " << filePath << std::endl;
    }
}

// Function to remove a file
void removeFile(const std::string& filename) {
    fs::path filePath = fs::current_path() / filename;
    if (fs::remove(filePath)) {
        std::cout << "File removed: " << filePath << std::endl;
    } else {
        std::cout << "Error removing file: " << filePath << std::endl;
    }
}

// Function to copy a file
void copyFile(const std::string& source, const std::string& destination, const std::string& destDir = "") {
    fs::path sourcePath = fs::current_path() / source;
    fs::path destinationPath;

    if (!destDir.empty()) {
        // Create a path from the destination directory and the new file name
        destinationPath = fs::path(destDir) / destination;
    } else {
        destinationPath = fs::current_path() / destination; // Default to current directory
    }

    try {
        fs::copy(sourcePath, destinationPath);
        std::cout << "File copied from " << sourcePath << " to " << destinationPath << std::endl;
    } catch (const fs::filesystem_error& e) {
        std::cout << "Error copying file: " << e.what() << std::endl;
    }
}


// Function to move a file
void moveFile(const std::string& source, const std::string& destination, const std::string& destDir = "") {
    fs::path sourcePath = fs::current_path() / source;
    
    fs::path destinationPath;
    if (!destDir.empty()) {
        // Create a path from the destination directory and the new file name
        destinationPath = fs::path(destDir) / destination;
    } else {
        destinationPath = fs::current_path() / destination; // Default to current directory
    }

    try {
        // Create the destination directory if it doesn't exist
        fs::create_directories(destinationPath.parent_path());

        // Move the file
        fs::rename(sourcePath, destinationPath);
        std::cout << "File moved from " << sourcePath << " to " << destinationPath << std::endl;
    } catch (const fs::filesystem_error& e) {
        std::cout << "Error moving file: " << e.what() << std::endl;
    }
}

// Function to rename a file
void renameFile(const std::string& current, const std::string& newName) {
    fs::path currentPath = fs::current_path() / current;
    fs::path newPath = fs::current_path() / newName;
    try {
        fs::rename(currentPath, newPath);
        std::cout << "File renamed from " << currentPath << " to " << newPath << std::endl;
    } catch (const fs::filesystem_error& e) {
        std::cout << "Error renaming file: " << e.what() << std::endl;
    }
}

// Function to search for files matching a pattern
void searchFiles(const std::string& pattern) {
    fs::path currentPath = fs::current_path();
    std::cout << "Searching for files matching pattern: " << pattern << std::endl;
    for (const auto& entry : fs::directory_iterator(currentPath)) {
        if (entry.path().filename().string().find(pattern) != std::string::npos) {
            std::cout << " - Found: " << entry.path() << std::endl;
        }
    }
}

// Function to save text to a file
void saveTextToFile(const std::string& filename, const std::string& content) {
    std::ofstream file(filename);
    if (file.is_open()) {
        file << content;
        file.close();
        std::cout << "File saved: " << filename << std::endl;
    }
    else {
        std::cout << "Error: Unable to save file." << std::endl;
    }
}

// Function to edit a text file
void editFile(const std::string& filename) {
    fs::path filePath = fs::current_path() / filename;
    std::cout << "Opening file for editing: " << filePath << std::endl;
    std::string content;
    std::cout << "\nEnter text (type 'EOF' on a new line to finish):" << std::endl;
    std::string line;
    while (std::getline(std::cin, line) && line != "EOF") {
        content += line + "\n";
    }
    saveTextToFile(filename, content);
}

// Function to run an external program
void runProgram(const std::string& program) {
    int result = std::system(program.c_str());
    if (result == 0) {
        std::cout << "Program executed successfully." << std::endl;
    }
    else {
        std::cout << "Error: Failed to execute program." << std::endl;
    }
}

// Function to display the current date
void displayDate() {
    auto now = std::time(nullptr);
    auto tm = *std::localtime(&now);
    std::cout << "Current Date: " << std::put_time(&tm, "%Y-%m-%d") << std::endl;
}

// Function to display the current time
void displayTime() {
    auto now = std::time(nullptr);
    auto tm = *std::localtime(&now);
    std::cout << "Current Time: " << std::put_time(&tm, "%H:%M:%S") << std::endl;
}

// Function to clear the screen
void clearScreen() {
#ifdef _WIN32
    std::system("cls");
#else
    std::system("clear");
#endif
}

void echo_command(const std::string& args) {
    std::cout << args << std::endl; // Print the provided arguments
}

void cat_command(const std::string& filename) {
    std::ifstream file(filename);
    if (!file) {
        std::cout << "Error: Could not open file '" << filename << "'" << std::endl;
        return;
    }
    
    std::string line;
    while (std::getline(file, line)) {
        std::cout << line << std::endl;
    }
    
    file.close();
}

void man_command(const std::string& cmd) {
    std::unordered_map<std::string, std::string> command_help = {
        {"exit", "Quits the program."},
        {"help", "This opens the help menu!"},
        {"sysinfo", "Displays system information and credits."},
        {"pwd", "Prints the current working directory."},
        {"ls", "Lists the contents of the current directory."},
        {"cd", "Changes the current working directory. You can use '..' to go back a directory; if you wish to run a combination of arguments, seperate each oner with '/'!"},
        {"mkdir", "Creates a new directory with the given name."},
        {"rmdir", "Removes the directory that is named."},
        {"touch", "Creates a new file. It's best prectice to also include the file type suffix. (e.g .txt)"},
        {"rm", "Removes the file that is specified."},
        {"cp", R"(Copies a file. To use, input the full name of the file you want copied, then afterwards, type in the name of the new file; all contents will be copied.
        Currently only copies folders to the same directory.)"},
        {"mv", "Moves a file. This command automatically creates a new directory. (Currently just works like the `cp` command, not working)"},
        {"rename", "Renames a file."},
        {"search", "Searches for files matching a pattern. This pattern being their name."},
        {"edit", "Edits a text file."},
        {"run", "Runs an external program."},
        {"calendar", "Displays the current date."},
        {"clock", "Displays the current time."},
        {"clear", "Clears the screen."},
        {"echo", "Prints the string that the user types into the console. It's funny."},
        {"cat", "Displays the contents of a folder"},
        {"man", "Gives more information on commands. It also displays this message."},
        {"|", "Takes the output of one command and uses it as the input for another command. This allows for chaining commands together to create complex operations."},
        {"<", "Redirects input from a specified file to a command. This allows a command to read data from a file instead of standard input (keyboard)."},
        {">", "Redirects the output of a command to a specified file. If the file already exists, it will be overwritten (unless you use >>, which appends)."},
        {"tetra", "Zora's package manager, this allows you to handle packages however you may."},
        {"flipper", "This function allows you to switch to sub-shells within SeaDrive, this means you can add/use external/custom CLI's with SeaDrive"},
        {"pull", "This function checks if there are any updates for SeaDrive, and if there are any, it applies them!"}
    };
    
    auto it = command_help.find(cmd);
    if (it != command_help.end()) {
        std::cout << it->first << ": " << it->second << std::endl;
    } else {
        std::cout << "Error: No manual entry for '" << cmd << "'" << std::endl;
    }
}

std::string executeCommand(const std::string& command) {
    std::stringstream buffer;
    std::streambuf* oldCoutStreamBuf = std::cout.rdbuf();
    std::cout.rdbuf(buffer.rdbuf());

    processCommand(command);  // Process the first command

    std::cout.rdbuf(oldCoutStreamBuf);
    return buffer.str();  // Return the captured output
}

void executeCommandWithInput(const std::string& command, const std::string& input) {
    std::istringstream inputStream(input);
    std::cin.rdbuf(inputStream.rdbuf());  // Redirect input stream to the given input

    processCommand(command);  // Process the second command

    std::cin.rdbuf(nullptr);  // Restore the input stream
}

void redirectOutputToFile(const std::string& command, const std::string& fileName) {
    std::ofstream file(fileName);
    std::streambuf* oldCoutStreamBuf = std::cout.rdbuf();
    std::cout.rdbuf(file.rdbuf());  // Redirect std::cout to the file

    processCommand(command);  // Process the command

    std::cout.rdbuf(oldCoutStreamBuf);  // Restore std::cout
    file.close();
}

void redirectInputFromFile(const std::string& command, const std::string& fileName) {
    std::ifstream file(fileName);
    if (!file) {
        std::cerr << "Error: Cannot open file " << fileName << std::endl;
        return;
    }

    std::streambuf* oldCinStreamBuf = std::cin.rdbuf();
    std::cin.rdbuf(file.rdbuf());  // Redirect std::cin to the file

    processCommand(command);  // Process the command

    std::cin.rdbuf(oldCinStreamBuf);  // Restore std::cin
    file.close();
}

void trim(std::string& s) {
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch) {
        return !std::isspace(ch);
    }));
    s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) {
        return !std::isspace(ch);
    }).base(), s.end());
}

// Function to parse commands
std::vector<wchar_t*> parseCommand(const std::string& command) {
    std::istringstream iss(command);
    std::string arg;
    std::vector<wchar_t*> args;

    while (iss >> arg) {
        int size_needed = MultiByteToWideChar(CP_UTF8, 0, arg.c_str(), (int)arg.size(), nullptr, 0);
        wchar_t* wArg = new wchar_t[size_needed + 1];
        MultiByteToWideChar(CP_UTF8, 0, arg.c_str(), (int)arg.size(), wArg, size_needed);
        wArg[size_needed] = 0; // Null-terminate
        args.push_back(wArg);
    }
    args.push_back(nullptr); // Null-terminate the vector

    return args;
}

/*critical shell functionality below*/

// Process command input
void processCommand(const std::string& command) {
    std::vector<std::string> tokens = split(command, ' '); //original do not delete

    if (tokens.empty()) { //original do not delete
        std::cout << "Error: Empty command." << std::endl;
        return;
    }

    std::istringstream iss(command);
    std::string subCommand;
    std::vector<HANDLE> pipes;

    STARTUPINFOW si;
    PROCESS_INFORMATION pi;
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));

    

    if (!std::getline(iss, subCommand, '|')) {
        return; // Error if no command is found
    }

    // Loop through subcommands separated by '|'
    while (std::getline(iss, subCommand, '|')) {
        HANDLE readPipe, writePipe;
        CreatePipe(&readPipe, &writePipe, nullptr, 0);
        pipes.push_back(readPipe); // Save the read end for the next command

        // Initialize STARTUPINFOW and PROCESS_INFORMATION again for each subcommand
        STARTUPINFOW si;
        PROCESS_INFORMATION pi;
        ZeroMemory(&si, sizeof(si));
        si.cb = sizeof(si);
        ZeroMemory(&pi, sizeof(pi));

        si.dwFlags |= STARTF_USESTDHANDLES;
        si.hStdOutput = writePipe;
        si.hStdError = writePipe; // Optional: redirect stderr as well

        // Parse the subCommand to wide characters
        std::wstring wideCommand = std::wstring(subCommand.begin(), subCommand.end());
        std::vector<wchar_t> wideArgs(wideCommand.begin(), wideCommand.end());
        wideArgs.push_back(0); // Null-terminate the command

        // Create the process using CreateProcessW
        if (!CreateProcessW(nullptr, wideArgs.data(), nullptr, nullptr, TRUE, 0, nullptr, nullptr, &si, &pi)) {
            std::wcerr << L"Error creating process: " << GetLastError() << std::endl;
            exit(1); // Handle the error as appropriate
        }

        CloseHandle(writePipe);
        WaitForSingleObject(pi.hProcess, INFINITE);
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
    }

    // Close all pipes
    for (HANDLE pipe : pipes) {
        CloseHandle(pipe);
    }

    std::string cmd = tokens[0]; //original do not delete
    std::vector<std::string> args(tokens.begin() + 1, tokens.end()); //original do not delete

    

    if (cmd == "exit") {
        std::cout << "Exiting..." << std::endl;
    }
    else if (cmd == "pull") {
    handlePullCommand(tokens);
    }
    else if (cmd == "sysinfo") {
        displaySystemInfo();
    }
    else if (cmd == "pwd") {
        printCurrentDirectory();
    }
    else if (cmd == "ls") {
        listFiles();
    }
    else if (cmd == "cd") {
        if (args.empty()) {
            std::cout << "Error: Directory path doesn't exist" << std::endl;
        }
        else {
            changeDirectory(args[0]);
        }
    }
    else if (cmd == "mkdir") {
        if (args.empty()) {
            std::cout << "Error: Missing directory name." << std::endl;
        }
        else {
            createDirectory(args[0]);
        }
    }
    else if (cmd == "rmdir") {
        if (args.empty()) {
            std::cout << "Error: Missing directory name." << std::endl;
        }
        else {
            removeDirectory(args[0]);
        }
    }
    else if (cmd == "touch") {
        if (args.empty()) {
            std::cout << "Error: Missing file name." << std::endl;
        }
        else {
            createFile(args[0]);
        }
    }
    else if (cmd == "rm") {
        if (args.empty()) {
            std::cout << "Error: Missing file name." << std::endl;
        }
        else {
            removeFile(args[0]);
        }
    }
    else if (cmd == "cp") {
        if (args.size() < 2) {
            std::cout << "Error: Missing source or destination file." << std::endl;
        }
        else {
            copyFile(args[0], args[1]);
        }
    }
    else if (cmd == "mv") {
        if (args.size() < 2) {
            std::cout << "Error: Missing source or destination file." << std::endl;
        }
        else {
            moveFile(args[0], args[1]);
        }
    }
    else if (cmd == "rename") {
        if (args.size() < 2) {
            std::cout << "Error: Missing current or new file name." << std::endl;
        }
        else {
            renameFile(args[0], args[1]);
        }
    }
    else if (cmd == "search") {
        if (args.empty()) {
            std::cout << "Error: Missing search pattern." << std::endl;
        }
        else {
            searchFiles(args[0]);
        }
    }
    else if (cmd == "edit") {
        if (args.empty()) {
            std::cout << "Error: Missing file name." << std::endl;
        }
        else {
            editFile(tokens[1]);
        }
    }
    else if (cmd == "run") {
        if (args.empty()) {
            std::cout << "Error: Missing program name." << std::endl;
        }
        else {
            runProgram(args[0]);
        }
    }
    else if (cmd == "echo") {
        if (args.empty()) {
            std::cout << std::endl; // Print a blank line if no arguments are provided
        } else {
            // Concatenate the arguments into a single string
            std::string echoOutput;
            for (size_t i = 0; i < args.size(); ++i) {
                echoOutput += args[i]; // Add the argument to the output
                if (i < args.size() - 1) {
                    echoOutput += " "; // Add a space between arguments
                }
            }
            echo_command(echoOutput); // Pass the concatenated string to the echo_command function
        }
    }
    else if (cmd == "flipper") {
    if (args.empty()) {
        std::cout << "Error: Missing executable name." << std::endl;
    } else {
        flipper(args[0]); // Pass the executable name directly
    }
}
    else if (cmd == "cat") {
        if (args.empty()) {
            std::cout << "Error: Missing file name." << std::endl;
        } else {
            cat_command(args[0]); // Pass the filename to the cat_command function
        }
    }
    else if (cmd == "man") {
        if (args.empty()) {
            std::cout << "Error: Missing command name." << std::endl;
        } else {
            man_command(args[0]); // Pass the command name to the man_command function
        }
    }
    // Handle package manager commands
    else if (cmd == "tetra") {
    if (tokens[1] == "install") {
        if (tokens.size() != 4) { // Check for exactly 4 tokens
            std::cout << "Error: Usage: tetra install <package_name> <git_link>" << std::endl;
        } else {
            // Validate if the third token is a valid GitHub link
            if (tokens[3].find("https://github.com/") == 0) {
                // Call tetraInstallFromGitHub with package name and git link
                installPackage(tokens[2], tokens[3]); // No ambiguity here
            } else {
                std::cout << "Error: Invalid GitHub link." << std::endl;
            }
        }
    } else if (tokens[1] == "uninstall" && tokens.size() == 3) {
        uninstallPackage(tokens[2]);
    } else if (tokens[1] == "list" && tokens.size() == 2) {
        listInstalledPackages();
    } else {
        std::cout << "Error: Missing arguments!" << std::endl;
    }
}
    /*above is the tetra function*/
    else if (cmd == "calendar") {
        displayDate();
    }
    else if (cmd == "clock") {
        displayTime();
    }
    else if (cmd == "help") {
std::cout << R"(
                                                
                                      
 _____         ____      _            
|   __|___ ___|    \ ___|_|_ _ ___    
|__   | -_| .'|  |  |  _| | | | -_|   
|_____|___|__,|____/|_| |_|\_/|___|   
                                      
                                      
 _____     _                          
|  |  |___| |___    _____ ___ ___ _ _ 
|     | -_| | . |  |     | -_|   | | |
|__|__|___|_|  _|  |_|_|_|___|_|_|___|
            |_|                       

        exit - Quit the program                                                                 Tetra Help:
        sysinfo - Display system information                                                    Install <github link> - installs the git repository using the provided link.
        pwd - Print the current working directory                                               Uninstall <directory name> - uninstall the library directory.
        ls - List the contents of the current directory                                         list - lists all installed libraries.
        cd <directory> - Change the current working directory                                   -----------------------------------------------------------------------------
        mkdir <name> - Create a new directory
        rmdir <name> - Remove a directory                                                       build (currently unavailable) - Builds ZoraNT in the same style Linux Arch is built.
        touch <name> - Create a new file                                                        flipper <module> - Flips between other low-level Zora processes.
        rm <name> - Remove a file                                                               pull (currently unavailable) - Pulls the latest updates if there are any!
        cp <source> <new_source> - Copy a file
        mv <source> <destination> - Move a file
        rename <current> <new> - Rename a file
        search <pattern> - Search for files matching a pattern
        edit <file> - Edit a text file
        run <program> - Run an external program
        echo <string> - Displays a line of text or a variable's value
        cat <file> - Opens contents of the specified file
        man <command> - Displays additional information based on the provided command.
        Pipe (|): Used to pass the output of one command as input to another.
        Input Redirection (<): Used to read input from a file instead of standard input.
        Output Redirection (>): Used to write the output of a command to a file, overwriting the file if it exists.
        Append Output Redirection (>>): Used to write the output of a command to a file, appending to the file if it exists.
        tetra <command> <package> - Tetra is Zora's package manager!                                  
        calendar - Display current date
        clock - Display current time
        clear - Clears out the screen
        )" << std::endl;
    }
    else if (cmd == "clear") {
        clearScreen();
    }
    else {
        std::cout << "Error: Unknown command '" << cmd << "'" << std::endl;
    }
}

// Main function
int main() {
    // Create installation directory if it doesn't exist
    fs::create_directory("installed_packages");

    std::string command;
    std::string input;

    // Introductory message
    std::cout << R"(
  ________  _______   ________  ________  ________  ___  ___      ___ _______       
 |\   ____\|\  ___ \ |\   __  \|\   ___ \|\   __  \|\  \|\  \    /  /|\  ___ \     
 \ \  \___|\ \   __/|\ \  \|\  \ \  \_|\ \ \  \|\  \ \  \ \  \  /  / | \   __/|     
  \ \_____  \ \  \_|/_\ \   __  \ \  \ \\ \ \   _  _\ \  \ \  \/  / / \ \  \_|/__   
   \|____|\  \ \  \_|\ \ \  \ \  \ \  \_\\ \ \  \\  \\ \  \ \    / /   \ \  \_|\ \ 
     ____\_\  \ \_______\ \__\ \__\ \_______\ \__\\ _\\ \__\ \__/ /     \ \_______\
    |\_________\|_______|\|__|\|__|\|_______|\|__|\|__|\|__|\|__|/       \|_______|
    \|_________|                                                            


Welcome to SeaDrive!

    )" << std::endl;
    std::cout << "\nType 'help' for a list of commands." << std::endl;

    // Main command loop
    while (true) {
        std::cout << "> ";
        std::getline(std::cin, input);

        processCommand(input);

        if (input == "exit") {
            break;
        } 
    }

    return 0;
}