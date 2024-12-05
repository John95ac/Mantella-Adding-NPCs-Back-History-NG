# Este archivo configura la "compilación" de este plugin SKSE
# - cómo compilar el plugin C++ (además de las configuraciones encontradas en CMakePresets.json)
# - dónde guardar el archivo .dll resultante
# Se recomienda establecer una versión mínima de CMake.
# Si usas características de CMake de versiones más recientes, actualiza esto para que coincida.
cmake_minimum_required(VERSION 3.21)

# [INICIO establecer_nombre_proyecto]
# Establece el nombre de tu proyecto. Este será el nombre de tu archivo .dll de SKSE.
project(Mantella-Adding-NPCs-Back-History-NG VERSION 0.3.0 LANGUAGES CXX)
# [FIN establecer_nombre_proyecto]

# Define la ruta a CommonLibSSE
set(COMMONLIBSSE_PATH "E:/001 SKYRIM RESPALDO DEL RESPALDO/001_CODE/GitHub/CommonLibSSE-NG")

# Define la ruta a RapidJSON
set(RAPIDJSON_PATH "E:/001 SKYRIM RESPALDO DEL RESPALDO/001_CODE/GitHub/rapidjson 1.1.0")

# Define la ruta a irrKlang
set(IRRKLANG_PATH "E:/001 SKYRIM RESPALDO DEL RESPALDO/001_CODE/GitHub/irrKlang-64bit-1.6.0")

# Si no estás usando un gestor de mods, probablemente quieras que el plugin SKSE vaya
# dentro de la carpeta "Data" de tu Skyrim.
# Para hacer esto automáticamente, establece la variable de entorno SKYRIM_FOLDER
# a la ruta de tu carpeta de Skyrim Special Edition
if(DEFINED ENV{SKYRIM_FOLDER} AND IS_DIRECTORY "$ENV{SKYRIM_FOLDER}/Data")
    set(OUTPUT_FOLDER "$ENV{SKYRIM_FOLDER}/Data")
endif()

# Si estás usando Mod Organizer 2 o Vortex, podrías querer que esto vaya dentro
# de tu carpeta "mods", dentro de una subcarpeta llamada "<tu mod>".
# Para hacer esto automáticamente, establece la variable de entorno SKYRIM_MODS_FOLDER
# a la ruta de tu carpeta "mods"
if(DEFINED ENV{SKYRIM_MODS_FOLDER} AND IS_DIRECTORY "$ENV{SKYRIM_MODS_FOLDER}")
    set(OUTPUT_FOLDER "$ENV{SKYRIM_MODS_FOLDER}/${PROJECT_NAME}")
endif()

# De lo contrario, puedes establecer OUTPUT_FOLDER a cualquier lugar que desees :)
# set(OUTPUT_FOLDER "C:/ruta/a/cualquier/carpeta")

# Añade CommonLibSSE a tu proyecto
set(CMAKE_PREFIX_PATH ${COMMONLIBSSE_PATH} ${CMAKE_PREFIX_PATH})

# Añade RapidJSON a tu proyecto
include_directories(${RAPIDJSON_PATH}/include)

# Añade irrKlang a tu proyecto
include_directories(${IRRKLANG_PATH}/include)
link_directories(${IRRKLANG_PATH}/lib/Winx64-visualStudio)

# Añade el directorio src a los directorios de inclusión
include_directories(${CMAKE_SOURCE_DIR}/src)

# Añade todos los archivos .cpp en el directorio src
file(GLOB SOURCES "src/*.cpp")

# Configura tu plugin SKSE como un plugin SKSE!
find_package(CommonLibSSE CONFIG REQUIRED)
add_commonlibsse_plugin(${PROJECT_NAME} SOURCES ${SOURCES}) # <--- especifica todos los archivos .cpp en src
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_23) # <--- usa el estándar C++23
target_precompile_headers(${PROJECT_NAME} PRIVATE PCH.h) # <--- PCH.h es requerido!

# Enlaza la biblioteca irrKlang
target_link_libraries(${PROJECT_NAME} PRIVATE irrKlang)

# Cuando tu archivo .dll de SKSE esté compilado, esto copiará automáticamente el .dll en tu carpeta de mods.
# Solo funciona si configuras DEPLOY_ROOT arriba (o estableces la variable de entorno SKYRIM_MODS_FOLDER)
if(DEFINED OUTPUT_FOLDER)
    # Si especificas una <OUTPUT_FOLDER> (incluyendo a través de variables de entorno)
    # entonces copiaremos tus archivos de mod en Skyrim o un gestor de mods por ti!
    # Copia los archivos .dll del plugin SKSE en la carpeta SKSE/Plugins/
    set(DLL_FOLDER "${OUTPUT_FOLDER}/SKSE/Plugins")
    message(STATUS "Carpeta de salida del plugin SKSE: ${DLL_FOLDER}")
    add_custom_command(
        TARGET "${PROJECT_NAME}"
        POST_BUILD
        COMMAND "${CMAKE_COMMAND}" -E make_directory "${DLL_FOLDER}"
        COMMAND "${CMAKE_COMMAND}" -E copy_if_different "$<TARGET_FILE:${PROJECT_NAME}>" "${DLL_FOLDER}/$<TARGET_FILE_NAME:${PROJECT_NAME}>"
        VERBATIM
    )
    # Si realizas una compilación "Debug", también copia el archivo .pdb (para símbolos de depuración)
    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        add_custom_command(
            TARGET "${PROJECT_NAME}"
            POST_BUILD
            COMMAND "${CMAKE_COMMAND}" -E copy_if_different "$<TARGET_PDB_FILE:${PROJECT_NAME}>" "${DLL_FOLDER}/$<TARGET_PDB_FILE_NAME:${PROJECT_NAME}>"
            VERBATIM
        )
    endif()
endif()
