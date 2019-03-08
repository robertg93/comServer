#include "pch.h"
#include "Message.h"




int Message::receive(int newDscp)
{
	char buf[256];
	int nbytes = recv(newDscp, buf, 256, 0);
	if (nbytes > 0)
	{
		std::string temp = "";
		temp = temp + buf[0];
		this->massageType = stoi(temp);  //set message type
		this->receivedBytes = nbytes; // set number of received bytes
		temp = "";
		for (int k = 1; k < 7; k++) { temp = temp + buf[k]; }
		this->senderID = stoi(temp); // set senderID
		temp = "";
		if (this->massageType == 2)
		{
			for (int k = 7; k < 13; k++) { temp = temp + buf[k]; }
			this->receiverID = stoi(temp);
			temp = "";
			for (int k = 13; k < nbytes; k++) { temp = temp + buf[k]; }
			this->data = temp;
		}
		if (this->massageType == 1)
		{
			temp = "";
			for (int k = 7; k < nbytes; k++) { temp = temp + buf[k]; }
			this->data = temp;
		}
	}
	else
	{
		this->receivedBytes = nbytes;
	}
	return this->receivedBytes;



}

int Message::sendMsg(int userDscp)
{
	//prepare msg for all
	if (this->massageType == 1)
	{
		std::string temp = std::to_string(this->massageType) + std::to_string(this->senderID) + this->data;
		this->data = temp;
	}
	//prepare msg for certain user
	if (this->massageType == 2)
	{
		std::string temp = std::to_string(this->massageType) + std::to_string(this->senderID) + this->data;
		this->data = temp;
	}
	int sendBytes = this->data.size();
	char * msg = new char[sendBytes];
	for (int n=0; n < sendBytes; n++) {	msg[n] = this->data[n]; }
	int result = send(userDscp, msg, sendBytes , 0);
	std::cout << result << std::endl;
	delete[] msg;
	return result;
}

Message::Message()
{
}


Message::~Message()
{
}
