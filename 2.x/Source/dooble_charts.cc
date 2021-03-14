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

#include "dooble_charts.h"

#ifdef DOOBLE_QTCHARTS_PRESENT
#include <QtCharts/QChart>
#include <QtCharts/QChartView>
#endif

const char *dooble_charts::PropertiesStrings[] =
  {
   "Animation Duration",
   "Animation Options",
   "Background Color",
   "Background Roundness",
   "Background Visible",
   "Chart Type",
   "Drop Shadow Enabled",
   "Locale",
   "Localize Numbers",
   "Margins",
   "Plot Area Background Visible",
   "Theme",
   "Title",
   "Title Color",
   "Title Font",
   nullptr
  };

dooble_charts::dooble_charts(QWidget *parent):QWidget(parent)
{
#ifdef DOOBLE_QTCHARTS_PRESENT
  m_chart = new QChart();
  m_chart_view = new QChartView(m_chart);
#endif
  m_property_editor = nullptr;
  m_ui.setupUi(this);
  m_ui.splitter->setStretchFactor(0, 1);
  m_ui.splitter->setStretchFactor(1, 0);
}

dooble_charts::~dooble_charts()
{
#ifdef DOOBLE_QTCHARTS_PRESENT
  m_chart->deleteLater();
#endif
}
