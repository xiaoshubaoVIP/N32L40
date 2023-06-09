/****************************************************************************/
/*	File   :             F:\Smoker\WiFiSmoke\trunk\src\include\qep\qep.h               */
/*	Author :                       songjiang                                */
/*	Version:                          V1.0                                  */
/*	Date   :                    2019/5/16,16:11:28                         */
/********************************Modify History******************************/
/*   yy/mm/dd     F. Lastname    Description of Modification                */
/****************************************************************************/
/*	Attention:                                                              */
/*	CopyRight:                CopyRight 2017--2025                          */
/*           Shenzhen Xiaojun Technology Co., Ltd.  Allright Reserved        */
/*	Project :                      SC01                           */
/****************************************************************************/

#ifndef qep_h
#define qep_h

/** \ingroup qep qf qk
* \file qep.h
* \brief Public QEP/C interface.
*
* This header file must be included in all modules (*.c files) that use QEP/C
*/

#include <stdint.h>                        /* C99 exact-width integer types */
#ifndef Q_ROM

/** \brief Macro to force allocating objects in ROM
*
* The Q_ROM macro is used inside the QP source code to denote these constant
* objects that can be allocated in ROM. On CPUs with the Harvard architecture
* (e.g., the 8051 or AVR), the code and data spaces are separate and are
* accessed through different CPU instructions. The compilers often provide
* specific extended keywords to designate code or data space, such as the
* __code extended keyword in the IAR 8051 compiler. To use QP with a
* different MCU/compiler, you need to check how to allocate constants to ROM.
* If you don not define the Q_ROM macro, it will default to noting.
*/
#define Q_ROM

#endif

/****************************************************************************/
/** \brief get the current QEP version number string
*
* \return version of the QEP as a constant 6-character string of the form
* x.y.zz, where x is a 1-digit major version number, y is a 1-digit minor
* version number, and zz is a 2-digit release number.
*/
char const Q_ROM *QEP_getVersion(void);

/****************************************************************************/
/** helper macro to calculate static dimension of a 1-dim array \a array_ */
#define Q_DIM(array_) (sizeof(array_) / sizeof(array_[0]))

/****************************************************************************/
#ifndef QEP_SIGNAL_SIZE

    /** \brief The size (in bytes) of the signal of an event. Valid values:
    * 1, 2, or 4; default 1
    *
    * This macro can be defined in the QEP port file (qep_port.h) to
    * configure the ::QSignal type. Here the macro is not defined so the
    * default of 1 byte is chosen.
    */
    #define QEP_SIGNAL_SIZE 1
#endif
#if (QEP_SIGNAL_SIZE == 1)
/** \brief QSignal represents the signal of an event.
*
* The relationship between an event and a signal is as follows. A signal in
* UML is the specification of an asynchronous stimulus that triggers reactions
* [<A HREF="http://www.omg.org/docs/ptc/03-08-02.pdf">UML document
* ptc/03-08-02</A>], and as such is an essential part of an event.
* (The signal conveys the type of the occurrence-what happened?) However, an
* event can also contain additional quantitative information about the
* occurrence in form of event parameters. Please refer to the document
* <A HREF="http://www.quantum-leaps.com/devzone/Recipe_IntroHSM.pdf">
* Brief Introduction to UML State Machines</A>) for more information about
* state machine concepts.
*/
    typedef uint8_t QSignal;
#elif (QEP_SIGNAL_SIZE == 2)
    typedef uint16_t QSignal;
#elif (QEP_SIGNAL_SIZE == 4)
    typedef uint32_t QSignal;
#else
    #error "QEP_SIGNAL_SIZE defined incorrectly, expected 1, 2, or 4"
#endif

/****************************************************************************/
/** \brief a name for QEventTag struct.
*
* QEvent represents events without parameters and serves as the base structure
* for derivation of events with parameters.
*
* The following example illustrates how to add an event parameter by
* derivation of the QEvent structure. Please note that the QEvent member
* super_ is defined as the FIRST member of the derived struct.
* \include qep_qevent.c
*
* \sa \ref derivation
*/
typedef struct QEventTag QEvent;
/** \brief Event structure
* \sa QEvent
*/
struct QEventTag {
    QSignal sig;                          /**< signal of the event instance */
    // uint8_t attrQF__;                 /**< private attribute used  */
};

/****************************************************************************/
/** \brief QEP reserved signals */
enum QReservedSignals {
    Q_ENTRY_SIG = 1,                   /**< signal for coding entry actions */
    Q_EXIT_SIG,                         /**< signal for coding exit actions */
    Q_INIT_SIG,                  /**< signal for coding initial transitions */
    Q_USER_SIG      /**< first signal that can be used in user applications */
};

/****************************************************************************/
/** \brief a name for QFsmTag struct
*
* QFsm represents a traditional non-hierarchical Finite State Machine (FSM)
* without state hierarchy, but with entry/exit actions.
*
* QFsm is also a base structure for derivation of the ::QHsm structure.
*
* \note QFsm is not intended to be instantiated directly, but rather serves
* as the base structure for derivation of state machines in the application
* code.
*
* The following example illustrates how to derive a state machine structure
* from QFsm. Please note that the QFsm member super_ is defined as the FIRST
* member of the derived struct.
* \include qep_qfsm.c
*
* \sa \ref derivation
*/
typedef struct QFsmTag QFsm;

                         /** \brief the signature of state handler function*/
typedef void (*QState)(QFsm *, QEvent const *);

  /** \brief a name for the return type from the HSM state handler function */
typedef QState QSTATE;

        /** transition type for QFsmTag::tran__ memer (NO transition taken) */
#define Q_TRAN_NONE_TYPE ((uint8_t)0)
   /** transition type for QFsmTag::tran__ memer (DYNAMIC transition taken) */
#define Q_TRAN_DYN_TYPE  ((uint8_t)1)
    /** transition type for QFsmTag::tran__ memer (STATIC transition taken) */
#define Q_TRAN_STA_TYPE  ((uint8_t)2)

/****************************************************************************/
/** \brief Finite State Machine
* \sa QFsm
*/
struct QFsmTag {
    /** \brief Union that represents current state of a state machine,
    * or state transition target.
    */
    union QStateTag {
        QState fsm;                         /**< non-hierarchical FSM state */
       
    } state__;                                    /**< current active state */
    uint8_t tran__;        /**< transition attributes (none/dynamic/static) */
};

/** \brief Performs the second step of FSM initialization by triggering the
* top-most initial transition.
*
* \param me pointer the state machine structure derived from QFsm
* \param e constant pointer the QEvent or a structure derived from QEvent
* \note Must be called only ONCE after the "constructor" QFsm_ctor_().
*
* The following example illustrates how to initialize a FSM, and dispatch
* events to it:
* \include qep_qfsm_use.c
*/
void QFsm_init(QFsm *me, QEvent const *e);

/** \brief Dispatches an event to a FSM
*
* Processes one event at a time in Run-to-Completion fashion. The argument
* \a me is the pointer the state machine structure derived from QFsm.
* The argument \a e is a constant pointer the QEvent or a structure
* derived from QEvent.
*
* \note Must be called after the "constructor" QFsm_ctor_() and QFsm_init().
*
* \sa example for QFsm_init() \n \ref derivation
*/
void QFsm_dispatch(QFsm *me, QEvent const *e);

/* protected methods */

/** \brief Protected "constructor" of a FSM.
*
* Performs the first step of FSM initialization by assigning the
* initial pseudostate to the currently active state of the state machine.
* \note Must be called only by the "constructors" of the derived state
* machines.
* \note Must be called only ONCE before QFsm_init().
*
* The following example illustrates how to invoke QFsm_ctor_() in the
* "constructor" of a derived state machine:
* \include qep_qfsm_ctor.c
*/
#define QFsm_ctor_(me_, initial_) do { \
    (me_)->state__.fsm = (initial_); \
    (me_)->tran__ = Q_TRAN_NONE_TYPE; \
} while (0)


/** \brief Returns current active state of a FSM.
*
* \note this is a protected function to be used only inside state handler
* functions.
*/
#define QFsm_getState_(me_) ((QState const)(me_)->super_.state__.fsm)

/** \brief Designates a target for an initial transition.
*
* Q_INIT() can be used both in the initial pseudostate handler functions
* and in the nested initial transitions triggered by ::Q_INIT_SIG. The
* following example demonstrates these two uses of Q_INIT():
*
* \include qep_qinit.c
*
* \note Q_INIT() should be only used for initial transitions and never
* be confused with macro Q_TRAN(), which is to be used exclusively for
* the regular state transitions.
*/
#define Q_INIT(target_) (((QFsm *)me)->state__.fsm = (QState)(target_))

/** \brief Designates a target for an regular transition.
*
* Q_TRAN() can be used both in the FSMs and HSMs:
*
* \include qep_qtran.c
*
* \note Q_TRAN() should be only used for regular transitions and never
* be confused with macro Q_INIT(), which is to be used exclusively for
* the initial transitions.
*
* \sa Q_TRAN_STA()
*/
#define Q_TRAN(target_) do { \
    ((QFsm *)me)->tran__ = Q_TRAN_DYN_TYPE; \
    ((QFsm *)me)->state__.fsm = (QState)(target_); \
} while (0)

/**
\page qep_rev QEP/C Revision History

\section qep_3_2_01 Version 3.2.01 (Product)
Release date: Sep 01, 2006\n

-# In file qep.c updated version number to 3.2.01
-# Added makefiles for building ports of all QP/C libraries at once.
-# Created the consolidated manual "QP/C Programmer's Manual".


\section qep_3_2_00 Version 3.2.00 (Product)
Release date: Aug 07, 2006\n

-# In file qep.h added the default definition of Q_ROM in case it's not
defined by the user.
-# In file qassert.h added the macro Q_ROM to allocate constant strings
to ROM.
-# In file qep.c updated version number to 3.2.00
-# Updated the "QEP/C Programmer's Manual".


\section qep_3_1_02 Version 3.1.02 (Product)
Release date: Feb 08, 2006\n

-# In file qep.c added the Revision History Doxygen comment, which was
previously in doxygen/qp.h
-# updated the QBomb example to use the key events described in the
"QEP/C Programmer's Manual"
-# changed C++ comments to C-comments in main.c of the QHsmTst example


\section qep_3_1_01 Version 3.1.01 (Product)
Release date: Oct 18, 2005\n

-# Removed <A HREF="http://www.quantum-leaps.com/products/qs.htm">
Quantum Spy</A> (QS) dependency from the examples


\section qep_3_1_00 Version 3.1.00 (Beta3)
Release date: Oct 03, 2005\n

-# Applied new directory structure desribed in
<A HREF="http://www.quantum-leaps.com/doc/AN_QP_Directory_Structure.pdf">
Application Note: QP Directory Structure</A>
-# Added <A HREF="http://www.quantum-leaps.com/products/qs.htm">
Quantum Spy</A> instrumentation.
-# Removed file qfsm_tra.c.
-# Introduced file qfsm_dis.c.


\section qep_3_0_11 Version 3.0.11 (Beta2)
Release date: Aug 14, 2005\n

-# Fixed potential race condition for static transitions
-# Changed names of helper function QFsm_tran_()/QHsm_tran_() to
QFsm_execTran()/QHsm_execTran() to match the QEP/C++ version.
-# Added assertion in QHsm_execTran() to catch potential path[] array
overrun that previously could go undetected.


\section qep_3_0_10 Version 3.0.10 (Beta1)
Release date: Aug 06, 2005\n

This release contains completely redesigned Quantum Event Processor (QEP). The
main focus is on compliance with standards (MISRA, Lint, Coding Standard),
better portability, stack-use efficiency.

-# This release includes a comprehensive "QEP/C v3.0 Programmer's Manual"
in PDF.
-# This release contains in-source comments for automatic generation of
this Reference Manual with <A HREF="http://www.doxygen.org">doxygen</A>.
-# This release includes re-packaging the code into much larger number of
modules (.c files) with typically one function per module. This
fine-granularity packaging allows for better automatic elimination of unused
code at link time and fine-tuning by applying different compiler options to
different files.
-# This release is 98% compliant with the Motor Industry Software Reliability
Association (MISRA) "Guidelines for the Use of the C Language in Vehicle Based
Software", April 1998, ISBN 0-9524156-9-0 (see also See also
http://www.misra.org.uk). A separate Application Note "QEP/C MISRA Compliance
Matrix" contains detailed account how QEP/C compiles with the 127 MISRA rules
(See http://www.quantum-leaps.com/doc/AN_QEP_C_MISRA.pdf).
-# This release is now "lint-free". The source code has been thoroughly
checked with the latest version of PC-lint(TM) (version 8.00q) from Gimpel
Software (www.gimpel.com). The PC-lint configuration files and output files
are included in the distribution. A separate Application Note "QEP/C PC-Lint
Compliance" contains detailed account how QEP/C compiles with PC-Lint. (See
http://www.quantum-leaps.com/doc/AN_QEP_C_PC-Lint.pdf).
-# This release is compliant with the "Quantum Leaps Coding C/C++ Standard"
(http://www.quantum-leaps.com/resources/QL_coding_standard.pdf). Among others,
all public names are prefixed by "Q_", or "QEP_" to minimize compile-time and
link-time name conflicts. In addition, QEP/C uses the C99 standard include
file <stdint.h> (C99 Standard, Section 7.18). Pre-standard compilers are still
supported by placing <stdint.h> in the compiler's header file directory.
-# The QEvent structure has been redesigned to use memory more efficiently.
QEvent takes up only 2 bytes now.
-# The QHsm structure now derives from the QFsm structure, which promotes
better code re-use. In particular, macros Q_INIT() and Q_TRAN() are now
common for both traditional non-hierarchical FSMs and for HSMs.
-# The QFsm class has been redesigned to allow using entry and exit
actions. Now the QFsm class strictly avoids recursion, just as QHsm does
(MISRA rule 70).
-# The QPseudoState type has been eliminated and replaced by QState.
-# The central QHsm class in QEP has been completely redesigned. The basic
transition algorithm is essentially the same as described in "PSiCC", however
the implementation is very different. The redesign was necessary to comply
with the MISRA rules and to eliminate Lint warnings.
-# The event processor is now strictly non-recursive (MISRA rule 70), that
is, event handlers never call themselves, even indirectly. (Previous algorithm
required one-level of indirect recursion.)
-# Dynamic state transition is now default. Static transition optimization is
still supported, but considered optional for performance tuning.
-# The artificial limitation of initial transitions targeting only immediate
substate of a composite state has been removed (see "PSiCC" Figure 5.4[a]).
Initial transitions can now target any substate, arbitrarily nested within the
composite state.
-# This distribution uses the legacy Borland Turbo C++ 1.01 as the default
compiler. Port to DOS with Turbo C++ 1.01 is provided along with
GNU-compatible makefiles for the QEP library and sample applications. This
compiler is not C99-compliant and does not provide the <stdint.h> header file.
However, the port of QEP includes a minimal <stdint.h>, which should be placed
into the INCLUDE directory of the Borland Turbo C++ 1.01 installation.
-# This distribution contains three sample applications: A simple time
bomb (QBomb) to demonstrate use of QFsm, a calculator (QCalc), and an
exhaustive test of the QHsm class (QHsmTst). All examples are implemented as
text-only application compiled with Turbo C++ 1.01. The QHsmTst sample
application supports interactive and batch mode for exhaustive testing of the
event processor. The source code for QHsmTst demonstrates also how QEP/C
application code can be MISRA-compliant.
-# This distribution contains make files for the Turbo C++ 1.01 make utility,
as well as GNU-compatible Makefiles.
*/

/** \defgroup qep Quantum Event Processor in C (QEP/C)
* \image html logo_qep_TM.jpg
*
* Quantum Event Processor (QEP) is a generic, portable, and reusable state
* machine engine that processes events according to the general semantics of
* UML state machines
* [<A HREF="http://www.omg.org/docs/ptc/03-08-02.pdf">UML
* document ptc/03-08-02</A>]. QEP allows you to map UML state diagrams
* directly and clearly to code without the assistance of complicated design
* automation tools. It allows you to create better designs because you can
* think in terms of events, states, and transitions directly at the level of
* the programming language. The resulting code is smaller, cleaner, more
* robust, and simpler to understand, and review. The testing is faster, more
* rigorous, and more complete because state machine approach cleanly defines
* a limited state space for the system and constraints the possible
* transitions among the states.
*
* QEP provides facilities for executing Hierarchical State Machines (HSMs)
* as well as the simpler traditional Finite State Machines (FSMs). QEP can
* operate with almost any event queuing and dispatching mechanisms, which
* are also necessary components of a hypothetical state machine
* [<A HREF="http://www.omg.org/docs/ptc/03-08-02.pdf">ptc/03-08-02</A>].
* For example, QEP can be used with virtually any event-driven environment,
* such as a Graphical User Interface infrastructure. When used in the domain
* of real-time embedded (RTE) systems, QEP can be combined with other
* <A HREF="http://www.quantum-leaps.com/products">Quantum Platform</A>
* components designed specifically for RTE systems. QEP has been used in
* hundreds of event-driven applications worldwide and has been originally
* described in Part 1 of the book
* <A HREF="http://www.quantum-leaps.com/writings/book.htm">Practical
* Statecharts in C/C++</A> by Miro Samek, CMP Books 2002.
*
* \sa <A HREF="http://www.quantum-leaps.com/doc/QEP_C_Manual.pdf">
*      QEP/C Programmer's Manual</A> \n
*      \ref qep_rev
*/

#endif                                                             /* qep_h */
