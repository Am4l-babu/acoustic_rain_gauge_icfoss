import librosa
import numpy as np
import pandas as pd
from os import path, listdir
from datetime import datetime
from keras.models import Sequential
from keras.layers import Conv2D, MaxPooling2D
from keras.layers import LSTM, Dense, Reshape, Input


# directories and filename
audio_dir = "/home/icfoss/hari_work/acoustic_raingauge/ML_inferencing/report/test4/audio_23-10-24_16:17_to_17:05"
infer_model_dir = "/home/icfoss/hari_work/acoustic_raingauge/ML_inferencing/model"
infer_model_name = "seq_stft_enc3_v8.hdf5"
log_dir = "/home/icfoss/hari_work/acoustic_raingauge/ML_inferencing/report/test4"
log_filename = "estimated_rainfall_enc3_v8.csv"
# audio
resolution = "S32"
sampling_rate = "8000"
# model
seq_len = 1368000
stft_shape = [None, 1, 1025, 2672]


def write_rain_data_to_csv(result_data):
    result_df = pd.DataFrame(result_data)
    result_df.to_csv(path.join(log_dir, log_filename), index=False)

def extract_timestamp_from_filename(filename: str) -> datetime:
    """
    extract time stamp from audio file name  
    """
    timestamp_str = filename.split(".")[0]
    return datetime.strptime(timestamp_str, "%Y_%m_%d_%H_%M_%S_%f")

def estimate_rainfall(audio_files: list[str]) -> float:
    """
    Computed the model prediction on input data provided
    """
    infer_model_path = path.join(infer_model_dir,infer_model_name)
    infer_model = load_estimate_model(infer_model_path) # loading model
    audio = combine_audios(audio_files)
    #print("combine_audios out:",audio)
    audio = audio[: seq_len] #Trims the combined audio signal to a specific sequence length (seq_len)
    stft_sample = create_cnn_data(audio)
    #print("create_cnn_data out:", stft_sample)
    y_pred = infer_model.predict(stft_sample, verbose=0)[0][0]
    return y_pred

def load_estimate_model(model_path: str) -> any:
    """
    Loads deep learning model, build it and loads weights
    """
    model = create_lstm_model_withcnn()   
    model.build(input_shape = stft_shape)
    model.load_weights(model_path)
    return model

def create_lstm_model_withcnn() -> any:
    """
    Creates and returns LSTM models with Conv and Dense blocks
    """
    model = Sequential()
    model.add(Input((1025, 2672, 1)))
    model.add(Conv2D(64, kernel_size=(8, 8), activation="relu"))
    model.add(MaxPooling2D(pool_size=(8, 8)))
    model.add(Conv2D(32, kernel_size=(4, 4), activation="relu"))
    model.add(MaxPooling2D(pool_size=(4, 4)))
    model.add(Conv2D(16, kernel_size=(2, 2), activation="relu"))
    model.add(MaxPooling2D(pool_size=(2, 2)))
    model.add(Reshape((1, -1)))
    model.add(LSTM(20))
    model.add(Dense(32))
    model.add(Dense(16))
    model.add(Dense(1))
    return model

def combine_audios(audio_files: list[str]) -> np.ndarray:
    """
    Combines various audio files in to a single file with desired sampling rate
    while keeping their chronological order. based on list of audio file paths provided
    """
    audio = np.array([])
    for file_path in audio_files:
        audio_segment, _ = librosa.load(file_path, sr=int(sampling_rate))
        audio = np.append(audio, audio_segment)    
    return audio

def create_cnn_data(raw_data: np.ndarray) -> np.ndarray:
    """
    Calculates and returns STFT on raw input audio provided
    """
    Zxx = librosa.stft(raw_data)
    stft_sample = np.abs(Zxx)
    return stft_sample[np.newaxis, :, :]

def main():
    result_data = []
    temp_audios = []

    try:
        wav_files = sorted(listdir(audio_dir)) # wav_files datatype list[str]
        for wav in wav_files:
            audio = path.join(audio_dir, wav)
            temp_audios.append(audio)
            if len(temp_audios)==18:
                dt = extract_timestamp_from_filename(wav)
                rain = estimate_rainfall(temp_audios) # estimating rainfall
                result_data.append({"time_stamp": dt,"estimated_rainfall": rain})
                write_rain_data_to_csv(result_data)
                temp_audios.clear()
                print(dt,"rainfall_estimated:", rain)

    except KeyboardInterrupt:
        print("Execution interrupted by user")
    finally:
        pass


if __name__ == "__main__":
    main()


# NOTE:
# which audio data are we using for prediction,the audio data from kaggle or not
    # if we are using data in kaggle,no.of audio files corresponding to zero rain is only 10% or less than that
    # zero rain condition is where we are getting most false data,so how we can compare the accuracy
    # kaggle data is made of audio filtering based on mech data,some mech data data point contain 17 audio files only
    # this may affect the if condition in code
    
# which data are we using for comparison with the inference data
    # if we are comparing inference data with mechanical raingauge data 
        # there should be some time stamping in inference data, how to resolve the time stamp issue
        # if the audios are in correct oreder of time (that is no missing audio files in between)
            # we should take timestamp from the name of last audio file in the temporary audio list + 10sec as time stamp
        # what if audio files are not consecutive and mixed timestamps
    # if we are comparing inferencing data of the 3 acoustic raingauges, we dont need time stamp
     
# code:
# time stamp issue
# doubt in function combined audios
    # is it actually sorting the input audio file
    # is it actually combining the audio files and make a single file or creating or an array of audios