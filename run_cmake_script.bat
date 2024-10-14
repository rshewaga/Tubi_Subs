@echo off

:: Call this script within [project]/build

REM Get directory where this script resides
REM https://stackoverflow.com/questions/17063947/get-current-batchfile-directory
set DIR=%~dp0

REM Build configuration
set BUILD_CONFIG=Release
REM Check for first argument being the build config
if [%1] NEQ [] (
    set BUILD_CONFIG=%1
)


REM ===============================
REM Create a new conan profile from detected properties named "autodetect"
REM detect      =>  auto detect settings and fill [settings] section
REM --force     =>  overwrite existing profile with given name, if exists
REM --name      =>  set profile name to "autodetect"
conan profile ^
detect ^
--force ^
--name autodetect

REM Install missing Conan packages
REM --settings               => apply an overwriting setting
REM   compiler.cppstd        => set the C++ standard to C++ 20 with GNU extensions
REM --build missing          => build packages from source whose binary package is not found
REM --generator              => generator to use
REM --profile autodetect     => Use the 'autodetect' profile as the host and build profile
conan install %DIR%\conanfile.py ^
--build=missing ^
--profile:host=autodetect ^
--profile:build=autodetect ^
--output-folder=. ^
--settings=build_type=%BUILD_CONFIG% ^
--settings=compiler.cppstd=20

REM ===============================
REM Replace \ with / for cmake
set "DIR=%DIR:\=/%"

REM  -D CMAKE_MODULE_PATH    =>  Semicolon-separated list of directories, using forward slashes,
REM                              specifying a search path for CMake modules to be loaded by the
REM                              include() or find_package() commands before checking the default
REM                              modules that come with CMake
REM  -D CMAKE_PREFIX_PATH    =>  Semicolon-separated list of directories specifying installation
REM                              prefixes to be searched by the find_package(), find_program(),
REM                              find_file(), and find_path() commands. Each command will add
REM                              appropriate subdirectories (like bin, lib, or include) as
REM                              specified in its own documentation
REM  -S                      =>  Source directory
REM  -B                      =>  Build directory
cmake ^
-D CMAKE_MODULE_PATH="%DIR%build" ^
-D CMAKE_PREFIX_PATH="%DIR%build" ^
-D CMAKE_BUILD_TYPE=%BUILD_CONFIG% ^
-S "%DIR%" ^
-B "%DIR%build