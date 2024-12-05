#include <RE/Skyrim.h>
#include <SKSE/API.h>
#include <SKSE/SKSE.h>
#include <rapidjson/document.h>
#include <rapidjson/istreamwrapper.h>
#include <rapidjson/ostreamwrapper.h>
#include <rapidjson/prettywriter.h>
#include <shlobj.h>  // Para SHGetFolderPath

#include <chrono>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <locale>  // Para configurar el locale
#include <sstream>

namespace fs = std::filesystem;

std::string GetDocumentsPath() {
    wchar_t path[MAX_PATH];
    if (SUCCEEDED(SHGetFolderPathW(NULL, CSIDL_PERSONAL, NULL, SHGFP_TYPE_CURRENT, path))) {
        std::wstring ws(path);
        return std::string(ws.begin(), ws.end());
    }
    return "";
}

std::string GetGamePath() {
    // Aquí puedes implementar la lógica para obtener la ruta de instalación del juego
    // Por ejemplo, leyendo desde un archivo de configuración o una variable de entorno
    return "G:/SteamLibrary/steamapps/common/Skyrim Special Edition";
}

void CopyAlignmentJson(std::ofstream& logFile) {
    std::string gamePath = GetGamePath();
    std::string sourceFolder = gamePath + "/Data/Mantella BackHistory/";
    std::string destinationFolder = GetDocumentsPath() + "/My Games/Mantella/data/Skyrim/character_overrides/";

    for (const auto& entry : fs::directory_iterator(sourceFolder)) {
        if (entry.path().extension() == ".json") {
            std::string sourceFile = entry.path().string();
            std::string destinationFile = destinationFolder + entry.path().filename().string();
            fs::copy_file(sourceFile, destinationFile, fs::copy_options::overwrite_existing);
            logFile << entry.path().filename().string() << std::endl;  // Escribir el nombre del archivo JSON en el log
        }
    }
}

extern "C" bool SKSEPluginLoad(const SKSE::LoadInterface* skse) {
    SKSE::Init(skse);

    // Este ejemplo imprime un arte ASCII de un gato en la consola de Skyrim y lo guarda en un archivo de log.
    SKSE::GetMessagingInterface()->RegisterListener([](SKSE::MessagingInterface::Message* message) {
        if (message->type == SKSE::MessagingInterface::kDataLoaded) {
            // 1. Obtener la ruta de la carpeta "Documentos"
            std::string documentsPath = GetDocumentsPath();

            // 2. Imprimir un arte ASCII de un gato en la consola de Skyrim
            const char* catArt =
                "Mantella NPCs Back History loaded successfully > ^ < .\n"
                "\n"
                "-------character_overrides-------\n"
                "";
            RE::ConsoleLog::GetSingleton()->Print(catArt);

            // 7. Crear una copia del log en la carpeta SKSE
            std::string logFilePath = documentsPath + "/My Games/Mantella/Mantella-Adding-NPCs-Back-History-NG.log";
            std::ofstream logFile(logFilePath, std::ios::app);
            if (logFile.is_open()) {
                auto now = std::chrono::system_clock::now();
                std::time_t in_time_t = std::chrono::system_clock::to_time_t(now);
                std::tm buf;
                localtime_s(&buf, &in_time_t);

                // Configurar el locale a inglés
                std::locale::global(std::locale("en_US.UTF-8"));

                logFile << "Log created on: " << std::put_time(&buf, "%Y-%m-%d %H:%M:%S") << std::endl;
                logFile << catArt << std::endl;

                // 8. Copiar los archivos JSON a la carpeta OStim al final del proceso y escribir sus nombres en el log
                CopyAlignmentJson(logFile);

                logFile.close();
            }

            std::string logDestinationFile =
                documentsPath + "/My Games/Skyrim Special Edition/SKSE/Mantella-Adding-NPCs-Back-History-NG.log";
            fs::copy_file(logFilePath, logDestinationFile, fs::copy_options::overwrite_existing);
        }
    });

    return true;
}
