#pragma once
#include "pch.h"

class User
{
private:
	int userID;
public:
	
	int getUserID();
	void setUserID(int id);
	friend bool operator< (const User &left, const User &right);
	User();
	~User();
};

