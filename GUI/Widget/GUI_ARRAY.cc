#include "GUI.h"
#include "GUI_ARRAY.h"
unsigned int GUI_ARRAY_GetNumItems(const GUI_ARRAY* pThis) {
	return pThis->NumItems;
}
int GUI_ARRAY_AddItem(GUI_ARRAY *pThis, const void *pNew, int Len) {
	void * hNewItem = 0;
	if (Len)
		if ((hNewItem = GUI_ALLOC_AllocInit(pNew, Len)) == 0) {
			return 1;
		}
	int NumItems = pThis->NumItems;
	void * *pNewBuffer = (void * *)GUI_ALLOC_Realloc(pThis->haHandle, (NumItems + 1) * sizeof(void *));
	if (pNewBuffer == 0) {
		GUI_ALLOC_Free(hNewItem);
		return 1;
	}
	pNewBuffer[NumItems] = hNewItem;
	pThis->haHandle = pNewBuffer;
	pThis->NumItems++;
	return 0;
}
void GUI_ARRAY_Delete(GUI_ARRAY* pThis) {
	void * *pa = pThis->haHandle;
	if (pa) {
		int i;
		for (i = 0; i < pThis->NumItems; i++) {
			GUI_ALLOC_Free(pa[i]);
			pa[i] = nullptr;
		}
		GUI_ALLOC_Free(pThis->haHandle);
		pThis->haHandle = nullptr;
		pThis->NumItems = 0;                    /* For safety, in case the array is used after it has been deleted */
	}
}
int GUI_ARRAY_SethItem(GUI_ARRAY *pThis, unsigned int Index, void * hItem) {
	if (Index >= (unsigned)pThis->NumItems)
		return 1;
	void * *pa = pThis->haHandle;
	if (!pa)
		return 1;
	GUI_ALLOC_Free(pa[Index]);
	pa[Index] = nullptr;
	return 0;
}
void * GUI_ARRAY_SetItem(GUI_ARRAY *pThis, unsigned int Index, const void *pData, int Len) {
	if (Index >= (unsigned)pThis->NumItems)
		return nullptr;
	void * *pa = pThis->haHandle;
	if (!pa)
		return nullptr;
	pa += Index;
	void * pItem = *pa;
//				if (GUI_ALLOC_GetSize(hItem) != Len) {
//					hItem = 0;
//				}
	if (!pItem) {
		pItem = GUI_ALLOC_AllocZero(Len);
		if (pItem) {
			GUI_ALLOC_Free(*pa);
			*pa = pItem;
		}
	}
	if (pData && pItem)
		GUI__memcpy(pItem, pData, Len);
	return pItem;
}
void *GUI_ARRAY_GetItem(const GUI_ARRAY *pThis, unsigned int Index) {
	if (Index >= (unsigned)pThis->NumItems)
		return nullptr;
	void * *pa = pThis->haHandle;
	return pa ? pa[Index] : nullptr;
}
void *GUI_ARRAY_ResizeItem(GUI_ARRAY *pThis, unsigned int Index, int Len) {
	void *pNew = GUI_ALLOC_AllocZero(Len);
	if (!pNew)
		return nullptr;
	void *pOld = GUI_ARRAY_GetItem(pThis, Index);
	GUI__memcpy(pNew, pOld, Len);
	if (!GUI_ARRAY_SethItem(pThis, Index, pNew))
		return pNew;
	GUI_ALLOC_Free(pNew);
	pNew = nullptr;
	
	return nullptr;
}
void GUI_ARRAY_DeleteItem(GUI_ARRAY *pThis, unsigned int Index) {
	if (Index >= (unsigned)pThis->NumItems)
		return;
	void * *pa = pThis->haHandle;
	if (!pa)
		return;
	GUI_ALLOC_Free(pa[Index]);
	int i, NumItems = --pThis->NumItems;
	for (i = Index; i <= NumItems - 1; i++)
		pa[i] = pa[i + 1];
}
char GUI_ARRAY_InsertBlankItem(GUI_ARRAY *pThis, unsigned int Index) {
	if (Index >= (unsigned)pThis->NumItems) {
		return 0;
	}
	void * *pNewBuffer = (void * *)GUI_ALLOC_AllocZero(sizeof(void *) * (pThis->NumItems + 1));
	if (pNewBuffer == 0) {
		return 0;
	}
	void * *pOldBuffer = pThis->haHandle;
	GUI__memcpy(pNewBuffer, pOldBuffer, Index * sizeof(void *));
	GUI__memcpy(pNewBuffer + (Index + 1), pOldBuffer + Index, (pThis->NumItems - Index) * sizeof(void *));
	GUI_ALLOC_Free(pThis->haHandle);
	pThis->haHandle = pNewBuffer;
	pThis->NumItems++;
	return 1;
}
void * GUI_ARRAY_InsertItem(GUI_ARRAY* pThis, unsigned int Index, int Len) {
	return GUI_ARRAY_InsertBlankItem(pThis, Index) ?
		GUI_ARRAY_SetItem(pThis, Index, 0, Len) : 0;
}
