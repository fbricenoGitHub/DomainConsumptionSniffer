#include "CommandLineArgumentValidator.h"




CommandLineArgumentValidator::CommandLineArgumentValidator(int argc, char const* argv[]) {
	this->argumentCount = argc;
	this->argumentValues = argv;
	this->ipAddressArgument = "";
	this->logFileArgument = "";
	this->monitorTimeArgument = 0;	
}

string CommandLineArgumentValidator::getIpAddressArgument() const {
	return this->ipAddressArgument;
}

string CommandLineArgumentValidator::getLogFileArgument() const {
	return this->logFileArgument;
}

unsigned int CommandLineArgumentValidator::getMonitorTimeArgument() const {
	return this->monitorTimeArgument;
}


void CommandLineArgumentValidator::printUsage() const {
	cout << endl;
	cout << "Usage: DomainConsumptionSniffer [options]" << endl;
	cout << "Options:" << endl;
	cout << "  -h, --help\t\t\t\tShow this help message and exit" << endl;
	cout << "  -i, --ipAddress\t\t[Required] NIC assigned ip address that traffic will be listened to" << endl;
	cout << "  -l, --LogFile <FilePath>\t[Required] File where results will be saved." << endl;
	cout << "  -t, --time\t\t\t[Required] Time interval in seconds to monitor the network interface traffic" << endl << endl;
	cout << "Example: DomainConsumptionSniffer -i 192.168.2.54 -l C:\\results.txt -t 60" << endl;
}


// parse argc/argv stored in the instance; returns true if parsing+validation succeeded.
// If help (-h/--help) is present, usage is printed and the method returns false (caller should exit).
bool CommandLineArgumentValidator::getArgumentValues() {
	for (int i = 1; i < this->argumentCount; ++i) {
		string arg = this->argumentValues[i];

		// Help: must work apart from other args -> print and return false immediately
		if (arg == "-h" || arg == "--help") {
			printUsage();
			return false;
		}

		// IP Address forms:
		// --ipAddress=val  |  --ipAddress val  |  -iVal  |  -i val
		const string longIpPrefix = "--ipAddress=";
		if (arg.rfind(longIpPrefix, 0) == 0) {
			this->ipAddressArgument = arg.substr(longIpPrefix.size());
			continue;
		}
		if (arg == "--ipAddress" || arg == "-i") {
			if (i + 1 < this->argumentCount) {
				++i;
				this->ipAddressArgument = this->argumentValues[i];
				continue;
			}
			else {
				cerr << "Missing value for " << arg << endl;
				printUsage();
				return false;
			}
		}
		if (arg.size() > 2 && arg[0] == '-' && arg[1] == 'i') {
			this->ipAddressArgument = arg.substr(2);
			continue;
		}

		// Log file forms:
		// --LogFile=val  |  --LogFile val  |  -lVal  |  -l val
		const string longLogPrefix = "--LogFile=";
		if (arg.rfind(longLogPrefix, 0) == 0) {
			this->logFileArgument = arg.substr(longLogPrefix.size());
			continue;
		}
		if (arg == "--LogFile" || arg == "-l") {
			if (i + 1 < this->argumentCount) {
				++i;
				this->logFileArgument = this->argumentValues[i];
				continue;
			}
			else {
				cerr << "Missing value for " << arg << endl;
				printUsage();
				return false;
			}
		}
		if (arg.size() > 2 && arg[0] == '-' && arg[1] == 'l') {
			this->logFileArgument = arg.substr(2);
			continue;
		}

		// Time forms:
		// --time=val  |  --time val  |  -tVal  |  -t val
		const string longTimePrefix = "--time=";
		if (arg.rfind(longTimePrefix, 0) == 0) {
			string val = arg.substr(longTimePrefix.size());
			try {
				this->monitorTimeArgument = static_cast<unsigned int>(stoul(val));
			}
			catch (...) {
				cerr << "Invalid value for time: " << val << endl;
				printUsage();
				return false;
			}
			continue;
		}
		if (arg == "--time" || arg == "-t") {
			if (i + 1 < this->argumentCount) {
				++i;
				string val = this->argumentValues[i];
				try {
					this->monitorTimeArgument = static_cast<unsigned int>(stoul(val));
				}
				catch (...) {
					cerr << "Invalid value for time: " << val << endl;
					printUsage();
					return false;
				}
				continue;
			}
			else {
				cerr << "Missing value for " << arg << endl;
				printUsage();
				return false;
			}
		}
		if (arg.size() > 2 && arg[0] == '-' && arg[1] == 't') {
			string val = arg.substr(2);
			try {
				this->monitorTimeArgument = static_cast<unsigned int>(stoul(val));
			}
			catch (...) {
				cerr << "Invalid value for time: " << val << endl;
				printUsage();
				return false;
			}
			continue;
		}

		// Unknown option
		cerr << "Unknown option: " << arg << endl;
		printUsage();
		return false;
	}

	// After parsing, run validations for required args
	if (!validateArgumentCount()) {
		cerr << "Missing required arguments." << endl;
		printUsage();
		return false;
	}
	if (!validateLocalIpAddressArgument()) {
		cerr << "Invalid IP address: " << this->ipAddressArgument << endl;
		printUsage();
		return false;
	}
	if (!validateLogFileArgument()) {
		cerr << "Invalid log file path: " << this->logFileArgument << endl;
		printUsage();
		return false;
	}
	if (!validateMonitorTimeArgument()) {
		cerr << "Invalid monitor time: " << this->monitorTimeArgument << endl;
		printUsage();
		return false;
	}

	return true;
}

void CommandLineArgumentValidator::printParsedArguments() const {
	cout << "Parsed Arguments:" << endl;
	cout << "IP Address: " << this->ipAddressArgument << endl;
	cout << "Log File: " << this->logFileArgument << endl;
	cout << "Monitor Time: " << this->monitorTimeArgument << " seconds" << endl;
}

bool CommandLineArgumentValidator::validateArgumentCount() const {
	// ensure required arguments were provided (ip, log file, time)
	return !this->ipAddressArgument.empty() &&
		!this->logFileArgument.empty() &&
		this->monitorTimeArgument > 0;
}

bool CommandLineArgumentValidator::validateLogFileArgument() const {
	// basic check: non-empty. More checks (writable path) could be added if needed.
	return !this->logFileArgument.empty();
}

bool CommandLineArgumentValidator::validateMonitorTimeArgument() const {
	// must be > 0 (already unsigned)
	return this->monitorTimeArgument > 0;
}

bool CommandLineArgumentValidator::validateLocalIpAddressArgument() const {
	// Simple IPv4 validation: four octets 0-255
	const regex ipv4(R"(^(\d{1,3}\.){3}\d{1,3}$)");
	if (!regex_match(this->ipAddressArgument, ipv4)) {
		return false;
	}

	// ensure each octet is 0-255
	istringstream iss(this->ipAddressArgument);
	string token;
	int octetCount = 0;
	while (getline(iss, token, '.')) {
		++octetCount;
		try {
			int v = stoi(token);
			if (v < 0 || v > 255) return false;
		}
		catch (...) {
			return false;
		}
	}
	return octetCount == 4;
}

