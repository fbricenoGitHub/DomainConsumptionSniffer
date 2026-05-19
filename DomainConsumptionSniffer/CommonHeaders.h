#pragma once

#pragma comment(lib, "ws2_32.lib")

#include <iostream>
#include <string>
#include <unordered_map>
#include <limits>
#include <regex>
#include <iomanip>
#include <sstream>
#include <stdexcept>
#include <fstream>
#include <filesystem>
#include <algorithm>
#include "stdlib.h"
#include "PcapLiveDeviceList.h"
#include "SystemUtils.h"
#include <Packet.h>
#include <chrono>
#include "HttpLayer.h"
#include "PacketStats.cpp"


using namespace std;