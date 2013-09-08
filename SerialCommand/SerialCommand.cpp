#include "SerialCommand.h"

SerialCommand::SerialCommand(SoftwareSerial sSerial)
  : commandList(NULL),
    commandCount(0),
    defaultHandler(NULL),
    term('\n'),  // default terminator for commands
    last(NULL)
{
  strcpy(delim, " "); // strtok_r needs a null-terminated string
  clearBuffer();
}

/**
 * Adds a "command" and a handler function to the list of available commands.
 * This is used for matching a found token in the buffer, and gives the pointer
 * to the handler function to deal with it.
 */
void SerialCommand::addCommand(const char *command, void (*function)()) {
  #ifdef SC_DEBUG
    sSerial.print("Adding command (");
    sSerial.print(commandCount);
    sSerial.print("): ");
    sSerial.println(command);
  #endif

  commandList = (SerialCommandCallback *) realloc(commandList, (commandCount + 1) * sizeof(SerialCommandCallback));
  strncpy(commandList[commandCount].command, command, SC_MAXLENGTH);
  commandList[commandCount].function = function;
  commandCount++;
}

/**
 * This sets up a handler to be called in the event that the receveived command string
 * isn't in the list of commands.
 */
void SerialCommand::setDefaultHandler(void (*function)(const char *)) {
  defaultHandler = function;
}


/**
 * This checks the Serial stream for characters, and assembles them into a buffer.
 * When the terminator character (default '\n') is seen, it starts parsing the
 * buffer for a prefix command, and calls handlers setup by addCommand() member
 */
void SerialCommand::readSerial() {
  while (sSerial.available() > 0) {
    char inChar = sSerial.read();   // Read single available character, there may be more waiting
    #ifdef SC_DEBUG
      sSerial.print(inChar);   // Echo back to serial stream
    #endif

    if (inChar == term) {     // Check for the terminator (default '\r') meaning end of command
      #ifdef SC_DEBUG
        sSerial.print("Received: ");
        sSerial.println(buffer);
      #endif

      char *command = strtok_r(buffer, delim, &last);   // Search for command at start of buffer
      if (command != NULL) {
        boolean matched = false;
        for (int i = 0; i < commandCount; i++) {
          #ifdef SC_DEBUG
            sSerial.print("Comparing [");
            sSerial.print(command);
            sSerial.print("] to [");
            sSerial.print(commandList[i].command);
            sSerial.println("]");
          #endif

          // Compare the found command against the list of known commands for a match
          if (strncmp(command, commandList[i].command, SC_MAXLENGTH) == 0) {
            #ifdef SC_DEBUG
              sSerial.print("Matched Command: ");
              sSerial.println(command);
            #endif

            // Execute the stored handler function for the command
            (*commandList[i].function)();
            matched = true;
            break;
          }
        }
        if (!matched && (defaultHandler != NULL)) {
          (*defaultHandler)(command);
        }
      }
      clearBuffer();
    }
    else if (isprint(inChar)) {     // Only printable characters into the buffer
      if (bufPos < SC_BUFFER) {
        buffer[bufPos++] = inChar;  // Put character into buffer
        buffer[bufPos] = '\0';      // Null terminate
      } else {
        #ifdef SC_DEBUG
          sSerial.println("Line buffer is full - increase SC_BUFFER");
        #endif
      }
    }
  }
}

/*
 * Clear the input buffer.
 */
void SerialCommand::clearBuffer() {
  buffer[0] = '\0';
  bufPos = 0;
}

/**
 * Retrieve the next token ("word" or "argument") from the command buffer.
 * Returns NULL if no more tokens exist.
 */
char *SerialCommand::next() {
  return strtok_r(NULL, delim, &last);
}
