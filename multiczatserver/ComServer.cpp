#include "pch.h"
#include "CCommunicator.h"


using namespace std;



int main()
{

	CCommunicator* ptr = CCommunicator::createObject();

	ptr->run();
;

	delete CCommunicator::createObject();



}
