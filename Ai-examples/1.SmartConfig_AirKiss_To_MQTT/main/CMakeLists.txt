set(COMPONENT_SRCS "app_main.c"
                   "User_HttpRequset_Time.c"
                   "User_DataProcess.c"
                   "User_HttpRequest_Weather.c"
                   "User_Sensor.c"
                   "Dev_Dht11.c"
                   "Dev_Oled.c")
set(COMPONENT_ADD_INCLUDEDIRS ".")

register_component()
