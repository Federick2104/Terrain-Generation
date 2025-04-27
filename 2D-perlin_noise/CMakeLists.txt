cmake_minimum_required(VERSION 3.22)
project(TerrainGeneration)


# Trova SFML
find_package(SFML 3.0 REQUIRED COMPONENTS Graphics Window System)

# Imposta lo standard C++
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Aggiungi i sorgenti
file(GLOB SOURCES "src/*.cpp")

# Crea l'eseguibile PRIMA di modificare le sue proprietà
add_executable(TerrainGeneration ${SOURCES})

# ------------------------------------------------------------------
# TEST NOISE Noise.cpp

# Aggiungi questa sezione dopo il target principale
add_executable(NoiseTest 
    src/Noise.cpp
)

target_include_directories(NoiseTest PRIVATE 
    ${CMAKE_SOURCE_DIR}/include
)

# Aggiungi definizione per abilitare il main di test
target_compile_definitions(NoiseTest PRIVATE 
    TEST_NOISE_STANDALONE
)

# ------------------------------------------------------------------

# Aggiungi include e librerie DOPO la creazione del target
target_include_directories(TerrainGeneration PRIVATE ${SFML_INCLUDE_DIRS})
target_link_libraries(TerrainGeneration PRIVATE
    SFML::Graphics
    SFML::Window
    SFML::System
)

target_include_directories(TerrainGeneration PRIVATE 
    ${SFML_INCLUDE_DIR}
    ${CMAKE_SOURCE_DIR}/SFML-3.0/include  # Percorso assoluto alternativo
    ${CMAKE_SOURCE_DIR}/include)
# Imposta il percorso di SFML 3.0 (modifica con il tuo percorso effettivo)
set(SFML_ROOT "${CMAKE_SOURCE_DIR}/SFML-3.0")

# Configurazione per Windows (copia DLL)
if(WIN32)
    add_custom_command(TARGET TerrainGeneration POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${SFML_ROOT}/bin/sfml-graphics-3.dll"
            "${SFML_ROOT}/bin/sfml-window-3.dll"
            "${SFML_ROOT}/bin/sfml-system-3.dll"
            $<TARGET_FILE_DIR:TerrainGeneration>
    )
endif()


if(APPLE)
    add_custom_command(TARGET TerrainGeneration POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${SFML_ROOT}/lib/"
            $<TARGET_FILE_DIR:TerrainGeneration>
    )
endif()