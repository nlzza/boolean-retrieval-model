#pragma once
#ifndef DOCUMENT_HPP
#define DOCUMENT_HPP

#include "List.hpp"
#define INVALID_DOC_ID (-1)

struct Document
{
    const unsigned ID; // Doc ID
    unsigned term_freq{0}; // freq of term in doc
    List<unsigned> positions; // The positions at which the term appears

    // Constructors
    Document()
        : ID(0) {}

    Document(const unsigned &ID)
        : ID(ID) {}

    Document(const unsigned &ID, const unsigned &pos)
        : ID(ID)
    {
        this->update(pos);
    }

    Document(const Document &other)
        : ID(other.ID), term_freq(other.term_freq), positions(other.positions) {}

    Document &operator=(const Document &other)
    {
        if (this == &other)
            return *this;

        this->term_freq = other.term_freq;
        this->positions = other.positions;
        return *this;
    }

    // Add a new position of the term in the doc
    void update(const unsigned &pos)
    {
        term_freq++;
        positions.push_back(pos);
    }

    bool operator==(const Document &other) const
    {
        return this->ID == other.ID;
    }
};

#endif
