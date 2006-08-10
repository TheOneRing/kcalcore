/*
 * This file is part of libsyndication
 *
 * Copyright (C) 2006 Frank Osterfeld <frank.osterfeld@kdemail.net>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 *
 */

#ifndef SYNDICATION_MAPPER_FEEDRDFIMPL_H
#define SYNDICATION_MAPPER_FEEDRDFIMPL_H

#include <rdf/document.h>
#include <feed.h>

namespace Syndication {

class FeedRDFImpl;
typedef SharedPtr<FeedRDFImpl> FeedRDFImplPtr;
class Image;
typedef SharedPtr<Image> ImagePtr;

/**
 * @internal
 */
class FeedRDFImpl : public Syndication::Feed
{
    public:
        
        FeedRDFImpl(Syndication::RDF::DocumentPtr doc);
        
        Syndication::SpecificDocumentPtr specificDocument() const;
        
        QList<ItemPtr> items() const;
        
        QList<CategoryPtr> categories() const;
        
        QString title() const;
        
        QString link() const;
        
        QString description() const;
        
        QList<PersonPtr> authors() const;
        
        QString language() const;
        
        QString copyright() const;
        
        ImagePtr image() const;
        
    private:
        
        Syndication::RDF::DocumentPtr m_doc;
};

} // namespace Syndication

#endif // SYNDICATION_MAPPER_FEEDRDFIMPL_H
