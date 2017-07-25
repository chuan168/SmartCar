#include <MillisTimer.h>

#include <SPI.h>
#include "nRF24L01.h"
#include "RF24.h"
#include <printf.h>

RF24 radio(49, 53);

int const SPEED = 255;
int const TURN_SPEED = 180;

const uint64_t localAddr = 2;
const uint64_t destAddr = 1;

const int min_payload_size = 4;
const int max_payload_size = 32;

char receive_payload[max_payload_size + 1]; // +1 to allow room for a terminating NULL char

int const PIN_FORWARD_1 = 5;
int const PIN_BACK_1 = 6;
int const PIN_PWM_1 = 7;

int const PIN_FORWARD_2 = 8;
int const PIN_BACK_2 = 9;
int const PIN_PWM_2 = 10;

int const PIN_RIGHT_YELLOW = 28;
int const PIN_LEFT_YELLOW = 29;

int const PIN_BACK_RED = 30;
int const PIN_FORWARD_WHITE = 31;

MillisTimer timer1 = MillisTimer(100);
MillisTimer timer2 = MillisTimer(100);


void turn_on_light(int pin) {
  digitalWrite(pin, HIGH);
}

void turn_off_light(int pin) {
  digitalWrite(pin, LOW);
}

void turn_left() {
  Serial.println("turn_left()");
  timer1.run();

  digitalWrite(PIN_FORWARD_1, LOW);
  digitalWrite(PIN_BACK_1, HIGH);
  digitalWrite(PIN_FORWARD_2, LOW);
  digitalWrite(PIN_BACK_2, HIGH);
  analogWrite(PIN_PWM_1, TURN_SPEED);
  analogWrite(PIN_PWM_2, TURN_SPEED);
}

void turn_right() {
  Serial.println("turn_right()");
  timer2.run();

  digitalWrite(PIN_FORWARD_1, HIGH);
  digitalWrite(PIN_BACK_1, LOW);
  digitalWrite(PIN_FORWARD_2, HIGH);
  digitalWrite(PIN_BACK_2, LOW);
  analogWrite(PIN_PWM_1, TURN_SPEED);
  analogWrite(PIN_PWM_2, TURN_SPEED);
}

void move_forward() {
  Serial.println("forward()");
  turn_on_light(PIN_FORWARD_WHITE);

  digitalWrite(PIN_FORWARD_1, HIGH);
  digitalWrite(PIN_BACK_1, LOW);
  digitalWrite(PIN_FORWARD_2, LOW);
  digitalWrite(PIN_BACK_2, HIGH);
  analogWrite(PIN_PWM_1, SPEED);
  analogWrite(PIN_PWM_2, SPEED);
}

void move_backward() {
  Serial.println("backward()");
  turn_on_light(PIN_BACK_RED);

  digitalWrite(PIN_FORWARD_1, LOW);
  digitalWrite(PIN_BACK_1, HIGH);
  digitalWrite(PIN_FORWARD_2, HIGH);
  digitalWrite(PIN_BACK_2, LOW);
  analogWrite(PIN_PWM_1, SPEED);
  analogWrite(PIN_PWM_2, SPEED);
}

void stop() {
  analogWrite(PIN_PWM_1, 0);
  analogWrite(PIN_PWM_2, 0);
  //  digitalWrite(PIN_FORWARD_1, LOW);
  //  digitalWrite(PIN_BACK_1, LOW);
  //  digitalWrite(PIN_FORWARD_2, LOW);
  //  digitalWrite(PIN_BACK_2, LOW);
  turn_off_light(PIN_FORWARD_WHITE);
  turn_off_light(PIN_BACK_RED);

  turn_off_light(PIN_LEFT_YELLOW);
  turn_off_light(PIN_RIGHT_YELLOW);

  if (!timer1.isRunning())
  {
    timer1.reset();
    timer1.start();
  }

  if (!timer2.isRunning())
  {
    timer2.reset();
    timer2.start();
  }
}

void turn_on_left_light(MillisTimer &mt) {
  turn_on_light(PIN_LEFT_YELLOW);
  //  delay(100);
  //  turn_ off_light(PIN_LEFT_YELLOW);
}

void turn_on_right_light(MillisTimer &mt) {
  turn_on_light(PIN_RIGHT_YELLOW);
  //  delay(100);
  //  turn_off_light(PIN_RIGHT_YELLOW);
}


void setup(void)
{

  Serial.begin(115200);
  printf_begin();

  radio.begin();
  radio.enableDynamicPayloads();
  radio.setRetries(5, 15);
  radio.openWritingPipe(destAddr);
  radio.openReadingPipe(1, localAddr);

  radio.startListening();
  radio.printDetails();


  pinMode(PIN_FORWARD_1, OUTPUT);
  pinMode(PIN_BACK_1, OUTPUT);
  pinMode(PIN_FORWARD_2, OUTPUT);
  pinMode(PIN_BACK_2, OUTPUT);
  pinMode(PIN_PWM_1, OUTPUT);
  pinMode(PIN_PWM_2, OUTPUT);

  pinMode(PIN_FORWARD_WHITE, OUTPUT);
  pinMode(PIN_BACK_RED, OUTPUT);
  pinMode(PIN_LEFT_YELLOW, OUTPUT);
  pinMode(PIN_RIGHT_YELLOW, OUTPUT);



  timer1.setInterval(100);
  timer1.expiredHandler(turn_on_left_light);
  timer1.setRepeats(1);
  timer1.start();

  timer2.setInterval(100);
  timer2.expiredHandler(turn_on_right_light);
  timer2.setRepeats(1);
  timer2.start();
}

void loop(void)
{
  if (radio.available()) {
    // Grab the response, compare, and send to debugging spew
    uint8_t len = radio.getDynamicPayloadSize();

    // If a corrupt dynamic payload is received, it will be flushed
    if (!len) {
      return;
    }

    radio.read( receive_payload, len );

    // Put a zero at the end for easy printing
    receive_payload[len] = 0;

    Serial.print(F("Got response size="));
    Serial.print(len);
    Serial.print(F(" value="));
    Serial.println(receive_payload);


    switch (receive_payload[0]) {
      case 'L':
        turn_left();
        break;
      case 'R':
        turn_right();
        break;
      case 'F':
        move_forward();
        break;
      case 'B':
        move_backward();
        break;
    }

    delay(300);

    stop();
  }

}



