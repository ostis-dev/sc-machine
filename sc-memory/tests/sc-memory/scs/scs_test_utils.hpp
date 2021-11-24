#pragma once

#include "dummy_file_interface.hpp"

class TestFileInterface : public SCsFileInterface
{
public:
  ~TestFileInterface() override
  {
  }

  ScStreamPtr GetFileContent(std::string const & fileURL) override
  {
    std::string const content = "content: " + fileURL;
    return ScStreamConverter::StreamFromString(content);
  }
};
