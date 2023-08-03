# Validation_AV

## Team intro 

The Team Parahybaja is formed by students of Engineering, Art and Media, Social Communication, Physics, Maths and Design, in order to develop an offroad vehicle.

The Baja project is an opportunity to put into practice what you learn in the classroom. Our team's ultimate goal is to prepare a professional for the job market, learning to deal with pressure, deadlines and budgets and to work in a group, in addition to developing leadership and programming skills.

## Instruction

For the AV, if you want to mesure the aceleration, the sensors 1 and 2 must be placed, respectively, at 0m and at 100m. Otherwise, if you want to measure final speed, the sensors 3 and 4 must be places at 100m and 130m, respectively. It is possible to use both system at the same time.

The receiver have to be connect to a computer with the Arduino IDE. Using the serial monitor to turn on the senders and receive the information from the sender via esp-now. 

Calibrate the sensors manually.

### How to use the receiver 

When you first turn on all the devices, the senders will start as turned off and will need a command from the receiver to start up. Once you have all plugged in, you can calibrate the minimal distance or turn on the devices with the standard distance (actually is 40 centimeters) by typing in the receiver terminal window "on" with no arguments.

If you want to change the default calibration, use the commando "set xx", where the xx must be the new value for the distance. When you change this value, all the device's data will be changed, such as the time sensor as the velocity. This command is only necessary when the sensors are calibrated by code.

Now that you have the sensors turned on and calibrated, use the command "new" to create a set of passages. The default number is five and for now, the only way to change that is by code.

In case of an invalid time, use the command "dtl", extends for delete last one, and make another passage.

Bellow, we have all the commands that you can use:

| command |        summary        |
|:-------:|:---------------------:|
|   new   |      new passage      |
|   dtl   |    delete last one    |
|    on   |   turn on the boards  |
|   off   |  turn off the boards  |
|  set xx | set calibration to xx |

## Component List

### Aceleration Test

| quantity |        component                          |
|:--------:|:-----------------------------------------:|
|    2x    |      ESP8266 ESP-01                       |
|    2x    |   Infrared Reflectance Sensor E18-D80NK   |
|    2x    |  Prototype paper PCB universal board      |
|    2x    |          Switch on/off                    |
|    2x    |             Jacks                         |
|    X     |        Conducting wire                    |
|    2x    |          10 uF Capacitor                  |

### Final Speed Test

| quantity |        component                          |
|:--------:|:-----------------------------------------:|
|    1x    |    ESP32 esp-wroom-32                     |
|    2x    |   Infrared Reflectance Sensor E18-D80NK   |
|    1x    |  Prototype paper PCB universal board      |
|    1x    |     Switch on/off                         |
|    1x    |             Jacks                         |
|     X    |         Conducting wire                   |

### Both

| quantity |        component                          |
|:--------:|:-----------------------------------------:|
|    1x    |      ESP8266 ESP-01                       |

## Circuit

## Code
