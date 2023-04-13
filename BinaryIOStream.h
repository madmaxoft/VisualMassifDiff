// BinaryIOStream.h

// Declares the BinaryIOStream class representing a QIODevice wrapper for reading and writing binary data in a platform-independent way





#ifndef BINARYIOSTREAM_H
#define BINARYIOSTREAM_H





#include <stdexcept>
#include <QIODevice>





class BinaryIOStreamException:
	public std::runtime_error
{
	typedef std::runtime_error Super;

public:
	BinaryIOStreamException(const char * a_Message):
		Super(a_Message)
	{
	}
};





class BinaryIOStream
{
public:
	BinaryIOStream(QIODevice & a_Device);

	// The write functions throw a BinaryIOStreamException on an error
	void writeBool(bool a_Value);
	void writeInt32(qint32 a_Value);
	void writeInt64(qint64 a_Value);
	void writeUInt32(quint32 a_Value);
	void writeUInt64(quint64 a_Value);
	void writeString(const char * a_Value);
	void writeString(const char * a_Value, size_t a_Length);
	void writeString(const std::string & a_Value);
	void writeString(const QString & a_Value);

	/** Writes a static constant block of data, represented as a char array.
	Throws BinaryIOStreamException on an error. */
	template <qint64 N> void writeConst(const char (&a_Value)[N])
	{
		auto bytesWritten = m_IODevice.write(&a_Value[0], N);
		if (bytesWritten == N)
		{
			return;
		}
		throw BinaryIOStreamException("Failed to write const");
	}


	// The read functions throw a BinaryIOStreamException on an error
	bool        readBool();
	qint32      readInt32();
	qint64      readInt64();
	quint32     readUInt32();
	quint64     readUInt64();
	std::string readString();
	QString     readQString();

protected:

	/** The QIODevice instance that provides the underlying IO operations. */
	QIODevice & m_IODevice;

	BinaryIOStream(const BinaryIOStream &) = delete;
};

#endif // BINARYIOSTREAM_H
