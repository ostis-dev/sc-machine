#include <gtest/gtest.h>

#include "builder_test.hpp"

TEST_F(ScBuilderTest, aliases)
{
  std::string const scsTempl = "test_element _-> _[] (* _<- alias_content;; *);;";

  ScTemplatePtr templ = ScTemplateSCsBuilder(*m_ctx).Make(scsTempl);
  EXPECT_TRUE(templ);

  ScTemplateSearch search(*m_ctx, *templ);
  ScTemplateSearch::Iterator found = search.begin();

  EXPECT_TRUE(found != search.end());
  EXPECT_EQ(++found == search.end(), 1u);
}
