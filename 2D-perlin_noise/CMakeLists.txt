cmake_minimum_required(VERSION 3.22)
project(TerrainGeneration)

# Imposta lo standard C++
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# --- Configurazione per SFML locale ---
# Definisci il percorso radice della tua copia locale di SFML
# CMAKE_CURRENT_SOURCE_DIR è la directory di questo CMakeLists.txt
set(SFML_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/SFML-3.0")

# Verifica se le directory SFML esistono (opzionale ma utile)
if(NOT EXISTS "${SFML_ROOT}/include/SFML/Graphics.hpp")
    message(FATAL_ERROR "SFML include directory not found at ${SFML_ROOT}/include. Please check the SFML_ROOT path. Expected relative path: SFML-3.0")
endif()
if(NOT EXISTS "${SFML_ROOT}/lib")
    message(FATAL_ERROR "SFML library directory not found at ${SFML_ROOT}/lib. Please check the SFML_ROOT path. Expected relative path: SFML-3.0")
endif()

# Definisci le directory di include e librerie di SFML
set(SFML_INCLUDE_DIR "${SFML_ROOT}/include")
set(SFML_LIBRARY_DIR "${SFML_ROOT}/lib")
# --- Fine Configurazione SFML locale ---

# Aggiungi i sorgenti
file(GLOB SOURCES "src/*.cpp")

# Crea l'eseguibile principale
add_executable(TerrainGeneration ${SOURCES})

# Aggiungi le directory di include necessarie al target
target_include_directories(TerrainGeneration PRIVATE
    ${SFML_INCLUDE_DIR}             # Include di SFML (da SFML_ROOT)
    ${CMAKE_CURRENT_SOURCE_DIR}/include # Include del tuo progetto (es. Terrain.hpp)
)

# Specifica la directory dove trovare le librerie SFML
target_link_directories(TerrainGeneration PRIVATE ${SFML_LIBRARY_DIR})

# Collega le librerie SFML principali
target_link_libraries(TerrainGeneration PRIVATE
    sfml-graphics
    sfml-window
    sfml-system
)

# Aggiungi dipendenze specifiche per piattaforma usando blocchi if()
if(APPLE)
    # Framework per macOS
    # Usare le virgolette qui è comune quando si passano flag specifici
    target_link_libraries(TerrainGeneration PRIVATE
        "-framework OpenGL"
        "-framework Carbon"
        "-framework AppKit"
        "-framework IOKit"
        "-framework Foundation"
        # Aggiungi altri framework macOS se necessario
    )
elseif(UNIX AND NOT APPLE) # Linux
    # Librerie di sistema per Linux
    target_link_libraries(TerrainGeneration PRIVATE
        GL
        X11
        Xrandr
        freetype
        udev
        # Aggiungi altre dipendenze Linux se necessario
    )
endif()
# Nota: Windows non richiede link espliciti a dipendenze di sistema qui di solito

# ------------------------------------------------------------------
# Target per il Test di Noise.cpp (NoiseTest)
# ------------------------------------------------------------------
add_executable(NoiseTest
    src/Noise.cpp
)

# Aggiungi la directory include del tuo progetto a NoiseTest
target_include_directories(NoiseTest PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# Aggiungi definizione per abilitare il main di test in Noise.cpp
target_compile_definitions(NoiseTest PRIVATE
    TEST_NOISE_STANDALONE
)
# Nota: NoiseTest non linka SFML.

# ------------------------------------------------------------------
# Comandi Post-Build (Copia librerie SFML accanto all'eseguibile)
# ------------------------------------------------------------------

if(WIN32)
    # Su Windows, copia le DLL necessarie
    add_custom_command(TARGET TerrainGeneration POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${SFML_ROOT}/bin/sfml-graphics-3.dll"  # Verifica nomi file
            "${SFML_ROOT}/bin/sfml-window-3.dll"
            "${SFML_ROOT}/bin/sfml-system-3.dll"
            # Aggiungi altre DLL SFML/dipendenze se necessario
            $<TARGET_FILE_DIR:TerrainGeneration>
        COMMENT "Copying SFML DLLs to build directory"
    )
endif()

if(APPLE)
    # Su macOS, copia le librerie dinamiche (.dylib)
    add_custom_command(TARGET TerrainGeneration POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different # Copia solo se cambiano
            "${SFML_LIBRARY_DIR}/libsfml-graphics.dylib" # Verifica nomi file
            "${SFML_LIBRARY_DIR}/libsfml-window.dylib"
            "${SFML_LIBRARY_DIR}/libsfml-system.dylib"
            # Aggiungi altre .dylib SFML/dipendenze se necessario
            $<TARGET_FILE_DIR:TerrainGeneration>
        COMMENT "Copying SFML dylibs to build directory"
    )
    # Imposta RPATH per trovare le dylib copiate
    set_target_properties(TerrainGeneration PROPERTIES
        MACOSX_RPATH TRUE
    )
    target_link_options(TerrainGeneration PRIVATE "-Wl,-rpath,@executable_path")
endif()

if(UNIX AND NOT APPLE) # Specifico per Linux
    # Trova tutti i file .so rilevanti di SFML nella directory lib
    # Questo include i file base (es. libsfml-graphics.so) e quelli versionati (es. libsfml-graphics.so.3.0)
    file(GLOB SFML_SO_FILES
        LIST_DIRECTORIES false # Non includere directory nei risultati
        RELATIVE "${SFML_LIBRARY_DIR}" # Rendi i percorsi relativi a SFML_LIBRARY_DIR
        "${SFML_LIBRARY_DIR}/libsfml-graphics.so*"
        "${SFML_LIBRARY_DIR}/libsfml-window.so*"
        "${SFML_LIBRARY_DIR}/libsfml-system.so*"
        # Aggiungi qui altri moduli SFML se li usi (es. audio, network)
        # "${SFML_LIBRARY_DIR}/libsfml-audio.so*"
        # "${SFML_LIBRARY_DIR}/libsfml-network.so*"
    )

    # Copia tutti i file .so trovati nella directory dell'eseguibile
    # Usiamo un ciclo perché copy_if_different potrebbe avere problemi con liste lunghe o caratteri speciali
    foreach(SFML_SO_FILE ${SFML_SO_FILES})
        add_custom_command(TARGET TerrainGeneration POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
                "${SFML_LIBRARY_DIR}/${SFML_SO_FILE}" # Percorso completo del file sorgente
                "$<TARGET_FILE_DIR:TerrainGeneration>"  # Directory di destinazione (QUOTATA)
            COMMENT "Copying ${SFML_SO_FILE} to build directory"
        )
    endforeach()

    # Imposta RPATH per trovare le .so nella stessa cartella dell'eseguibile
    set_target_properties(TerrainGeneration PROPERTIES
        INSTALL_RPATH "$ORIGIN"
        BUILD_WITH_INSTALL_RPATH TRUE
    )
endif()