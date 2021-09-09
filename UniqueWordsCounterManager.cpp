#include <iostream>
#include <fstream>
#include <vector>
#include <memory>
#include <algorithm>
#include <thread>
#include <mutex>

#include "UniqueWordsCounterManager.hpp"
#include "UniqueWordsCounter.hpp"

namespace {
size_t getFileSize(const std::string &filename) {
    std::ifstream file(filename, std::ifstream::ate);
    return file.tellg();
}
};

const size_t UniqueWordsCounterManager::s_effectiveMinChunkSizePerWorker = 32768;
const size_t UniqueWordsCounterManager::s_effectiveMaxChunkSizePerWorker = 2097152;


UniqueWordsCounterManager::UniqueWordsCounterManager(const std::string &filename, size_t workersCount)
    : m_filename(filename),
      m_workersCount(workersCount) {
          m_filesize = getFileSize(filename);
          m_workersCount = _adjustWorkersCount();
          m_chunkSizeInBytes = (m_filesize > s_effectiveMaxChunkSizePerWorker * 2) ? s_effectiveMaxChunkSizePerWorker : m_filesize;
          std::cout << "[UniqueWordsCounterManager::UniqueWordsCounterManager()] "
                    << "filesize = " << m_filesize << ", workers count = " << m_workersCount
                    << ", fileChunk per worker in bytes = " << m_chunkSizeInBytes << std::endl;
}

size_t UniqueWordsCounterManager::CountUniqueWords() {
    std::vector<UniqueWordsCounterPtr> counters;
    std::vector<std::thread> workers;
    counters.reserve(m_workersCount);
    workers.reserve(m_workersCount);

    TasksQueue startEndPosTasks;
    std::condition_variable cv;
    std::mutex tasksLock;
    bool isNoMoreTasks = false;

    for (size_t i = 0; i < m_workersCount; i++) {
        counters.emplace_back(std::make_shared<UniqueWordsCounter>(m_filename, m_filesize));
        workers.emplace_back(UniqueWordsCounterManager::_workerFunc, counters[i], std::ref(startEndPosTasks),
                             std::ref(cv), std::ref(tasksLock), std::ref(isNoMoreTasks));
    }

    _loadUniqueWordsCounterTasks(startEndPosTasks);
    cv.notify_all();

    std::for_each(workers.begin(), workers.end(), [](std::thread &t){ t.join();});
    return _calculateTotalWordsCount(counters);
}

void UniqueWordsCounterManager::_workerFunc(UniqueWordsCounterPtr wordsCounter, TasksQueue &startEndPosTasks,
                                                  std::condition_variable &cv, std::mutex &tasksLock, bool &isNoMoreTasks) {
    while (true) {
        std::unique_lock<std::mutex> lock(tasksLock);
        cv.wait(lock, [&startEndPosTasks, &isNoMoreTasks]{ return !startEndPosTasks.empty() || isNoMoreTasks; });

        if (isNoMoreTasks) {
            return;
        }

        std::pair<size_t, size_t> startEndPos = startEndPosTasks.front();
        startEndPosTasks.pop();
        isNoMoreTasks = startEndPosTasks.empty();
        lock.unlock();

        wordsCounter->LoadUniqueWordsSet(startEndPos);
        cv.notify_one();
    }
}

void UniqueWordsCounterManager::_loadUniqueWordsCounterTasks(TasksQueue &outStartEndPosTasks) const {
    size_t currentReadStartPos = 0;
    size_t currentReadEndPos = 0;
    do {
        currentReadStartPos = (currentReadEndPos == 0)? 0 : currentReadEndPos - 1;
        currentReadEndPos += m_chunkSizeInBytes;
        if (currentReadEndPos > m_filesize) {
            currentReadEndPos = m_filesize;
        }
        outStartEndPosTasks.emplace(std::make_pair(currentReadStartPos, currentReadEndPos));
    } while (currentReadEndPos < m_filesize);

}

size_t UniqueWordsCounterManager::_adjustWorkersCount() {
    size_t adjustedWorkersCount = m_workersCount;

    if (m_filesize / m_workersCount <= s_effectiveMinChunkSizePerWorker) {
        adjustedWorkersCount = std::ceil(static_cast<double> (m_filesize) / s_effectiveMinChunkSizePerWorker);
    }

    return adjustedWorkersCount;
}

size_t UniqueWordsCounterManager::_calculateTotalWordsCount(const std::vector<UniqueWordsCounterPtr> &counters) const {
    std::unordered_set<std::string> result;

    for (const auto &counter : counters) {
        const auto &wordsSet = counter->GetUniqueWordsSet();
        std::copy(wordsSet.begin(), wordsSet.end(), std::inserter(result, std::next(result.begin())));
    }

    return result.size();
}