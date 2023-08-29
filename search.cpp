#include "thread_pool.h"
#include <iostream>
#include <filesystem>
#include <functional>
#include <cstring>
#include <list>
#include <mutex>

thread_pool pool(8);
std::list<std::future<int>> futures;
std::mutex futures_m;

namespace fs = std::filesystem;
int rec_search(fs::path root_path, char *filename)
{
  try {
    for (const auto & entry : fs::directory_iterator(root_path))
    {
      if (entry.path().filename().compare(filename) == 0)
      {
        std::cout << entry.path() << " FILE FIND!" << '\n';
        return 1;
      }
      else
      {
        if (entry.is_directory())
        {
          futures_m.lock();
          futures.push_back(pool.submit([=]()
          {
            return rec_search(entry.path(), filename);
            }));
            futures_m.unlock();
          }
      }
    }
  } catch (const fs::filesystem_error& e)
  {
    std::cout << e.what() << std::endl;
  }
  return 0;
}

int search (char *path, char *filename)
{
  int result = 0;
  fs::path root_path = path; 
  futures_m.lock();                   
  futures.push_back(pool.submit([=]()
    {
      return rec_search(root_path, filename);
    }));
  futures_m.unlock();
  for (auto iter = futures.begin(); iter != futures.end(); iter++)
  {
    if ((*iter).get() == 1)
    {
      result = 1;
    }
  }
  return result;
}
            