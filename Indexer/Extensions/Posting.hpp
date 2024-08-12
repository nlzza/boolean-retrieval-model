#pragma once
#ifndef POSTING_HPP
#define POSTING_HPP

#include "Document.hpp"

struct Posting
{
    unsigned doc_count{0}; // The number of docs in which the term appears
    unsigned total_count{0}; // The total number of times the term appears
    unsigned prev_docID = INVALID_DOC_ID;
    List<Document> documents; // List of docs in which term appears

    // Constructors
    Posting() = default;

    Posting(const unsigned &doc_ID, const unsigned &pos)
    {
        push_directly(doc_ID, pos);
    }

    Posting(const Posting &other)
        : documents(other.documents), doc_count(other.doc_count) {}

    Posting &operator=(const Posting &other)
    {
        if (this == &other)
            return *this;

        this->doc_count = other.doc_count;
        this->documents = other.documents;
        return *this;
    }

    // Add a new position of the term in the doc or a new doc all together
    void push_directly(const unsigned &doc_ID, const unsigned &pos)
    {
        total_count++;
        if (prev_docID == doc_ID)
            documents.last()->data.update(pos);
        else
        {
            prev_docID = doc_ID;
            doc_count++;
            documents.push_back(Document(doc_ID, pos));
        }
    }
};

#endif
