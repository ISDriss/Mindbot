import serial
import time
from pynput.keyboard import Controller, Key
from pylsl import StreamInfo, StreamOutlet

# Création du flux LSL avec 4 canaux (1 par touche)
info = StreamInfo('ArduinoMarkers', 'Markers', 4, 0, 'int32', 'arduino123')
outlet = StreamOutlet(info)

# Connexion série avec l'Arduino
ser = serial.Serial('COM9', 9600)
keyboard = Controller()

while True:
    line = ser.readline().decode().strip()
    if line:
        # Convertir directement en tableau d'entiers
        boutons = list(map(int, line.split(',')))
        
        # Assigner les valeurs
        left, right, up, down = boutons  # Décomposition du tableau
        timestamp = time.time()  # Capture du temps UNIX
        
        # Simule la pression des touches
        keyboard.press(Key.left) if left else keyboard.release(Key.left)
        keyboard.press(Key.right) if right else keyboard.release(Key.right)
        keyboard.press(Key.up) if up else keyboard.release(Key.up)
        keyboard.press(Key.down) if down else keyboard.release(Key.down)

        # Envoi des 4 valeurs dans le flux LSL
        outlet.push_sample(boutons, timestamp)

# Might be usefull to rewrite this script into 2 functions: 
# with and without the LSL stream
