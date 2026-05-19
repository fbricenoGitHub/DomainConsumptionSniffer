// DomainConsumptionSniffer.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "CommandLineArgumentValidator.h"
#include "DomainHttpAccessCounter.h"

static DomainHttpAccessCounter* accessCounter;

static void onPacketArrives(pcpp::RawPacket* packet, pcpp::PcapLiveDevice* dev, void* cookie);

void testCommandLineArgumentValidator();
void testDomainHttpAccessCounter();
void testPrintDomainConsumptionHistogram();

int main(int argc, char const* argv[])
{
	CommandLineArgumentValidator argumentValidator(argc, argv);
	if (!argumentValidator.getArgumentValues()) {
		// If getArgumentValues returns false, it means either help was printed or there was an error in parsing/validation.
		// In both cases, we should exit the program.
		return 0;	
	}

	argumentValidator.printParsedArguments();
	
	accessCounter = new DomainHttpAccessCounter(argumentValidator.getLogFileArgument());



	// find the interface by IP address
	auto* dev = pcpp::PcapLiveDeviceList::getInstance().getDeviceByIp(argumentValidator.getIpAddressArgument());
	if (dev == nullptr)
	{
		cerr << "Cannot find interface with IPv4 address of '" << argumentValidator.getIpAddressArgument() << "'" << endl;
	}

	// before capturing packets let's print some info about this interface
	cout
		<< "Interface info:" << endl
		<< "   Interface name:        " << dev->getName() << endl // get interface name
		<< "   Interface description: " << dev->getDesc() << endl // get interface description
		<< "   MAC address:           " << dev->getMacAddress() << endl // get interface MAC address
		<< "   Default gateway:       " << dev->getDefaultGateway() << endl // get default gateway
		<< "   Interface MTU:         " << dev->getMtu() << endl; // get interface MTU

	if (!dev->getDnsServers().empty())
	{
		cout << "   DNS server:            " << dev->getDnsServers().front() << endl;
	}

	// open the device before start capturing/sending packets
	if (!dev->open())
	{
		cerr << "Cannot open device" << endl;
		return 0;
	}

	cout << endl << "Starting async capture..." << endl;
	PacketStats stats;

	dev->startCapture(onPacketArrives, &stats);

	// sleep for 10 seconds in main thread, in the meantime packets are captured in the async thread
	this_thread::sleep_for(chrono::seconds(argumentValidator.getMonitorTimeArgument()));


	// stop capturing packets
	dev->stopCapture();

	accessCounter->printDomainConsumptionHistogram(CONSOLE);
	accessCounter->printDomainConsumptionHistogram(LOG_FILE);


	delete accessCounter;
	


	return 1;

	/* TESTs 
		testCommandLineArgumentValidator();
		testDomainHttpAccessCounter();	
		testPrintDomainConsumptionHistogram();
	*/
	
}





static void onPacketArrives(pcpp::RawPacket* packet, pcpp::PcapLiveDevice* dev, void* cookie) {
	// extract the stats object form the cookie
	PacketStats* stats = static_cast<PacketStats*>(cookie);

	// parsed the raw packet
	pcpp::Packet parsedPacket(packet);

	if (parsedPacket.isPacketOfType(pcpp::HTTP)) {
		auto* httpRequestLayer = parsedPacket.getLayerOfType<pcpp::HttpRequestLayer>();
		if (httpRequestLayer == nullptr)
		{
			cerr << "Something went wrong, couldn't find HTTP request layer" << endl;
			return;
		}

		accessCounter->incrementDomainCount(httpRequestLayer->getFieldByName(PCPP_HTTP_HOST_FIELD)->getFieldValue());
	}

	// collect stats from packet
	stats->consumePacket(parsedPacket);
}




// Test Functions
void testCommandLineArgumentValidator() {
	// Test cases for CommandLineArgumentValidator
	int argumentCount = 7;
	char const* testArgs[] = {"DomainConsumptionSniffer", "-i", "192.168.2.54", "-l", "C:\\results.txt", "-t", "60"};
	CommandLineArgumentValidator cmdLineArgsValidator(argumentCount, testArgs);

	if (cmdLineArgsValidator.getArgumentValues()) {
		cmdLineArgsValidator.printParsedArguments();
	}
	else {
		cerr << "Failed to parse command line arguments." << endl;
	}	
}



void testDomainHttpAccessCounter() {
	DomainHttpAccessCounter counter("C:\\results.txt");
	counter.incrementDomainCount("http://example.com");
	counter.incrementDomainCount("https://google.com");
	counter.incrementDomainCount("http://example.com");
	counter.incrementDomainCount("invalid_domain");
	counter.incrementDomainCount("http://example.com/with/path");
	counter.printDomainConsumption();
}


void testPrintDomainConsumptionHistogram() {
	DomainHttpAccessCounter counter("E:\\Usuarios\\Fernando\\Clientes\\LUMU\\prueba\\code\\results\\logFile.txt");
	counter.incrementDomainCount("http://example.com");
	counter.incrementDomainCount("https://google.com");
	counter.incrementDomainCount("https://google.com");
	counter.incrementDomainCount("https://google.com");
	counter.incrementDomainCount("https://google.com");
	counter.incrementDomainCount("http://example.com");
	counter.incrementDomainCount("http://example.com/with/path");
	counter.printDomainConsumptionHistogram(CONSOLE);
	counter.printDomainConsumptionHistogram(LOG_FILE);
}