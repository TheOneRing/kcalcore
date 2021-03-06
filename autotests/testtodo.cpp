/*
  This file is part of the kcalcore library.

  Copyright (C) 2006,2008 Allen Winter <winter@kde.org>

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Library General Public
  License as published by the Free Software Foundation; either
  version 2 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Library General Public License for more details.

  You should have received a copy of the GNU Library General Public License
  along with this library; see the file COPYING.LIB.  If not, write to
  the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
  Boston, MA 02110-1301, USA.
*/
#include "testtodo.h"
#include "todo.h"
#include "event.h"
#include "attachment.h"
#include "utils.h"

#include <QTest>
QTEST_MAIN(TodoTest)

using namespace KCalCore;

void TodoTest::initTestCase()
{
    qputenv("TZ", "GMT");
}

void TodoTest::testValidity()
{
    QDate dt = QDate::currentDate();
    Todo *todo = new Todo();
    todo->setDtStart(QDateTime(dt, {}));
    todo->setDtDue(QDateTime(dt, {}).addDays(1));
    todo->setSummary(QStringLiteral("To-do1 Summary"));
    todo->setDescription(QStringLiteral("This is a description of the first to-do"));
    todo->setLocation(QStringLiteral("the place"));
    todo->setPercentComplete(5);

    QCOMPARE(todo->summary(), QStringLiteral("To-do1 Summary"));
    QCOMPARE(todo->location(), QStringLiteral("the place"));
    QCOMPARE(todo->percentComplete(), 5);
}

void TodoTest::testCompare()
{
    QDate dt = QDate::currentDate();
    Todo todo1;
    todo1.setDtStart(QDateTime(dt, {}));
    todo1.setDtDue(QDateTime(dt, {}).addDays(1));
    todo1.setSummary(QStringLiteral("To-do1 Summary"));
    todo1.setDescription(QStringLiteral("This is a description of the first to-do"));
    todo1.setLocation(QStringLiteral("the place"));
    todo1.setCompleted(true);

    Todo todo2;
    todo2.setDtStart(QDateTime(dt, {}).addDays(1));
    todo2.setDtDue(QDateTime(dt, {}).addDays(2));
    todo2.setSummary(QStringLiteral("To-do2 Summary"));
    todo2.setDescription(QStringLiteral("This is a description of the second to-do"));
    todo2.setLocation(QStringLiteral("the other place"));
    todo2.setCompleted(false);

    QVERIFY(!(todo1 == todo2));
    QCOMPARE(todo1.dtDue(), todo2.dtStart());
    QCOMPARE(todo2.summary(), QStringLiteral("To-do2 Summary"));
    QVERIFY(!(todo1.isCompleted() == todo2.isCompleted()));
}

void TodoTest::testClone()
{
    QDate dt = QDate::currentDate();
    Todo todo1;
    todo1.setDtStart(QDateTime(dt, {}));
    todo1.setDtDue(QDateTime(dt, {}).addDays(1));
    todo1.setSummary(QStringLiteral("Todo1 Summary"));
    todo1.setDescription(QStringLiteral("This is a description of the first todo"));
    todo1.setLocation(QStringLiteral("the place"));

    Todo *todo2 = todo1.clone();
    QCOMPARE(todo1.summary(), todo2->summary());
    QCOMPARE(todo1.dtStart(), todo2->dtStart());
    QCOMPARE(todo1.dtDue(), todo2->dtDue());
    QCOMPARE(todo1.description(), todo2->description());
    QCOMPARE(todo1.location(), todo2->location());
    QCOMPARE(todo1.isCompleted(), todo2->isCompleted());
}

void TodoTest::testCopyIncidence()
{
    QDate dt = QDate::currentDate();
    Event event;
    event.setDtStart(QDateTime(dt, {}));
    event.setSummary(QStringLiteral("Event1 Summary"));
    event.setDescription(QStringLiteral("This is a description of the first event"));
    event.setLocation(QStringLiteral("the place"));

    Todo todo(event);
    QCOMPARE(todo.uid(), event.uid());
    QCOMPARE(todo.dtStart(), event.dtStart());
    QCOMPARE(todo.summary(), event.summary());
    QCOMPARE(todo.description(), event.description());
    QCOMPARE(todo.location(), event.location());
}

void TodoTest::testAssign()
{
    QDate dt = QDate::currentDate();
    Todo todo1;
    todo1.setDtStart(QDateTime(dt, {}));
    todo1.setDtDue(QDateTime(dt, {}).addDays(1));
    todo1.setSummary(QStringLiteral("Todo1 Summary"));
    todo1.setDescription(QStringLiteral("This is a description of the first todo"));
    todo1.setLocation(QStringLiteral("the place"));

    Todo todo2 = todo1;
    QVERIFY(todo1 == todo2);
}

void TodoTest::testSetCompleted()
{
    Todo todo1, todo2, todo3;
    todo1.setSummary(QStringLiteral("Todo Summary"));
    todo2.setSummary(QStringLiteral("Todo Summary"));
    todo3.setSummary(QStringLiteral("Todo Summary"));
    QDateTime today = QDateTime::currentDateTimeUtc();

    // due yesterday
    QDateTime originalDueDate = today.addDays(-1);

    todo1.setDtStart(originalDueDate);
    todo1.setDtDue(originalDueDate);
    todo1.recurrence()->setDaily(1);
    todo1.setCompleted(today);

    todo2.setCompleted(true);

    todo3.setStatus(Incidence::StatusCompleted);

    QVERIFY(originalDueDate != todo1.dtDue());
    QVERIFY(!todo1.isCompleted());
    QVERIFY(todo2.isCompleted());
    QCOMPARE(todo2.status(), Incidence::StatusCompleted);
    QVERIFY(todo3.isCompleted());
    todo2.setCompleted(false);
    QVERIFY(!todo2.isCompleted());
}

void TodoTest::testStatus()
{
    QDateTime today = QDateTime::currentDateTimeUtc();
    QDateTime yesterday = today.addDays(-1);

    Todo todo1;
    todo1.setDtStart(yesterday);
    todo1.setDtDue(today);
    todo1.setPercentComplete(50);
    QVERIFY(todo1.isInProgress(false));
    QVERIFY(!todo1.isNotStarted(false));
    QVERIFY(!todo1.isOverdue());
    todo1.setPercentComplete(100);
    QVERIFY(todo1.isCompleted());

    Todo todo2 = todo1;
    todo2.setPercentComplete(33);
    todo2.setDtDue(QDateTime());
    QVERIFY(todo2.isOpenEnded());
}

void TodoTest::testSerializer_data()
{
    QTest::addColumn<KCalCore::Todo::Ptr>("todo");

    QDateTime today = QDateTime::currentDateTimeUtc();
    QDateTime yesterday = today.addDays(-1);

    Todo::Ptr todo1 = Todo::Ptr(new Todo());
    Todo::Ptr todo2 = Todo::Ptr(new Todo());
    Todo::Ptr todo3 = Todo::Ptr(new Todo());
    Todo::Ptr todo4 = Todo::Ptr(new Todo());
    Todo::Ptr todo5 = Todo::Ptr(new Todo());
    Todo::Ptr todo6 = Todo::Ptr(new Todo());

    todo1->setSummary(QStringLiteral("Summary"), false);
    todo1->setDescription(QStringLiteral("description"), false);
    todo1->setCreated(yesterday);
    todo1->setRevision(50);
    todo1->setDtDue(yesterday);
    todo1->setDtStart(today);
    todo1->setPercentComplete(50);
    todo1->setLocation(QStringLiteral("<b>location</b>"), false);

    todo2->setDescription(QStringLiteral("<b>description</b>"), true);
    todo2->setSummary(QStringLiteral("<b>Summary2</b>"), true);
    todo2->setLocation(QStringLiteral("<b>location</b>"), true);
    todo2->setDtDue(yesterday);
    todo2->setPercentComplete(100);

    todo3->setDtStart(today);
    todo3->setPercentComplete(100);
    todo3->setCategories(QStringList()
                         << QStringLiteral("a") << QStringLiteral("b")
                         << QStringLiteral("c") << QStringLiteral("d"));
    todo3->setResources(QStringList()
                        << QStringLiteral("a") << QStringLiteral("b")
                        << QStringLiteral("c") << QStringLiteral("d"));
    todo3->setPriority(5);

    QVERIFY(!todo4->dirtyFields().contains(IncidenceBase::FieldRecurrence));
    todo4->recurrence()->setDaily(1);
    QVERIFY(todo4->dirtyFields().contains(IncidenceBase::FieldRecurrence));

    Attachment::Ptr attachment = Attachment::Ptr(new Attachment(QStringLiteral("http://www.kde.org")));
    todo4->addAttachment(attachment);

    todo5->recurrence()->setDaily(1);
    todo5->setCompleted(today);
    todo5->setStatus(Incidence::StatusDraft);
    todo5->setSecrecy(Incidence::SecrecyPrivate);
    todo5->setRelatedTo(QStringLiteral("uid1"), Incidence::RelTypeParent);
    todo5->setHasGeo(true);
    todo5->setGeoLatitude(40);
    todo5->setGeoLongitude(40);
    todo5->setOrganizer(QStringLiteral("organizer@mail.com"));

    todo6->recurrence()->setDaily(1);
    todo6->setCompleted(today);
    todo6->setRecurrenceId(yesterday);
    todo6->setStatus(Incidence::StatusDraft);
    todo6->setSecrecy(Incidence::SecrecyPrivate);
    todo6->setRelatedTo(QStringLiteral("uid1"), Incidence::RelTypeParent);
    todo6->setHasGeo(true);
    todo6->setGeoLatitude(40);
    todo6->setGeoLongitude(40);
    todo6->setUid(QStringLiteral("uid22"));
    todo6->setLastModified(today);
    todo6->addContact(QStringLiteral("addContact"));

    // Remaining properties tested in testevent.cpp

    QTest::newRow("todo1") << todo1;
    QTest::newRow("todo2") << todo2;
    QTest::newRow("todo3") << todo3;
    QTest::newRow("todo4") << todo4;
    QTest::newRow("todo5") << todo5;
    QTest::newRow("todo6") << todo6;
}

void TodoTest::testSerializer()
{
    QFETCH(KCalCore::Todo::Ptr, todo);
    IncidenceBase::Ptr incidenceBase = todo.staticCast<KCalCore::IncidenceBase>();

    QByteArray array;
    QDataStream stream(&array, QIODevice::WriteOnly);
    stream << incidenceBase;

    Todo::Ptr todo2 = Todo::Ptr(new Todo());
    IncidenceBase::Ptr incidenceBase2 = todo2.staticCast<KCalCore::IncidenceBase>();
    QVERIFY(*todo != *todo2);
    QDataStream stream2(&array, QIODevice::ReadOnly);
    stream2 >> incidenceBase2;
    QVERIFY(*todo == *todo2);
}

void TodoTest::testRoles()
{
    const QDateTime today = QDateTime::currentDateTimeUtc();
    const QDateTime yesterday = today.addDays(-1);
    Todo todo;
    todo.setDtStart(today.addDays(-1));
    todo.setDtDue(today);
    QCOMPARE(todo.dateTime(Incidence::RoleDisplayStart), today);
    QCOMPARE(todo.dateTime(Incidence::RoleDisplayEnd), today);
    todo.setDtDue(QDateTime());
    QCOMPARE(todo.dateTime(Incidence::RoleDisplayStart), yesterday);
    QCOMPARE(todo.dateTime(Incidence::RoleDisplayEnd), yesterday);
}
