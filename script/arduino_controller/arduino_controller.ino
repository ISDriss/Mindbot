const int left = 2;
const int right = 3;
const int forward = 4;
const int backward = 5;

void setup() {
    Serial.begin(9600);
    pinMode(left, INPUT_PULLUP);
    pinMode(right, INPUT_PULLUP);
    pinMode(forward, INPUT_PULLUP);
    pinMode(backward, INPUT_PULLUP);
}

void loop() {
    int b1 = digitalRead(left) == LOW ? 1 : 0;
    int b2 = digitalRead(right) == LOW ? 1 : 0;
    int b3 = digitalRead(forward) == LOW ? 1 : 0;
    int b4 = digitalRead(backward) == LOW ? 1 : 0;
    
    Serial.print(b1);
    Serial.print(",");
    Serial.print(b2);
    Serial.print(",");
    Serial.print(b3);
    Serial.print(",");
    Serial.println(b4);
    
    delay(100);
}

