#ifndef __LIST_H_
#define __LIST_H_

#include <malloc.h>

#ifdef _USE_VBASE_
#include "types.h"
#endif


template <class T>
class vList {
	private:
		typedef struct vtagLIST {
			vtagLIST	*next;
			vtagLIST	*prev;
			T					data;
		} vListItem, *LPvListItem;

	public:
		vList(bool freeMem=false) {
			first = NULL;
			last  = NULL;
			cache = NULL;
			count = 0;
			freemem = freeMem;
		}

		virtual ~vList() { delAll(); }


	protected:
		//called once an item is successfully added to the list
		virtual void onAddItem(T data) {}

		//called once an item is deleted from the list.
		//NOTE!!  pointer is invalid is freeMem is set to TRUE
		virtual void onDelete(T data) {}

		//this is called when one of the move operations happens,
		//this way you can update anything that depends on list
		//positioning or anything like that
		virtual void onItemMoved(T data) {}

	public:
		int getTotalItems() { return count; }
		BOOL hasItems() { return (count>0); }
		BOOL itemExists(T data) { if (findItem(data)) return TRUE; return FALSE; }

		
		virtual int addToBegining(T data) {
			if (data == NULL) return -1;
			LPvListItem item = createNew(data);
			if (first) {
				first->prev = item;
				item->next = first; }
			if (count < 2) last = item;
			first = item;
			onAddItem(data);
			return count;
		}

		virtual int addToEnd(T data) {
			if (data == NULL) return -1;
			LPvListItem item = createNew(data);
			if (last) {
				last->next = item;
				item->prev = last; }
			if (count < 2) first = item;
			last = item;
			onAddItem(data);
			return count;
		}

		virtual int addBefore(T data, T prev) {
			if (data == NULL) return -1;
			LPvListItem item = findItem(prev);
			if (item == NULL) item = first;

			LPvListItem newitem = createNew(data);
			if (item) {
				if (item->prev) {
					item->prev->next = newitem;
					newitem->prev = item->prev;
				}
				item->prev = newitem;
				newitem->next = item;
				if (item == first) first = newitem;
			}
			cache = newitem;
			onAddItem(data);
			return count;
		}

		virtual int addAfter(T data, T prev) {
			if (data == NULL) return -1;
			LPvListItem item = findItem(prev);
			if (item == NULL) item = last;

			LPvListItem newitem = createNew(data);
			if (item) {
				if (item->next) {
					item->next->prev = newitem;
					newitem->next = item->next;
				}
				item->next = newitem;
				newitem->prev = item;
				if (item == last) last = newitem;
			}
			cache = newitem;
			onAddItem(data);
			return count;
		}

		virtual int delFirst() { deleteOld(first); return count; }
		virtual int delLast()  { deleteOld(last);  return count; }
		
		virtual int delItem(T data) {
			if (first == NULL) return -1;
			LPvListItem item = first;
			while (1) {
				if (item->data == data) { deleteOld(item); break; }
				if (item->next == NULL) break;
				item = item->next; }
			return count;
		}
		
		virtual int delAll() {
			if (first == NULL) return -1;
			LPvListItem item = first;
			while (1) {
				LPvListItem next = item->next;
				deleteOld(item);
				if (next == NULL) break;
				item = next; }
			first = NULL;
			last = NULL;
			cache = NULL;
			count = 0;
			return count;
		}


		virtual T getFirst() { if (first) return first->data; return NULL; }
		virtual T getLast() { if (last)  return last->data; return NULL; }
		virtual T getCached() { if (cache) return cache->data; return NULL; } 
		
		virtual T getNext(T data) {
			LPvListItem item = findItem(data);
			if (item == NULL) return NULL;
			if (item->next == NULL) return NULL;
			cache = item->next;
			return cache->data;
		}

		virtual T getPrev(T data) {
			LPvListItem item = findItem(data);
			if (item == NULL)	return NULL;
			if (item->prev == NULL)	return NULL;
			cache = item->prev;
			return cache->data;
		}


		virtual BOOL moveToBegining(T data) {
			LPvListItem itm = findItem(data);
			if (itm == NULL) return FALSE;
			if (itm->prev) itm->prev->next = itm->next;
			if (itm->next) itm->next->prev = itm->prev;
			if (first) first->prev = itm;
			itm->prev = NULL;
			itm->next = first;
			first = itm;
			onItemMoved(data);
			return TRUE;
		}

		virtual BOOL moveToEnd(T data) {
			LPvListItem itm = findItem(data);
			if (itm == NULL) return FALSE;
			if (itm->prev) itm->prev->next = itm->next;
			if (itm->next) itm->next->prev = itm->prev;
			if (last) last->next = itm;
			itm->next = NULL;
			itm->prev = last;
			last = itm;
			onItemMoved(data);
			return TRUE;
		}

		virtual BOOL moveBefore(T data, T data2) {
			LPvListItem itm = findItem(data);
			LPvListItem itm2 = findItem(data2);
			if ( (itm == NULL) | (itm2 == NULL) )return FALSE;
			if (itm->prev) itm->prev->next = itm->next;
			if (itm->next) itm->next->prev = itm->prev;
			itm->prev = itm2->prev;
			itm->next = itm2;
			itm2->prev = itm;
			onItemMoved(data);
			return TRUE;
		}

		virtual BOOL moveAfter(T data, T data2) {
			LPvListItem itm = findItem(data);
			LPvListItem itm2 = findItem(data2);
			if ( (itm == NULL) | (itm2 == NULL) )return FALSE;
			if (itm->prev) itm->prev->next = itm->next;
			if (itm->next) itm->next->prev = itm->prev;
			itm->next = itm2->next;
			itm->prev = itm2;
			itm2->next = itm;
			onItemMoved(data);
			return TRUE;
		}

		
	protected:
		LPvListItem createNew(T data) {
			LPvListItem item = (LPvListItem) malloc(sizeof(vtagLIST));
			vZeroMemory(item, sizeof(vtagLIST));
			item->data = data;
			count++;
			return item;
		}

		void deleteOld(LPvListItem item) {
			if (item == NULL) return;
			count--;
			if (count == 0) {
				first = NULL;
				last = NULL;
				cache = NULL;
			} else {
				if (item == cache) cache = NULL;
				if (item->next) item->next->prev = item->prev;
				if (item->prev) item->prev->next = item->next;
				if (item == first) first = item->next;
				if (item == last) last = item->prev;
			}
			
			if (freemem) free(item->data);
			onDelete(item->data);
			free(item);
		}

		LPvListItem findItem(T data) {
			if (first == NULL) return NULL;
			if (cache) if (cache->data == data) return cache;
			LPvListItem item = first;
			while (1) {
				if (item->data == data) {
					cache = item;
					return item; }
				if (item->next == NULL) break;
				item = item->next; }
			return NULL;
		}

	protected:
		LPvListItem first;
		LPvListItem last;
		LPvListItem cache;
		int count;
		bool freemem;
};


#endif