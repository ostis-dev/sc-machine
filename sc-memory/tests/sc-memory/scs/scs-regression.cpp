#include <gtest/gtest.h>

#include "sc-memory/sc_memory.hpp"
#include "sc-memory/sc_scs_helper.hpp"
#include "sc-memory/sc_templates.hpp"

#include "sc_test.hpp"

#include "scs_test_utils.hpp"

using SCsHelperRegressionTest = ScMemoryTest;

TEST_F(SCsHelperRegressionTest, issue_353)
{
  std::string const data =
      "a -> [*"
        "_x <- sc_node_class;;"
      "*];;";

  SCsHelper helper(*m_ctx, std::make_shared<TestFileInterface>());
  EXPECT_TRUE(helper.GenerateBySCsText(data));

  // try to find generated construction
  ScAddr const aAddr = m_ctx->HelperResolveSystemIdtf("a");
  EXPECT_TRUE(aAddr.IsValid());

  ScAddr const classAddr = m_ctx->HelperResolveSystemIdtf("sc_node_class");
  EXPECT_TRUE(classAddr.IsValid());

  ScAddr const _xAddr = m_ctx->HelperResolveSystemIdtf("_x");
  EXPECT_TRUE(_xAddr.IsValid());

  ScTemplatePtr templ = ScTemplateBuilder()
      .Triple(aAddr,
              ScType::EdgeAccessVarPosPerm,
              ScType::NodeVarStruct >> "_struct")
      .Triple(classAddr,
              ScType::EdgeAccessVarPosPerm >> "_edge",
              _xAddr)
      .Triple("_struct",
              ScType::EdgeAccessVarPosPerm,
              classAddr)
      .Triple("_struct",
              ScType::EdgeAccessVarPosPerm,
              "_edge")
      .Triple("_struct",
              ScType::EdgeAccessVarPosPerm,
              _xAddr)
      .Make();


  ScTemplateSearch result(*m_ctx, *templ);
  ScTemplateSearch::Iterator found = result.begin();
  EXPECT_NE(found, result.end());

  EXPECT_EQ(m_ctx->GetElementType(found["_struct"]), ScType::NodeConstStruct);
  EXPECT_EQ(++found, result.end());
}
