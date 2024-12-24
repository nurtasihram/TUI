#pragma once

#include <stdint.h>
#include <memory>
#include <memory.h>

#include "GUI.h"

template<class AnyType>
class GUI_Array {
	AnyType *pElems = nullptr;
	uint16_t nItems = 0;
public:
	GUI_Array() {}
	GUI_Array(uint16_t nItems) : pElems((AnyType *)GUI_MEM_AllocZero(nItems * sizeof(AnyType))), nItems(nItems) {}
	~GUI_Array() { Delete(); }
public:
	void Delete() {
		GUI_MEM_Free(pElems);
		pElems = nullptr;
		nItems = 0;
	}
	void Destruct() {
		if (pElems) {
			for (auto i = 0; i < nItems; ++i)
				pElems[i].~AnyType();
			GUI_MEM_Free(pElems);
			pElems = nullptr;
		}
		nItems = 0;
	}
	void Resize(uint16_t num) {
		if (pElems)
			pElems = (AnyType *)GUI_MEM_Realloc(pElems, num * sizeof(AnyType));
		else
			pElems = (AnyType *)GUI_MEM_Alloc(num * sizeof(AnyType));
		nItems = num;
	}
	void Delete(uint16_t Index) {
		if (Index >= nItems)
			return;
		if (!pElems)
			return;
		pElems->~AnyType();
		memmove(pElems + Index, pElems + Index - 1, sizeof(AnyType));
		pElems = (AnyType *)GUI_MEM_Realloc(pElems, --nItems * sizeof(AnyType));
	}
	void Add(const AnyType &i) {
		pElems = (AnyType *)GUI_MEM_Realloc(pElems, ++nItems * sizeof(AnyType));
		GUI__memcpy(pElems + nItems - 1, &i, sizeof(AnyType));
	}
	void Insert(const AnyType &i, unsigned Index) {
		if (Index >= nItems) Index = nItems - 1;
		pElems = (AnyType *)GUI_MEM_Realloc(pElems, ++nItems * sizeof(AnyType));
		if (auto size = (nItems - Index) * sizeof(AnyType))
			memmove(pElems + Index + 1, pElems + Index, size);
		GUI__memcpy(pElems + Index, &i, sizeof(AnyType));
	}
	inline auto begin() { return pElems; }
	inline auto begin() const { return pElems; }
	inline auto end() const { return pElems + nItems; }
public: // Property - NumItems
	/* R */ inline uint16_t NumItems() const { return nItems; }
public:
	inline AnyType &operator[](int Index) { return pElems[Index]; }
	inline const AnyType &operator[](int Index) const { return pElems[Index]; }
};
