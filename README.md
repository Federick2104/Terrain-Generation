# # CMake build script for the project
creare cartella build
mkdir build
cd build

# # CMake build script for the project
cmake .. && make 
# # Run the TestNoise program
make TestNoise 
./TestNoise 

# # Run the program
./Terraingenerator 