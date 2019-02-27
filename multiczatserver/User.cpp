#include "pch.h"
#include "User.h"


int User::getUserID()
{
	return userID;
}

void User::setUserID(int id)
{
	userID = id;
}

User::User()
{
}


User::~User()
{
}

bool operator<(const User & left, const User & right)
{
	return left.userID < right.userID;
}
