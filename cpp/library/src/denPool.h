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
#include "config.h"

template<class T> class denPool;

/**
 * \brief Pool item.
 */
template<class T> class denPoolItem{
public:
	typedef std::shared_ptr<denPoolItem<T>> Ref;
	
	denPoolItem(denPool<T> &pool, const std::shared_ptr<T> &item) : pPool(pool), pRealItem(item){
	}
	
	~denPoolItem() noexcept{
		pPool.Return(pRealItem);
	}
	
	inline T& Item() const{ return *pRealItem; }
	
private:
	denPool<T> &pPool;
	std::shared_ptr<T> pRealItem;
};

/**
 * \brief Pool.
 */
template<class T> class denPool{
public:
	/** \brief Create pool. */
	denPool() = default;
	
	/** \brief Get item from pool or create a new one if empty. */
	typename denPoolItem<T>::Ref Get(){
		if(pItems.empty()){
			return std::make_shared<denPoolItem<T>>(*this, std::make_shared<T>());
		}else{
			typename std::vector<std::shared_ptr<T>>::iterator iter(pItems.end() - 1);
			const typename denPoolItem<T>::Ref item(std::make_shared<denPoolItem<T>>(*this, *iter));
			pItems.erase(iter);
			return item;
		}
	}
	
	/** \brief Clear pool. */
	void Clear(){
		pItems.clear();
	}
	
private:
	friend class denPoolItem<T>;
	
	/** \brief Return item to pool. */
	void Return(const std::shared_ptr<T> &realItem){
		pItems.push_back(realItem);
	}
	
	std::vector<std::shared_ptr<T>> pItems;
};
