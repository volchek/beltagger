:: Create build directory
md build
cd build

:: Build the database assembler executable file
cmake ..\src_assembler
cmake --build . --config Release
move Release\assembler.exe ..

:: Build the lemmatizer executable file
del CMakeCache.txt
cmake ..\src_lemmatizer
cmake --build . --config Release
move Release\lemmatizer.exe ..

:: Clean up
cd ..
rmdir build /S /Q

:: Assemble the database
assembler.exe -i src_db

pause