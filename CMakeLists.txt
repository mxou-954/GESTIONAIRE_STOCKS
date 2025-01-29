cmake_minimum_required(VERSION 3.16)

project(stock_VAE VERSION 0.1 LANGUAGES CXX)

# Activer les fonctionnalités automatiques de Qt
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

# Configurer le standard C++
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Désactiver certains avertissements spécifiques à MSVC
if(MSVC)
    add_compile_options(/Zc:__cplusplus /wd4242 /wd4244) # Assurer la conformité avec C++17 et désactiver les warnings
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)          # Supprimer les warnings sur strcpy
endif()

# Trouver les modules Qt nécessaires
find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets Sql Core Gui Charts)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets Sql Core Gui Charts)

# Ajouter OpenXLSX comme sous-dossier
add_subdirectory(OpenXLSX)  # Assurez-vous que le chemin vers OpenXLSX est correct

set(CMAKE_AUTOUIC_SEARCH_PATHS
    ${CMAKE_SOURCE_DIR}/Forms
    ${CMAKE_SOURCE_DIR}/Forms/Widgets
)

set(PROJECT_SOURCES
    main.cpp
    Source_Files/MainWindow.cpp
    Source_Files/Widgets/ProductsWidget.cpp
    Source_Files/Widgets/SupplierWidget.cpp
    Source_Files/Widgets/CategoryWidget.cpp
    Source_Files/Widgets/ImportWidget.cpp
    Source_Files/Widgets/LogsWidget.cpp
    Source_Files/Widgets/statsWidget.cpp

    Header_Files/mainwindow.h
    Header_Files/Widgets/CategoryWidget.h
    Header_Files/Widgets/LogsWidget.h
    Header_Files/Widgets/ImportWidget.h
    Header_Files/Widgets/ProductsWidget.h
    Header_Files/Widgets/SupplierWidget.h
    Header_Files/Widgets/statsWidget.h

    Forms/mainwindow.ui
    Forms/Widgets/SupplierWindow.ui
    Forms/Widgets/importwindow.ui
    Forms/Widgets/logswindow.ui
    Forms/Widgets/productswindow.ui
    Forms/Widgets/categorywindow.ui
    Forms/Widgets/statswindow.ui
)

# Créer l'exécutable
if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(stock_VAE
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}

    )
else()
    add_executable(stock_VAE
        ${PROJECT_SOURCES}
    )
endif()

# Ajouter les chemins d'inclusion
include_directories(${CMAKE_SOURCE_DIR}/Header_Files)
include_directories(${CMAKE_SOURCE_DIR}/Header_Files/Widgets)
include_directories(${CMAKE_BINARY_DIR}/stock_VAE_autogen/include)

message(STATUS "Source dir: ${CMAKE_SOURCE_DIR}")
message(STATUS "Forms dir: ${CMAKE_SOURCE_DIR}/Forms/productswindow.ui")
message(STATUS "ProductsWindow.ui: ${CMAKE_SOURCE_DIR}/Forms/Widgets/ProductsWindow.ui")
message(STATUS "Forms dir: ${CMAKE_SOURCE_DIR}/Forms/Widgets/CategoryWindow.ui")

# Lier les bibliothèques Qt nécessaires
target_link_libraries(stock_VAE PRIVATE Qt${QT_VERSION_MAJOR}::Widgets Qt${QT_VERSION_MAJOR}::Sql Qt${QT_VERSION_MAJOR}::Core Qt${QT_VERSION_MAJOR}::Gui Qt${QT_VERSION_MAJOR}::Charts)

# Lier OpenXLSX
target_link_libraries(stock_VAE PRIVATE OpenXLSX)

# Définir les propriétés pour macOS/iOS
if(${QT_VERSION} VERSION_LESS 6.1.0)
    set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.stock_VAE)
endif()
set_target_properties(stock_VAE PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

# Ajouter l'installation des fichiers nécessaires
include(GNUInstallDirs)
install(TARGETS stock_VAE
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

# Finaliser l'exécutable pour Qt 6
if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(stock_VAE)
endif()
