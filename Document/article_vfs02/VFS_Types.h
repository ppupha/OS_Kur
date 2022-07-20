//****************************************************************************
//**
//**    VFS_TYPES.H
//**    Header - Virtual File System Type Definitions
//**
//**	Project:	VFS
//**	Component:	VFS
//**    Author:		Michael Walter
//**
//**	History:
//**		25.07.2001		Created (Michael Walter)
//****************************************************************************
#ifndef __VFS_VFS_TYPES_H__
#define __VFS_VFS_TYPES_H__

//============================================================================
//    INTERFACE REQUIRED HEADERS
//============================================================================
#include <vector>
#include <string>
#include <cstring>

//============================================================================
//    INTERFACE COMPILER SETTINGS
//============================================================================
#ifdef _MSC_VER
#pragma warning ( disable : 4786 )
#endif

//============================================================================
//    INTERFACE DEFINITIONS / ENUMERATIONS / SIMPLE TYPEDEFS
//============================================================================
// Numeric Types.
typedef bool						VFS_BOOL;
typedef unsigned char				VFS_BYTE;
typedef unsigned short				VFS_WORD;
typedef unsigned long				VFS_DWORD;
typedef long						VFS_LONG;
typedef __int64						VFS_LONGLONG;

// Numeric Macros.
#define VFS_TRUE					true
#define VFS_FALSE					false

// String types.
typedef wchar_t						VFS_CHAR;
typedef std::wstring				VFS_String;

// String Macros.
#define VFS_TEXT( String )			L ## String

//============================================================================
//    INTERFACE CLASS PROTOTYPES / EXTERNAL CLASS REFERENCES
//============================================================================
//============================================================================
//    INTERFACE STRUCTURES / UTILITY CLASSES
//============================================================================
//============================================================================
//    INTERFACE DATA DECLARATIONS
//============================================================================
//============================================================================
//    INTERFACE FUNCTION PROTOTYPES
//============================================================================
//============================================================================
//    INTERFACE OBJECT CLASS DEFINITIONS
//============================================================================
//============================================================================
//    INTERFACE TRAILING HEADERS
//============================================================================

#endif // __VFS_VFS_TYPES_H__
