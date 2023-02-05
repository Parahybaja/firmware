/**
 * @brief init file system
 * 
 */
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

/**
 * @brief delete existing data and write the message to the top:
 * 
 * @param fs fs::FS: file object
 * @param path const char*: complete file path starting with / (/folder/file.csv)
 * @param message const char*: message to be written on the file
 */
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

/**
 * @brief keep existing data and write the message to the botton:
 * 
 * @param fs fs::FS: file object
 * @param path const char*: complete file path starting with / (/folder/file.csv)
 * @param message const char*: message to be written on the file
 */
void appendFile(fs::FS &fs, const char * path, const char * message){
    xSemaphoreTake(sh_SD, portMAX_DELAY);
        File file = fs.open(path, FILE_APPEND);
        
        if(!file)
            ERROR("ERROR: appendFile: error opening file");
        
        if(!file.print(message))
            ERROR("ERROR: appendFile: error appending file");  
        
        file.close();
    xSemaphoreGive(sh_SD);
}

/**
 * @brief Create a File object
 * 
 */
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

/**
 * @brief convert data struct to string
 * 
 * @param database struct *system_t: array of struct data to be converted
 * @return String: returns the converted string
 */
String data_to_string(system_t database[]){
    String return_msg;
    
    for(int i = 0; i < BUFFER_SIZE; i++){
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