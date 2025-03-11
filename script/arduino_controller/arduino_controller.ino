const int left = 2;
const int right = 3;
const int forward = 4;
const int backward = 5;

// Variables pour stocker l'état précédent des boutons
int last_b1 = 0, last_b2 = 0, last_b3 = 0, last_b4 = 0;

void setup() {
    Serial.begin(9600);
    pinMode(left, INPUT_PULLUP);
    pinMode(right, INPUT_PULLUP);
    pinMode(forward, INPUT_PULLUP);
    pinMode(backward, INPUT_PULLUP);
}

void loop() {
    // Lire l'état actuel des boutons
    int b1 = digitalRead(left) == LOW ? 1 : 0;
    int b2 = digitalRead(right) == LOW ? 1 : 0;
    int b3 = digitalRead(forward) == LOW ? 1 : 0;
    int b4 = digitalRead(backward) == LOW ? 1 : 0;

    // Vérifier si l'état a changé
    if (b1 != last_b1 || b2 != last_b2 || b3 != last_b3 || b4 != last_b4) {
        Serial.print(b1);
        Serial.print(",");
        Serial.print(b2);
        Serial.print(",");
        Serial.print(b3);
        Serial.print(",");
        Serial.println(b4);

        // Mettre à jour les derniers états connus
        last_b1 = b1;
        last_b2 = b2;
        last_b3 = b3;
        last_b4 = b4;
    }
    
    delay(1); // Petit délai pour éviter une surcharge inutile
}