import serial
import time

class RainSensor:
    def __init__(self, port='/dev/serial0', baudrate=115200, timeout=1):
        """
        Initialize the rain sensor.
        :param port: Serial port to use (default is '/dev/serial0').
        :param baudrate: Communication baud rate (default is 115200).
        :param timeout: Serial read timeout in seconds (default is 1 second).
        """
        self.serial = serial.Serial(port, baudrate, timeout=timeout)

    def synchronize_frame(self):
        """
        Synchronize to the start of a valid frame.
        Returns the synchronized frame if successful, otherwise None.
        """
        attempts = 50  # Max attempts to synchronize
        while attempts > 0:
            byte = self.serial.read(1)  # Read one byte
            if byte == b'\x3A':  # Found the frame header
                frame = byte + self.serial.read(4)  # Read the rest of the frame
                return frame
            attempts -= 1
        return None  # Failed to synchronize

    def check_rain(self):
        """
        Query the rain status.
        :return: True if raining, False otherwise.
        """
        command = bytes([0x3A, 0x01, 0x00, 0x00, 0x0D])  # Rain status query
        self.serial.write(command)  # Send the command
        time.sleep(0.01)  # Allow response time

        frame = self.synchronize_frame()  # Synchronize to a valid frame
        if frame and len(frame) == 5:
            if frame[2] in [0x01, 0x02, 0x03]:  # Rain detected
                return True
        return False  # No rain detected
