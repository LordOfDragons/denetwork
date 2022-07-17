// testipv6.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <denServer.h>

int main()
{
	try{
		denServer::Ref server(std::make_shared<denServer>());

		std::cout << "*** any address ***" << std::endl;
		std::vector<std::string> list(server->FindAllAddresses());
		std::vector<std::string>::const_iterator iter;
		for(iter=list.cbegin(); iter!=list.cend(); iter++){
			std::cout << *iter << std::endl;
		}
	
		std::cout << "*** public address ***" << std::endl;
		list = server->FindPublicAddresses();
		for(iter=list.cbegin(); iter!=list.cend(); iter++){
			std::cout << *iter << std::endl;
		}
	
		std::cout << "*** tostring test ***" << std::endl;

		std::cout << server->ResolveAddress(list.front()).ToString() << std::endl;
		std::cout << server->ResolveAddress("0123:4567:89ab:cdef:0123:4567:89ab:cdef").ToString() << std::endl;
		std::cout << server->ResolveAddress("0123:4567:0:0:0123:4567:0:0").ToString() << std::endl;
		std::cout << server->ResolveAddress("0:0:0:cdef:0123:4567:0:0").ToString() << std::endl;
		std::cout << server->ResolveAddress("0123:4567:89ab:cdef:0123:0:0:0").ToString() << std::endl;
		std::cout << server->ResolveAddress("0:0:0:0:0:0:0:1").ToString() << std::endl;
		std::cout << server->ResolveAddress("2:3:4:5:6:7:8:1").ToString() << std::endl;
		std::cout << server->ResolveAddress("dragondreams.ch").ToString() << std::endl;
		std::cout << server->ResolveAddress("localhost").ToString() << std::endl;
	}catch(const std::exception &e){
		std::cout << "EXCEPTION: " << e.what() << std::endl;
		return 1;
	}
	return 0;
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
