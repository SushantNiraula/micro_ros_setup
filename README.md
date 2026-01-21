# This is how i created a microros setup for my agribot Project

1. First step is to create a microros agent in your raspberry pi device or any device running your ROS2 environment. 

    I used the <a href="https://technologiehub.at/project-posts/micro-ros-on-esp32-tutorial/">technologiehub.at</a> blog to setup. 

    Just run:


## Source the ROS 2 installation
    source /opt/ros/$ROS_DISTRO/setup.bash

## Create a workspace and download the micro-ROS tools
    mkdir microros_ws
    cd microros_ws
    git clone -b $ROS_DISTRO https://github.com/micro-ROS/micro_ros_setup.git src/micro_ros_setup

## Update dependencies using rosdep
    sudo apt update && rosdep update
    rosdep install --from-paths src --ignore-src -y

## Build micro-ROS tools and source them
    colcon build
    source install/local_setup.bash

## Download micro-ROS-Agent packages
    ros2 run micro_ros_setup create_agent_ws.sh

## Build step
    ros2 run micro_ros_setup build_agent.sh
    source install/local_setup.bash

2. Then setup your esp32 for the microros. We will use platformio so first edit the `platformio.ini` file first:

        [env:esp32doit-devkit-v1]
        platform = espressif32
        board = esp32doit-devkit-v1
        framework = arduino

        monitor_speed = 115200

        board_microros_distro = jazzy
        board_microros_transport = serial

        lib_deps = 
            https://github.com/micro-ROS/micro_ros_platformio

3. After which run the code in `src/main.cpp`:

    this code will creat a node `esp32_dummy_node` and publishes on a topic `/esp32/chatter` and subscribes to topic `/esp32/led` which lights the esp32 internal led if the msg in the topic = 1 else off. 