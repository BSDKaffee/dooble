/*
** Copyright (c) 2008 - present, Alexis Megas.
** All rights reserved.
**
** Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions
** are met:
** 1. Redistributions of source code must retain the above copyright
**    notice, this list of conditions and the following disclaimer.
** 2. Redistributions in binary form must reproduce the above copyright
**    notice, this list of conditions and the following disclaimer in the
**    documentation and/or other materials provided with the distribution.
** 3. The name of the author may not be used to endorse or promote products
**    derived from Dooble without specific prior written permission.
**
** DOOBLE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
** IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
** OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
** IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
** INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
** NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
** DOOBLE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include <QKeyEvent>

#include "dooble_about.h"
#include "dooble_version.h"

dooble_about::dooble_about(void):QMainWindow()
{
  m_ui.setupUi(this);
  connect(m_ui.license,
	  SIGNAL(linkActivated(const QString &)),
	  this,
	  SLOT(slot_link_activated(const QString &)));
  connect(m_ui.release_notes,
	  SIGNAL(linkActivated(const QString &)),
	  this,
	  SLOT(slot_link_activated(const QString &)));

  QString qversion("");
  const char *tmp = qVersion();

  if(tmp)
    qversion = tmp;

  qversion = qversion.trimmed();

  if(qversion.isEmpty())
    qversion = "unknown";

  m_ui.license->setText
    (tr("<a href=\"qrc://Documentation/DOOBLE-LICENSE.html\">"
	"Dooble 3-Clause BSD License</a>"));

  QString text
    (tr("Architecture %1.<br>Qt version %2 (runtime %3).").
     arg(DOOBLE_ARCHITECTURE_STR).
     arg(QT_VERSION_STR).
     arg(qversion));

  m_ui.local_information->setText(text);
  m_ui.release_notes->setText
    (tr("<a href=\"qrc://Documentation/RELEASE-NOTES.html\">"
	"Release Notes</a>"));
  m_ui.version->setText
    (tr("Dooble version %1, July.").arg(DOOBLE_VERSION_STRING));
}

void dooble_about::keyPressEvent(QKeyEvent *event)
{
  if(event && event->key() == Qt::Key_Escape)
    close();

  QMainWindow::keyPressEvent(event);
}

void dooble_about::slot_link_activated(const QString &url)
{
  emit link_activated(QUrl::fromUserInput(url));
}
