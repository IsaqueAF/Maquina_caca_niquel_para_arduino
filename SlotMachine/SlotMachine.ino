#include <AccelStepper.h>

AccelStepper FStepper(AccelStepper::FULL4WIRE, 10, 11, 12, 13);
AccelStepper SStepper(AccelStepper::FULL4WIRE, 6, 7, 8, 9);
AccelStepper TStepper(AccelStepper::FULL4WIRE, 2, 3, 4, 5);

int SlotsLuck[6][2] = {
  {0, 1},
  {1, 10},
  {2, 20},
  {3, 20},
  {4, 20},
  {5, 30}
};

// 0 sete
// 1 bar
// 2 diamante
// 3 trevo
// 4 moeda
// 5 cereja

int fSlots[6] = {0,3,4,1,2,5};
int sSlots[6] = {0,2,1,5,4,3}; 
int tSlots[6] = {0,4,3,2,5,1};

const int maxSpeed = 700;
const int acceleration = 400;

const int winChance = 30;
const int twoSlotsChance = 40;

struct Slots {
  int slot1;
  int slot2;
  int slot3;
};

Slots luck () {
  Slots results;
  int win = random(100);

  if (win <= winChance) {
    int slot = random(100);
    
    for (int i = 0; i < 6; i++) {
      if (slot <= SlotsLuck[i][1]) {
        results.slot1 = SlotsLuck[i][0];
        results.slot2 = SlotsLuck[i][0];
        results.slot3 = SlotsLuck[i][0];
        return results;
      }
      slot -= SlotsLuck[i][1];
    }
  }
  win -= winChance;

  if (win < twoSlotsChance) {
    int sortSlot[3];
    sortSlot[0] = random(3);
    while (true) {
      sortSlot[1] = random(3);
      if (sortSlot[1] != sortSlot[0]) {break;}
    }
    sortSlot[2] = 3 - sortSlot[0] - sortSlot[1];

    int chose[3];
    int i = random(6);
    chose[sortSlot[0]] = SlotsLuck[i][0];
    chose[sortSlot[1]] = SlotsLuck[i][0];
    while (true) {
      chose[sortSlot[2]] = random(6);
      if (chose[sortSlot[2]] != SlotsLuck[i][0]) {break;}
    }
    results.slot1 = chose[0];
    results.slot2 = chose[1];
    results.slot3 = chose[2];
    return results;
  }

  int chose[3];
  chose[0] = random(6);
  while (true) {
    chose[1] = random(6);
    if (chose[1] != chose[0]) {break;}
  }
  while (true) {
    chose[2] = random(6);
    if (chose[2] != chose[0] && chose[2] != chose[1]) {break;}
  }
  return results;
}

Slots CalculateSpin(Slots currentLuck) {
  Slots results;

  int newSlot;
  for (int i = 0; i < 6; i++) {
    if (fSlots[i] == currentLuck.slot1) {
      newSlot = i;
      break;
    }
  }
  results.slot1 = 2048 * 2 + (2048 / 6 * newSlot);

  newSlot;
  for (int i = 0; i < 6; i++) {
    if (sSlots[i] == currentLuck.slot2) {
      newSlot = i;
      break;
    }
  }
  results.slot2 = 2048 * 3 + (2048 / 6 * newSlot);

  newSlot;
  for (int i = 0; i < 6; i++) {
    if (tSlots[i] == currentLuck.slot3) {
      newSlot = i;
      break;
    }
  }
  results.slot3 = 2048 * 4 + (2048 / 6 * newSlot);

  return results;
}

void Spin () {
  Slots currentLuck = luck();

  Serial.print(currentLuck.slot3);
  Serial.print(" - ");
  Serial.print(currentLuck.slot2);
  Serial.print(" - ");
  Serial.println(currentLuck.slot1);

  Slots rotation = CalculateSpin(currentLuck);

  FStepper.moveTo(rotation.slot1);
  SStepper.moveTo(rotation.slot2);
  TStepper.moveTo(rotation.slot3);

  while (true) {
    
    FStepper.run();
    SStepper.run();
    TStepper.run();

    if (FStepper.distanceToGo() == 0 && SStepper.distanceToGo() == 0 && TStepper.distanceToGo() == 0) {
      FStepper.setCurrentPosition(FStepper.currentPosition() - 2048 * 2);
      SStepper.setCurrentPosition(SStepper.currentPosition() - 2048 * 3);
      TStepper.setCurrentPosition(TStepper.currentPosition() - 2048 * 4);

      return;
    }
  }
}

void Reset () {
  FStepper.moveTo(0);
  SStepper.moveTo(0);
  TStepper.moveTo(0);

  while (true) {
    FStepper.run();
    SStepper.run();
    TStepper.run();

    if (FStepper.distanceToGo() == 0 && SStepper.distanceToGo() == 0 && TStepper.distanceToGo() == 0) {
      return;
    }
  }
}

void Calibrate () {
  FStepper.setMaxSpeed(200);
  SStepper.setMaxSpeed(200);
  TStepper.setMaxSpeed(200);

  FStepper.moveTo(999999);
  SStepper.moveTo(999999);
  TStepper.moveTo(999999);

  bool fMove = true;
  bool sMove = true;
  bool tMove = true;

  while (true) {
    FStepper.run();
    SStepper.run();
    TStepper.run();

    if (TStepper.distanceToGo() == 0) {
      FStepper.setCurrentPosition(0);
      SStepper.setCurrentPosition(0);
      TStepper.setCurrentPosition(0);

      FStepper.setMaxSpeed(maxSpeed);
      SStepper.setMaxSpeed(maxSpeed);
      TStepper.setMaxSpeed(maxSpeed);
      
      return;
    }

    if (Serial.read() == 105) {
      if (!sMove) {
        tMove = false;
        TStepper.moveTo(TStepper.currentPosition() + 2048);
      }

      if (!fMove && sMove) {
        sMove = false;
        SStepper.moveTo(SStepper.currentPosition() + 2048);
      }

      if (fMove) {
        fMove = false;
        FStepper.moveTo(FStepper.currentPosition() + 2048);
      }
    }
  }
}

void setup() {
  FStepper.setMaxSpeed(maxSpeed);
  FStepper.setAcceleration(acceleration);
  SStepper.setMaxSpeed(maxSpeed);
  SStepper.setAcceleration(acceleration);
  TStepper.setMaxSpeed(maxSpeed);
  TStepper.setAcceleration(acceleration);

  randomSeed(analogRead(A0));

  Serial.begin(9600);
}

void loop() {
  int input = Serial.read();

  if (analogRead(A5) <= 400 || input == 115) {
    Spin();
  }

  if (input == 114) {
    Reset();
  }

  if (input == 99) {
    Calibrate();
  }
}