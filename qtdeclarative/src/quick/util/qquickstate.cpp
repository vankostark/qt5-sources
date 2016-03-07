/****************************************************************************
**
** Copyright (C) 2012 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the QtQml module of the Qt Toolkit.
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

#include "qquickstate_p_p.h"
#include "qquickstate_p.h"

#include "qquickstategroup_p.h"
#include "qquickstatechangescript_p.h"

#include <private/qqmlglobal_p.h>

#include <QtCore/qdebug.h>

QT_BEGIN_NAMESPACE

DEFINE_BOOL_CONFIG_OPTION(stateChangeDebug, STATECHANGE_DEBUG);

QQuickAction::QQuickAction()
: restore(true), actionDone(false), reverseEvent(false), deletableToBinding(false), fromBinding(0), event(0),
  specifiedObject(0)
{
}

QQuickAction::QQuickAction(QObject *target, const QString &propertyName,
               const QVariant &value)
: restore(true), actionDone(false), reverseEvent(false), deletableToBinding(false),
  property(target, propertyName, qmlEngine(target)), toValue(value),
  fromBinding(0), event(0),
  specifiedObject(target), specifiedProperty(propertyName)
{
    if (property.isValid())
        fromValue = property.read();
}

QQuickAction::QQuickAction(QObject *target, const QString &propertyName,
               QQmlContext *context, const QVariant &value)
: restore(true), actionDone(false), reverseEvent(false), deletableToBinding(false),
  property(target, propertyName, context), toValue(value),
  fromBinding(0), event(0),
  specifiedObject(target), specifiedProperty(propertyName)
{
    if (property.isValid())
        fromValue = property.read();
}


QQuickActionEvent::~QQuickActionEvent()
{
}

void QQuickActionEvent::execute(Reason)
{
}

bool QQuickActionEvent::isReversable()
{
    return false;
}

void QQuickActionEvent::reverse(Reason)
{
}

bool QQuickActionEvent::changesBindings()
{
    return false;
}

void QQuickActionEvent::clearBindings()
{
}

bool QQuickActionEvent::override(QQuickActionEvent *other)
{
    Q_UNUSED(other);
    return false;
}

QQuickStateOperation::QQuickStateOperation(QObjectPrivate &dd, QObject *parent)
    : QObject(dd, parent)
{
}

/*!
    \qmltype State
    \instantiates QQuickState
    \inqmlmodule QtQuick 2
    \ingroup qtquick-states
    \brief Defines configurations of objects and properties

    A \e state is a set of batched changes from the default configuration.

    All items have a default state that defines the default configuration of objects
    and property values. New states can be defined by adding State items to the \l {Item::states}{states} property to
    allow items to switch between different configurations. These configurations
    can, for example, be used to apply different sets of property values or execute
    different scripts.

    The following example displays a single \l Rectangle. In the default state, the rectangle
    is colored black. In the "clicked" state, a PropertyChanges object changes the
    rectangle's color to red. Clicking within the MouseArea toggles the rectangle's state
    between the default state and the "clicked" state, thus toggling the color of the
    rectangle between black and red.

    \snippet qml/state.qml 0

    Notice the default state is referred to using an empty string ("").

    States are commonly used together with \l{Animation and Transitions in Qt Quick}{Transitions} to provide
    animations when state changes occur.

    \note Setting the state of an object from within another state of the same object is
    not allowed.

    \sa {declarative/animation/states}{states example}, {Qt Quick States}{Qt Quick States},
    {Animation and Transitions in Qt Quick}{Transitions}, QtQml
*/
QQuickState::QQuickState(QObject *parent)
: QObject(*(new QQuickStatePrivate), parent)
{
    Q_D(QQuickState);
    d->transitionManager.setState(this);
}

QQuickState::~QQuickState()
{
    Q_D(QQuickState);
    if (d->group)
        d->group->removeState(this);
}

/*!
    \qmlproperty string QtQuick2::State::name
    This property holds the name of the state.

    Each state should have a unique name within its item.
*/
QString QQuickState::name() const
{
    Q_D(const QQuickState);
    return d->name;
}

void QQuickState::setName(const QString &n)
{
    Q_D(QQuickState);
    d->name = n;
    d->named = true;
}

bool QQuickState::isNamed() const
{
    Q_D(const QQuickState);
    return d->named;
}

bool QQuickState::isWhenKnown() const
{
    Q_D(const QQuickState);
    return d->when != 0;
}

/*!
    \qmlproperty bool QtQuick2::State::when
    This property holds when the state should be applied.

    This should be set to an expression that evaluates to \c true when you want the state to
    be applied. For example, the following \l Rectangle changes in and out of the "hidden"
    state when the \l MouseArea is pressed:

    \snippet qml/state-when.qml 0

    If multiple states in a group have \c when clauses that evaluate to \c true
    at the same time, the first matching state will be applied. For example, in
    the following snippet \c state1 will always be selected rather than
    \c state2 when sharedCondition becomes \c true.
    \qml
    Item {
        states: [
            State { name: "state1"; when: sharedCondition },
            State { name: "state2"; when: sharedCondition }
        ]
        // ...
    }
    \endqml
*/
QQmlBinding *QQuickState::when() const
{
    Q_D(const QQuickState);
    return d->when;
}

void QQuickState::setWhen(QQmlBinding *when)
{
    Q_D(QQuickState);
    d->when = when;
    if (d->group)
        d->group->updateAutoState();
}

/*!
    \qmlproperty string QtQuick2::State::extend
    This property holds the state that this state extends.

    When a state extends another state, it inherits all the changes of that state.

    The state being extended is treated as the base state in regards to
    the changes specified by the extending state.
*/
QString QQuickState::extends() const
{
    Q_D(const QQuickState);
    return d->extends;
}

void QQuickState::setExtends(const QString &extends)
{
    Q_D(QQuickState);
    d->extends = extends;
}

/*!
    \qmlproperty list<Change> QtQuick2::State::changes
    This property holds the changes to apply for this state
    \default

    By default these changes are applied against the default state. If the state
    extends another state, then the changes are applied against the state being
    extended.
*/
QQmlListProperty<QQuickStateOperation> QQuickState::changes()
{
    Q_D(QQuickState);
    return QQmlListProperty<QQuickStateOperation>(this, &d->operations, QQuickStatePrivate::operations_append,
                                              QQuickStatePrivate::operations_count, QQuickStatePrivate::operations_at,
                                              QQuickStatePrivate::operations_clear);
}

int QQuickState::operationCount() const
{
    Q_D(const QQuickState);
    return d->operations.count();
}

QQuickStateOperation *QQuickState::operationAt(int index) const
{
    Q_D(const QQuickState);
    return d->operations.at(index);
}

QQuickState &QQuickState::operator<<(QQuickStateOperation *op)
{
    Q_D(QQuickState);
    d->operations.append(QQuickStatePrivate::OperationGuard(op, &d->operations));
    return *this;
}

void QQuickStatePrivate::complete()
{
    Q_Q(QQuickState);

    for (int ii = 0; ii < reverting.count(); ++ii) {
        for (int jj = 0; jj < revertList.count(); ++jj) {
            const QQuickRevertAction &revert = reverting.at(ii);
            const QQuickSimpleAction &simple = revertList.at(jj);
            if ((revert.event && simple.event() == revert.event) ||
                simple.property() == revert.property) {
                revertList.removeAt(jj);
                break;
            }
        }
    }
    reverting.clear();

    if (group)
        group->stateAboutToComplete();
    emit q->completed();
}

// Generate a list of actions for this state.  This includes coelescing state
// actions that this state "extends"
QQuickStateOperation::ActionList
QQuickStatePrivate::generateActionList() const
{
    QQuickStateOperation::ActionList applyList;
    if (inState)
        return applyList;

    // Prevent "extends" recursion
    inState = true;

    if (!extends.isEmpty()) {
        QList<QQuickState *> states = group ? group->states() : QList<QQuickState *>();
        for (int ii = 0; ii < states.count(); ++ii)
            if (states.at(ii)->name() == extends) {
                qmlExecuteDeferred(states.at(ii));
                applyList = static_cast<QQuickStatePrivate*>(states.at(ii)->d_func())->generateActionList();
            }
    }

    foreach(QQuickStateOperation *op, operations)
        applyList << op->actions();

    inState = false;
    return applyList;
}

QQuickStateGroup *QQuickState::stateGroup() const
{
    Q_D(const QQuickState);
    return d->group;
}

void QQuickState::setStateGroup(QQuickStateGroup *group)
{
    Q_D(QQuickState);
    d->group = group;
}

void QQuickState::cancel()
{
    Q_D(QQuickState);
    d->transitionManager.cancel();
}

void QQuickAction::deleteFromBinding()
{
    if (fromBinding) {
        QQmlPropertyPrivate::setBinding(property, 0);
        fromBinding->destroy();
        fromBinding = 0;
    }
}

bool QQuickState::containsPropertyInRevertList(QObject *target, const QString &name) const
{
    Q_D(const QQuickState);

    if (isStateActive()) {
        QListIterator<QQuickSimpleAction> revertListIterator(d->revertList);

        while (revertListIterator.hasNext()) {
            const QQuickSimpleAction &simpleAction = revertListIterator.next();
            if (simpleAction.specifiedObject() == target && simpleAction.specifiedProperty() == name)
                return true;
        }
    }

    return false;
}

bool QQuickState::changeValueInRevertList(QObject *target, const QString &name, const QVariant &revertValue)
{
    Q_D(QQuickState);

    if (isStateActive()) {
        QMutableListIterator<QQuickSimpleAction> revertListIterator(d->revertList);

        while (revertListIterator.hasNext()) {
            QQuickSimpleAction &simpleAction = revertListIterator.next();
            if (simpleAction.specifiedObject() == target && simpleAction.specifiedProperty() == name) {
                    simpleAction.setValue(revertValue);
                    return true;
            }
        }
    }

    return false;
}

bool QQuickState::changeBindingInRevertList(QObject *target, const QString &name, QQmlAbstractBinding *binding)
{
    Q_D(QQuickState);

    if (isStateActive()) {
        QMutableListIterator<QQuickSimpleAction> revertListIterator(d->revertList);

        while (revertListIterator.hasNext()) {
            QQuickSimpleAction &simpleAction = revertListIterator.next();
            if (simpleAction.specifiedObject() == target && simpleAction.specifiedProperty() == name) {
                if (simpleAction.binding())
                    simpleAction.binding()->destroy();

                simpleAction.setBinding(binding);
                return true;
            }
        }
    }

    return false;
}

bool QQuickState::removeEntryFromRevertList(QObject *target, const QString &name)
{
    Q_D(QQuickState);

    if (isStateActive()) {
        QMutableListIterator<QQuickSimpleAction> revertListIterator(d->revertList);

        while (revertListIterator.hasNext()) {
            QQuickSimpleAction &simpleAction = revertListIterator.next();
            if (simpleAction.property().object() == target && simpleAction.property().name() == name) {
                QQmlAbstractBinding *oldBinding = QQmlPropertyPrivate::binding(simpleAction.property());
                if (oldBinding) {
                    QQmlPropertyPrivate::setBinding(simpleAction.property(), 0);
                    oldBinding->destroy();
                }

                simpleAction.property().write(simpleAction.value());
                if (simpleAction.binding())
                    QQmlPropertyPrivate::setBinding(simpleAction.property(), simpleAction.binding());

                revertListIterator.remove();
                return true;
            }
        }
    }

    return false;
}

void QQuickState::addEntryToRevertList(const QQuickAction &action)
{
    Q_D(QQuickState);

    QQuickSimpleAction simpleAction(action);

    d->revertList.append(simpleAction);
}

void QQuickState::removeAllEntriesFromRevertList(QObject *target)
{
     Q_D(QQuickState);

     if (isStateActive()) {
         QMutableListIterator<QQuickSimpleAction> revertListIterator(d->revertList);

         while (revertListIterator.hasNext()) {
             QQuickSimpleAction &simpleAction = revertListIterator.next();
             if (simpleAction.property().object() == target) {
                 QQmlAbstractBinding *oldBinding = QQmlPropertyPrivate::binding(simpleAction.property());
                 if (oldBinding) {
                     QQmlPropertyPrivate::setBinding(simpleAction.property(), 0);
                     oldBinding->destroy();
                 }

                 simpleAction.property().write(simpleAction.value());
                 if (simpleAction.binding())
                     QQmlPropertyPrivate::setBinding(simpleAction.property(), simpleAction.binding());

                 revertListIterator.remove();
             }
         }
     }
}

void QQuickState::addEntriesToRevertList(const QList<QQuickAction> &actionList)
{
    Q_D(QQuickState);
    if (isStateActive()) {
        QList<QQuickSimpleAction> simpleActionList;

        QListIterator<QQuickAction> actionListIterator(actionList);
        while(actionListIterator.hasNext()) {
            const QQuickAction &action = actionListIterator.next();
            QQuickSimpleAction simpleAction(action);
            action.property.write(action.toValue);
            if (!action.toBinding.isNull()) {
                QQmlAbstractBinding *oldBinding = QQmlPropertyPrivate::binding(simpleAction.property());
                if (oldBinding)
                    QQmlPropertyPrivate::setBinding(simpleAction.property(), 0);
                QQmlPropertyPrivate::setBinding(simpleAction.property(), action.toBinding.data(), QQmlPropertyPrivate::DontRemoveBinding);
            }

            simpleActionList.append(simpleAction);
        }

        d->revertList.append(simpleActionList);
    }
}

QVariant QQuickState::valueInRevertList(QObject *target, const QString &name) const
{
    Q_D(const QQuickState);

    if (isStateActive()) {
        QListIterator<QQuickSimpleAction> revertListIterator(d->revertList);

        while (revertListIterator.hasNext()) {
            const QQuickSimpleAction &simpleAction = revertListIterator.next();
            if (simpleAction.specifiedObject() == target && simpleAction.specifiedProperty() == name)
                return simpleAction.value();
        }
    }

    return QVariant();
}

QQmlAbstractBinding *QQuickState::bindingInRevertList(QObject *target, const QString &name) const
{
    Q_D(const QQuickState);

    if (isStateActive()) {
        QListIterator<QQuickSimpleAction> revertListIterator(d->revertList);

        while (revertListIterator.hasNext()) {
            const QQuickSimpleAction &simpleAction = revertListIterator.next();
            if (simpleAction.specifiedObject() == target && simpleAction.specifiedProperty() == name)
                return simpleAction.binding();
        }
    }

    return 0;
}

bool QQuickState::isStateActive() const
{
    return stateGroup() && stateGroup()->state() == name();
}

void QQuickState::apply(QQuickTransition *trans, QQuickState *revert)
{
    Q_D(QQuickState);

    qmlExecuteDeferred(this);

    cancel();
    if (revert)
        revert->cancel();
    d->revertList.clear();
    d->reverting.clear();

    if (revert) {
        QQuickStatePrivate *revertPrivate =
            static_cast<QQuickStatePrivate*>(revert->d_func());
        d->revertList = revertPrivate->revertList;
        revertPrivate->revertList.clear();
    }

    // List of actions caused by this state
    QQuickStateOperation::ActionList applyList = d->generateActionList();

    // List of actions that need to be reverted to roll back (just) this state
    QQuickStatePrivate::SimpleActionList additionalReverts;
    // First add the reverse of all the applyList actions
    for (int ii = 0; ii < applyList.count(); ++ii) {
        QQuickAction &action = applyList[ii];

        if (action.event) {
            if (!action.event->isReversable())
                continue;
            bool found = false;
            for (int jj = 0; jj < d->revertList.count(); ++jj) {
                QQuickActionEvent *event = d->revertList.at(jj).event();
                if (event && event->type() == action.event->type()) {
                    if (action.event->override(event)) {
                        found = true;

                        if (action.event != d->revertList.at(jj).event() && action.event->needsCopy()) {
                            action.event->copyOriginals(d->revertList.at(jj).event());

                            QQuickSimpleAction r(action);
                            additionalReverts << r;
                            d->revertList.removeAt(jj);
                            --jj;
                        } else if (action.event->isRewindable())    //###why needed?
                            action.event->saveCurrentValues();

                        break;
                    }
                }
            }
            if (!found) {
                action.event->saveOriginals();
                // Only need to revert the applyList action if the previous
                // state doesn't have a higher priority revert already
                QQuickSimpleAction r(action);
                additionalReverts << r;
            }
        } else {
            bool found = false;
            action.fromBinding = QQmlPropertyPrivate::binding(action.property);

            for (int jj = 0; jj < d->revertList.count(); ++jj) {
                if (d->revertList.at(jj).property() == action.property) {
                    found = true;
                    if (d->revertList.at(jj).binding() != action.fromBinding) {
                        action.deleteFromBinding();
                    }
                    break;
                }
            }

            if (!found) {
                if (!action.restore) {
                    action.deleteFromBinding();;
                } else {
                    // Only need to revert the applyList action if the previous
                    // state doesn't have a higher priority revert already
                    QQuickSimpleAction r(action);
                    additionalReverts << r;
                }
            }
        }
    }

    // Any reverts from a previous state that aren't carried forth
    // into this state need to be translated into apply actions
    for (int ii = 0; ii < d->revertList.count(); ++ii) {
        bool found = false;
        if (d->revertList.at(ii).event()) {
            QQuickActionEvent *event = d->revertList.at(ii).event();
            if (!event->isReversable())
                continue;
            for (int jj = 0; !found && jj < applyList.count(); ++jj) {
                const QQuickAction &action = applyList.at(jj);
                if (action.event && action.event->type() == event->type()) {
                    if (action.event->override(event))
                        found = true;
                }
            }
        } else {
            for (int jj = 0; !found && jj < applyList.count(); ++jj) {
                const QQuickAction &action = applyList.at(jj);
                if (action.property == d->revertList.at(ii).property())
                    found = true;
            }
        }
        if (!found) {
            QVariant cur = d->revertList.at(ii).property().read();
            QQmlAbstractBinding *delBinding =
                QQmlPropertyPrivate::setBinding(d->revertList.at(ii).property(), 0);
            if (delBinding)
                delBinding->destroy();

            QQuickAction a;
            a.property = d->revertList.at(ii).property();
            a.fromValue = cur;
            a.toValue = d->revertList.at(ii).value();
            a.toBinding = QQmlAbstractBinding::getPointer(d->revertList.at(ii).binding());
            a.specifiedObject = d->revertList.at(ii).specifiedObject();
            a.specifiedProperty = d->revertList.at(ii).specifiedProperty();
            a.event = d->revertList.at(ii).event();
            a.reverseEvent = d->revertList.at(ii).reverseEvent();
            if (a.event && a.event->isRewindable())
                a.event->saveCurrentValues();
            applyList << a;
            // Store these special reverts in the reverting list
            if (a.event)
                d->reverting << a.event;
            else
                d->reverting << a.property;
        }
    }
    // All the local reverts now become part of the ongoing revertList
    d->revertList << additionalReverts;

#ifndef QT_NO_DEBUG_STREAM
    // Output for debugging
    if (stateChangeDebug()) {
        foreach(const QQuickAction &action, applyList) {
            if (action.event)
                qWarning() << "    QQuickAction event:" << action.event->type();
            else
                qWarning() << "    QQuickAction:" << action.property.object()
                           << action.property.name() << "From:" << action.fromValue
                           << "To:" << action.toValue;
        }
    }
#endif

    d->transitionManager.transition(applyList, trans);
}

QQuickStateOperation::ActionList QQuickStateOperation::actions()
{
    return ActionList();
}

QQuickState *QQuickStateOperation::state() const
{
    Q_D(const QQuickStateOperation);
    return d->m_state;
}

void QQuickStateOperation::setState(QQuickState *state)
{
    Q_D(QQuickStateOperation);
    d->m_state = state;
}

QT_END_NAMESPACE
