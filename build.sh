#! /usr/bin/env bash

# Create build directory
mkdir build
cd build

# Build the database assembler executable file
cmake ../src_assembler
cmake --build . --config Release
mv assembler ..

# Build the lemmatizer executable file
rm CMakeCache.txt
cmake ../src_lemmatizer
cmake --build . --config Release
mv lemmatizer ..

# Clean up
cd ..
rm -rf build

# Assemble the database
./assembler -i src_db

read -p "Press any key to exit..." nothing