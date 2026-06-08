import yaml
from os import path,makedirs,remove


def load_config(config_name: str) -> dict:
    """
    A function to load and return config file in YAML format.
    assuming relative path of config
    """
    config_path = path.join(path.dirname(path.dirname(path.abspath(__file__))),"config")
    with open(path.join(config_path, config_name)) as file:
        config = yaml.safe_load(file)
    return config

def time_stamp_fnamer(tstamp) -> str:
    """
    A function to generate filenames from timestamps
    """
    cdate, ctime = str(tstamp).split(" ")
    current_date = "_".join(cdate.split("-"))
    chour, cmin, csec = ctime.split(":")
    csec, cmilli = csec.split(".")
    current_time = "_".join([chour, cmin, csec, cmilli])
    current_date_time_name = "_".join([current_date, current_time])
    return current_date_time_name

def create_folder(directory: str) -> None:
    """
    Function to create a folder in a location if it does not exist
    """
    if not path.exists(directory):
        makedirs(directory)

def delete_files(file_paths):
    """
    Function to delete wav files after inference
    """
    for file_path in file_paths:
        try:
            if path.exists(file_path):
                remove(file_path)
                print(f"Deleted: {file_path}")
            else:
                print(f"File does not exist: {file_path}")
        except Exception as e:
            print(f"Error deleting {file_path}: {e}")


# loading config files
config = load_config("config.yaml")