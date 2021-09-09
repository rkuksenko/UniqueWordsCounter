#ifndef UNIQUEWORDSCOUNTER_UNIQUEWORDSCOUNTERMANAGER
#define UNIQUEWORDSCOUNTER_UNIQUEWORDSCOUNTERMANAGER

#include <string>
#include <queue>
#include <condition_variable>

class UniqueWordsCounter;
using TasksQueue = std::queue<std::pair<size_t, size_t>>;
using UniqueWordsCounterPtr = std::shared_ptr<UniqueWordsCounter>;


class UniqueWordsCounterManager {
public:
    UniqueWordsCounterManager(const std::string &filename, size_t workersCount);
    size_t CountUniqueWords();

private:
    size_t _adjustWorkersCount();
    void _loadUniqueWordsCounterTasks(TasksQueue &outStartEndPosTasks) const;
    size_t _calculateTotalWordsCount(const std::vector<UniqueWordsCounterPtr> &counters) const;
    static void _workerFunc(UniqueWordsCounterPtr counter, TasksQueue &startEndPosTasks,
                            std::condition_variable &cv, std::mutex &tasksLock, bool &isNoMoreTasks);

private:
    size_t m_workersCount;
    const std::string m_filename;
    size_t m_filesize;
    size_t m_chunkSizeInBytes;

    static const size_t s_effectiveMaxChunkSizePerWorker;
    static const size_t s_effectiveMinChunkSizePerWorker;
};

#endif // UNIQUEWORDSCOUNTER_UNIQUEWORDSCOUNTERMANAGER