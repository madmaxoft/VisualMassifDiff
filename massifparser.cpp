// massifparser.cpp

// Implements the MassifParser class representing the parser for Massif-generated datafiles





#include "globals.h"
#include "massifparser.h"
#include <QIODevice>
#include "parseinteger.h"
#include "snapshot.h"
#include "allocation.h"





MassifParser::MassifParser(void):
	Super(nullptr),
	m_LastAllocationDepth(0)
{
}





void MassifParser::parse(QIODevice & a_Device)
{
	m_CurrentLine = 1;
	m_ShouldContinueParsing = true;
	while (m_ShouldContinueParsing)
	{
		char buf[3000];
		auto lineLen = a_Device.readLine(buf, sizeof(buf));
		if (lineLen < 0)
		{
			break;
		}
		processLine(buf, static_cast<int>(lineLen));
		m_CurrentLine += 1;
	}

	// End any snapshot that was parsed up until now, without a terminating line:
	endCurrentSnapshot();
}





void MassifParser::abortParsing(void)
{
	m_ShouldContinueParsing = false;
}





void MassifParser::processLine(char * a_Line, int a_LineLen)
{
	// Text constants used for comparisons:
	static const char strMemHeapB[] = "mem_heap_B=";
	static const char strMemHeapExtraB[] = "mem_heap_extra_B=";
	static const char strTime[] = "time=";
	static const char strTimeUnit[] = "time_unit: ";
	static const char strCmd[] = "cmd: ";

	// If the line is too short, bail out early:
	if (a_LineLen < 2)
	{
		return;
	}

	// Terminate the line before the '\n':
	a_Line[a_LineLen - 1] = 0;

	// Process the line; take the first guess based on the start letter:
	switch (a_Line[0])
	{
		case '#':
		{
			// End any snapshot that we were parsing up until now:
			endCurrentSnapshot();
			break;
		}  // case '#'

		case 'm':
		{
			if (strncmp(a_Line, strMemHeapB, sizeof(strMemHeapB) - 1) == 0)
			{
				createNewSnapshotIfNeeded();
				quint64 heapSize;
				if (!parseInteger(a_Line + sizeof(strMemHeapB) - 1, heapSize))
				{
					emit parseError(m_CurrentLine, "Bad number as heap size", a_Line);
					break;
				}
				m_CurrentSnapshot->setHeapSize(heapSize);
			}
			else if (strncmp(a_Line, strMemHeapExtraB, sizeof(strMemHeapExtraB) - 1) == 0)
			{
				createNewSnapshotIfNeeded();
				quint64 heapExtraSize;
				if (!parseInteger(a_Line + sizeof(strMemHeapExtraB) - 1, heapExtraSize))
				{
					emit parseError(m_CurrentLine, "Bad number as heap extra size", a_Line);
					break;
				}
				m_CurrentSnapshot->setHeapExtraSize(heapExtraSize);
			}
			break;
		}  // case 'm'

		case 't':
		{
			if (strncmp(a_Line, strTimeUnit, sizeof(strTimeUnit) - 1) == 0)
			{
				// Report the time units up, abort parsing if they don't agree:
				emit parsedTimeUnit(a_Line + sizeof(strTimeUnit) - 1);
				break;
			}
			if (strncmp(a_Line, strTime, sizeof(strTime) - 1) == 0)
			{
				createNewSnapshotIfNeeded();
				quint64 timestamp;
				if (!parseInteger(a_Line + sizeof(strTime) - 1, timestamp))
				{
					emit parseError(m_CurrentLine, "Bad number as snapshot time", a_Line);
					break;
				}
				m_CurrentSnapshot->setTimestamp(timestamp);
				break;
			}
			break;
		}  // case 't'

		case 'c':
		{
			if (strncmp(a_Line, strCmd, sizeof(strCmd) - 1) == 0)
			{
				// Report the command used for generating the report:
				emit parsedCommand(a_Line + sizeof(strCmd));
			}
			break;
		}  // case 'c'

		case 'n':
		{
			if ((a_Line[1] < '0') || (a_Line[1] > '9'))
			{
				break;
			}
			// The first Allocation line. Create the root Allocation and reset allocation stack:
			createNewSnapshotIfNeeded();
			m_LastAllocationDepth = 0;
			m_LastAllocation = std::make_shared<Allocation>();
			parseAllocationDetails(a_Line, a_LineLen);
			m_LastAllocation->setType(Allocation::atRoot);
			m_CurrentSnapshot->setRootAllocation(m_LastAllocation);
			break;
		}
		case ' ':
		{
			if (m_CurrentSnapshot == nullptr)
			{
				parseError(m_CurrentLine, "Data line found without a header in front of it", a_Line);
				break;
			}
			createAllocationFromLine(a_Line, a_LineLen);
			break;
		}
	}  // switch (first letter)
}





void MassifParser::createNewSnapshotIfNeeded(void)
{
	if (m_CurrentSnapshot == nullptr)
	{
		m_CurrentSnapshot = std::make_shared<Snapshot>();
	}
}





void MassifParser::endCurrentSnapshot(void)
{
	if (m_CurrentSnapshot != nullptr)
	{
		// Sort the allocations:
		auto allocation = m_CurrentSnapshot->getRootAllocation();
		if (allocation != nullptr)
		{
			allocation->sortBySize();
		}

		// Notify about the new snapshot:
		emit newSnapshotParsed(m_CurrentSnapshot);

		// Reset everything:
		m_CurrentSnapshot.reset();
		m_LastAllocation.reset();
		m_LastAllocationDepth = 0;
	}
}





void MassifParser::createAllocationFromLine(const char * a_Line, int a_LineLen)
{
	// Check that we already have an allocation present:
	if (m_LastAllocation == nullptr)
	{
		emit parseError(m_CurrentLine, "Child data line without a parent line", a_Line);
		return;
	}

	// Calculate the depth of the new Allocation, by enumerating all the spaces at line start:
	unsigned depth = 0;
	for (int i = 0; i < a_LineLen; i++)
	{
		if (a_Line[i] != ' ')
		{
			depth = static_cast<unsigned>(i);
			break;
		}
	}

	// Create the new allocation as a child of a correct parent
	// Walk up from m_LastAllocation to match the calculated depth:
	if (m_LastAllocationDepth >= depth)
	{
		for (unsigned i = m_LastAllocationDepth; i >= depth; i--)
		{
			m_LastAllocation = m_LastAllocation->getParent();
			assert(m_LastAllocation != nullptr);
		}
	}
	m_LastAllocation = m_LastAllocation->addChild();
	m_LastAllocationDepth = depth;

	// Parse the information on the rest of the line into the Allocation's details:
	parseAllocationDetails(a_Line + depth, a_LineLen - depth);
}





void MassifParser::parseAllocationDetails(const char * a_Line, int a_LineLen)
{
	/* Line examples:
	"n77: 76933037 (heap allocation functions) malloc/new/new[], --alloc-fns, etc."
	"n1: 24903680 0x55C871: cChunk::SetAllData(cSetChunkData&) (Chunk.cpp:313)"
	"n0: 640 in 1 place, below massif's threshold (0.00%)"
	"n1: 122880 0x551202E: ??? (in /usr/lib/x86_64-linux-gnu/libstdc++.so.6.0.21)"
	*/

	// Skip any initial whitespaces:
	int idx = 0;
	while ((idx < a_LineLen) && (a_Line[idx] <= ' '))
	{
		idx += 1;
	}
	if (idx >= a_LineLen)
	{
		return;
	}

	// Check and skip the "n<int>:" signature
	if (a_Line[idx] != 'n')
	{
		return;
	}
	idx += 1;  // Skip the "n"
	while ((idx < a_LineLen) && isdigit(a_Line[idx]))  // Skip the number
	{
		idx += 1;
	}
	if (idx + 1 >= a_LineLen)
	{
		return;
	}
	if (a_Line[idx] != ':')
	{
		return;
	}
	idx += 1;  // Skip the colon
	while ((idx < a_LineLen) && (a_Line[idx] <= ' '))  // Skip any trailing whitespace
	{
		idx += 1;
	}
	if (idx + 1 >= a_LineLen)
	{
		return;
	}

	// Parse the allocation size:
	quint64 allocationSize = 0;
	while ((idx < a_LineLen) && isdigit(a_Line[idx]))
	{
		allocationSize = allocationSize * 10 + (a_Line[idx] - '0');
		idx += 1;
	}
	m_LastAllocation->setAllocationSize(allocationSize);
	if (idx >= a_LineLen)
	{
		return;
	}
	while ((idx < a_LineLen) && (a_Line[idx] <= ' '))  // Skip the trailing whitespace
	{
		idx += 1;
	}
	if (idx >= a_LineLen)
	{
		return;
	}

	// If the next item starts with "in", conside this a "below threshold" entry:
	if (
		(idx + 1 > a_LineLen) &&
		(a_Line[idx] == 'i') &&
		(a_Line[idx] == 'n')
	)
	{
		// No more data to be parsed for this entry
		m_LastAllocation->setType(Allocation::atBelowThreshold);
		return;
	}

	// If the next item starts with "(", consider this the root entry:
	if (
		(idx > a_LineLen) &&
		(a_Line[idx] == '(')
	)
	{
		m_LastAllocation->setType(Allocation::atRoot);
		return;
	}

	// TODO: Parse the address, function name etc.
}




