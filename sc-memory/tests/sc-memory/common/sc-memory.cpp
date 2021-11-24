#include <gtest/gtest.h>

#include "sc_test.hpp"

#include "sc-memory/sc_link.hpp"
#include "sc-memory/sc_memory.hpp"

TEST_F(ScMemoryTest, LinkContent)
{
  std::string str("test content string");
  ScStreamPtr const stream = ScStreamMakeRead(str);

  ScAddr const link = m_ctx->CreateLink();

  EXPECT_TRUE(link.IsValid());
  EXPECT_TRUE(m_ctx->IsElement(link));
  EXPECT_TRUE(m_ctx->SetLinkContent(link, stream));

  ScStreamPtr const stream2 = m_ctx->GetLinkContent(link);
  EXPECT_TRUE(m_ctx->GetLinkContent(link));

  EXPECT_EQ(stream->Size(), stream2->Size());
  EXPECT_TRUE(stream2->Seek(SC_STREAM_SEEK_SET, 0));

  std::string str2;
  str2.resize(stream2->Size());
  for (uint32_t i = 0; i < stream2->Size(); ++i)
  {
    sc_char c;
    size_t readBytes;
    EXPECT_TRUE(stream2->Read(&c, sizeof(c), readBytes));
    EXPECT_EQ(readBytes, sizeof(c));

    str2[i] = c;
  }

  EXPECT_EQ(str, str2);

  ScAddrVector const result = m_ctx->FindLinksByContent(stream);
  EXPECT_EQ(result.size(), 1u);
  EXPECT_EQ(result.front(), link);
}

TEST_F(ScMemoryTest, FindByLinkContent)
{
  ScAddr const linkAddr1 = m_ctx->CreateLink();
  EXPECT_TRUE(linkAddr1.IsValid());

  std::string const linkContent1 = "ScMemoryContext_FindLinksByContent_content_1";
  ScLink link1(*m_ctx, linkAddr1);
  EXPECT_TRUE(link1.Set(linkContent1));

  ScAddrVector const result = m_ctx->FindLinksByContent(linkContent1);
  EXPECT_EQ(result.size(), 1u);
}
