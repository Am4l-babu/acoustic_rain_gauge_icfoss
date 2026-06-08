import os
import board
import time
import busio
import adafruit_ads1x15.ads1115 as ADS
from adafruit_ads1x15.analog_in import AnalogIn
import csv
from datetime import datetime

# Initialize the I2C interface
i2c = busio.I2C(board.SCL, board.SDA)

# Create an ADS1115 object
ads = ADS.ADS1115(i2c)

# Define the analog input channel
channel = AnalogIn(ads, ADS.P0)

# Initialize list to store sensor values for 50 seconds
sensorvalue = [0] * 50

# Function for finding the maximum change in 50 seconds
def maxchng():
    maxvalue = sensorvalue[0]
    minvalue = sensorvalue[0]
    for i in range(50):
        if sensorvalue[i] > maxvalue:
            maxvalue = sensorvalue[i]
        if sensorvalue[i] < minvalue:
            minvalue = sensorvalue[i]
    return minvalue

def save_csv(date, time, avg_value, max_change, voltage):
    filename = "output.csv"
    
    # Check if the file exists and is non-empty
    file_exists = os.path.exists(filename) and os.path.getsize(filename) > 0
    
    # Open the file in append mode
    with open(filename, mode="a", newline="") as file:
        writer = csv.writer(file)
        
        # Write the header if the file is empty
        if not file_exists:
            writer.writerow(["Date", "Time", "Average Value", "Min Value", "Voltage"])
        
        # Append the data values
        writer.writerow([date, time, avg_value, max_change, voltage])
        print(f"Data appended $$ date: {date}, time: {time}, avg_value: {avg_value}, maX_change: {max_change}, voltage: {voltage}")
# Loop to read the analog input continuously
while True:
    avg = 0
    for i in range(50):
        value = channel.value
        avg += value
        sensorvalue[i] = value
        time.sleep(1.2)

    avg /= 50
    chng = maxchng()

    # Get current date and time
    now = datetime.now()
    date = now.strftime("%Y-%m-%d")
    current_time = now.strftime("%H:%M:%S")

    # Read the sensor voltage
    voltage = channel.voltage

    # Print to terminal
    # print(f"Date: {date}, Time: {current_time}, Average Value: {avg}, MaxChange: {chng}, Voltage: {voltage:.2f}V")

    # Write to CSV file
    save_csv(date, current_time, avg, chng, voltage)

