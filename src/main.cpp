#include <iostream>
#include <filesystem>
#include <fstream>
#include <chrono>
#include <cctype>

namespace fs = std::filesystem;

// One known criteria:  everything after the equal sign is the url
void UrlFromString( std::string& strIn, std::string& strOut, size_t foundAt) {
    std::size_t foundFirstAlpha = 0;
    std::size_t foundEq = strIn.find_first_of("=", foundAt);
    foundFirstAlpha = foundEq+1;

    while( std::isalnum(strIn.at(foundEq)) && foundFirstAlpha <= strIn.size())
        ++foundFirstAlpha;

    if ( foundFirstAlpha <= strIn.size())
        strOut = strIn.substr (foundFirstAlpha);
}


bool GetRemoteUrl( const fs::path& p, std::string& ru ) {

    std::string remoteToken("[remote \"origin\"]");
    std::string lineBuf;

    // We know the file exists now lets open it
    std::ifstream stream(p.c_str());
    if ( stream.is_open() ) {
        while( !stream.eof()) {
            std::getline(stream,lineBuf);

            // Did we find the remote origin section?
            if( lineBuf == remoteToken)
            {
                // search for the url
                while (!stream.eof()) {
                    std::getline(stream,lineBuf);
                    std::size_t foundAt = lineBuf.find("url");

                    // Check if found
                    if (foundAt != std::string::npos) {
                        UrlFromString(lineBuf, ru, foundAt);
                        stream.close();
                        return true;
                    }
                }
            }
        }
        stream.close();
    }

    return false;
}

int main( int argc, char **argv) {

    if ( argc < 2) {
        std::cout << "\nEnter a path to recursevely search from\n";
        return 0;
    }

    fs::path mPath = argv[1];
    fs::path mPathWithConfig;
    fs::path configFile;
    std::string remoteUrl;

    bool hasRemoteUrl = false;
    int remoteUrlCount = 0;
    int gitReposCount = 0;
    int filesCheckedCount = 0;
    int filesIsSymlinksCount = 0;

    // Just to time the scan
    std::chrono::time_point<std::chrono::system_clock> scanStart;
    scanStart = std::chrono::system_clock::now();

    // Start the recursively scan loop
    for(auto& p: fs::recursive_directory_iterator(mPath, fs::directory_options::skip_permission_denied )) {

       filesCheckedCount++;

        // Skip symlinks
        if ( fs::is_symlink(p)) {
            filesIsSymlinksCount++;
            continue;
        }

        if(fs::is_directory(p) ) {

            // the path is a directory named .git
            if ( p.path().filename().string() == ".git") {
                gitReposCount++;

                configFile = p.path();
                configFile /= "config";

                if( fs::exists(configFile)) {
                    // std::cout << "Config file exists\n";
                    remoteUrl.clear();
                    hasRemoteUrl = false;
                    if( GetRemoteUrl( configFile, remoteUrl)) {
                        hasRemoteUrl = true;
                        remoteUrlCount++;
                    }
                }

                std::cout << p.path().parent_path() << ", " << remoteUrl << "\n";
            }
       }
   }

    std::chrono::duration<double> elapsed_seconds = std::chrono::system_clock::now() - scanStart;

    std::cout << "\n\nStatus after " << int(elapsed_seconds.count()) << " seconds\n";
    std::cout << "  Files scanned   : " << filesCheckedCount << "\n";
    std::cout << "  Symlinks skipped: " << filesIsSymlinksCount << "\n";
    std::cout << "  GitHub repos    : " << gitReposCount << "\n";
    std::cout << "  With remotes    : " << remoteUrlCount << "\n";

    return 0;
}
