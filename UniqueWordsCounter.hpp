#ifndef UNIQUEWORDSCOUNTER_UNIQUEWORDSCOUNTER
#define UNIQUEWORDSCOUNTER_UNIQUEWORDSCOUNTER

#include <string>
#include <fstream>
#include <unordered_set>


class UniqueWordsCounter {
public:
    UniqueWordsCounter(const std::string &filename, const size_t &filesize);
    
    UniqueWordsCounter(UniqueWordsCounter &&) = delete;
    UniqueWordsCounter(const UniqueWordsCounter &) = delete;
    UniqueWordsCounter& operator=(const UniqueWordsCounter &) = delete;
    UniqueWordsCounter& operator=(UniqueWordsCounter &&) = delete;

    void LoadUniqueWordsSet(const std::pair<size_t, size_t> &startEndPos);
    const std::unordered_set<std::string>& GetUniqueWordsSet() const;

private:
    void _readFileChunk(size_t startPos, size_t endPos, std::stringstream &outFileChunk);
    /**
     * Used to prevent single-word split
     */
    void _adjustReaderPosition(size_t &pos);

private:
    const std::string m_filename;
    const size_t m_filesize;
    std::ifstream m_file;
    std::unordered_set<std::string> m_uniqueWords;
};

#endif // UNIQUEWORDSCOUNTER_UNIQUEWORDSCOUNTER