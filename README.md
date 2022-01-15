
# Building

## Common
```bash
mkdir build
```
```bash
cd build
```

### Windows :
```bash
cmake ..
```
- open created .sln file and build the solution (Release mode recommended)
- opening the folder from visual studio works best (Does not require common step)

### Linux/OSX :
```bash
cmake -DCMAKE_BUILD_TYPE=Release ..
```
```bash
make -j 16
```
 - output file located inside out folder
