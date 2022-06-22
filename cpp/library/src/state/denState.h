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
#include "../config.h"
#include "../denLogger.h"
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
	
	/** \brief Free state. */
	virtual ~denState() noexcept;
	
	/** \brief Values. */
	inline const Values &GetValues() const{ return pValues; }
	
	/** \brief Add value. */
	void AddValue(const denValue::Ref &value);
	
	/** \brief Remove value. */
	void RemoveValue(const denValue::Ref &value);
	
	/** \brief State links. */
	inline const StateLinks GetLinks() const{ return pLinks; }
	
	/** \brief Find link. */
	StateLinks::const_iterator FindLink(denStateLink *link) const;
	
	/** \brief Logger or null. */
	inline const denLogger::Ref &GetLogger() const{ return pLogger; }
	
	/** \brief Set logger or null to clear. */
	void SetLogger(const denLogger::Ref &listener);
	
	/** \brief Read only state. */
	inline bool GetReadOnly() const{ return pReadOnly; }
	
	/**
	 * \brief Remote value changed.
	 * 
	 * For use by subclass to react to remote value changes. After this call
	 * denValue::RemoteValueChanged() is called too so you can decide where to subclass.
	 */
	virtual void RemoteValueChanged(denValue &value);
	
private:
	Values pValues;
	StateLinks pLinks;
	bool pReadOnly;
	denLogger::Ref pLogger;
	
	friend denConnection;
	StateLinks::iterator FindLink(denStateLink *link);
	
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
	
	friend denValue;
	
	/** \brief Invalid value in all state links. */
	void InvalidateValueAt(size_t index);
	
	/** \brief Invalid value in all state links. */
	void InvalidateValueAtExcept(size_t index, denStateLink &link);
	
	void ValueChanged(denValue &value);
};
