#pragma once

#include <iostream>
#include <string>
#include <unordered_map>
#include <regex>
#include <limits>
#include <iomanip>


using namespace std;

class DomainHttpAccessCounter
{
	// Attributes
	private:
		unordered_map<string, unsigned int> domainAccessCount;

	// Constructors and Destructors
	public: 
			DomainHttpAccessCounter();
			~DomainHttpAccessCounter();


	// Methods
	public:
		bool incrementDomainCount(const string& domain);
		void printDomainConsumption() const;

	private:
		bool isValidHttpDomain(const string& domain) const;
		bool isDomainPresent(const string& domain) const;
		bool insertDomain(const string& domain);
		bool isDomainCountIncrementSafe(const string& domain);

};