# Hello-World

- Install 
    ```
    sudo apt-get install build-essential
    sudo apt install xorg-dev libglu1-mesa-dev freeglut3-dev mesa-common-dev
    sudo apt-get install libfreetype-dev libfreetype6 libfreetype6-dev
    pkg-config --cflags freetype2
    ```

- Install `cmake`
    ```
    sudo apt install cmake
    ```

- Run following commands:

    ```
    mkdir build
    cd build
    cmake ..
    make
    ./Hello-World
    ```