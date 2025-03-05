import serial
from pynput.keyboard import Controller, Key

ser = serial.Serial('COM9', 9600)
keyboard = Controller()

while True:
    line = ser.readline().decode().strip()
    if line:
        boutons = line.split(',')

        if boutons[0] == "1":
            keyboard.press(Key.left)  # Flèche gauche
        else:
            keyboard.release(Key.left)

        if boutons[1] == "1":
            keyboard.press(Key.right)  # Flèche droite
        else:
            keyboard.release(Key.right)

        if boutons[2] == "1":
            keyboard.press(Key.up)  # Flèche haut (Accélérer)
        else:
            keyboard.release(Key.up)

        if boutons[3] == "1":
            keyboard.press(Key.down)  # Flèche bas (Frein)
        else:
            keyboard.release(Key.down)




