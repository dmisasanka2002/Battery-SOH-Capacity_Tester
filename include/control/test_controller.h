#pragma once

#include <Arduino.h>

#include "storage/file_manager.h"   // TestMode

void testControllerBegin();
void testControllerUpdate();

bool testControllerStart();
void testControllerStop();
bool testControllerIsRunning();

void testControllerSetConfig(uint16_t moduleNumber, uint16_t cycleNumber, TestMode mode);
bool testControllerSetSequence(const String sequence[], uint8_t length);

uint16_t testControllerGetModuleNumber();
uint16_t testControllerGetCycleNumber();
TestMode  testControllerGetMode();
uint32_t  testControllerGetElapsedSeconds();
String    testControllerGetCurrentMethodName();
String    testControllerGetCurrentFilePath();

uint8_t testControllerGetSequenceLength();
String  testControllerGetSequenceStep(uint8_t index);