#pragma once

#include "CommonHeaders.h"

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