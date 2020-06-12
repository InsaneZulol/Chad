#include "pch.h"
#include "console_utilities.h"

ConsoleUtilities::ConsoleUtilities() {
  SetConsoleOutputCP(CP_UTF8);
  // from now on remember to either: a) only use wcout 
  // b) setmode to O_TEXT before couting
  _setmode(_fileno(stdout), _O_U8TEXT);  
}