import librosa
import numpy as np
from .helper import config
from keras.models import Sequential
from keras.layers import Conv2D, MaxPooling2D
from keras.layers import LSTM, Dense, Reshape, Input


def load_estimate_model(model_path: str) -> any:
    """
    Loads deep learning model, build it and loads weights
    """
    model = create_lstm_model_withcnn()
    model.build(input_shape=config["stft_shape"])
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

def estimate_rainfall(model: any, file_paths: list) -> float:
    """
    Computed the models prediction on input data provided
    """
    audio = combine_audios(file_paths)
    audio = audio[: config["seq_len"]]
    stft_sample = create_cnn_data(audio)
    y_pred = model.predict(stft_sample, verbose=0)[0][0]
    return y_pred

def combine_audios(file_paths: list) -> np.ndarray:
    """
    Combines various audio files in their chronological order
    based on list of audio file paths provided
    """
    audio = np.array([])
    for file_path in file_paths:
        audio_segment, _ = librosa.load(file_path, sr=int(config["sampling_rate"]))
        audio = np.append(audio, audio_segment)
    return audio

def create_cnn_data(raw_data: np.ndarray) -> np.ndarray:
    """
    Calculates and returns STFT on raw input audio provided
    """
    Zxx = librosa.stft(raw_data)
    stft_sample = np.abs(Zxx)
    return stft_sample[np.newaxis, :, :]


