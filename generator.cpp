#include <iostream>
#include <fstream>
#include <filesystem>
#include <vector>
#include <string>
#include <algorithm>
#include <set>

namespace fs = std::filesystem;

struct FileCategories {
    std::vector<std::string> client_scripts;
    std::vector<std::string> server_scripts;
    std::vector<std::string> shared_scripts;
    std::vector<std::string> files;
    std::vector<std::string> ui_pages;
    std::set<std::string> dependencies;
};

bool endsWith(const std::string& str, const std::string& suffix) {
    if (str.length() < suffix.length()) return false;
    return str.compare(str.length() - suffix.length(), suffix.length(), suffix) == 0;
}

bool contains(const std::string& str, const std::string& substr) {
    return str.find(substr) != std::string::npos;
}

std::string getRelativePath(const fs::path& base, const fs::path& target) {
    fs::path relative = fs::relative(target, base);
    return relative.generic_string();
}

void categorizeFile(const std::string& relativePath, FileCategories& categories) {
    std::string lowerPath = relativePath;
    std::transform(lowerPath.begin(), lowerPath.end(), lowerPath.begin(), ::tolower);
    
    if (endsWith(lowerPath, ".lua")) {
        if (contains(lowerPath, "client") || contains(lowerPath, "cl_")) {
            categories.client_scripts.push_back(relativePath);
        } else if (contains(lowerPath, "server") || contains(lowerPath, "sv_")) {
            categories.server_scripts.push_back(relativePath);
        } else if (contains(lowerPath, "shared") || contains(lowerPath, "sh_")) {
            categories.shared_scripts.push_back(relativePath);
        } else {
            categories.shared_scripts.push_back(relativePath);
        }

        std::ifstream luaFile(relativePath);
        if (luaFile.is_open()) {
            std::string line;
            while (std::getline(luaFile, line)) {
                std::string lowerLine = line;
                std::transform(lowerLine.begin(), lowerLine.end(), lowerLine.begin(), ::tolower);
                if (contains(lowerLine, "dependency")) {
                    size_t start = lowerLine.find("'");
                    size_t end = lowerLine.rfind("'");
                    if (start != std::string::npos && end != std::string::npos && end > start) {
                        std::string dep = lowerLine.substr(start + 1, end - start - 1);
                        categories.dependencies.insert(dep);
                    }
                }
            }
        }
    } else if (endsWith(lowerPath, ".html")) {
        categories.ui_pages.push_back(relativePath);
        categories.files.push_back(relativePath);
    } else if (endsWith(lowerPath, ".js") || endsWith(lowerPath, ".css") || 
               endsWith(lowerPath, ".png") || endsWith(lowerPath, ".jpg") || 
               endsWith(lowerPath, ".jpeg") || endsWith(lowerPath, ".gif") || 
               endsWith(lowerPath, ".svg") || endsWith(lowerPath, ".ttf") || 
               endsWith(lowerPath, ".woff") || endsWith(lowerPath, ".woff2") ||
               endsWith(lowerPath, ".otf") || endsWith(lowerPath, ".eot") ||
               endsWith(lowerPath, ".json") || endsWith(lowerPath, ".ogg") ||
               endsWith(lowerPath, ".mp3") || endsWith(lowerPath, ".wav")) {
        categories.files.push_back(relativePath);
    }
}

void scanDirectory(const fs::path& directory, FileCategories& categories, bool verbose) {
    for (const auto& entry : fs::recursive_directory_iterator(directory)) {
        if (!entry.is_regular_file()) continue;
        std::string filename = entry.path().filename().string();
        std::string lowerFilename = filename;
        std::transform(lowerFilename.begin(), lowerFilename.end(), lowerFilename.begin(), ::tolower);
        if (lowerFilename == "fxmanifest.lua" || lowerFilename == "__resource.lua") continue;
        std::string relativePath = getRelativePath(directory, entry.path());
        categorizeFile(relativePath, categories);
        if (verbose) std::cout << "[Found] " << relativePath << std::endl;
    }
}

void generateManifest(const fs::path& directory, const FileCategories& categories, bool dryRun) {
    auto sortedClient = categories.client_scripts;
    auto sortedServer = categories.server_scripts;
    auto sortedShared = categories.shared_scripts;
    auto sortedFiles = categories.files;
    auto sortedUI = categories.ui_pages;

    std::sort(sortedClient.begin(), sortedClient.end());
    std::sort(sortedServer.begin(), sortedServer.end());
    std::sort(sortedShared.begin(), sortedShared.end());
    std::sort(sortedFiles.begin(), sortedFiles.end());
    std::sort(sortedUI.begin(), sortedUI.end());

    std::ostream* outStream;
    std::ofstream fileOut;
    if (dryRun) {
        outStream = &std::cout;
    } else {
        fs::path manifestPath = directory / "fxmanifest.lua";
        fileOut.open(manifestPath);
        if (!fileOut.is_open()) {
            std::cerr << "Error: Could not create fxmanifest.lua" << std::endl;
            return;
        }
        outStream = &fileOut;
    }
    std::ostream& out = *outStream;

    out << "fx_version 'cerulean'\n";
    out << "game 'gta5'\n\n";

    std::string folderName = directory.filename().string();
    out << "author 'Auto-Generated'\n";
    out << "description '" << folderName << "'\n";
    out << "version '1.0.0'\n\n";

    if (!categories.dependencies.empty()) {
        for (const auto& dep : categories.dependencies) {
            out << "dependency '" << dep << "'\n";
        }
        out << "\n";
    }

    if (!sortedShared.empty()) {
        out << "shared_scripts {\n";
        for (const auto& s : sortedShared) out << "    '" << s << "',\n";
        out << "}\n\n";
    }
    if (!sortedClient.empty()) {
        out << "client_scripts {\n";
        for (const auto& s : sortedClient) out << "    '" << s << "',\n";
        out << "}\n\n";
    }
    if (!sortedServer.empty()) {
        out << "server_scripts {\n";
        for (const auto& s : sortedServer) out << "    '" << s << "',\n";
        out << "}\n\n";
    }
    if (!sortedUI.empty()) {
        out << "ui_page {\n";
        for (const auto& s : sortedUI) out << "    '" << s << "',\n";
        out << "}\n\n";
    }
    if (!sortedFiles.empty()) {
        out << "files {\n";
        for (const auto& s : sortedFiles) out << "    '" << s << "',\n";
        out << "}\n";
    }

    if (!dryRun) {
        fileOut.close();
        std::cout << "\nfxmanifest.lua generated successfully at: " << (directory / "fxmanifest.lua") << std::endl;
    }
}

int main(int argc, char* argv[]) {
    std::string path;
    bool dryRun = false;
    bool verbose = false;

    std::cout << "FiveM FXManifest Generator\n";
    std::cout << "==========================\n\n";

    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "--dry-run") dryRun = true;
        else if (arg == "--verbose") verbose = true;
        else path = arg;
    }

    if (path.empty()) {
        std::cout << "Enter the resource folder path: ";
        std::getline(std::cin, path);
    }

    fs::path directory(path);

    if (!fs::exists(directory)) {
        std::cerr << "Error: Directory does not exist!" << std::endl;
        return 1;
    }
    if (!fs::is_directory(directory)) {
        std::cerr << "Error: Path is not a directory!" << std::endl;
        return 1;
    }

    FileCategories categories;

    std::cout << "\nScanning directory..." << std::endl;
    scanDirectory(directory, categories, verbose);

    if (categories.client_scripts.empty()) std::cout << "Warning: No client scripts found!" << std::endl;
    if (categories.server_scripts.empty()) std::cout << "Warning: No server scripts found!" << std::endl;

    std::cout << "\nSummary:\n";
    std::cout << "  Client scripts: " << categories.client_scripts.size() << "\n";
    std::cout << "  Server scripts: " << categories.server_scripts.size() << "\n";
    std::cout << "  Shared scripts: " << categories.shared_scripts.size() << "\n";
    std::cout << "  UI pages: " << categories.ui_pages.size() << "\n";
    std::cout << "  Files: " << categories.files.size() << "\n";
    std::cout << "  Dependencies: " << categories.dependencies.size() << "\n";

    std::cout << "\nGenerating fxmanifest.lua...\n";
    generateManifest(directory, categories, dryRun);

    if (!dryRun) {
        std::cout << "\nPress Enter to exit...";
        std::cin.get();
    }

    return 0;
}
