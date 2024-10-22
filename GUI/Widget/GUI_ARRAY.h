#pragma once
#include <stdint.h>

typedef enum {
	GUI_ARRAY_STATE_NOT_CREATED,
	GUI_ARRAY_STATE_CREATED,
	GUI_ARRAY_STATE_DELETED
} GUI_ARRAY_STATE;
typedef struct {
	uint16_t NumItems;
	void * *haHandle; /* Handle to buffer holding handles */
} GUI_ARRAY;
int      GUI_ARRAY_AddItem(GUI_ARRAY *pThis, const void *pNew, int Len);
void     GUI_ARRAY_Delete(GUI_ARRAY *pThis);
unsigned GUI_ARRAY_GetNumItems(const GUI_ARRAY *pThis);
void    *GUI_ARRAY_GetItem(const GUI_ARRAY *pThis, unsigned int Index);
int      GUI_ARRAY_SethItem(GUI_ARRAY *pThis, unsigned int Index, void * hItem);
void * GUI_ARRAY_SetItem(GUI_ARRAY *pThis, unsigned int Index, const void *pData, int Len);
void     GUI_ARRAY_DeleteItem(GUI_ARRAY *pThis, unsigned int Index);
char     GUI_ARRAY_InsertBlankItem(GUI_ARRAY *pThis, unsigned int Index);
void * GUI_ARRAY_InsertItem(GUI_ARRAY *pThis, unsigned int Index, int Len);
void    *GUI_ARRAY_ResizeItem(GUI_ARRAY *pThis, unsigned int Index, int Len);
