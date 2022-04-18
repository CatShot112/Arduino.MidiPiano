// Piano keyboard definees
#define NUM_COLS 11
#define NUM_ROWS 6

// MIDI defines
#define NOTE_START 24
#define NOTE_VELOCITY 127

#include "PinDefinitions.h"
#include <MIDI.h>

// TODO: Comment this thing
byte keyPressed[NUM_COLS];
uint8_t keyToMidiMap[NUM_COLS][NUM_ROWS];

// Initialie MIDI interface
MIDI_CREATE_DEFAULT_INSTANCE();

void setup() {
    int note = NOTE_START + 1;

    // I don't know which one is 'C1' key so I will just fill all column with default note :)
    for (int row = 0;row < NUM_ROWS;++row) {
        keyPressed[0] = 0;
        keyToMidiMap[0][row] = NOTE_START;
    }

    // Fill arrays with default values
    for (int col = 1;col < NUM_COLS;++col) {
        keyPressed[col] = 0;
        
        for (int row = 0;row < NUM_ROWS;++row) {
            keyToMidiMap[col][row] = note;
            note++;
        }
    }

    // Setup column pins
    DDRD  = 0B11111100; // Pins 2-7 as OUTPUT
    PORTD = 0B11111100; // Set them as HIGH
    DDRB  = 0B00011111; // Pins 8-12 as OUTPUT
    PORTB = 0B00011111; // Set them as HIGH

    // Setup row pins
    DDRC  = 0B00000000; // Pins A0-A5 as INPUT
    PORTC = 0B00111111; // Enable INPUT_PULLUP

    // Start MIDI interface
    MIDI.begin(MIDI_CHANNEL_OMNI);
}

// Bitmask for columns
byte cols[] = {
    0B00000100,
    0B00001000,
    0B00010000,
    0B00100000,
    0B01000000,
    0B10000000,
    0B00000001,
    0B00000010,
    0B00000100,
    0B00001000,
    0B00010000
};

void loop() {
    for (int col = 0; col < NUM_COLS; ++col) {
        col > 4 ? PORTB ^= cols[col] : PORTD ^= cols[col];

        // Read column row values
        byte rowValues = PINC;

        // Process key press
        for (int row = 0;row < NUM_ROWS;++row) {
            if(rowValues & (1 << row) == 0 && !keyPressed[col] & (1 << row)) {
                keyPressed[col] |= (1 << row);
                MIDI.sendNoteOn(keyToMidiMap[col][row], NOTE_VELOCITY, 1);
            }
        }

        // Process key release
        for (int row = 0;row < NUM_ROWS;++row) {
            if(rowValues & (1 << row) != 0 && keyPressed[col] & (1 << row)) {
                keyPressed[col] &= -(1 << row);
                MIDI.sendNoteOff(keyToMidiMap[col][row], 0, 1);
            }
        }

        col > 4 ? PORTB ^= cols[col] : PORTD ^= cols[col];
    }
}
