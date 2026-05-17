// DomainConsumptionSniffer.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "CommandLineArgumentValidator.h"
#include "DomainHttpAccessCounter.h"



void testCommandLineArgumentValidator();
void testDomainHttpAccessCounter();

int main(int argc, char const* argv[])
{

	

	CommandLineArgumentValidator argumentValidator(argc, argv);
	if (!argumentValidator.getArgumentValues()) {
		// If getArgumentValues returns false, it means either help was printed or there was an error in parsing/validation.
		// In both cases, we should exit the program.
		return 0;	
	}
	
	/* TESTs 
		testCommandLineArgumentValidator();
		testDomainHttpAccessCounter();
	
	*/
	
}





// Validation Functions


// Information Functions



// Test Functions
void testCommandLineArgumentValidator() {
	// Test cases for CommandLineArgumentValidator
	int argumentCount = 7;
	char const* testArgs[] = {"DomainConsumptionSniffer", "-i", "192.168.2.54", "-l", "C:\\results.txt", "-t", "60"};
	CommandLineArgumentValidator cmdLineArgsValidator(argumentCount, testArgs);

	if (cmdLineArgsValidator.getArgumentValues()) {
		cout << "IP Address: " << cmdLineArgsValidator.getIpAddressArgument() << endl;
		cout << "Log File: " << cmdLineArgsValidator.getLogFileArgument() << endl;
		cout << "Monitor Time: " << cmdLineArgsValidator.getMonitorTimeArgument() << " seconds" << endl;
	}
	else {
		cerr << "Failed to parse command line arguments." << endl;
	}	
}



void testDomainHttpAccessCounter() {
	DomainHttpAccessCounter counter;
	counter.incrementDomainCount("http://example.com");
	counter.incrementDomainCount("https://google.com");
	counter.incrementDomainCount("http://example.com");
	counter.incrementDomainCount("invalid_domain");
	counter.incrementDomainCount("http://example.com/with/path");
	counter.printDomainConsumption();
}
