#include "pch.h"
#include "CCommunicator.h"


using namespace std;



int main()
{

	CCommunicator* ptr = CCommunicator::run();

	ptr->startConnection();
	ptr->connectionHandling();

	delete CCommunicator::run();



}
