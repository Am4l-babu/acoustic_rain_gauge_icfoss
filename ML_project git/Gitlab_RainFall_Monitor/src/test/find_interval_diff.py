import csv
import os
from datetime import datetime

def extract_timestamp_from_filename(filename: str) -> datetime:
    """
    extract time stamp from audio file name  
    """
    timestamp_str = filename.split(".")[0]
    return datetime.strptime(timestamp_str, "%Y_%m_%d_%H_%M_%S_%f")

def mech_interval_diff(input_file: str) -> None:
    """
    find maximum value of data logging interval in mech data
    """
    with open(input_file, mode="r") as file:
        reader = csv.reader(file)
        next(reader) # skip header
        dt = datetime.now()
        max_diff = dt - dt # creating datetime.timedelta 
        i = 0
        for row in reader:
            i = i+1
            timestamp = datetime.strptime(row[0], "%Y-%m-%d %H:%M:%S.%f")
            if i==1:
                time_stamp_ref = timestamp
                continue
            diff = timestamp - time_stamp_ref
            time_stamp_ref = timestamp
            if diff > max_diff:
                max_diff = diff 
        print("max diff in mech data:",max_diff)

def audio_interval_diff(dir: str) -> None:
    """
    find maximum value of data logging interval in audio data
    """
    wav_files = sorted(os.listdir(dir))
    i = 0
    dt = datetime.now()
    max_diff = dt - dt # creating datetime.timedelta
    for wav in wav_files:
        i = i+1
        timestamp = extract_timestamp_from_filename(wav)
        if i==1:
            time_stamp_ref = timestamp
            continue
        time_diff = timestamp - time_stamp_ref
        if time_diff > max_diff:
            max_diff = time_diff
        time_stamp_ref = timestamp
    print("max diff in audio data:",max_diff)  

def main():
    mech_dir = "/home/icfoss/hari_work/acoustic_raingauge/data set/audios/test/02-11-24/raw/davis_label.csv"
    # wav_dir = "/home/icfoss/hari_work/acoustic_raingauge/data set/audios/test/02-11-24/raw/wav"       
    mech_interval_diff(mech_dir)
    # audio_interval_diff(wav_dir)

if __name__ == "__main__":
    main()