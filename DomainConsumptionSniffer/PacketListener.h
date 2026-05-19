#pragma once

#include "CommonHeaders.h"
#include "DomainHttpAccessCounter.h"
#include "CommandLineArgumentValidator.h"
#include "PacketStats.cpp"

class PacketListener
{
private:	
	DomainHttpAccessCounter* accessCounter;
	CommandLineArgumentValidator* argumentValidator;

public:
	

public:
	PacketListener(DomainHttpAccessCounter* accessCounter, CommandLineArgumentValidator* argumentValidator);
	void Listen() const;
	void onPacketArrives(pcpp::RawPacket* packet, pcpp::PcapLiveDevice* dev, void* cookie);
};

