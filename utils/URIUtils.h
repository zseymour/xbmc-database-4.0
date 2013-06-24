/*
 * URIUtils.h
 *
 *  Created on: Jun 24, 2013
 *      Author: zachary
 */

#ifndef URIUTILS_H_
#define URIUTILS_H_

#include "StdString.h"

class URIUtils {
public:
	URIUtils(void);
	virtual ~URIUtils(void);
	static CStdString GetParentFolderURI(const CStdString& uri, bool preserveFileNameInPath);
	static bool IsInPath(const CStdString &uri, const CStdString &baseURI);
	static CStdString GetParentPath(const CStdString& strPath);
	static bool GetParentPath(const CStdString& strPath, CStdString& strParent);
	static bool ProtocolHasParentInHostname(const CStdString& prot);
	static bool ProtocolHasEncodedFilename(const CStdString& prot);
	static bool ProtocolHasEncodedHostname(const CStdString& prot);

	static void GetDirectory(const CStdString& strFilePath,
			CStdString& strDirectoryPath);
	static CStdString GetDirectory(const CStdString &filePath);
	static const CStdString GetFileName(const CStdString& strFileNameAndPath);

	static CStdString GetExtension(const CStdString& strFileName);

	static bool IsDOSPath(const CStdString &path);
	static bool IsURL(const CStdString& strFile);
	static bool IsStack(const CStdString& strFile);

	static void AddSlashAtEnd(CStdString& strFolder);
	static bool HasSlashAtEnd(const CStdString& strFile, bool checkURL = false);
	static void RemoveSlashAtEnd(CStdString& strFolder);
	static bool CompareWithoutSlashAtEnd(const CStdString& strPath1, const CStdString& strPath2);
	static CStdString AddFileToFolder(const CStdString& strFolder,
			const CStdString& strFile);
};

#endif /* URIUTILS_H_ */
