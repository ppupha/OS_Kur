//****************************************************************************
//**
//**	VFS.H
//**	Header - Virtual File System
//**
//**	Project:	VFS
//**	Component:	VFS
//**	Author:		Michael Walter
//**
//**	History:
//**		18.06.2001		Created (Michael Walter)
//****************************************************************************
#ifndef __VFS_H__
#define __VFS_H__

//============================================================================
//    INTERFACE REQUIRED HEADERS
//============================================================================
#include "VFS_Types.h"

//============================================================================
//    INTERFACE DEFINITIONS / ENUMERATIONS / SIMPLE TYPEDEFS
//============================================================================
// The Handle Type.
enum VFS_Handle
{ VFS_HANDLE_FORCE_DWORD = 0xFFFFFFFF };

// The current Version.
const VFS_DWORD		VFS_VERSION					= 0x0100;
const VFS_CHAR		VFS_PATH_SEPARATOR			= VFS_TEXT( '/' );
const VFS_CHAR*		VFS_ARCHIVE_FILE_EXTENSION	= VFS_TEXT( "PAK" );
const VFS_Handle	VFS_INVALID_HANDLE			= ( VFS_Handle ) 0;
const VFS_DWORD		VFS_ERROR					= 0xFFFFFFFF;

// The File_Open/Create() Flags.
enum VFS_OpenFlags
{
	VFS_READ	= 0x0001,
	VFS_WRITE	= 0x0002
};

// The File_Seek() Origin.
enum VFS_SeekOrigin
{
	VFS_SET,
	VFS_CURRENT,
	VFS_END
};

// The Error Constants.
enum VFS_ErrorCode
{
	VFS_ERROR_NONE,
	VFS_ERROR_NOT_INITIALIZED_YET,
	VFS_ERROR_ALREADY_INITIALIZED,
	VFS_ERROR_ALREADY_EXISTS,
	VFS_ERROR_NOT_FOUND,
	VFS_ERROR_INVALID_PARAMETER,
	VFS_ERROR_GENERIC,
	VFS_ERROR_READ_ONLY,
	VFS_NUM_ERRORS
};

// The Type of an Entity
enum VFS_EntityType
{
	VFS_FILE,
	VFS_DIRECTORY,
	VFS_ARCHIVE_FILE
};

// The Filter Reader and Writer Procedures.
typedef VFS_BOOL ( *VFS_FilterReadProc )( VFS_BYTE* pBuffer, VFS_DWORD dwBytesToRead, VFS_DWORD* pBytesRead = NULL );
typedef VFS_BOOL ( *VFS_FilterWriteProc )( VFS_BYTE* pBuffer, VFS_DWORD dwBytesToWrite, VFS_DWORD* pBytesWritten = NULL );

// An Iteration Procedure.
typedef VFS_BOOL ( *VFS_DirIterationProc )( struct VFS_EntityInfo* pInfo );

// A List of Filter Names.
typedef std::vector< class VFS_Filter* > VFS_FilterList;
typedef std::vector< VFS_String > VFS_FilterNameList;
typedef std::vector< VFS_String > VFS_RootPathList;
typedef std::vector< struct VFS_EntityInfo > VFS_EntityInfoList;
typedef std::vector< VFS_String > VFS_FileNameList;

//============================================================================
//    INTERFACE COMPONENT HEADERS
//============================================================================
//============================================================================
//    INTERFACE CLASS PROTOTYPES / EXTERNAL CLASS REFERENCES
//============================================================================
//============================================================================
//    INTERFACE STRUCTURES / UTILITY CLASSES
//============================================================================
// A Filter for the VFS.
class VFS_Filter
{
public:
	// Constructor.
	VFS_Filter( const VFS_String& strName, const VFS_String& strDescription )
		: m_strName( strName ), m_strDescription( strDescription )
	{}

	// The Filter's Name.
	VFS_String m_strName;

	// The Filter's Description.
	VFS_String m_strDescription;

	// Encoding / Decoding Procedures.
	virtual VFS_BOOL Encode( VFS_FilterReadProc Reader, VFS_FilterWriteProc Writer );
	virtual VFS_BOOL Decode( VFS_FilterReadProc Reader, VFS_FilterWriteProc Writer );

	// Filter State Management.
	virtual VFS_BOOL LoadData( VFS_FilterReadProc Reader );
	virtual VFS_BOOL SaveData( VFS_FilterWriteProc Writer );
};

// Information about a VFS Entity.
struct VFS_EntityInfo
{
	// The Entity Type.
	VFS_EntityType eType;

	// Is the Entity archived (Archive files are NEVER archived)?
	VFS_BOOL bArchived;

	// The complete Path and the Name.
	VFS_String strPath;
	VFS_String strName;

	// The Size ( 0 for Directories ).
	VFS_LONGLONG llSize;
};

// A tree entry of an entity information tree.
struct VFS_ContentTreeEntry
{
	// A pointer to the parent.
	VFS_ContentTreeEntry*	pParent;

	// A pointer to the previous sibling.
	VFS_ContentTreeEntry*	pPrev;

	// A pointer to the next sibling.
	VFS_ContentTreeEntry*	pNext;

	// A pointer to the first child entry ( NULL for files ).
	VFS_ContentTreeEntry*	pChild;

	// The entity information record.
	VFS_EntityInfo Info;
};

//============================================================================
//    INTERFACE DATA DECLARATIONS
//============================================================================
//============================================================================
//    INTERFACE FUNCTION PROTOTYPES
//============================================================================
///////////////////////////////////////////////////////////////////////////////
// Basic VFS Interface.
///////////////////////////////////////////////////////////////////////////////
// Initialize / Shutdown the VFS.
VFS_BOOL VFS_Init();
VFS_BOOL VFS_Shutdown();
VFS_BOOL VFS_IsInit();

// Register / Unregister a Filter.
VFS_BOOL VFS_RegisterFilter( VFS_Filter* pFilter );
VFS_BOOL VFS_UnregisterFilter( VFS_Filter* pFilter );
VFS_BOOL VFS_UnregisterFilter( VFS_DWORD dwIndex );
VFS_BOOL VFS_UnregisterFilter( const VFS_String& strFilterName );
VFS_BOOL VFS_ExistsFilter( const VFS_String& strFilterName );
const VFS_Filter* VFS_GetFilter( const VFS_String& strFilterName );
VFS_DWORD VFS_GetNumFilters();
const VFS_Filter* VFS_GetFilter( VFS_DWORD dwIndex );
VFS_BOOL VFS_GetFilters( VFS_FilterList& Filters );
VFS_BOOL VFS_GetFilterNames( VFS_FilterNameList& FilterNames );

// Root Path Handling.
VFS_BOOL VFS_AddRootPath( const VFS_String& strRootPath );
VFS_BOOL VFS_RemoveRootPath( const VFS_String& strRootPath );
VFS_BOOL VFS_RemoveRootPath( VFS_DWORD dwIndex );
VFS_DWORD VFS_GetNumRootPaths();
VFS_BOOL VFS_GetRootPath( VFS_DWORD dwIndex, VFS_String& strRootPath );
VFS_BOOL VFS_GetRootPaths( VFS_RootPathList& RootPaths );

// Flush the VFS (close all unused Archives etc).
VFS_BOOL VFS_Flush();

// Information.
VFS_BOOL VFS_ExistsEntity( const VFS_String& strPath );
VFS_BOOL VFS_GetEntityInfo( const VFS_String& strPath, VFS_EntityInfo& Info );
VFS_WORD VFS_GetVersion();

// Error Handling.
VFS_ErrorCode VFS_GetLastError();
VFS_BOOL VFS_GetErrorString( VFS_ErrorCode eError, VFS_String& Error );

///////////////////////////////////////////////////////////////////////////////
// The File Interface (the file interface will try to create a file in each root path. If no root path has been added, the current directory will be used instead).
///////////////////////////////////////////////////////////////////////////////
// Create / Open / Close a File.
VFS_Handle VFS_File_Create( const VFS_String& strFile, VFS_DWORD dwFlags );
VFS_Handle VFS_File_Open( const VFS_String& strFile, VFS_DWORD dwFlags );
VFS_BOOL VFS_File_Close( VFS_Handle hHandle );

// Read / Write from / to the File.
VFS_BOOL VFS_File_Read( VFS_Handle hHandle, VFS_BYTE* pBuffer, VFS_DWORD dwToRead, VFS_DWORD* pRead = NULL );
VFS_BOOL VFS_File_Write( VFS_Handle hHandle, const VFS_BYTE* pBuffer, VFS_DWORD dwToWrite, VFS_DWORD* pWritten = NULL );

// Direct File Reading / Writing.
VFS_BOOL VFS_File_ReadEntireFile( const VFS_String& strFile, VFS_BYTE* pByte );
VFS_BOOL VFS_File_WriteEntireFile( const VFS_String& strFile, const VFS_BYTE* pBuffer, VFS_DWORD dwToWrite, VFS_DWORD* dwWritten  );

// Positioning.
VFS_BOOL VFS_File_Seek( VFS_Handle hHandle, VFS_LONGLONG dwPos, VFS_SeekOrigin eOrigin = VFS_SET );
VFS_LONGLONG VFS_File_Tell( VFS_Handle hHandle );

// Sizing.
VFS_BOOL VFS_File_Resize( VFS_Handle hHandle, VFS_LONGLONG dwSize );
VFS_LONGLONG VFS_File_GetSize( VFS_Handle hHandle );

// Information.
VFS_BOOL VFS_File_Exists( const VFS_String& strFile );
VFS_BOOL VFS_File_GetInfo( const VFS_String& strFile, VFS_EntityInfo& Info );
VFS_BOOL VFS_File_GetInfo( VFS_Handle hHandle, VFS_EntityInfo& Info );

// File Management.
VFS_BOOL VFS_File_Delete( const VFS_String& strFile );
VFS_BOOL VFS_File_Copy( const VFS_String& strFrom, const VFS_String& strTo );
VFS_BOOL VFS_File_Move( const VFS_String& strFrom, const VFS_String& strTo );
VFS_BOOL VFS_File_Rename( const VFS_String& strFrom, const VFS_String& strTo );				// pszTo has to be a single File Name without a Path.

// Set the Filters used by a File (file must exist be an Archive).
VFS_BOOL VFS_File_SetUsedFilters( const VFS_String& strFile, const VFS_FilterNameList& Filters );
VFS_BOOL VFS_File_GetUsedFilters( const VFS_String& strFile, VFS_FilterNameList& Filters );

///////////////////////////////////////////////////////////////////////////////
// The Archive Interface (Never provide a extension for the archive, instead, change the VFS_ARCHIVE_EXTENSION definition and recompile; I removed some functions because they seemed to become to slow, such as VFS_AddFile() and so. You can only create archives in the first root path).
///////////////////////////////////////////////////////////////////////////////
// Create an Archive.
VFS_BOOL VFS_Archive_CreateFromDirectory( const VFS_String& strArchive, const VFS_String& strDirectory, const VFS_FilterNameList& DefaultFilters = VFS_FilterNameList(), VFS_BOOL bRecursive = VFS_TRUE );
VFS_BOOL VFS_Archive_CreateFromFileList( const VFS_String& strArchive, const VFS_FileNameList& Files, const VFS_FilterNameList& DefaultFilters = VFS_FilterNameList() );

// Set the default Filters for an Archive
VFS_BOOL VFS_Archive_SetDefaultFilters( const VFS_String& strArchive, const VFS_FilterNameList& Filters );
VFS_BOOL VFS_Archive_GetDefaultFilters( const VFS_String& strArchive, VFS_FilterNameList& Filters );

// Update the Filter List for an Archive.
VFS_BOOL VFS_Archive_UpdateFilterList( const VFS_String& strArchive );

// Extract an Archive.
VFS_BOOL VFS_Archive_Extract( const VFS_String& strArchive, const VFS_String& strTargetDir );

// Information.
VFS_BOOL VFS_Archive_Exists( const VFS_String& strArchive );
VFS_BOOL VFS_Archive_GetInfo( const VFS_String& strDir, VFS_EntityInfo& Info );

// Archive Management.
VFS_BOOL VFS_Archive_Delete( const VFS_String& strArchive );

// Flush the Archive System.
VFS_BOOL VFS_Archive_Flush();

///////////////////////////////////////////////////////////////////////////////
// The Directory Interface (You can only create/delete standard directories in the first root path).
///////////////////////////////////////////////////////////////////////////////
// Directory Management.
VFS_BOOL VFS_Dir_Create( const VFS_String& strDir, VFS_BOOL bRecursive = VFS_FALSE );	// Recursive mode would create a directory c:\alpha\beta even if alpha doesn't exist.
VFS_BOOL VFS_Dir_Delete( const VFS_String& strDir, VFS_BOOL bRecursive = VFS_FALSE );	// Recursive mode would delete a directory c:\alpha even if it contains files and/or subdirectories.

// Information.
VFS_BOOL VFS_Dir_Exists( const VFS_String& strDir );
VFS_BOOL VFS_Dir_GetInfo( const VFS_String& strDir, VFS_EntityInfo& Info );

// Iterate a Directory and call the iteration procedure for each 
VFS_BOOL VFS_Dir_Iterate( const VFS_String& strDir, VFS_DirIterationProc pIterationProc, VFS_BOOL bRecursive = VFS_FALSE );

// Get the Contents of a Directory.
VFS_BOOL VFS_Dir_GetContents( const VFS_String& strDir, VFS_EntityInfoList& EntityInfoList, VFS_BOOL bRecursive = VFS_FALSE );

// Set the default Filters for a Directory (which must reside in an Archive).
VFS_BOOL VFS_Dir_SetDefaultFilters( const VFS_String& strDir, const VFS_FilterNameList& Filters );
VFS_BOOL VFS_Dir_GetDefaultFilters( const VFS_String& strDir, VFS_FilterNameList& Filters );

// Get the Contents of a Directory as a tree structure.
VFS_BOOL VFS_Dir_GetContentTree( const VFS_String& strDir, VFS_ContentTreeEntry*& First, VFS_BOOL bRecursive = VFS_FALSE );
VFS_BOOL VFS_Dir_FreeContentTree( VFS_ContentTreeEntry* pFirst );

///////////////////////////////////////////////////////////////////////////////
// The Utility Interface
///////////////////////////////////////////////////////////////////////////////
// File Name Management Functions.
VFS_BOOL VFS_Util_GetPath( const VFS_String& strFileName, VFS_String& pszPath );
VFS_BOOL VFS_Util_GetName( const VFS_String& strFileName, VFS_String& pszName );
VFS_BOOL VFS_Util_GetBaseName( const VFS_String& strFileName, VFS_String& pszBaseName );
VFS_BOOL VFS_Util_GetExtension( const VFS_String& strFileName, VFS_String& pszExtension );

//============================================================================
//    INTERFACE OBJECT CLASS DEFINITIONS
//============================================================================
//============================================================================
//    INTERFACE TRAILING HEADERS
//============================================================================

#endif // __VFS_H__