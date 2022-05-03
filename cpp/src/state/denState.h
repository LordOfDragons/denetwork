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

#include <memory>
#include <vector>
#include <list>
#include "denStateLink.h"
#include "denStateListener.h"
#include "../config.h"
#include "../value/denValue.h"

class denMessageReader;
class denMessageWriter;

/**
 * \brief Network state.
 */
class denState{
public:
	/** \brief Shared pointer. */
	typedef std::shared_ptr<denState> Ref;
	
	/** \brief Value list. */
	typedef std::vector<denValue::Ref> Values;
	
	/** \brief State link list. */
	typedef std::list<denStateLink::Ref> StateLinks;
	
	/** \brief Create state. */
	denState(bool readOnly);
	
	/** \brief Clean up state. */
	virtual ~denState();
	
	/** \brief User data. */
	inline void *GetUserData() const{ return pUserData; }
	
	/** \brief Set user data. */
	void SetUserData(void *userData);
	
	/** \brief Values. */
	inline Values &GetValues(){ return pValues; }
	inline const Values &GetValues() const{ return pValues; }
	
	/** \brief State links. */
	inline StateLinks GetLinks(){ return pLinks; }
	inline const StateLinks GetLinks() const{ return pLinks; }
	
	/** \brief Find link. */
	StateLinks::iterator FindLink(denStateLink *link);
	StateLinks::const_iterator FindLink(denStateLink *link) const;
	
	/** \brief Set listener or null to clear. */
	void SetListener(const denStateListener::Ref &listener);
	
	/** \brief Read only state. */
	inline bool GetReadOnly() const{ return pReadOnly; }
	
	/** \brief Update state. */
	void Update();
	
	/** \brief Read values from message. */
	void LinkReadValues(denMessageReader &reader, denStateLink &link);
	
	/** \brief Read all values from message. */
	void LinkReadAllValues(denMessageReader &reader, denStateLink &link);
	
	/**
	 * \brief Read all values from message including types.
	 * 
	 * Verifies that the values in the state match in type and update their values.
	 * Returns true if the state matches and has been updated or false otherwise.
	 */
	bool LinkReadAndVerifyAllValues(denMessageReader &reader);
	
	/**
	 * \brief Write all values to message.
	 * \param[in] withTypes Include types.
	 */
	void LinkWriteValues(denMessageWriter &writer);
	
	/**
	 * \brief Write all values to message.
	 * \param[in] withTypes Include types.
	 */
	void LinkWriteValuesWithVerify(denMessageWriter &writer);
	
	/**
	 * \brief Write values to message if changed in link.
	 * \param[in] withTypes Include types. Value only if \em allValues is true.
	 * \param[in] allValues Ignore changed flags and send all values.
	 */
	void LinkWriteValues(denMessageWriter &writer, denStateLink &link);
	
	/** \brief Invalid value in all state links. */
	void InvalidateValue(int index);
	
	/** \brief Invalid value in all state links. */
	void InvalidateValueExcept(int index, denStateLink &link);
	
	
private:
	void *pUserData;
	Values pValues;
	StateLinks pLinks;
	bool pReadOnly;
	denStateListener::Ref pListener;
};
