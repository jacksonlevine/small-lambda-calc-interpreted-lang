mkdir build
cd build
cmake -DCMAKE_TOOLCHAIN_FILE=~/Documents/vcpkg/scripts/buildsystems/vcpkg.cmake ..
cmake --build . --config Release
copy /Y Release\y.exe ..
pause