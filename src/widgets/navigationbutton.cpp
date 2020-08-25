/*
 * Copyright (C) 2017 ~ 2018 Deepin Technology Co., Ltd.
 *
 * Author:     kirigaya <kirigaya@mkacg.com>
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "navigationbutton.h"
#include <QDebug>

NavigationButton::NavigationButton(QString text, DWidget *parent)
    : DPushButton(text, parent)
{
    setCheckable(true);
    setFocusPolicy(Qt::NoFocus);
    this->setFlat(true);
    this->setText("");
    isEnter = false;

    m_label = new DLabel(this);
    m_label->setElideMode(Qt::ElideRight);
    m_label->setText(text);
    m_label->setFixedSize(95, 30);
    QHBoxLayout *layout = new QHBoxLayout;
    layout->setMargin(0);
    layout->setContentsMargins(5, 0, 0, 0);
    layout->addWidget(m_label);
    setLayout(layout);
    initButton();
    needFrame = false;

    connect(DGuiApplicationHelper::instance(), &DGuiApplicationHelper::themeTypeChanged, this,
            &NavigationButton::initButton);
    connect(this, &NavigationButton::clicked, this, &NavigationButton::initButton);
}

void NavigationButton::mousePressEvent(QMouseEvent *event)
{
    needFrame = false;
    isEnter = false;
    DPushButton::mousePressEvent(event);
}

void NavigationButton::initButton()
{
    int type = DGuiApplicationHelper::instance()->themeType();
    DPalette pl = m_label->palette();
    if (type == 1) {
        if (isChecked()) {
            pl.setColor(DPalette::WindowText, Qt::white);
        } else {
            pl.setColor(DPalette::WindowText, QColor(65, 77, 104));
        }
    } else {
        if (isChecked()) {
            pl.setColor(DPalette::WindowText, Qt::white);
        } else {
            pl.setColor(DPalette::WindowText, QColor(192, 198, 212));
        }
    }

    m_label->autoFillBackground();
    m_label->setForegroundRole(DPalette::WindowText);
    m_label->setPalette(pl);
}

void NavigationButton::enterEvent(QEvent *event) {
    isEnter = true;
}

void NavigationButton::leaveEvent(QEvent *event) {
    isEnter = false;
}

void NavigationButton::paintEvent(QPaintEvent *event)
{
    if (isEnter) {
        QRect rect = this->rect();
        QPainter painter(this);
        painter.setRenderHint(QPainter::Antialiasing);
        DPalette pa;
        painter.setBrush(pa.light());
        QColor penColor = pa.base().color();
        painter.setPen(QPen(penColor));

        QPainterPath painterPath;
        painterPath.addRoundedRect(rect, 8, 8);
        painter.drawPath(painterPath);
    }

    DPushButton::paintEvent(event);

    if (needFrame) {
        QPainter painter(this);
        painter.setRenderHint(QPainter::Antialiasing, true);
        QPainterPath path;
        path.addRoundedRect(rect().adjusted(4, 4, -4, -4), 5, 5);
        painter.setClipRect(QRect(), Qt::NoClip);

        QColor color(Qt::white);
        QPen pen(color);
        pen.setWidth(1);
        painter.setPen(pen);
        painter.drawPath(path);
    }

    initButton();
}


/*QSize NavigationButton::setHint()
{
    QFontMetrics fm(this->font());
    QRect rec = fm.boundingRect(m_label->text());
    return rec.size();
}*/

/*void NavigationButton::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    QFont font;
    font.setFamily("SourceHanSansSC-Medium");
    font.setPixelSize(14);
    painter.setFont(font);
    QRect r = rect();
    r.setLeft(rect().left() + 10);
    r.setTop(rect().top() + 4);
    if (!isChecked()) {
        painter.fillRect(rect(),QColor(248,248,248));
        painter.setPen(QPen(QColor(65,77,104)));
        painter.drawText(r, m_text);
    } else {
        QRect bacrRect = rect();
        painter.setRenderHint(QPainter :: Antialiasing);
        QPainterPath path;
        path.addRoundedRect(bacrRect, 8, 8);
        painter.setPen(Qt::NoPen);
        painter.fillPath(path, QColor(0,129,255));
        painter.drawPath(path);
        painter.setPen(QPen(QColor(255,255,255)));
        painter.drawText(r, m_text);
    }
}*/
