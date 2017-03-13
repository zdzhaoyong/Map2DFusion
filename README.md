# Map2DFusion
------------------------------------------------------------------------------
## Brief Introduction
This is an open-source implementation of paper:
Map2DFusion: Real-time Incremental UAV Image Mosaicing based on Monocular SLAM.

Website : http://zhaoyong.adv-ci.com/map2dfusion/

Video   : https://www.youtube.com/watch?v=-kSTDvGZ-YQ

PDF     : http://zhaoyong.adv-ci.com/Data/map2dfusion/map2dfusion.pdf   

## 1. Compilation
### 1.1. Resources
  * Download the latest code with: 
    * Git: 
    
            git clone https://github.com/zdzhaoyong/Map2DFusion

### 1.2. Dependencies
- OpenCV  : sudo apt-get install libopencv-dev
- Qt      : sudo apt-get install build-essential g++ libqt4-core libqt4-dev libqt4-gui qt4-doc qt4-designer
- QGLViewer : sudo apt-get install libqglviewer-dev libqglviewer2
- Boost   : sudo apt-get install libboost1.54-all-dev
- GLEW    : sudo apt-get install libglew-dev libglew1.10
- GLUT : sudo apt-get install freeglut3 freeglut3-dev
- CUDA (optional) : see https://developer.nvidia.com/cuda-downloads

> Warnning: Compilation with CUDA can be enabled after CUDA_PATH defined.

### 1.3. Compilation
If you are using linux systems, it can be compiled with one command (tested on ubuntu 14.04):

    cd Map2DFusion;make

## 2. Usage
Obtain the sample sequence and launch:

    git clone https://github.com/zdzhaoyong/phantom3-village-kfs
    ./Map2DFusion DataPath=phantom3-village-kfs
    
More sequences can be downloaded at the [NPU DroneMap Dataset](http://zhaoyong.adv-ci.com/npu-dronemap-dataset).
## 3. Contact

If you have any issue compiling/running Map2DFusion or you would like to know anything about the code, please contact the authors:

     Yong Zhao -> zd5945@126.com



