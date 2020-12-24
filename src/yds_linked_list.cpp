#include "../include/yds_linked_list.h"

ysLink::ysLink() {
    Next = NULL;
    Previous = NULL;
    Data= NULL;
}

ysLink::~ysLink() {
    /* void */
}

ysLinkedListIterator::ysLinkedListIterator(ysLink *start) {
    SetStart(start);
    Reset();
}

ysLinkedListIterator::ysLinkedListIterator() {
    Reset(NULL);
}

ysLinkedListIterator::~ysLinkedListIterator() {
    /* void */
}

ysLinkedList::ysLinkedList() {
    m_head = NULL;
}

ysLinkedList::~ysLinkedList() {
    /* void */
}

void ysLinkedList::AppendLink(ysLink *link) {
    if (!m_head) {
        m_head = link;
        link->Next = link;
        link->Previous = link;
    }
    else {
        m_head->Previous->Next = link;
        m_head->Previous = link;

        link->Previous = m_head->Previous;
        link->Next = m_head;
    }
}

void ysLinkedList::PrependLink(ysLink *link) {
    link->Next = m_head;
    link->Previous = m_head->Previous;

    m_head->Previous->Next = link;
    m_head->Previous = link;

    m_head = link;
}

void ysLinkedList::DeleteLink(ysLink* link) {
    if (link->Previous == link) {
        // Indicates one link in the entire list
        m_head = NULL;
    }
    else {
        link->Previous->Next = link->Next;
        link->Next->Previous = link->Previous;
    }
}

ysLink *ysLinkedList::GetHead() {
    return m_head;
}

ysLink *ysLinkedList::GetTail() {
    return m_head->Previous;
}
