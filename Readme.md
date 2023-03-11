# ShabbyRenderer  

![avatar](img/preview.png)

This repository contains a trivial implementation of pure-CPU ray tracer, which supports some basic features: 
* Simple loaders for materials and triangular-meshes    
* Anti-aliasing(via stratified sampling)  
* Texture mapping
* Transparent material  
* Ideal mirror reflection  
* Hard shadow
* Parallel rendering(using openMP)
* Orthogonal camera & perspective camera
* Parallel lighting

## How To Build  

1. Get Eigen3  
Eigen is a C++ template library for linear algebra: matrices, vectors, numerical solvers, and related algorithms.  
You can also refer Eigen's official page( http://eigen.tuxfamily.org/) if you want to build Eigen on your own.  
```bash  
# For Ubuntu
sudo apt update
sudo apt install libeigen3-dev
```

2. Get dependencies(lodepng, a png loader)  
```bash
cd 'project dir'
git submodule init
git submodule update
```

3. Build ShabbyRenderer
```bash
cd 'project dir'
mkdir build
cd build
cmake ..
make
```

## How to Run
```bash
cd build
./ShabbyRenderer
```
It takes about 170 seconds to render the 1024x768 preview image on i5-8300H CPU @ 2.30GHz × 8.  