// massifparser.cpp

// Implements the MassifParser class representing the parser for Massif-generated datafiles

#include "massifparser.h"
#include <QIODevice>
#include "parseinteger.h"
#include "snapshot.h"
#include "allocation.h"





MassifParser::MassifParser(void):
	Super(nullptr)
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
		processLine(buf, lineLen);
		m_CurrentLine += 1;
	}

	// End any snapshot that was parsed up until now, without a terminating line:
	if (m_CurrentSnapshot != nullptr)
	{
		emit newSnapshotParsed(m_CurrentSnapshot);
		m_CurrentSnapshot.reset();
	}

	m_LastAllocation.reset();
	m_CurrentSnapshot.reset();
}





void MassifParser::abortParsing(void)
{
	m_ShouldContinueParsing = false;
}





void MassifParser::processLine(char * a_Line, qint64 a_LineLen)
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
			if (m_CurrentSnapshot != nullptr)
			{
				emit newSnapshotParsed(m_CurrentSnapshot);
				m_CurrentSnapshot.reset();
			}
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
			// TODO
			m_LastAllocation = std::make_shared<Allocation>();
			m_CurrentSnapshot->setRootAllocation(m_LastAllocation);

			break;
		}
		case ' ':
		{
			createNewSnapshotIfNeeded();
			// TODO: processAllocationLine(a_Line);
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




