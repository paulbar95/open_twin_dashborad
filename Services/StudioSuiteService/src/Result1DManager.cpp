#include "Result1DManager.h"

#include <stdexcept>

Result1DManager::Result1DManager(char* dataBuffer, size_t dataBufferLength)
{
	size_t bufferIndex = 0;

	// First, we read the number of runids from the buffer
	size_t numberRunIds = readIntegerFromBuffer(bufferIndex, dataBuffer, dataBufferLength);

	for (size_t runIndex = 0; runIndex < numberRunIds; runIndex++)
	{
		RunIDContainer* container = new RunIDContainer();
		size_t runID = container->readData(bufferIndex, dataBuffer, dataBufferLength);

		runIDtoContainerMap[runID] = container;
	}
}

Result1DManager::~Result1DManager()
{
	for (auto item : runIDtoContainerMap)
	{
		delete item.second;
	}

	runIDtoContainerMap.clear();
}

size_t Result1DManager::readIntegerFromBuffer(size_t &bufferIndex, char *dataBuffer, size_t dataBufferLength)
{
	size_t value;
	char* valueBuffer = (char*)(&value);

	for (size_t index = 0; index < sizeof(size_t); index++)
	{
		if (bufferIndex == dataBufferLength) throw std::overflow_error("buffer overflow when reading data");

		valueBuffer[index] = dataBuffer[bufferIndex];
		bufferIndex++;
	}

	return value;
}
