#ifndef SerialCommand_h
#define SerialCommand_h

#include <Arduino.h>
#include <SoftwareSerial.h>
#include <string.h>


// Size of the input buffer in bytes, maximum length of one command plus
// arguments.
#define SC_BUFFER 32
// Maximum length of a command, excluding the terminating null
#define SC_MAXLENGTH 8

// Uncomment to turn on debug mode
//#define SC_DEBUG


class SerialCommand {
  public:
    SerialCommand(SoftwareSerial *sSerial);      // Constructor
    void addCommand(const char *command, void(*function)());  // Add a command to the processing dictionary.
    void setDefaultHandler(void (*function)(const char *));   // A handler to call when no valid command received.

    void readSerial();    // Main entry point.
    void clearBuffer();   // Clears the input buffer.
    char *next();         // Returns pointer to next token found in command buffer (for getting arguments to commands).

  private:
    // Command/handler dictionary
    struct SerialCommandCallback {
      char command[SC_MAXLENGTH + 1];
      void (*function)();
    };                                    // Data structure to hold Command/Handler function key-value pairs
    SerialCommandCallback *commandList;   // Actual definition for command/handler array
    byte commandCount;
    SoftwareSerial &sSerial;

    // Pointer to the default handler function
    void (*defaultHandler)(const char *);

    char delim[2]; // null-terminated list of character to be used as delimeters for tokenizing (default " ")
    char term;     // Character that signals end of command (default '\n')

    char buffer[SC_BUFFER + 1]; // Buffer of stored characters while waiting for terminator character
    byte bufPos;                        // Current position in the buffer
    char *last;                         // State variable used by strtok_r during processing
};

#endif //SerialCommand_h
