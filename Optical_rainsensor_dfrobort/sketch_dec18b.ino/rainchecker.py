import os
import time
import csv
from datetime import datetime
from RainSensor import RainSensor
from connectivity import send_data_via_internet
# Initialize the RainSensor
sensor = RainSensor(port="/dev/ttyAMA2", baudrate=115200)

# List to store rain detection status for 50 seconds
rain_status = [False] * 50

def save_csv(date_time, rain_count, total_checks):
    """
    Save the data to a CSV file.
    :param date_time: The current date and time.
    :param rain_count: Number of rain detections in 50 seconds.
    :param total_checks: Total number of checks performed.
    """
    filename = "rain_output.csv"
    
    # Check if the file exists and is non-empty
    file_exists = os.path.exists(filename) and os.path.getsize(filename) > 0
    
    # Open the file in append mode
    with open(filename, mode="a", newline="") as file:
        writer = csv.writer(file)
        
        # Write the header if the file is empty
        if not file_exists:
            writer.writerow(["Date & Time", "Rain Count", "Total Checks", "Rain Percentage"])
        
        # Calculate rain percentage
        rain_percentage = (rain_count / total_checks) * 100
        
        # Append the data values
        writer.writerow([date_time, rain_count, total_checks, f"{rain_percentage:.2f}%"])
        print(f"Data appended - Date and Time: {date_time}, Rain Count: {rain_count}, Total Checks: {total_checks}, Rain Percentage: {rain_percentage:.2f}%")
        send_data_via_internet(rain_percentage)
# Main loop
while True:
    rain_count = 0
    total_checks = 50

    for i in range(total_checks):
        try:
            # Check rain status
            if sensor.check_rain():
                rain_status[i] = True
                rain_count += 1
            else:
                rain_status[i] = False
            
            time.sleep(1.2)  # Delay between checks
        except Exception as e:
            print(f"Error checking rain status: {e}")
            rain_status[i] = False

    # Log data
    date_time = datetime.now()
    save_csv(date_time, rain_count, total_checks)
