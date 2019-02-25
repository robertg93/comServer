#pragma once

class Message
{
private:
	int type;
	int from;
	int destination;
	std::string data;
	int numOfBytes;

public:
	Message();
	void setnumOfByes(int num);
	~Message();
};

