/*
    Copyright (c) 2007 Volker Krause <vkrause@kde.org>

    This library is free software; you can redistribute it and/or modify it
    under the terms of the GNU Library General Public License as published by
    the Free Software Foundation; either version 2 of the License, or (at your
    option) any later version.

    This library is distributed in the hope that it will be useful, but WITHOUT
    ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
    FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Library General Public
    License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to the
    Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
    02110-1301, USA.
*/

#include "collectionattributetest.h"
#include "collectionattributetest.moc"

#include <akonadi/collection.h>
#include <akonadi/attributefactory.h>
#include <akonadi/collectioncreatejob.h>
#include <akonadi/collectiondeletejob.h>
#include <akonadi/collectionfetchjob.h>
#include <akonadi/collectionmodifyjob.h>
#include <akonadi/collectionpathresolver.h>
#include <akonadi/control.h>

#include <qtest_kde.h>

using namespace Akonadi;

QTEST_KDEMAIN( CollectionAttributeTest, NoGUI )

class TestAttribute : public Attribute
{
  public:
    TestAttribute() : Attribute() {}
    TestAttribute( const QByteArray &data ) : mData( data ) {};
    TestAttribute* clone() const { return new TestAttribute( mData ); }
    QByteArray type() const { return "TESTATTRIBUTE"; }
    QByteArray serialized() const { return mData; }
    void deserialize( const QByteArray &data ) { mData = data; }
  private:
    QByteArray mData;
};

static int parentColId = -1;

void CollectionAttributeTest::initTestCase()
{
  Control::start();
  AttributeFactory::registerAttribute<TestAttribute>();

  CollectionPathResolver *resolver = new CollectionPathResolver( "res3", this );
  QVERIFY( resolver->exec() );
  parentColId = resolver->collection();
  QVERIFY( parentColId > 0 );
}

void CollectionAttributeTest::testAttributes_data()
{
  QTest::addColumn<QByteArray>("attr1");
  QTest::addColumn<QByteArray>("attr2");

  QTest::newRow("basic") << QByteArray("foo") << QByteArray("bar");
  QTest::newRow("emtpy1") << QByteArray("") << QByteArray("non-empty");
  QTest::newRow("empty2") << QByteArray("non-empty") << QByteArray("");
  QTest::newRow("space") << QByteArray("foo bar") << QByteArray("bar foo");
  QTest::newRow("quotes") << QByteArray("\"quoted \\ test\"") << QByteArray("single \" quote \\");
  QTest::newRow("parenthesis") << QByteArray(")") << QByteArray("(");
  QTest::newRow("binary") << QByteArray("\000") << QByteArray("\001");
}

void CollectionAttributeTest::testAttributes()
{
  QFETCH( QByteArray, attr1 );
  QFETCH( QByteArray, attr2 );

  // add a custom attribute
  TestAttribute *attr = new TestAttribute();
  attr->deserialize( attr1 );
  Collection col;
  col.setName( "attribute test" );
  col.setParent( parentColId );
  col.addAttribute( attr );
  CollectionCreateJob *create = new CollectionCreateJob( col, this );
  QVERIFY( create->exec() );
  col = create->collection();
  QVERIFY( col.isValid() );

  attr = col.attribute<TestAttribute>();
  QVERIFY( attr != 0 );
  QCOMPARE( attr->serialized(), QByteArray( attr1 ) );

  CollectionFetchJob *list = new CollectionFetchJob( col, CollectionFetchJob::Local, this );
  QVERIFY( list->exec() );
  QCOMPARE( list->collections().count(), 1 );
  col = list->collections().first();

  QVERIFY( col.isValid() );
  attr = col.attribute<TestAttribute>();
  QVERIFY( attr != 0 );
  QCOMPARE( attr->serialized(), QByteArray( attr1 ) );


  // modify a custom attribute
  col.attribute<TestAttribute>( Collection::AddIfMissing )->deserialize( attr2 );
  CollectionModifyJob *modify = new CollectionModifyJob( col, this );
  QVERIFY( modify->exec() );

  list = new CollectionFetchJob( col, CollectionFetchJob::Local, this );
  QVERIFY( list->exec() );
  QCOMPARE( list->collections().count(), 1 );
  col = list->collections().first();

  QVERIFY( col.isValid() );
  attr = col.attribute<TestAttribute>();
  QVERIFY( attr != 0 );
  QCOMPARE( attr->serialized(), QByteArray( attr2 ) );


  // delete a custom attribute
  col.removeAttribute<TestAttribute>();
  modify = new CollectionModifyJob( col, this );
  QVERIFY( modify->exec() );

  list = new CollectionFetchJob( col, CollectionFetchJob::Local, this );
  QVERIFY( list->exec() );
  QCOMPARE( list->collections().count(), 1 );
  col = list->collections().first();

  QVERIFY( col.isValid() );
  attr = col.attribute<TestAttribute>();
  QVERIFY( attr == 0 );


  // cleanup
  CollectionDeleteJob *del = new CollectionDeleteJob( col, this );
  QVERIFY( del->exec() );

}

void CollectionAttributeTest::testDefaultAttributes()
{
  Collection col;
  QCOMPARE( col.attributes().count(), 0 );
  Attribute* attr = AttributeFactory::createAttribute( "TYPE" );
  QVERIFY( attr );
  attr->deserialize( "VALUE" );
  col.addAttribute( attr );
  QCOMPARE( col.attributes().count(), 1 );
  QVERIFY( col.hasAttribute( "TYPE" ) );
  QCOMPARE( col.attribute( "TYPE" )->serialized(), QByteArray("VALUE") );
}
