#include "./include/logger.h"

namespace scratchpad
{
	logger::logger(io_device& LoggerIODevice)
	{

	}

	logger::~logger()
	{
		// destroy logger IO
	}

	void logger::ConsoleInfo(const char* Message)
	{
		string Prefix("[INFO] ");

		string MessageWrapper(Prefix);
		MessageWrapper.append(Message);

		IO->Write(MessageWrapper.c_str(), MessageWrapper.size());
	}

	void logger::ConsoleNotice(const char* Message)
	{
		string Prefix("[NOTICE] ");

		string MessageWrapper(Prefix);
		MessageWrapper.append(Message);

		IO->Write(MessageWrapper.c_str(), MessageWrapper.size());
	}

	void logger::ConsoleWarning(const char* Message)
	{
		string Prefix("[WARNING] ");

		string MessageWrapper(Prefix);
		MessageWrapper.append(Message);

		IO->Write(MessageWrapper.c_str(), MessageWrapper.size());
	}

	void logger::ConsoleError(const char* Message)
	{
		string Prefix("[ERROR] ");

		string MessageWrapper(Prefix);
		MessageWrapper.append(Message);

		IO->Write(MessageWrapper.c_str(), MessageWrapper.size());
	}
}
