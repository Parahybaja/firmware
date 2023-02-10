/**
 * @file SD_module.h
 * @author Jefferson Lopes (jefferson.lopes@ufcg.edu.br)
 * @brief SD card manipulation functions
 * @version 0.1
 * @date 2023-02-10
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#ifndef __SD_MODULE_H__
#define __SD_MODULE_H__

#include <FS.h>
#include <SPI.h>
#include <SD.h>

#include "definitions.h"

// -----VSPI pinout-----
#define VSPI_CLK  18
#define VSPI_MISO 19
#define VSPI_MOSI 23
#define VSPI_SS   05
#define VSPI_RST  17

// -----HSPI pinout-----
#define HSPI_CLK  27
#define HSPI_MISO 14
#define HSPI_MOSI 13
#define HSPI_SS   04

// -----microSD configs-----
#define SD_BUFFER_SIZE 64           // block of data size to be written on SD card
#define FILE_PATH        "/PB22_nac/file" // should be the test or subsystem name
#define TITLE       "time,speed,rpm,fuel_emer,battery,temp,rollover,tilt_x,tilt_y,tilt_z\r\n" // file title
#define EXTENSION   ".csv"       // file extension (.csv, .txt, ...)

// -----SPI pointers-----
SPIClass * vspi = NULL;
SPIClass * hspi = NULL;

// -----SD card variables-----
String file_title = FILE_PATH; // init default value
String file_name = "max_length";    // current file name generated by the system (don't change)

/**
 * @brief run at the setup start to use multiple SPI
 * 
 */
void pre_config_SPI(void);

/**
 * @brief init file system
 * 
 */
void init_file(void);

/**
 * @brief delete existing data and write the message to the top:
 * 
 * @param fs fs::FS: file object
 * @param path const char*: complete file path starting with / (/folder/file.csv)
 * @param message const char*: message to be written on the file
 */
void writeFile(fs::FS&, const char*, const char*);

/**
 * @brief keep existing data and write the message to the botton:
 * 
 * @param fs fs::FS: file object
 * @param path const char*: complete file path starting with / (/folder/file.csv)
 * @param message const char*: message to be written on the file
 */
void appendFile(fs::FS&, const char*, const char*);

/**
 * @brief Create a File object
 * 
 */
void createFile(void);

/**
 * @brief convert data struct to string
 * 
 * @param database struct *system_t: array of struct data to be converted
 * @return String: returns the converted string
 */
String data_to_string(system_t*);


void pre_config_SPI(void){
    // -----both SPI inittialization-----
    vspi = new SPIClass(VSPI);
    hspi = new SPIClass(HSPI);

    vspi->begin(VSPI_CLK, VSPI_MISO, VSPI_MOSI, VSPI_SS);
    hspi->begin(HSPI_CLK, HSPI_MISO, HSPI_MOSI, HSPI_SS);

    pinMode(vspi->pinSS(), OUTPUT);
    pinMode(hspi->pinSS(), OUTPUT);
}

void init_file(void){
    // -----File initiallization-----
    while (!SD.begin(hspi->pinSS(), *hspi)){
        INFO("ERROR: File initialization failed!");
        delay(5000);
    }
    INFO("INFO: file initialization done.");
    delay(50); // give time to send the espnow message

    createFile();
    INFO("INFO: file created");
    delay(50); // give time to send the espnow message
}

void writeFile(fs::FS &fs, const char *path, const char *message){
    xSemaphoreTake(sh_SD, portMAX_DELAY);
        File file = fs.open(path, FILE_WRITE);
        
        if(!file)
            //-----if the file didn't open, print an error-----
            ERROR("ERROR: writeFile: error opening file");
        
        if(!file.print(message))
            //-----if the file didn't write, print an error-----
            ERROR("ERROR: writeFile: error writing file"); 
        
        file.close();
    xSemaphoreGive(sh_SD);
}

void appendFile(fs::FS &fs, const char *path, const char *message){
    xSemaphoreTake(sh_SD, portMAX_DELAY);
        File file = fs.open(path, FILE_APPEND);
        
        if(!file)
            ERROR("ERROR: appendFile: error opening file");
        
        if(!file.print(message))
            ERROR("ERROR: appendFile: error appending file");  
        
        file.close();
    xSemaphoreGive(sh_SD);
}

void createFile(void){
    xSemaphoreTake(sh_SD, portMAX_DELAY);
        // -----find the next file name available-----
        uint8_t n = 0;
        while (SD.exists(file_title + String(n) + EXTENSION)) n++;
        
        file_name = file_title + String(n) + EXTENSION;
    xSemaphoreGive(sh_SD);
        
    // -----create file and write title-----
    writeFile(SD, file_name.c_str(), TITLE);
}

String data_to_string(system_t database[]){
    String return_msg;
    
    for(int i = 0; i < SD_BUFFER_SIZE; i++){
        return_msg += 
            String(millis()) + "," +
            String(database[i].speed, 2) + "," +
            String(database[i].rpm, 2) + "," +
            String(database[i].fuel_emer, 2) + "," +
            String(database[i].battery, 2) + "," +
            String(database[i].temp, 2) + "," +
            String(database[i].rollover, 2) + "," +
            String(database[i].tilt_x, 2) + "," +
            String(database[i].tilt_y, 2) + "," +
            String(database[i].tilt_z, 2) + "\r\n"; // convert float to string with 2 decimal places
    }

    return return_msg;
}

#endif // __SD_MODULE_H__