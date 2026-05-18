#pragma once

#include "CommonHeaders.h"


class CommandLineArgumentValidator
{
private:
	int argumentCount;
	char const* const* argumentValues;

	string ipAddressArgument;
	string logFileArgument;
	unsigned int monitorTimeArgument;


public:
	CommandLineArgumentValidator(int argc, char const* argv[]);

	string getIpAddressArgument() const ;
	string getLogFileArgument() const;
	unsigned int getMonitorTimeArgument() const;

	bool getArgumentValues();
	void printParsedArguments() const;

private:
	void printUsage() const;	
	bool validateArgumentCount() const;
	bool validateLogFileArgument() const;
	bool validateMonitorTimeArgument() const;
	bool validateLocalIpAddressArgument() const;
	
};

