# TODO

* resolve multiple instance issue while using .basrc for running the python script on reboot
    * using the .bashrc file for running the python script at reboot will cause issue of multiple instance
    * .bashrc is a shell script that is executed whenever a new interactive shell is started.
    * It contains user-specific aliases, environment variables, and other shell configurations.
* Create an error log to find the breaking point of python script
* explore the possibility of running the script as service to overcome the above mentioned issues
* explore the possibility of using cron-tab for running the script on reboot 
    ```bash
    # notes
    sudo crontab -e
    file : /temp/crontab.wggrzz/crontab
    append @reboot python3 <script path>
    ```  
* add detailed report of library installation at the end of setup.h execution
* find solution for config path issue
* add log for battery monitoring in training mode
* explore possibility of automatic machine learning and model creation using kaggle api 
* data preprocessing
    * check how to resolve time synchronization issues of input data
    * find different use cases for audio and mech data time frame changes
    * find solution for processing audio data lies in two different mech.time frame
    * create python code to find details of change in logging interval in acoustic and pi.mech csv file
        * find max,min and avg value of logging intervals in both cases 
## errors
source ~/.bashrc : command is not working as expected