# soil.py

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

# Open CSV file in append mode so that new data is added each time the script runs

with open("output.csv", mode="a", newline="") as file:

   writer = csv.writer(file)

   # Write the header only if the file is empty

   file.seek(0, 2)  # Move the cursor to the end of the file

   if file.tell() == 0:

       writer.writerow(["Date", "Time", "Value", "Voltage"])  # Write header

   # Loop to read the analog input continuously

   while True:

       # Get current date and time

       now = datetime.now()

       date = now.strftime("%Y-%m-%d")

       current_time = now.strftime("%H:%M:%S")

       # Read the sensor value and voltage

       value = channel.value

       voltage = channel.voltage

       # Print to terminal

       print(f"Date: {date}, Time: {current_time}, Value: {value}, Voltage: {voltage:.2f}V")

       # Write to CSV file

       writer.writerow([date, current_time, value, voltage])

       # Delay before the next reading

       time.sleep(0.2)
