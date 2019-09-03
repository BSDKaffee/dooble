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

#include <QContextMenuEvent>
#include <QDesktopWidget>
#include <QWebEngineContextMenuData>
#include <QWebEngineProfile>

#include "dooble.h"
#include "dooble_accepted_or_blocked_domains.h"
#include "dooble_gopher.h"
#include "dooble_search_engines_popup.h"
#include "dooble_web_engine_page.h"
#include "dooble_web_engine_view.h"

dooble_web_engine_view::dooble_web_engine_view
(QWebEngineProfile *web_engine_profile, QWidget *parent):QWebEngineView(parent)
{
  m_is_private = QWebEngineProfile::defaultProfile() != web_engine_profile &&
    web_engine_profile;

  if(m_is_private)
    m_page = new dooble_web_engine_page(web_engine_profile, m_is_private, this);
  else
    m_page = new dooble_web_engine_page(this);

  connect(m_page,
	  SIGNAL(certificate_exception_accepted(const QUrl &)),
	  this,
	  SLOT(slot_certificate_exception_accepted(const QUrl &)));
  connect(m_page,
	  SIGNAL(featurePermissionRequestCanceled(const QUrl &,
						  QWebEnginePage::Feature)),
	  this,
	  SIGNAL(featurePermissionRequestCanceled(const QUrl &,
						  QWebEnginePage::Feature)));
  connect(m_page,
	  SIGNAL(featurePermissionRequested(const QUrl &,
					    QWebEnginePage::Feature)),
	  this,
	  SIGNAL(featurePermissionRequested(const QUrl &,
					    QWebEnginePage::Feature)));
#if (QT_VERSION >= QT_VERSION_CHECK(5, 12, 0))
  connect(m_page,
	  SIGNAL(printRequested(void)),
	  this,
	  SIGNAL(printRequested(void)));
#endif
  connect(m_page,
	  SIGNAL(windowCloseRequested(void)),
	  this,
	  SIGNAL(windowCloseRequested(void)));
  connect(this,
	  SIGNAL(loadProgress(int)),
	  this,
	  SLOT(slot_load_progress(int)));

  if(QWebEngineProfile::defaultProfile() != m_page->profile())
    connect(m_page->profile(),
	    SIGNAL(downloadRequested(QWebEngineDownloadItem *)),
	    this,
	    SIGNAL(downloadRequested(QWebEngineDownloadItem *)));

  if(!m_page->profile()->urlSchemeHandler("gopher"))
    m_page->profile()->installUrlSchemeHandler
      ("gopher", new dooble_gopher(this));

  setPage(m_page);
}

dooble_web_engine_view::~dooble_web_engine_view()
{
  for(int i = m_dialog_requests.size() - 1; i >= 0; i--)
    if(m_dialog_requests.at(i) && m_dialog_requests.at(i)->parent() == this)
      {
	m_dialog_requests.at(i)->deleteLater();
	m_dialog_requests.removeAt(i);
      }
}

QWebEngineProfile *dooble_web_engine_view::web_engine_profile(void) const
{
  return m_page->profile();
}

QSize dooble_web_engine_view::sizeHint(void) const
{
  return QApplication::desktop()->screenGeometry(this).size();
}

bool dooble_web_engine_view::is_private(void) const
{
  return m_is_private;
}

dooble_web_engine_view *dooble_web_engine_view::createWindow
(QWebEnginePage::WebWindowType type)
{
  dooble_web_engine_view *view = new dooble_web_engine_view
    (m_page->profile(), nullptr);

  if(!m_page->last_clicked_link().isEmpty() &&
     m_page->last_clicked_link().isValid())
    view->setUrl(m_page->last_clicked_link());

  m_page->reset_last_clicked_link();

  if(type == QWebEnginePage::WebBrowserWindow ||
     type == QWebEnginePage::WebDialog)
    if(dooble_settings::setting("javascript").toBool() &&
       dooble_settings::setting("javascript_block_popups").toBool())
      {
	QUrl url(QUrl::fromUserInput(this->url().host()));

	url.setScheme(this->url().scheme());

	if(!(dooble_settings::
	     site_has_javascript_block_popup_exception(this->url()) ||
	     dooble_settings::site_has_javascript_block_popup_exception(url)))
	  {
	    m_dialog_requests << view;
	    view->setParent(this);
	    QTimer::singleShot
	      (250, this, SLOT(slot_create_dialog_requests(void)));
	    return view;
	  }
      }

  switch(type)
    {
    case QWebEnginePage::WebBrowserWindow:
      {
	emit create_window(view);
	break;
      }
    case QWebEnginePage::WebDialog:
      {
	emit create_dialog(view);
	break;
      }
    default:
      {
	emit create_tab(view);
	break;
      }
    }

  return view;
}

void dooble_web_engine_view::contextMenuEvent(QContextMenuEvent *event)
{
  QMenu *menu = m_page->createStandardContextMenu();

  if(!menu)
    menu = new QMenu(this);

  QAction *action = nullptr;

  /*
  ** Change some text.
  */

  if((action = m_page->action(QWebEnginePage::CopyImageToClipboard)))
    action->setText(tr("Copy Image"));

  if((action = m_page->action(QWebEnginePage::CopyImageUrlToClipboard)))
    action->setText(tr("Copy Image Address"));

  if((action = m_page->action(QWebEnginePage::CopyLinkToClipboard)))
    action->setText(tr("Copy Link Address"));

  if((action = m_page->action(QWebEnginePage::CopyMediaUrlToClipboard)))
    action->setText(tr("Copy Media Link"));

  if((action = m_page->action(QWebEnginePage::PasteAndMatchStyle)))
    action->setText(tr("Paste and Match Style"));

  if((action = m_page->action(QWebEnginePage::DownloadImageToDisk)))
    action->setText(tr("Save Image"));

  if((action = m_page->action(QWebEnginePage::DownloadLinkToDisk)))
    action->setText(tr("Save Link"));

  if((action = m_page->action(QWebEnginePage::SavePage)))
    action->setText(tr("Save Page"));

  if((action = m_page->action(QWebEnginePage::SelectAll)))
    action->setText(tr("Select All"));

  /*
  ** Hide some actions.
  */

  QList<QWebEnginePage::WebAction> list;

  list << QWebEnginePage::OpenLinkInNewTab
       << QWebEnginePage::OpenLinkInNewWindow;

  for(int i = 0; i < list.size(); i++)
    if((action = m_page->action(list.at(i))))
      action->setVisible(false);

  action = m_page->action(QWebEnginePage::ViewSource);

  if(action)
    action->setText(tr("View Page Source"));

  if(!menu->actions().isEmpty() && !menu->actions().last()->isSeparator())
    menu->addSeparator();

  action = menu->addAction
    (tr("Open &Link"),
     this,
     SLOT(slot_open_link_in_current_page(void)));

  QWebEngineContextMenuData context_menu_data = m_page->contextMenuData();

  if(context_menu_data.isValid())
    {
      if(context_menu_data.linkUrl().isValid())
	action->setProperty("url", context_menu_data.linkUrl());
      else if(context_menu_data.mediaUrl().isValid())
	action->setProperty("url", context_menu_data.mediaUrl());
      else
	action->setEnabled(false);
    }
  else
    action->setEnabled(false);

  action = menu->addAction
    (tr("Open Link in a New P&rivate Window"),
     this,
     SLOT(slot_open_link_in_new_private_window(void)));

  if(context_menu_data.isValid())
    {
      if(context_menu_data.linkUrl().isValid())
	action->setProperty("url", context_menu_data.linkUrl());
      else if(context_menu_data.mediaUrl().isValid())
	action->setProperty("url", context_menu_data.mediaUrl());
      else
	action->setEnabled(false);
    }
  else
    action->setEnabled(false);

  action = menu->addAction
    (tr("Open Link in a New &Tab"),
     this,
     SLOT(slot_open_link_in_new_tab(void)));

  if(context_menu_data.isValid())
    {
      if(context_menu_data.linkUrl().isValid())
	action->setProperty("url", context_menu_data.linkUrl());
      else if(context_menu_data.mediaUrl().isValid())
	action->setProperty("url", context_menu_data.mediaUrl());
      else
	action->setEnabled(false);
    }
  else
    action->setEnabled(false);

  action = menu->addAction
    (tr("Open Link in a &New Window"),
     this,
     SLOT(slot_open_link_in_new_window(void)));

  if(context_menu_data.isValid())
    {
      if(context_menu_data.linkUrl().isValid())
	action->setProperty("url", context_menu_data.linkUrl());
      else if(context_menu_data.mediaUrl().isValid())
	action->setProperty("url", context_menu_data.mediaUrl());
      else
	action->setEnabled(false);
    }
  else
    action->setEnabled(false);

  menu->addSeparator();

  if(dooble_settings::
     setting("accepted_or_blocked_domains_mode").toString() == "accept")
    action = menu->addAction(tr("Accept Link's Domain(s)"),
			     this,
			     SLOT(slot_accept_or_block_domain(void)));
  else
    action = menu->addAction(tr("Block Link's Domain(s)"),
			     this,
			     SLOT(slot_accept_or_block_domain(void)));

  if(context_menu_data.isValid())
    {
      if(context_menu_data.linkUrl().isValid())
	action->setProperty("url", context_menu_data.linkUrl());
      else if(context_menu_data.mediaUrl().isValid())
	action->setProperty("url", context_menu_data.mediaUrl());
      else
	action->setEnabled(false);
    }
  else
    action->setEnabled(false);

  if(dooble::s_search_engines_window && !selectedText().isEmpty())
    {
      QList<QAction *> actions(dooble::s_search_engines_window->actions());

      if(!actions.isEmpty())
	{
	  menu->addSeparator();

	  QMenu *sub_menu = menu->addMenu("Search Selected Text");

	  for(int i = 0; i < actions.size(); i++)
	    {
	      QAction *action = sub_menu->addAction(actions.at(i)->icon(),
						    actions.at(i)->text(),
						    this,
						    SLOT(slot_search(void)));

	      action->setProperty("selected_text", selectedText());
	      action->setProperty("url", actions.at(i)->property("url"));
	    }
	}
    }

  menu->exec(mapToGlobal(event->pos()));
  menu->deleteLater();
}

void dooble_web_engine_view::download(const QString &file_name, const QUrl &url)
{
#if (QT_VERSION >= QT_VERSION_CHECK(5, 10, 0))
#ifndef DOOBLE_FREEBSD_WEBENGINE_MISMATCH
  m_page->download(url, file_name);
#else
  Q_UNUSED(file_name);
  Q_UNUSED(url);
#endif
#else
  Q_UNUSED(file_name);
  Q_UNUSED(url);
#endif
}

void dooble_web_engine_view::save(const QString &file_name)
{
  m_page->save(file_name, QWebEngineDownloadItem::CompleteHtmlSaveFormat);
}

void dooble_web_engine_view::slot_accept_or_block_domain(void)
{
  QAction *action = qobject_cast<QAction *> (sender());

  if(!action)
    return;

  QString host(action->property("url").toUrl().host());
  int index = -1;

  while(!host.isEmpty())
    {
      dooble::s_accepted_or_blocked_domains->accept_or_block_domain(host);

      if((index = host.indexOf('.')) > 0)
	host.remove(0, index + 1);

      if(host.indexOf('.') < 0)
	break;
    }
}

void dooble_web_engine_view::set_feature_permission
(const QUrl &security_origin,
 QWebEnginePage::Feature feature,
 QWebEnginePage::PermissionPolicy policy)
{
  dooble::s_settings->set_site_feature_permission
    (security_origin,
     feature,
     policy == QWebEnginePage::PermissionGrantedByUser);
  m_page->setFeaturePermission(security_origin, feature, policy);
}

void dooble_web_engine_view::slot_certificate_exception_accepted
(const QUrl &url)
{
  if(!url.isEmpty() && url.isValid())
    load(url);
}

void dooble_web_engine_view::slot_create_dialog_requests(void)
{
  while(!m_dialog_requests.isEmpty())
    emit create_dialog_request(m_dialog_requests.takeFirst());
}

void dooble_web_engine_view::slot_load_progress(int progress)
{
  if(progress == 100)
    emit loadFinished(true);
}

void dooble_web_engine_view::slot_open_link_in_current_page(void)
{
  QAction *action = qobject_cast<QAction *> (sender());

  if(!action)
    return;

  QUrl url(action->property("url").toUrl());

  if(!url.isEmpty() && url.isValid())
    emit open_link_in_current_page(url);
}

void dooble_web_engine_view::slot_open_link_in_new_private_window(void)
{
  QAction *action = qobject_cast<QAction *> (sender());

  if(!action)
    return;

  QUrl url(action->property("url").toUrl());

  if(!url.isEmpty() && url.isValid())
    emit open_link_in_new_private_window(url);
}

void dooble_web_engine_view::slot_open_link_in_new_window(void)
{
  QAction *action = qobject_cast<QAction *> (sender());

  if(!action)
    return;

  QUrl url(action->property("url").toUrl());

  if(!url.isEmpty() && url.isValid())
    emit open_link_in_new_window(url);
}

void dooble_web_engine_view::slot_open_link_in_new_tab(void)
{
  QAction *action = qobject_cast<QAction *> (sender());

  if(!action)
    return;

  QUrl url(action->property("url").toUrl());

  if(!url.isEmpty() && url.isValid())
    emit open_link_in_new_tab(url);
}

void dooble_web_engine_view::slot_search(void)
{
  QAction *action = qobject_cast<QAction *> (sender());

  if(!action)
    return;

  QUrl url(action->property("url").toUrl());

  if(!url.isEmpty() && url.isValid())
    {
      QString str
	(url.query().
	 append(QString("\"%1\"").arg(action->property("selected_text").
				      toString())));

      url.setQuery(str);
      emit open_link_in_new_tab(url);
    }
}

void dooble_web_engine_view::slot_settings_applied(void)
{
}
