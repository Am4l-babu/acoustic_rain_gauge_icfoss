import influxdb_client
from influxdb_client.client.write_api import SYNCHRONOUS
from requests.exceptions import ConnectionError


def send_data_via_internet(rain_percentage: float) -> bool:
    """
    function to write data to influxdb using internet
    """
    try:

        # loading influxDB credentials
       
        url = "http://117.223.185.200:8086"
        org = "icfoss"
        bucket = "DFRAINSENSOR"
        token = "kM33X6kLIEJEtZNOTKRPApSn5h1v2PwlbwO_QuDbewo4biPO8tKfcyiQfpBWFyUmlHtUYHypJfUNpDQzpMg1MA=="
        # creating an object of influxdb_client
        client = influxdb_client.InfluxDBClient(
            url=url, token=token, org=org, timeout=30_000
        )
        write_api = client.write_api(write_options=SYNCHRONOUS)
        p = (
            influxdb_client.Point("dfrain sensor")
            .field("rain percentage", rain_percentage)
            
        )

        write_api.write(bucket=bucket, org=org, record=p)
        client.close()
        return True
    except ConnectionError as e:
        print(f"Connection to InfluxDB failed: {e}")
        return False

