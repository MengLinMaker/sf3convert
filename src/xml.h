//=============================================================================
//  MuseScore sftools
//
//  Copyright (C) 2004-2011 Werner Schweer
//
//  This work is free software; you can redistribute it and/or
//  modify it under the terms of the GNU Library General Public
//  License as published by the Free Software Foundation; either
//  version 2 of the License, or (at your option) any later version.
//
//  See LICENCE for the licence text and disclaimer of warranty.
//=============================================================================

#pragma once
#include <QtCore/Qt>
#include <QtWidgets/QtWidgets>
#include <QtCore/QTextStream>
#include <QtCore/QString>
#include <QtXml/QDomElement>

//---------------------------------------------------------
//   Xml
//---------------------------------------------------------

class Xml : public QTextStream
{
	static const int BS = 2048;

	QList<QString> stack;
	void putLevel();

public:
	Xml(QIODevice *dev);
	Xml();

	void header();

	void stag(const QString &);
	void etag();

	void tagE(const QString &);
	void tagE(const char *format, ...);
	void ntag(const char *name);
	void netag(const char *name);

	void tag(const QString &, QVariant data);
	void tag(const char *name, const char *s) { tag(name, QVariant(s)); }
	void tag(const char *name, const QString &s) { tag(name, QVariant(s)); }
	void tag(const char *name, const QWidget *);

	void writeHtml(const QString &s);
	void dump(int len, const unsigned char *p);

	static QString xmlString(const QString &);
	static void htmlToString(const QDomElement &, int level, QString *);
	static QString htmlToString(const QDomElement &);
};

extern QString docName;
extern void domError(const QDomElement &);
extern void domNotImplemented(const QDomElement &);
