/**
 * @file diagnostic_new.h
 * @brief Macro for replacing global operator new with one that invokes diagnostics.
 * 
 */
#pragma once


/**
 * @brief Macro to replace operator new with code that captures call details.
 * 
 * Credit for this technique of "stamping" allocated memory with the caller's
 * details goes to: http://www.almostinfinite.com/memtrack.html, Copyright (c)
 * 2002, 2008 Curtis Bartley.
 * 
 */
#define new allok8or::diagnostic::CallerDetails(__FILE__, __LINE__) * new