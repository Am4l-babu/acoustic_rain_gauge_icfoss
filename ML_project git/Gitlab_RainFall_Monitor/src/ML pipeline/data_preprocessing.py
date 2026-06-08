import csv
import random
import os
from os import listdir
import shutil
import math
from datetime import datetime, timedelta


def extract_timestamp_from_filename(filename: str) -> datetime:
    """
    extract time stamp from audio file name  
    """
    timestamp_str = filename.split(".")[0]
    return datetime.strptime(timestamp_str, "%Y_%m_%d_%H_%M_%S_%f")

def create_folder(dir: str, folder_name: str) -> str:
    """
    Function to create a folder in a location if it does not exist
    """
    folder = os.path.join(dir, folder_name)
    if not os.path.exists(folder):
        os.makedirs(folder)
    return folder 

def clean_directory(dir: str) -> None:
    """
    Delete all audio files to avoid issues from random sampling of zero data points, 
    as running the code multiple times on same data set and directory may leave unnecessary files.
    """
    count = 0
    for filename in os.listdir(dir):
        file_path = os.path.join(dir, filename)
        os.remove(file_path)
        count += 1
    #print(f"Deleted audio files from previous session: {count}")

def generate_csv(output_dir: str, filename: str, header: list[str], data: list[list[str]]) -> None:
    """
    create a csv file using input data
    """
    path = os.path.join(output_dir,filename)
    with open(path, mode="w", newline="") as newcsv:
        writer = csv.writer(newcsv)
        
        # Write the header
        writer.writerow(header)
        
        # Write data
        for row in data:
            writer.writerow(row)

def find_no_of_samples(wav_files: list[str], row: list[str]) -> bool:
    """
    check no.of samples in a specific time frame
    """
    audios = []
    time = datetime.strptime(row[0], "%Y-%m-%d %H:%M:%S.%f") # convert timestamp date type (str to datetime.datetime)
    for wav_file in wav_files:
        file_datetime = extract_timestamp_from_filename(wav_file)
        if time - timedelta(minutes=3) <= file_datetime <= time:
            audios.append(wav_file) # append wav file to temp list of audios
    if len(audios) >= 17:
        # print(f"{row} sufficient data points = {len(audios)}")
        return True
    else:
        print(f"{row} insufficient no.of audio samples,{len(audios)} only: discarding data point")
        return False
    
def collect_audio_samples(wav_files: list[str], csv_data: list[list[str]]) -> list[str]:
    """
    create a list of audio samples corresponding to the time check points in input data 
    """
    audios = []
    for row in csv_data:
        time = datetime.strptime(row[0], "%Y-%m-%d %H:%M:%S.%f") # convert timestamp date type (str to datetime.datetime)
        for wav in wav_files:
            file_datetime = extract_timestamp_from_filename(wav)
            if time - timedelta(minutes=3) <= file_datetime <= time:
                audios.append(wav)
    return audios

def copy_audios(audios : list[str], source_dir: str, destination_dir: str) -> None:
    """
    copy audio files to a specific directory
    """
    # create wav folder with in destination dir for audio
    destination_dir = create_folder(destination_dir,"wav")
    clean_directory(destination_dir) # clean wav directory before copying audios
    for audio in audios:
        source_path = os.path.join(source_dir, audio)
        try:
            shutil.copy2(source_path, destination_dir) # copy file,preserve metadata of file
        except FileNotFoundError:
            print(f"File '{audio}' not found in source directory.")
        except Exception as e:
            print(f"Error copying '{audio}': {e}")        

def false_data_filter(data: list[list[str]]) -> list[list[str]]:
    """
    remove false data from mech.raingauge between 5.55 am to 6.05 am
    """
    process_data = []
    for inner_list in data:
        time = inner_list[0]
        rain = float(inner_list[1])
        # Convert timestamp string to datetime object and taking time from it
        time_dt = datetime.strptime(time, "%Y-%m-%d %H:%M:%S.%f").time()
        low_time_limit = datetime.strptime("05:55:00.000000", "%H:%M:%S.%f").time()
        up_time_limit = datetime.strptime("06:05:00.000000", "%H:%M:%S.%f").time()
        if rain == 0.2 and low_time_limit <= time_dt <= up_time_limit:  
            print (f"{inner_list} found false value and discarding it")
        else:
            process_data.append(inner_list)
    return process_data
  
def process_data(input_file: str, wav_files: list[str]) -> tuple[list[str],list[list[str]]]:
    zero_rain_rows = [] 
    non_zero_rain_rows = []
    non_zero_row_count = 0
    zero_row_count = 0
    
    # Open the input file in read mode
    with open(input_file, mode="r") as infile:
        reader = csv.reader(infile)
        
        # Read the header and move to next row
        header = next(reader)
        
        # Filter rows based on rain value
        for row in reader:
            if find_no_of_samples(wav_files,row):
                rain = row[1] # assume rain data is in coloumn two
                if float(rain) == 0:
                    zero_row_count += 1
                    zero_rain_rows.append(row)
                else:
                    non_zero_row_count += 1
                    non_zero_rain_rows.append(row)
                                               
    # Calculate 10% of total non-zero rain data points
    zero_to_append_count = math.ceil(0.20 * non_zero_row_count) # round up to nearest possible integer

    # Select a random sample of zero rain data points to append
    if len(zero_rain_rows) >= zero_to_append_count:
        zero_rain_sample = random.sample(zero_rain_rows, zero_to_append_count)
    else:
        zero_rain_sample = zero_rain_rows

    # Combine zero and non-zero rows and sort them by time
    processed_data = zero_rain_sample + non_zero_rain_rows  # data structure: list of lists
    processed_data.sort(key=lambda row: row[0])  # Sort by time column (assumes time is in row[0])
    processed_data = false_data_filter(processed_data) # delete false interrupt data 
    print(f"input = total data points: {zero_row_count + non_zero_row_count}, zero data points: {zero_row_count} & non-zero data points: {non_zero_row_count}")
    print(f"Total no.of input audio files: {len(wav_files)}")
    print(f"Zero rain data points to append: {zero_to_append_count}")

    return header, processed_data
    
def main():
    # input directories
    input_mech_dir = "/home/icfoss/hari_work/acoustic raingauge/data set/test/04-11-24/raw/davis_label.csv"
    input_audios_dir = "/home/icfoss/hari_work/acoustic raingauge/data set/test/04-11-24/raw/wav"
    # output directories and file name
    output_dir = "/home/icfoss/hari_work/acoustic raingauge/data set/test/04-11-24"
    new_csv_filename = "filtered_rain.csv"
    
    data_dir = create_folder(output_dir,"processed data")
    wav_files = sorted(listdir(input_audios_dir)) # wav_files datatype list[str]
    csv_header, csv_data = process_data(input_mech_dir,wav_files)
    generate_csv(data_dir,new_csv_filename,csv_header,csv_data)
    audios = collect_audio_samples(wav_files,csv_data)
    copy_audios(audios,input_audios_dir, data_dir)
    print(f"output = mech. data points: {len(csv_data)} & audio files: {len(audios)}")


if __name__ == "__main__":
    main()






# notes: 
# change input and output directories as per needs
# random zero sampling could cause errors in audio file copying.
    # Running the code multiple times on the same inputs may leave behind unnecessary audio files from previous execution.
    # total no.of audio files in output may vary from 17*total no.of mech data points to 18*total no.of mech data points
# why we are getting only 17 or 16 samples in some cases,
    # this is happening because some audio files are partially inside a perticular mech data timeframe and some are partially outside.
    # also it is found that some delay(in seconds) in audio recording this may also affect the number of samples in a time frame