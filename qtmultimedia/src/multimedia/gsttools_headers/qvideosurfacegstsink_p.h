/****************************************************************************
**
** Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef VIDEOSURFACEGSTSINK_P_H
#define VIDEOSURFACEGSTSINK_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API. It exists purely as an
// implementation detail. This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include <gst/video/gstvideosink.h>

#include <QtCore/qlist.h>
#include <QtCore/qmutex.h>
#include <QtCore/qqueue.h>
#include <QtCore/qpointer.h>
#include <QtCore/qwaitcondition.h>
#include <qvideosurfaceformat.h>
#include <qvideoframe.h>
#include <qabstractvideobuffer.h>

#include "qgstbufferpoolinterface_p.h"

QT_BEGIN_NAMESPACE
class QAbstractVideoSurface;

class QVideoSurfaceGstDelegate : public QObject
{
    Q_OBJECT
public:
    QVideoSurfaceGstDelegate(QAbstractVideoSurface *surface);
    ~QVideoSurfaceGstDelegate();

    QList<QVideoFrame::PixelFormat> supportedPixelFormats(
            QAbstractVideoBuffer::HandleType handleType = QAbstractVideoBuffer::NoHandle) const;

    QVideoSurfaceFormat surfaceFormat() const;

    bool start(const QVideoSurfaceFormat &format, int bytesPerLine);
    void stop();

    bool isActive();

    QGstBufferPoolInterface *pool() { return m_pool; }
    QMutex *poolMutex() { return &m_poolMutex; }
    void clearPoolBuffers();

    GstFlowReturn render(GstBuffer *buffer);

    GstBuffer *lastPrerolledBuffer() const { return m_lastPrerolledBuffer; }
    void setLastPrerolledBuffer(GstBuffer *lastPrerolledBuffer); // set prerolledBuffer to 0 to discard prerolled buffer

private slots:
    void queuedStart();
    void queuedStop();
    void queuedRender();

    void updateSupportedFormats();

private:
    QPointer<QAbstractVideoSurface> m_surface;
    QList<QVideoFrame::PixelFormat> m_supportedPixelFormats;
    //pixel formats of buffers pool native type
    QList<QVideoFrame::PixelFormat> m_supportedPoolPixelFormats;
    QGstBufferPoolInterface *m_pool;
    QList<QGstBufferPoolInterface *> m_pools;
    QMutex m_poolMutex;
    QMutex m_mutex;
    QWaitCondition m_setupCondition;
    QWaitCondition m_renderCondition;
    QVideoSurfaceFormat m_format;
    QVideoFrame m_frame;
    GstFlowReturn m_renderReturn;
    // this pointer is not 0 when there is a prerolled buffer waiting to be displayed
    GstBuffer *m_lastPrerolledBuffer;
    int m_bytesPerLine;
    bool m_started;
    bool m_startCanceled;
};

class QVideoSurfaceGstSink
{
public:
    GstVideoSink parent;

    static QVideoSurfaceGstSink *createSink(QAbstractVideoSurface *surface);
    static QVideoSurfaceFormat formatForCaps(GstCaps *caps,
                                             int *bytesPerLine = 0,
                                             QAbstractVideoBuffer::HandleType handleType = QAbstractVideoBuffer::NoHandle);
    static void setFrameTimeStamps(QVideoFrame *frame, GstBuffer *buffer);

    static void handleShowPrerollChange(GObject *o, GParamSpec *p, gpointer d);

private:
    static GType get_type();
    static void class_init(gpointer g_class, gpointer class_data);
    static void base_init(gpointer g_class);
    static void instance_init(GTypeInstance *instance, gpointer g_class);

    static void finalize(GObject *object);

    static GstStateChangeReturn change_state(GstElement *element, GstStateChange transition);

    static GstCaps *get_caps(GstBaseSink *sink);
    static gboolean set_caps(GstBaseSink *sink, GstCaps *caps);

    static GstFlowReturn buffer_alloc(
            GstBaseSink *sink, guint64 offset, guint size, GstCaps *caps, GstBuffer **buffer);

    static gboolean start(GstBaseSink *sink);
    static gboolean stop(GstBaseSink *sink);

    static gboolean unlock(GstBaseSink *sink);

    static gboolean event(GstBaseSink *sink, GstEvent *event);
    static GstFlowReturn preroll(GstBaseSink *sink, GstBuffer *buffer);
    static GstFlowReturn render(GstBaseSink *sink, GstBuffer *buffer);

private:
    QVideoSurfaceGstDelegate *delegate;

    GstCaps *lastRequestedCaps;
    GstCaps *lastBufferCaps;
    QVideoSurfaceFormat *lastSurfaceFormat;
};


class QVideoSurfaceGstSinkClass
{
public:
    GstVideoSinkClass parent_class;
};

QT_END_NAMESPACE

#endif
