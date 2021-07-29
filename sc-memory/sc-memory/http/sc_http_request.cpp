#include "sc_http_request.hpp"

#include "sc-memory/sc_debug.hpp"

#include <curl/curl.h>

namespace
{
size_t CurlWrite_CallbackFunc_StdString(void * contents, size_t size, size_t nmemb, std::string * s)
{
  size_t newLength = size * nmemb;
  size_t oldLength = s->size();
  try
  {
    s->resize(oldLength + newLength);
  }
  catch (std::bad_alloc &)
  {
    // handle memory problem
    return 0;
  }

  std::copy((char *)contents, (char *)contents + newLength, s->begin() + oldLength);
  return size * nmemb;
}

}  // namespace

ScHttpRequest::ScHttpRequest(std::string const & url)
  : m_handle(nullptr)
  , m_type(Type::GET)
{
  m_handle = (void *)curl_easy_init();
  if (!url.empty())
    SetURL(url);
}

ScHttpRequest::~ScHttpRequest()
{
  curl_easy_cleanup((CURL *)m_handle);
}

void ScHttpRequest::Perform()
{
  CURL * curl = (CURL *)m_handle;

  if (m_type == Type::POST)
  {
    curl_easy_setopt(curl, CURLOPT_POST, 1L);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, m_data.c_str());
  }
  else if (m_type == Type::PUT)
  {
    SC_NOT_IMPLEMENTED("Type::PUT not implemented yet");
  }

  curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
  curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);

  // prepare headers
  curl_slist * headers = nullptr;
  for (std::string const & h : m_headers)
    headers = curl_slist_append(headers, h.c_str());
  curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

  std::string responseString;
  curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, CurlWrite_CallbackFunc_StdString);
  curl_easy_setopt(curl, CURLOPT_WRITEDATA, &responseString);

  // perform request
  CURLcode const resultCode = curl_easy_perform(curl);
  curl_slist_free_all(headers);

  m_response.reset(new ScHttpResponse(responseString, (uint8_t)resultCode));
}

void ScHttpRequest::SetURL(std::string const & url)
{
  CURLcode const r = curl_easy_setopt((CURL *)m_handle, CURLOPT_URL, url.c_str());
  SC_ASSERT(r == CURLE_OK, ());
}

void ScHttpRequest::SetType(Type type)
{
  m_type = type;
}

void ScHttpRequest::SetData(std::string const & data)
{
  m_data = data;
}

void ScHttpRequest::SetHeaders(std::vector<std::string> && headers)
{
  m_headers = headers;
}

void ScHttpRequest::AddHeader(std::string const & header)
{
  m_headers.push_back(header);
}

ScHttpResponsePtr ScHttpRequest::GetResponse() const
{
  return m_response;
}