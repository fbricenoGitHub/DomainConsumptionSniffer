// DomainConsumptionSniffer.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "DomainHttpAccessCounter.h"


void testDomainHttpAccessCounter();

int main()
{
	testDomainHttpAccessCounter();
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
