#ifndef TEST_HELPER_H
#define TEST_HELPER_H

#include "pstr_helper.h"

#define SUCCESS_BUTTON 5
#define FAILURE_BUTTON 4

P_STR(MANUAL_TEST,"Flip the pass or fail switch now, then reset to off.");

bool ReadButton(int digital_pin);

bool AskUserForTestSuccess();

#endif // TEST_HELPER_H
