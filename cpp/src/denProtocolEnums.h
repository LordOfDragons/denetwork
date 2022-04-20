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

#pragma once

namespace denProtocol{
	/**
	* \brief Protocol command codes
	*/
	enum class CommandCodes{
		/**
		*  Connection Request:
		*  [ 0 ] [ protocols ]
		*  
		*  protocols:  // list of protocols supported by client
		*     [ count:uint16 ] [ protocol:uint16 ]{ 1..n }
		*/
		connectionRequest = 0,
		
		/**
		* Connection Ack:
		* [ 1 ] [ resultCode:uint8 ]
		* 
		* resultCode:
		*    0: Connection Accepted
		*    1: Connection Rejected
		*    2: Connection Rejected because no common protocols
		* 
		* if connection is accepted the message also contains:
		*    [ protocol:uint16 ]
		* 
		* protocol:
		*    The chosen protocol
		*/
		connectionAck = 1,
		
		/**
		* Close Connection:
		* [ 2 ]
		*/
		connectionClose = 2,
		
		/**
		* Message:
		* [ 3 ] [ data ]
		*/
		message = 3,
		
		/**
		* Reliable message:
		* [ 4 ] [ number:uint16 ] [ data ]
		*/
		reliableMessage = 4,
		
		/**
		* Link state:
		* [ 5 ] [ number:uint16 ] [ link_id:uint16 ] [ flags ] [ message ] [ values ]
		* 
		* flags:
		*    0x1: create read only state
		* 
		* message:
		*    [ length:uint16 ] [ message_bytes:uint8 ]{ 1..n }
		* 
		* values:
		*    [ value_count:uint16 ] ( [ value_type:uint8 ] [ value_data:* ] ){ 1..n }
		*/
		reliableLinkState = 5,
		
		/**
		* Reliable ack:
		* [ 6 ] [ number:uint16 ] [ code:uint8 ]
		* 
		* code:
		*    [ 0 ] received successfully
		*    [ 1 ] failed
		*/
		reliableAck = 6,
		
		/**
		* Link up:
		* [ 7 ] [ link_id:uint16 ]
		*/
		linkUp = 7,
		
		/**
		* Link down:
		* [ 8 ] [ link_id:uint16 ]
		*/
		linkDown = 8,
		
		/**
		* Link update: 
		* [ 9 ] [ link_count:uint8 ] [ link ]{ 1..link_count }
		* 
		* link:
		*    [ link_id:uint16 ] [ value_count:uint8 ] [ value ]{ 1..value_count }
		* 
		* value:
		*    [ value_index:uint16 ] [ value_data:* ]
		*/
		linkUpdate = 9
	};

	/**
	* \brief Acknowledge connection codes.
	*/
	enum class ConnectionAck{
		accepted = 0,
		rejected = 1,
		noCommonProtocol = 2
	};

	/**
	* \brief Reliable acknowledge codes.
	*/
	enum class ReliableAck{
		success = 0,
		failed = 1
	};

	/**
	* \brief State value types.
	*/
	enum class ValueTypes{
		sint8 = 0, //<! integer: signed char ( 8-bit )
		uint8 = 1, //<! integer: unsigned char ( 8-bit )
		sint16 = 2, //<! integer: signed short ( 16-bit )
		uint16 = 3, //<! integer: unsigned short ( 16-bit )
		sint32 = 4, //<! integer: signed long ( 32-bit )
		uint32 = 5, //<! integer: unsigned long ( 32-bit )
		sint64 = 6, //<! integer: signed long ( 64-bit )
		uint64 = 7, //<! integer: unsigned long ( 64-bit )
		float16 = 8, //<! float: half float ( 16-bit )
		float32 = 9, //<! float: float ( 32-bit )
		float64 = 10, //<! float: float ( 32-bit )
		string = 11, //<! string zero terminated
		data = 12, //<! data: length unsigned 8-bit
		point2S8 = 13, //<! point2: signed 8-bit per component
		point2U8 = 14, //<! point2: unsigned 8-bit per component
		point2S16 = 15, //<! point2: signed 16-bit per component
		point2U16 = 16, //<! point2: unsigned 16-bit per component
		point2S32 = 17, //<! point2: signed 32-bit per component
		point2U32 = 18, //<! point2: unsigned 32-bit per component
		point2S64 = 19, //<! point2: signed 64-bit per component
		point2U64 = 20, //<! point2: unsigned 64-bit per component
		point3S8 = 21, //<! point3: signed 8-bit per component
		point3U8 = 22, //<! point3: unsigned 8-bit per component
		point3S16 = 23, //<! point3: signed 16-bit per component
		point3U16 = 24, //<! point3: unsigned 16-bit per component
		point3S32 = 25, //<! point3: signed 32-bit per component
		point3U32 = 26, //<! point3: unsigned 32-bit per component
		point3S64 = 27, //<! point3: signed 64-bit per component
		point3U64 = 28, //<! point3: unsigned 64-bit per component
		vector2F16 = 29, //<! vector2: 16-bit per component
		vector2F32 = 30, //<! vector2: 32-bit per component
		vector2F64 = 31, //<! vector2: 64-bit per component
		vector3F16 = 32, //<! vector3: 16-bit per component
		vector3F32 = 33, //<! vector3: 32-bit per component
		vector3F64 = 34, //<! vector3: 64-bit per component
		quaternionF16 = 35, //<! quaternion: 16-bit per component
		quaternionF32 = 36, //<! quaternion: 32-bit per component
		quaternionF64 = 37 //<! quaternion: 64-bit per component
	};
	
	/**
	 * \brief Supported protocols.
	 */
	enum class Protocols{
		DENetworkProtocol = 0 //<! Drag[en]gine Network Protocol: Version 1
	};
}
