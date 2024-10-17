#define MOVE_UP 5
#define MOVE_DOWN 6

const bool byCurrentStaticLimit = false;    // count current by const (true) or dynamic (false)
const int CURRENT_LIMIT = 340;              // applied only if byCurrentStaticLimit = true

volatile bool isUp = false;
volatile bool isDown = false;
volatile bool autoUp = false;
volatile bool autoDown = false;
volatile bool interrupted = false;
bool upStarted = false;
bool downStarted = false;
volatile uint32_t timerUp;
volatile uint32_t timerDown;

const int WORKING_TIME_LIMIT = 6000;
const int AUTO_MODE_CLICK_DURANCE = 300;
int previousCurrentAmperes = 0;
int currentAmperes = 0;

uint32_t checkCurrentTimer = millis();
uint32_t checkTimer = millis();
uint32_t checkAmperes = millis();

int analogPin = A1; 

void setup() {
  pinMode(MOVE_UP, OUTPUT);
  pinMode(MOVE_DOWN, OUTPUT);
  pinMode(2, INPUT);
  pinMode(3, INPUT);
  attachInterrupt(0, irIsr0, CHANGE);
  attachInterrupt(1, irIsr1, CHANGE);
}

void loop()  {
  if (millis() - checkAmperes > 500) {
    checkAmperes = millis();
    previousCurrentAmperes = currentAmperes;
  }
  currentAmperes = analogRead(analogPin);
  if (byCurrentStaticLimit) {
    if (currentAmperes > CURRENT_LIMIT) {
      stopMoving();      
    }  
  } else {
    if (millis() - checkCurrentTimer > 100 && (autoDown || autoUp) && (previousCurrentAmperes - currentAmperes > 85)) {
      stopMoving();
    }
  }  
  if (millis() - checkTimer > 1000) {
    checkTimer = millis();
  }
  if (isUp && !interrupted) {
    if (autoUp) {
      if (millis() - timerUp > WORKING_TIME_LIMIT) {
        stopMoving();
      } else {
        if (!upStarted) {
          digitalWrite(MOVE_UP, HIGH);
          upStarted = true;
        }
      }
    } else if (digitalRead(2) == HIGH) {
      digitalWrite(MOVE_UP, HIGH);
    } else {
      stopMoving();
    }
  }
  if (isDown && !interrupted) {
    if (autoDown) {
      if (millis() - timerDown > WORKING_TIME_LIMIT) {
        stopMoving();
      } else {
        if (!downStarted) {
          digitalWrite(MOVE_DOWN, HIGH);
          downStarted = true;
        }
      }
    } else if (digitalRead(3) == HIGH) {
      digitalWrite(MOVE_DOWN, HIGH);
    } else {
      stopMoving();
    }
  }
}



volatile uint32_t debounce0;
void irIsr0() {
  checkCurrentTimer = millis();
  if (millis() - debounce0 >= 100 && digitalRead(2) == HIGH) {
    debounce0 = millis();
    if (autoDown) {
      stopMoving();
      interrupted = true;
    } else {
      isUp = true;
      timerUp = millis();
    }
  }
  if (millis() - debounce0 >= 100 && digitalRead(2) == LOW && interrupted) {
    interrupted = false;
    autoDown = false;
  } else
  if (millis() - debounce0 >= 100 && digitalRead(2) == LOW && isUp) {
    debounce0 = millis();
    if (isUp && millis() - timerUp <= AUTO_MODE_CLICK_DURANCE) {
        autoUp = true;
        upStarted = false;
    }
  }
}



volatile uint32_t debounce1;
void irIsr1() {
  checkCurrentTimer = millis();
  if (millis() - debounce1 >= 100 && digitalRead(3) == HIGH) {
    debounce1 = millis();
    if (autoUp) {
      stopMoving();
      interrupted = true;
    } else {
      isDown = true;
      timerDown = millis();
    }
  }
  if (millis() - debounce1 >= 100 && digitalRead(3) == LOW && interrupted) {
    interrupted = false;
    autoUp = false;
  } else
  if (millis() - debounce1 >= 100 && digitalRead(3) == LOW && isDown) {
    debounce1 = millis();
    if (isDown && millis() - timerDown <= AUTO_MODE_CLICK_DURANCE) {
        autoDown = true;
        downStarted = false;
    }
  }
}

void stopMoving() {
  isUp = false;
  isDown = false;
  autoUp = false;
  autoDown = false;
  upStarted = false;
  downStarted = false;
  digitalWrite(MOVE_UP, LOW);
  digitalWrite(MOVE_DOWN, LOW);
  currentAmperes = 0;
  previousCurrentAmperes = 0;  
}