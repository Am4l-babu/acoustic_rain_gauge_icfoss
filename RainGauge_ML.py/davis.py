import pandas as pd
from os import path
import RPi.GPIO as GPIO
from datetime import datetime
from utils.helper import load_config, create_folder, time_stamp_fnamer
import influxdb_client
from influxdb_client.client.write_api import SYNCHRONOUS
from requests.exceptions import ConnectionError
import time

# Initialize variables and configurations
dt_start = datetime.now()
config = load_config("config.yaml")
labels_df = pd.DataFrame(columns=["time", "rainfall"])
session_dir = time_stamp_fnamer(dt_start)
label_dir = path.join(config["log_dir"], session_dir)

create_folder(label_dir)
BUCKET_SIZE = 0.2
count = 0
log_count = 0
interrupt_pin = config["davis_interrupt_pin"]
logging_interval = config["davis_log_interval_sec"]
DEBOUNCE_TIME = 0.05  # 50ms debounce

# Reset count function
def reset_rainfall():
    global count
    count = 0

# Function to handle bucket tipping
def handle_bucket_tipping():
    global count
    print("Bucket Tipped")
    count += 1
    if count > 50:
        reset_rainfall()

# Function to save data to InfluxDB
def influxdb(rain: float):
    try:
        name = "rainpi_mech"
        location = "greenfield tvm"
        influxdb_config = load_config("influxdb_api.yaml")
        org = influxdb_config["org"]
        url = influxdb_config["url"]
        bucket = influxdb_config[name]["bucket"]
        token = influxdb_config[name]["token"]

        client = influxdb_client.InfluxDBClient(
            url=url, token=token, org=org, timeout=30_000
        )
        write_api = client.write_api(write_options=SYNCHRONOUS)
        p = (
            influxdb_client.Point("pi_davis_raingauge")
            .tag("location", location)
            .field("rain", rain)
        )
        write_api.write(bucket=bucket, org=org, record=p)
        client.close()
        return True
    except ConnectionError as e:
        print(f"Connection to InfluxDB failed: {e}")
        return False

# Function to save data locally
def saving_data(label_dir, dt_now):
    rainfall = count * BUCKET_SIZE
    influxdb(rainfall)
    labels_df.loc[len(labels_df)] = (dt_now, rainfall)
    labels_df.to_csv(path.join(label_dir, config["davis_log_filename"]), index=False)

# Setup GPIO
GPIO.setmode(GPIO.BCM)
GPIO.setup(interrupt_pin, GPIO.IN, pull_up_down=GPIO.PUD_UP)

# Initialize debounce tracking
last_state = GPIO.LOW
last_tip_time = 0

# Main loop
try:
    while True:
        # Read the current state of the GPIO pin
        current_state = GPIO.input(interrupt_pin)
        current_time = time.time()

        # Detect a valid rising edge with debouncing
        if current_state == GPIO.HIGH and last_state == GPIO.LOW and (current_time - last_tip_time) > DEBOUNCE_TIME:
            handle_bucket_tipping()
            last_tip_time = current_time

        last_state = current_state  # Update the last state

        # Handle periodic data logging
        dt_now = datetime.now()
        elapsed_time = dt_now - dt_start
        if elapsed_time.seconds % logging_interval == 0:
            if log_count == 0:
                saving_data(label_dir, dt_now)
                reset_rainfall()
                log_count = 1
        else:
            log_count = 0
except KeyboardInterrupt:
    GPIO.cleanup()

