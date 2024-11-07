#pragma once

#include "ConfReaderLine.h"

class ConfReader
{
	//const std::string ext = ".conf";

	bool verbose = false;

	ofFile file; // file
	string uid; // file name -ext
	string dataPath; // path to file +ext

	vector<ConfReaderLine*> lines; // content of file, line by line

	void setup(string path, string ext)
	{
		dataPath = ofToDataPath(path + "." + ext);

		file = ofFile(dataPath);

		if (!file.exists())
		{
			cout << "ERROR : missing file:  " << dataPath << endl;
			return;
		}

		read();
	}

public:


	/// <summary>
	/// without extension
	/// </summary>
	ConfReader(string filepath)
	{
		setup(filepath, "conf");
	}

	/// <summary>
	/// without .
	/// </summary>
	ConfReader(string filepath, string extension)
	{
		setup(filepath, extension);
	}

	bool isFile(string uid)
	{
		return this->uid == uid;
	}

	void set(string puid, int column, string value)
	{
		auto line = getset(puid);
		line->setField(column, value);
	}

	/// <summary>
	/// deprecated
	/// </summary>
	/*
	void set(int line, int column, string value)
	{
		if (lines.size() <= line)
		{
			cout << "oob:reader" << endl;

			while (lines.size() <= line)
			{
				lines.push_back(new ConfReaderLine(""));
			}
		}


		//cout << "set:" << column << ":" << value << endl;
		//cout << "line:" << line << " x" << lines.size() << endl;

		lines[line]->setField(column, value);
	}
	*/

	ConfReaderLine* get(int lineIndex)
	{
		if (lines.size() <= lineIndex)
		{
			while (lines.size() <= lineIndex)
			{
				lines.push_back(new ConfReaderLine(""));
			}
		}

		return lines[lineIndex];
	}

	/// <summary>
	/// check is field is present and active ( = 1 )
	/// </summary>
	bool isFieldActive(string field)
	{
		auto value = get(field);

		if (value == nullptr)
		{
			return false;
		}

		return ofToFloat(value->getValue(0)) > 0;
	}

	ConfReaderLine* get(string uid)
	{
		for (auto l : lines)
		{
			if (l->isProperty(uid))
			{
				return l;
			}
		}

		cout << dataPath << " has no #" << uid << endl;

		return nullptr;
	}

	ConfReaderLine* add(string uid)
	{
		auto ret = get(uid);

		if (ret != nullptr)
			return ret;

		ret = new ConfReaderLine(uid);
		lines.push_back(ret);

		return ret;
	}

	/// <summary>
	/// will fetch and create if null
	/// </summary>
	ConfReaderLine* getset(string uid)
	{
		return add(uid);
	}

	/// <summary>
	/// returns ALL lines with property
	/// </summary>
	vector<ConfReaderLine*> filter(string fieldValue)
	{
		vector<ConfReaderLine*> ret;

		for (auto l : lines)
		{
			if (l->isProperty(fieldValue))
			{
				ret.push_back(l);
			}
		}

		return ret;
	}

	void read()
	{
		if (verbose)	cout << "reader:read @" << dataPath << endl;

		file.open(dataPath, ofFile::ReadOnly, false);
		ofBuffer raw = file.readToBuffer();

		if (verbose) cout << "reader:	buffer raw size : " << raw.size() << endl;

		lines.clear();

		for (auto line : raw.getLines())
		{
			// empties
			if (line.length() <= 0) continue;

			// skip comments
			if (line[0] == '#') continue;

			//cout << "	+line : " << line << endl;

			ConfReaderLine* blob = new ConfReaderLine(line);
			lines.push_back(blob);
		}

		if (verbose) cout << "reader:	solved line(s) x" << lines.size() << endl;

		file.close();
	}

	void save()
	{
		file.open(dataPath, ofFile::WriteOnly, false);

		for (auto l : lines)
		{
			string str = l->stringify();

			// skip empties
			if (str.length() <= 0) continue;

			file << str << endl;

			//file < l->stringify();
		}

		if (verbose)	cout << "conf:save @ " << dataPath << endl;

		file.close();
	}

};