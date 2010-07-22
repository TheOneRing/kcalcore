/*
    Copyright (c) 2009 Constantin Berzan <exit3219@gmail.com>

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

#include "specialmailcollections.h"

#include "specialmailcollectionssettings.h"

#include <KGlobal>

#include "akonadi/agentinstance.h"

using namespace Akonadi;

class Akonadi::SpecialMailCollectionsPrivate
{
  public:
    SpecialMailCollectionsPrivate();
    ~SpecialMailCollectionsPrivate();

    SpecialMailCollections *mInstance;
};

typedef SpecialMailCollectionsSettings Settings;

K_GLOBAL_STATIC( SpecialMailCollectionsPrivate, sInstance )

static inline QByteArray enumToType( SpecialMailCollections::Type type )
{
  switch ( type ) {
    case SpecialMailCollections::Root: return "local-mail"; break;
    case SpecialMailCollections::Inbox: return "inbox"; break;
    case SpecialMailCollections::Outbox: return "outbox"; break;
    case SpecialMailCollections::SentMail: return "sent-mail"; break;
    case SpecialMailCollections::Trash: return "trash"; break;
    case SpecialMailCollections::Drafts: return "drafts"; break;
    case SpecialMailCollections::Templates: return "templates"; break;
    case SpecialMailCollections::LastType: // fallthrough
    default: return QByteArray(); break;
  }
}

SpecialMailCollectionsPrivate::SpecialMailCollectionsPrivate()
  : mInstance( new SpecialMailCollections( this ) )
{
}

SpecialMailCollectionsPrivate::~SpecialMailCollectionsPrivate()
{
  delete mInstance;
}

SpecialMailCollections::SpecialMailCollections( SpecialMailCollectionsPrivate *dd )
  : SpecialCollections( Settings::self() ),
    d( dd )
{
}

SpecialMailCollections *SpecialMailCollections::self()
{
  return sInstance->mInstance;
}

bool SpecialMailCollections::hasCollection( Type type, const AgentInstance &instance ) const
{
  return SpecialCollections::hasCollection( enumToType( type ), instance );
}

Collection SpecialMailCollections::collection( Type type, const AgentInstance &instance ) const
{
  return SpecialCollections::collection( enumToType( type ), instance );
}

bool SpecialMailCollections::registerCollection( Type type, const Collection &collection )
{
  return SpecialCollections::registerCollection( enumToType( type ), collection );
}

bool SpecialMailCollections::hasDefaultCollection( Type type ) const
{
  return SpecialCollections::hasDefaultCollection( enumToType( type ) );
}

Collection SpecialMailCollections::defaultCollection( Type type ) const
{
  return SpecialCollections::defaultCollection( enumToType( type ) );
}

#include "specialmailcollections.moc"