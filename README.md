# Firmware v5

<p align="middle">
    <img alt="display Nextion" height="250" src="resources/HMI.png"/>
    <img alt="display LCD backup" height="250" src="resources/HMI_backup.jpg"/>
</p>

<p align="middle">
    <img alt="ECU Front case" height="250" src="resources/ECU_Front.png"/>
    <img alt="telemetry" height="250" src="resources/telemetry.jpg"/>
</p>

# Embedded system workflow

![workflow](resources/workflow.jpg)

# Build instructions(Ubuntu)

Install the [Docker Engine](https://docs.docker.com/engine/install/ubuntu/#install-using-the-repository) and [Docker Desktop](https://docs.docker.com/desktop/install/ubuntu/)

On the project folder within the vs code, create a new terminal and run the following command to build the project:

    docker run --rm -v $PWD:/project -w /project -u $UID -e HOME=/tmp espressif/idf idf.py build
