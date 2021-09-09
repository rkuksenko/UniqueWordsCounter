#include <sstream>

#include "UniqueWordsCounter.hpp"


UniqueWordsCounter::UniqueWordsCounter(const std::string &filename, const size_t &filesize) :
    m_filename(filename),
    m_filesize(filesize) {
        m_file.open(filename.c_str());
}

void UniqueWordsCounter::LoadUniqueWordsSet(const std::pair<size_t, size_t> &startEndPos) {
    std::stringstream fileChunkSs;
    _readFileChunk(startEndPos.first, startEndPos.second, fileChunkSs);

    std::string word;
    while (getline(fileChunkSs, word, ' ')) {
        m_uniqueWords.insert(word);
    }
}

const std::unordered_set<std::string>& UniqueWordsCounter::GetUniqueWordsSet() const {
    return m_uniqueWords;
}

void UniqueWordsCounter::_readFileChunk(size_t startPos, size_t endPos, std::stringstream &outFileChunk) {
    m_file.seekg(startPos);
    _adjustReaderPosition(startPos);

    if (endPos != m_filesize) {
        _adjustReaderPosition(endPos);
    }

    std::string strChunk(endPos - startPos, '\0');
    m_file.seekg(startPos);
    m_file.read(&strChunk[0], endPos - startPos);
    outFileChunk << strChunk;
}

void UniqueWordsCounter::_adjustReaderPosition(size_t &pos) {
    while (static_cast<char> (m_file.get()) != ' ' && pos != 0) {
        m_file.seekg(--pos);
    }
    if (pos != 0) {
        ++pos;
    }
}
