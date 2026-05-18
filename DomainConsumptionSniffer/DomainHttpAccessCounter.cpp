#include "DomainHttpAccessCounter.h"


DomainHttpAccessCounter::DomainHttpAccessCounter(string logFilePath):logFilePath(logFilePath){}

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

void DomainHttpAccessCounter::printDomainConsumptionHistogram(sendResultsTo target) const{
	ofstream logFile;
	switch (target)
	{
	case CONSOLE:
		printDomainConsumptionHistogram(cout);
		break;
	case LOG_FILE:
		logFile = createLogFile();
		if (logFile.is_open()) {
			printDomainConsumptionHistogram(logFile);
			logFile.close();
		}
		break;
	default:
		break;
	}
}

void DomainHttpAccessCounter::printDomainConsumptionHistogram(ostream& os) const {
	vector<pair<string, int>> orderedDomainAccessCount = getOrderedDomainAccessCount();

	for (int i = 0; i < min(TOP_DOMAINS_TO_PRINT, static_cast<int>(orderedDomainAccessCount.size())); ++i) {
		const auto& pair = orderedDomainAccessCount[i];
		os << std::left << std::setw(50) << pair.first << std::setw(60) << string(pair.second, '*') << endl;
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


vector<pair<string, int>> DomainHttpAccessCounter::getOrderedDomainAccessCount() const {
	vector<pair<string, int>> orderedDomainAccessCount(this->domainAccessCount.begin(), this->domainAccessCount.end());
	sort(orderedDomainAccessCount.begin(), orderedDomainAccessCount.end(), [](const pair<string, int>& a, const pair<string, int>& b) {
		return a.second > b.second; // Sort in descending order based on access count
		});
	return orderedDomainAccessCount;
}

ofstream DomainHttpAccessCounter::createLogFile() const {
	filesystem::path dirPath = this->logFilePath.parent_path();
	if (!dirPath.empty()) {
		filesystem::create_directories(dirPath);
	}

	ofstream logFileStream(this->logFilePath, ios::out | ios::trunc);

	if (logFileStream.is_open()) {
		cout << "Log file opened!" << endl;
	}
	else {
		logFileStream.setstate(ios::failbit);
		cerr << "Error: Could not open file." << endl;
	}

	return logFileStream;
}
