#include "pch.h"
#include "CCommunicator.h"


using namespace std;



int main()
{

	std::unique_ptr<CCommunicator> ptr(CCommunicator::createObject());
	 
	ptr->run();
;


}
