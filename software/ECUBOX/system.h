/**
 * @brief init system peripherals
 * 
 */
void init_system(void){
    Serial.begin(74880);

    // -----header-----
    Serial.println("|-----------------------|");
    Serial.println("|                       |");
    Serial.println("|        control        |");
    Serial.println("|                       |");
    Serial.println("|-----------------------|");

    Serial.println("\nCommands:");
    Serial.println("start   - start writing data");
    Serial.println("stop    - stop writing data");
    Serial.println("new     - create new file");
    Serial.println("restart - restart board\n");
}