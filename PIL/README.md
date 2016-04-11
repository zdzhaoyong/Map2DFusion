#The Pilot Intelligent Library
------------------------------------------------------------------------------

##1. Introduction
------------------------------------------------------------------------------
The [PIL](http://zhaoyong.adv-ci.com/pil/) library includes some pretty useful tools for C++ programing espesially in the areas of moblile robotics and computer vision. 

website: 
- http://adv-ci.com/pil/
- http://zhaoyong.adv-ci.com/pil/
         
### 1.1. Folder structure
* src       --- source code of the library
* apps      --- source code of examples
* scripts   --- compile scripts
* Thirdparty--- some other librarys PIL depends on

### 1.2. Modules
* base     --- basic types and utils for configuration, threads, debug and time statistics
* network  --- network transmission framework for IP/TCP, UDP
* hardware --- inplementation of Camera models, UART, GPS, IMU and joystick, etc.
* gui      --- a gui framework based on Qt and OpenGL (QGLViewer), including some basic objects
* cv       --- some computer vision utils based on OpenCV
* lua      --- the lua warper of some APIs

##2. Resources and Compilation on Linux
------------------------------------------------------------------------------
###2.1. Resources
  * Download the latest code with: 
    * Git: 
    
            git clone https://github.com/zdzhaoyong/PIL

###2.2. Dependency
**OpenCV** : sudo apt-get install libopencv-dev 

**Qt** : sudo apt-get install build-essential g++ libqt4-core libqt4-dev libqt4-gui qt4-doc qt4-designer 

**QGLViewer** : sudo apt-get install libqglviewer-dev libqglviewer2 

**Boost** : sudo apt-get install libboost1.48-all-dev

###2.2. Compilation
As we develop the whole project on Linux, only compilation *Makefile* for linux is provided. If you are using linux systems, it can be compiled with one command:

    cd PIL;make

The compilation should be ok in most circumstances and once not, you may need to modified the file *scripts/LIBS_CONFIG.linux* and config the library dependences manually.
##3. Usage and examples
Lots of examples are provided in folder apps, cd to the app folder you wanna and input *make run* to excute the application.

##4. Contact
------------------------------------------------------------------------------
If you have any problem, or you have some suggestions for this code, please contact Yong Zhao by zd5945@126.com, thank you very much!

