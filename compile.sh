clear
mkdir -p build
cd build
cmake -DCMAKE_EXPORT_COMPILE_COMMANDS=ON -G Ninja ..
cp -r compile_commands.json ../

ninja install

cd ..
