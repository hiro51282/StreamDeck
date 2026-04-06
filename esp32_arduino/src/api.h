#pragma once
#include "model.h"

State fetchMenu();
void sendAction(String type);
bool pingServer();
bool waitForServer();
void connectWiFi();
void sendWOL();