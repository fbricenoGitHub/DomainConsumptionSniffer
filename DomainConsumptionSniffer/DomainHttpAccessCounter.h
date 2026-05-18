#pragma once

#include "CommonHeaders.h"

const int TOP_DOMAINS_TO_PRINT = 10;

enum sendResultsTo {
	CONSOLE,
	LOG_FILE
};

class DomainHttpAccessCounter
{
	// Attributes
	private:
		filesystem::path logFilePath;
		unordered_map<string, unsigned int> domainAccessCount;

	// Constructors and Destructors
	public: 
			DomainHttpAccessCounter(string logFilePath);
			~DomainHttpAccessCounter();


	// Methods
	public:
		bool incrementDomainCount(const string& domain);
		void printDomainConsumption() const;
		void printDomainConsumptionHistogram(sendResultsTo target) const;

	private:
		bool isValidHttpDomain(const string& domain) const;
		bool isDomainPresent(const string& domain) const;
		bool insertDomain(const string& domain);
		bool isDomainCountIncrementSafe(const string& domain);
		vector<pair<string, int>> getOrderedDomainAccessCount() const;
		void printDomainConsumptionHistogram(ostream& os) const;
		ofstream createLogFile() const;

};