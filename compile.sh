clear
mkdir -p build
cd build
cmake -DCMAKE_EXPORT_COMPILE_COMMANDS=ON ..
cp -r compile_commands.json ../

make -j install

cd ..
