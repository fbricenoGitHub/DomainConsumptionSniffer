#include "DomainHttpAccessCounter.h"


DomainHttpAccessCounter::DomainHttpAccessCounter(){}
DomainHttpAccessCounter::~DomainHttpAccessCounter(){
    if (this->domainAccessCount.empty()) {
		this->domainAccessCount.clear();
    }
}

bool DomainHttpAccessCounter::incrementDomainCount(const string& domain){
	if (!isValidHttpDomain(domain)) {
		cerr << "Invalid domain : " << domain << endl;
		return false;
	}

	if (!isDomainCountIncrementSafe(domain)) {
		cerr << "Increment domain counter would overflow : " << domain << endl;
		return false;
	}

	if (!isDomainPresent(domain)) {
		insertDomain(domain); 
	}
	this->domainAccessCount[domain]++;
	return true;
}


void DomainHttpAccessCounter::printDomainConsumption() const {
	cout << endl << endl;

	for (auto it = this->domainAccessCount.begin(); it != this->domainAccessCount.end(); it++) {
		std::cout << std::left << std::setw(50) << it->first << std::setw(60) << it->second << endl;
	}

}


bool DomainHttpAccessCounter::isValidHttpDomain(const string& domain) const{
    // Regex pattern for HTTP/HTTPS domain validation
    const regex pattern(R"(^((http|https)://)([a-zA-Z0-9.-]+)\.([a-z]{2,6})(/[a-zA-Z0-9@:%._\+~#?&//=]*)?$)");

    // Use std::regex_match to check the full string
    return regex_match(domain, pattern);
}

bool DomainHttpAccessCounter::isDomainPresent(const string& domain) const{
	if (this->domainAccessCount.empty()) {
		return false;
	}
	if (this->domainAccessCount.find(domain) != this->domainAccessCount.end()) {
		return true;
	}
}

bool DomainHttpAccessCounter::insertDomain(const string& domain) {
	this->domainAccessCount.insert(make_pair(domain, 1));
	return true;
}

bool DomainHttpAccessCounter::isDomainCountIncrementSafe(const string& domain) {

	unsigned int count = this->domainAccessCount[domain];
	if (count != std::numeric_limits<unsigned int>::max()) {
		return true;
	}
	else {
		return false;
	}
}