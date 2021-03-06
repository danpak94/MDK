#pragma once

#include <memory>

#include <QString>
#include <QFile>
#include <QIODevice>
#include <QByteArray>
#include <QJsonDocument>
#include <QTextStream>

#include "mdkString.h"

namespace mdk
{

class TextFile : public Object
{
private:
	std::unique_ptr<QFile> m_QFile;
	std::unique_ptr<QTextStream> m_QTextStream;

public:
	TextFile(const String& FilePathAndName);
	~TextFile();
	bool Clear();//clear anything in file

	TextFile& operator<<(char InputChar);
	TextFile& operator<<(const char* InputString);
	TextFile& operator<<(const String& InputString);
	
	void Close();

private:
	TextFile() = delete;

public:
	static void Save(const String& InputText, const String& FilePathAndName);
	static bool Load(String& OutputText, const String& FilePathAndName);
};

}//namespace mdk
