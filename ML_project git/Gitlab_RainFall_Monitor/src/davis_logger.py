import csv
from os import path
import RPi.GPIO as GPIO
from datetime import datetime
from utils.helper import load_config, create_folder, time_stamp_fnamer
import influxdb_client
from influxdb_client.client.write_api import SYNCHRONOUS
from requests.exceptions import ConnectionError

dt_start = datetime.now()
config = load_config("config.yaml")
session_dir = path.join(config["log_dir"], time_stamp_fnamer(dt_start))
create_folder(session_dir)
interrupt_pin = config["davis_interrupt_pin"]
logging_interval = config["davis_log_interval_sec"]
count = 0


def reset_count():
    global count
    count = 0


def bucket_tipped(interrupt_pin):
    print("Bucket Tipped")
    global count
    count += 1
    if count > 50:
        reset_count()


def influxdb(rain: float):
    """
    function to write data to influxdb
    """
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


def save_csv(data: list[str]) -> None:
    """
    Append data to a CSV file. If the file does not exist, create it with a header.
    """
    csv_path = path.join(session_dir, config["davis_log_filename"])
    
    # Check if the file already exists
    file_exists = path.isfile(csv_path)
    
    with open(csv_path, mode="a", newline="") as newcsv:
        writer = csv.writer(newcsv)
        if not file_exists:
            writer.writerow(["time", "rainfall"])
        writer.writerow(data)


def calculate_rainfall(time_stamp):
    bucket_size = 0.2
    rainfall = count * bucket_size
    data = [time_stamp, rainfall]
    influxdb(rainfall)
    save_csv(data)


GPIO.setmode(GPIO.BCM)
GPIO.setup(interrupt_pin, GPIO.IN, pull_up_down=GPIO.PUD_OFF)
GPIO.add_event_detect(interrupt_pin, GPIO.RISING, callback=bucket_tipped, bouncetime=50)

try:
    log_count = 1
    while True:
        dt_now = datetime.now()
        elapsed_time = dt_now - dt_start
        if elapsed_time.seconds % logging_interval == 0:
            if log_count == 0:
                calculate_rainfall(dt_now)
                reset_count()
                log_count = 1
        else:
            log_count = 0
except KeyboardInterrupt:
    GPIO.cleanup()
