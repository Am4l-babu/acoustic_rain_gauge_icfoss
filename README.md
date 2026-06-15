# 🌧️ ICFOSS Acoustic Rain Gauge 

[![Python Version](https://img.shields.io/badge/python-3.14-blue.svg)](https://python.org)
[![ML Framework](https://img.shields.io/badge/ML-Scikit--Learn%20%2F%20TensorFlow-orange.svg)](https://scikit-learn.org/)
[![UI Framework](https://img.shields.io/badge/UI-Streamlit-red.svg)](https://streamlit.io/)
[![License](https://img.shields.io/badge/license-MIT-green.svg)](LICENSE)
[![Organization](https://img.shields.io/badge/ICFOSS-R%26D-blue)](https://icfoss.in/)

An innovative research and development project by **ICFOSS (International Centre for Free and Open Source Software, Kerala)** to estimate rainfall rate and precipitation dynamics in real-time by analyzing the **sound of raindrops** falling on a metallic enclosure using Machine Learning.

---

## 📸 System Concept & UI

<div align="center">
  <img src="https://media.giphy.com/media/v1.Y2lkPTc5MGI3NjExdm15b3gwZ2o3dnoxZWNxd3Bod3l0MXNsdHprdm41M2N0ajdpZ2k0NSZlcD12MV9pbnRlcm5hbF9naWZfYnlfaWQmY3Q9Zw/wxpaNs949jOarTPxFZ/giphy.gif" width="700" alt="Rainfall Sound Estimation Animation" />
</div>

> [!NOTE]  
> Traditional rain gauges are mechanical (tipping buckets) or optical, which require high maintenance and are prone to debris clogging. The acoustic rain gauge presents a low-cost, solid-state, maintenance-free alternative.

---

## 🛠️ Hardware & Data Acquisition (DAQ) Stack

The system consists of a recording setup calibrated against a physical mechanical reference:

1.  **Acoustic Sensor**: USB Microphone (Jieli Technology UACDemoV1.0) placed inside a specialized metallic enclosure.
2.  **Data Logger**: Raspberry Pi running automated audio acquisition scripts.
3.  **Reference Sensor**: **Davis AeroCone 6466M Rain Gauge** (mechanical tipping bucket) used for continuous calibration and ground-truth labels.

---

## 📂 Project Structure

```directory
├── software/
│   ├── rainfall_monitor_nonmechanical/
│   │   ├── model_pipeline.py       # <-- End-to-end alignment & ML pipeline
│   │   ├── demo_app.py             # Streamlit visualization dashboard
│   │   ├── raindrop_counter.py     # Real-time rain drop counting script
│   │   └── requirements.txt        # Python dependencies
│   ├── Firmware/                   # LoRaWAN and Arduino firmware
│   └── hardware/                   # Circuit diagrams & CAD drawings
├── DATA/                           # Local calibration data
├── experiments/                    # Model validation records
└── README.md                       # Root documentation
```

---

## 🧠 Machine Learning Pipeline

The pipeline aligns audio files with the mechanical sensor log, extracts acoustic features, and trains regressors to estimate rainfall.

### 1. Data Alignment
Since the mechanical Davis rain gauge reports precipitation accumulated over 3-minute windows, the pipeline parses the timestamps of 3.12-second audio clips (`yyyy_mm_dd_hh_mm_ss_micro.wav`) and groups/concatenates the 18 audio clips that fall into each 3-minute reporting window.

### 2. Acoustic Feature Extraction
For each aligned 3-minute window, we extract a **264-dimensional feature vector**:
*   **Mel-Frequency Spectrogram (64 bins)**: We compute the log Mel spectrogram and extract the **Mean** and **Standard Deviation** for each bin to represent pitch profile and loudness.
*   **Zero Crossing Rate (ZCR)**: Captures the rate of waveform sign changes to identify high-frequency drop impacts.
*   **RMS Energy**: Measures average sound amplitude (physical loudness).
*   **Spectral Centroid & Rolloff**: Identifies the center of spectral mass and skewness to differentiate rain from wind or traffic noise.

### 3. Model Performance Comparison
Models are split chronologically (75% train, 25% test) to mimic real-world deployment. The results:

| Model | Test MAE (mm) | Test RMSE (mm) | Test Correlation (Pearson $r$) |
| :--- | :---: | :---: | :---: |
| **Baseline (Mean)** | 0.7713 | 1.4733 | 0.00% |
| **Random Forest** | 0.9184 | 1.8899 | **50.23%** |
| **Gradient Boosting** | 0.8822 | 1.7740 | **49.92%** |
| **Neural Network (MLP)** | 0.9654 | 1.6861 | 36.00% |

> [!TIP]
> **Random Forest** and **Gradient Boosting** are the best performing models, achieving a **~50% correlation** with actual physical tipping events, demonstrating the viability of acoustic rainfall monitoring.

---

## 🚀 Getting Started

### 📋 Prerequisites
*   Python 3.11+ (tested on Python 3.14)
*   Virtual environment manager

### 🔧 Setup & Virtualenv
1. Clone the repository and navigate to the software folder:
   ```bash
   git clone https://github.com/Am4l-babu/acoustic_rain_gauge_icfoss.git
   cd acoustic_rain_gauge_icfoss
   ```
2. Initialize virtual environment and install dependencies:
   ```bash
   python -m venv .venv
   .venv\Scripts\activate      # On Windows
   source .venv/bin/activate   # On Unix/macOS
   pip install -r software/rainfall_monitor_nonmechanical/requirements.txt
   pip install tabulate
   ```

### ⚡ Running the Pipeline
Run the model pipeline script to align datasets, cache features, and train the regressors:
```bash
python software/rainfall_monitor_nonmechanical/model_pipeline.py
```

### 🖥️ Streamlit Dashboard
Launch the visualization dashboard to view predictions and live estimation:
```bash
streamlit run software/rainfall_monitor_nonmechanical/demo_app.py
```

---

## 👥 Project Contributors
*   **Gopika T G**
*   **Sajil C K** ([@cksajil](https://github.com/cksajil/))
*   **Manu Mohan M S**
*   **Aiswarya Babu**
*   **Harikrishnan K P**
