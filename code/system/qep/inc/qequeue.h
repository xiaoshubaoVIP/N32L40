/*************************************************************************
* Product:  QF/C
* Last Updated for Version: 3.1.03
* Date of the Last Update:  Nov 18, 2005
*
* Copyright (C) 2002-2005 Quantum Leaps, LLC. All rights reserved.
*
* This software may be distributed and modified under the terms of the GNU
* General Public License version 2 (GPL) as published by the Free Software
* Foundation and appearing in the file GPL.TXT included in the packaging of
* this file. Please note that GPL Section 2[b] requires that all works based
* on this software must also be made publicly available under the terms of
* the GPL ("Copyleft").
*
* Alternatively, this software may be distributed and modified under the
* terms of Quantum Leaps commercial licenses, which expressly allow the
* licensees to retain the proprietary status of their code. The licensees
* who use this software under one of Quantum Leaps commercial licenses do
* not use this software under the GPL and therefore are not subject to any
* of its terms.
*
* Contact information:
* Quantum Leaps Web site:  http://www.quantum-leaps.com
* Quantum Leaps licensing: http://www.quantum-leaps.com/licensing
* Quantum Leaps products:  http://www.quantum-leaps.com/products
* e-mail:                  sales@quantum-leaps.com
************************************************************************/
#ifndef qequeue_h
#define qequeue_h

/** \ingroup qf
* \file qequeue.h
* \brief platform-independent event queue interface.
*
* This header file must be included in all QF ports that use native QF
* event queue implementation. Also, this file is needed when the "raw"
* thread-safe queues are used for communication between active objects
* and non-framework entities, such as ISRs, device drivers, or legacy
* code.
*/

#ifndef QF_EQUEUE_CTR_SIZE

    /** \brief The size (in bytes) of the ring-buffer counters used in the
    * native QF event queue implementation. Valid values: 1, 2, or 4;
    * default 1.
    *
    * This macro can be defined in the QF port file (qf_port.h) to
    * configure the ::QEQueueCtr type. Here the macro is not defined so the
    * default of 1 byte is chosen.
    */
    #define QF_EQUEUE_CTR_SIZE 1
#endif
#if (QF_EQUEUE_CTR_SIZE == 1)

    /** \brief The data type to store the ring-buffer counters based on
    * the macro #QF_EQUEUE_CTR_SIZE.
    *
    * The dynamic range of this data type determines the maximum length
    * of the ring buffer managed by the native QF event queue.
    */
    typedef uint8_t QEQueueCtr;
#elif (QF_EQUEUE_CTR_SIZE == 2)
    typedef uint16_t QEQueueCtr;
#elif (QF_EQUEUE_CTR_SIZE == 4)
    typedef uint32_t QEQueueCtr;
#else
    #error "QF_EQUEUE_CTR_SIZE defined incorrectly, expected 1, 2, or 4"
#endif

/****************************************************************************/
/** \brief a name for QEQueueTag struct
*
* This structure describes the native QF event queue, which can be used as
* the event queue for active objects, or as a simple "raw" event queue for
* thread-safe event passing among non-framework entities, such as ISRs,
* device drivers, or other third-party components.
*
* The native QF event queue is configured by defining the macro
* #QF_EQUEUE_TYPE as ::QEQueue in the specific QF port header file.
*
* The ::QEQueue structure contains only data members for managing an event
* queue, but does not contain the storage for the queue buffer, which must
* be provided externally during the queue initialization.
*
* The event queue can store only event pointers, not the whole events. The
* internal implementation uses the standard ring-buffer plus one external
* location that optimizes the queue operation for the most frequent case
* of empty queue.
*
* The ::QEQueue structure is used with two sets of functions. One set is for
* the active object event queue, which needs to block the active object
* task when the event queue is empty and unblock it when events are posted
* to the queue. The interface for the native active object event queue
* consists of the following functions: QActive_postFIFO__(),
* QActive_postLIFO__(), and QActive_get__(). Additionally the function
* QEQueue_init() is used to initialize the queue.
*
* The other set of functions, uses this structure as a simple "raw" event
* queue to pass events between entities other than active objects, such as
* ISRs. The "raw" event queue is not capable of blocking on the get()
* operation, but is still thread-safe because it uses QF critical section
* to protect its integrity. The interface for the "raw" thread-safe queue
* consists of the following functions: QEQueue_postFIFO(),
* QEQueue_postLIFO(), and QEQueue_get(). Additionally the function
* QEQueue_init() is used to initialize the queue.
*
* \note Most event queue operations (both the active object queues and
* the "raw" queues) internally use  the QF critical section. You should be
* careful not to invoke those operations from other critical sections when
* nesting of critical sections is not supported.
*
* \sa ::QEQueueTag for the description of the data members
*/
typedef struct QEQueueTag QEQueue;
/** \brief Native QF Event Queue
* \sa ::QEQueue
*/
struct QEQueueTag {
    /** \brief pointer to event at the front of the queue
    *
    * All incoming and outgoing events pass through the frontEvt__ location.
    * When the queue is empty (which is most of the time), the extra
    * frontEvt__ location allows to bypass the ring buffer altogether,
    * greatly optimizing the performance of the queue. Only bursts of events
    * engage the ring buffer.
    *
    * The additional role of this attribute is to indicate the empty status
    * of the queue. The queue is empty if the frontEvt__ location is NULL.
    */
    QEvent const *frontEvt__;

    /** \brief pointer to the start of the ring buffer
    */
    QEvent const **ring__;

    /** \brief offset of the end of the ring buffer from the start of the
    * buffer ring__
    */
    QEQueueCtr end__;

    /** \brief offset to where next event will be inserted into the buffer
    */
    QEQueueCtr head__;

    /** \brief offset of where next event will be extracted from the buffer
    */
    QEQueueCtr tail__;

    /** \brief total number of entries in the ring buffer
    */
    QEQueueCtr nTot__;

    /** \brief number of events currently present in the ring buffer
    */
    QEQueueCtr nUsed__;

    /** \brief maximum number of events ever present in the ring buffer.
    *
    * \note this attribute remembers the high-watermark of the ring buffer,
    * which provides a valuable information for sizing event queues.
    * \sa QF_getQueueMargin().
    */
    // QEQueueCtr nMax__;
};

/* public methods */

/** \brief Initializes the native QF event queue
*
* The parameters are as follows: \a me points to the ::QEvent struct to be
* initialized, \a qSto[] is the ring buffer storage, \a qLen is the length
* of the ring buffer in the units of event-pointers.
*
* \note The actual capacity of the queue is qLen + 1, because of the extra
* location fornEvt__.
*/
void QEQueue_init(QEQueue *me, QEvent const *qSto[], QEQueueCtr qLen);

/** \brief "raw" thread-safe QF event queue implementation for the
* First-In-First-Out (FIFO) event posting. You can call this function from
* any task context or ISR context. Please note that this function uses
* internally a critical section.
*
* \note The function raises an assertion if the native QF queue becomes
* full and cannot accept the event.
*
* \sa QEQueue_postLIFO(), QEQueue_get()
*/
void QEQueue_postFIFO(QEQueue *me, QEvent const *e);

/** \brief "raw" thread-safe QF event queue implementation for the
* Last-In-First-Out (LIFO) event posting.
*
* \note The LIFO policy should be used only with great caution because it
* alters order of events in the queue.
* \note The function raises an assertion if the native QF queue becomes
* full and cannot accept the event. You can call this function from
* any task context or ISR context. Please note that this function uses
* internally a critical section.
*
* \sa QEQueue_postFIFO(), QEQueue_get()
*/
void QEQueue_postLIFO(QEQueue *me, QEvent const *e);

/** \brief "raw" thread-safe QF event queue implementation for the
* the get operation.
*
* If the queue has one or more events, the function returns the event
* at the front of the queue, otherwise it returns NULL. You can call this
* function from any task context or ISR context. Please note that this
* function uses internally a critical section.
*
* \sa QEQueue_postFIFO(), QEQueue_postLIFO()
*/
QEvent const *QEQueue_get(QEQueue *me);

/* friend class QF */
/* friend class QActive */

#endif                                                         /* qequeue_h */

