/**
 * MIT License
 * 
 * Copyright (c) 2022 DragonDreams (info@dragondreams.ch)
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include "denSocket.h"
#include "denSocketUnix.h"
#include "denSocketWindows.h"

namespace denSocketShared{

denSocket::Ref CreateSocket(){
#if defined OS_UNIX || defined OS_BEOS
	return std::make_shared<denSocketUnix>();
#elif defined OS_W32
	return std::make_shared<denSocketWindows>();
#else
	throw std::invalid_argument("Unsupported platform");
#endif
}

denSocketAddress ResolveAddress(const std::string &address){
#if defined OS_UNIX || defined OS_BEOS
	return denSocketUnix::ResolveAddress(address);
#elif defined OS_W32
	return denSocketWindows::ResolveAddress(address);
#else
	throw std::invalid_argument("Unsupported platform");
#endif
}

std::vector<std::string> FindPublicAddresses(){
#if defined OS_UNIX || defined OS_BEOS
	return denSocketUnix::FindPublicAddresses();
#elif defined OS_W32
	return denSocketWindows::FindPublicAddresses();
#else
	throw std::invalid_argument("Unsupported platform");
#endif
}

std::vector<std::string> FindAllAddresses(){
#if defined OS_UNIX || defined OS_BEOS
	return denSocketUnix::FindAllAddresses();
#elif defined OS_W32
	return denSocketWindows::FindAllAddresses();
#else
	throw std::invalid_argument("Unsupported platform");
#endif
}

}
