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

#ifndef dooble_accepted_or_blocked_domains_h
#define dooble_accepted_or_blocked_domains_h

#include <QMainWindow>
#include <QSqlDatabase>
#include <QTimer>

#include "ui_dooble_accepted_or_blocked_domains.h"

class dooble_accepted_or_blocked_domains: public QMainWindow
{
  Q_OBJECT

 public:
  dooble_accepted_or_blocked_domains(void);
  bool contains(const QString &domain) const;
  bool exception(const QUrl &url) const;
  void accept_or_block_domain(const QString &domain);
  void new_exception(const QString &url);
  void purge(void);

 public slots:
  void show(void);
  void showNormal(void);

 protected:
  void closeEvent(QCloseEvent *event);
  void keyPressEvent(QKeyEvent *event);
  void resizeEvent(QResizeEvent *event);

 private:
  QHash<QString, char> m_domains;
  QHash<QString, char> m_exceptions;
  QTimer m_search_timer;
  Ui_dooble_accepted_or_blocked_domains m_ui;
  void create_tables(QSqlDatabase &db);
  void populate(void);
  void populate_exceptions(void);
  void save_blocked_domain(const QString &domain, bool state);
  void save_exception(const QString &url, bool state);
  void save_settings(void);

 private slots:
  void slot_add(void);
  void slot_delete_all_exceptions(void);
  void slot_delete_selected(void);
  void slot_delete_selected_exceptions(void);
  void slot_exceptions_item_changed(QTableWidgetItem *item);
  void slot_find(void);
  void slot_import(void);
  void slot_item_changed(QTableWidgetItem *item);
  void slot_new_exception(const QString &url);
  void slot_new_exception(void);
  void slot_populate(void);
  void slot_radio_button_toggled(bool state);
  void slot_search_timer_timeout(void);

 signals:
  void populated(void);
};

#endif
