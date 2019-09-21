// Piano keyboard definees
#define NUM_COLS 11
#define NUM_ROWS 6

// MIDI defines
#define NOTE_START 24
#define NOTE_VELOCITY 127

#include "PinDefinitions.h"
#include <MIDI.h>

// Arrays of column and row pins
int colPins[NUM_COLS] = {R0,R1,R2,R3,R4,R5,R6,R7,R8,R9,R10};
int rowPins[NUM_ROWS] = {N5,N4,N3,N2,N1,N0};

// TODO: Comment this thing
boolean keyPressed[NUM_COLS][NUM_ROWS];
uint8_t keyToMidiMap[NUM_COLS][NUM_ROWS];

// Initialie MIDI interface
MIDI_CREATE_DEFAULT_INSTANCE();

void setup() {
    int note = NOTE_START + 1;

    // I don't know whitch one is 'C1' key so I will just fill all column with default note :)
    for (int row = 0; row < NUM_ROWS; ++row) {
        keyPressed[0][row] = false;
        keyToMidiMap[0][row] = NOTE_START;
    }

    // Fill arrays with default values
    for (int col = 1; col < NUM_COLS; ++col) {
        for (int row = 0; row < NUM_ROWS; ++row) {
            keyPressed[col][row] = false;
            keyToMidiMap[col][row] = note;
            note++;
        }
    }

    // Setup column pins
    for (int col = 0; col < NUM_COLS; ++col) {
        pinMode(colPins[col], OUTPUT);
        digitalWrite(colPins[col], HIGH);
    }

    // Setup row pins
    for (int row = 0; row < NUM_ROWS; ++row) {
        pinMode(rowPins[row], INPUT_PULLUP);
    }

    // Start MIDI interface
    MIDI.begin(MIDI_CHANNEL_OMNI);
}

void loop() {
    for (int col = 0; col < NUM_COLS; ++col) {
        digitalWrite(colPins[col], LOW);

        // Read column row values
        int rowValues[NUM_ROWS];
        for (int row = 0; row < NUM_ROWS; ++row) {
            rowValues[row] = !digitalRead(rowPins[row]);
        }

        // Process key press
        for (int row = 0; row < NUM_ROWS; ++row) {
            if(rowValues[row] != 0 && !keyPressed[col][row]) {
                keyPressed[col][row] = true;
                MIDI.sendNoteOn(keyToMidiMap[col][row], NOTE_VELOCITY, 1);
            }
        }

        // Process key release
        for (int row = 0; row < NUM_ROWS; ++row) {
            if(rowValues[row] == 0 && keyPressed[col][row]) {
                keyPressed[col][row] = false;
                MIDI.sendNoteOff(keyToMidiMap[col][row], 0, 1);
            }
        }

        digitalWrite(colPins[col], HIGH);
    }
}
