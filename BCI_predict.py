from pylsl import StreamInlet, resolve_byprop
import time
import torch
import numpy as np
import mne
import pandas as pd
import requests  # <-- Ajouté pour envoyer les commandes HTTP
from EEGNet import EEGNet

ESP32_IP = "http://10.1.224.145" # ESP32 local IP

# Paramètres
Fs = 256            
n_channels = 4      
Wn = 1               # Fenêtre de 1 seconde
n_samples = int(Wn * Fs)  

# Charger le modèle PyTorch
def load_model(model_path):
    model = torch.load(model_path, weights_only=False)  # Charger le modèle
    model.eval()  # Mode évaluation
    return model

# Fonction de prédiction
def predict(x_pred, model):
    x_pred = np.transpose(x_pred, (0, 2, 1))
    x_pred = x_pred[:, :, :, np.newaxis]
    x_pred_tensor = torch.tensor(x_pred, dtype=torch.float32)  # Conversion en tensor PyTorch
    with torch.no_grad():  # Désactiver le calcul des gradients pendant la prédiction
        y_pred = model(x_pred_tensor)
    return y_pred.numpy()  # Convertir le tensor PyTorch en numpy array

# Fonction pour envoyer une commande à l'ESP32
def send_command(command):
    url = f"{ESP32_IP}/{command}"
    try:
        response = requests.get(url, timeout=1)
        if response.status_code == 200:
            print(f"Commande envoyée : {command}")
        else:
            print(f"⚠️ Erreur ESP32 (Code {response.status_code})")
    except requests.exceptions.RequestException as e:
        print(f"🚨 Erreur de connexion à l'ESP32 : {e}")

# Fonction principale
def main():
    print("🔍 Recherche d'un flux EEG...")
    streams = resolve_byprop('type', 'EEG')
    inlet = StreamInlet(streams[0]) 
    samples = []

    # Collecter les premières données
    for _ in range(n_samples):
        sample, _ = inlet.pull_sample()
        samples.append(sample[:-1])

    last_command = "stop"  # Pour éviter d'envoyer la même commande plusieurs fois

    while True:
        time.sleep(0.05)  # Attente avant le prochain échantillon
        sample, _ = inlet.pull_sample()
        samples.pop(0)  
        samples.append(sample[:-1])  

        # Préparer les données pour la prédiction
        x_pred = np.array([samples])
        y_pred = predict(x_pred, test_model)

        # Prédiction et envoi de commande à l'ESP32
        if y_pred[0][0] > y_pred[0][1] and y_pred[0][0] > y_pred[0][2] and y_pred[0][0] > 0.85:
            command = "left"
        elif y_pred[0][1] > y_pred[0][0] and y_pred[0][1] > y_pred[0][2] and y_pred[0][1] > 0.95:
            command = "right"
        else:
            command = "stop"

        # Envoyer la commande uniquement si elle change
        if command != last_command:
            send_command(command)
            last_command = command

        print(f"Prédiction : {command.upper()} (Confiance : {max(y_pred[0]):.2f})")

# Exécution du programme
if __name__ == '__main__':
    test_model = load_model('model/model.pth')  # Charger le modèle
    main()
