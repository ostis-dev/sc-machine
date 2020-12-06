#include <gtest/gtest.h>

#include "sc-memory/sc_memory.hpp"
#include "sc-memory/sc_scs_helper.hpp"

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

  ScTemplate templ;
  templ.Triple(
        aAddr,
        ScType::EdgeAccessVarPosPerm,
        ScType::NodeVarStruct >> "_struct");

  templ.Triple(
        classAddr,
        ScType::EdgeAccessVarPosPerm >> "_edge",
        _xAddr);

  templ.Triple(
        "_struct",
        ScType::EdgeAccessVarPosPerm,
        classAddr);

  templ.Triple(
        "_struct",
        ScType::EdgeAccessVarPosPerm,
        "_edge");

  templ.Triple(
        "_struct",
        ScType::EdgeAccessVarPosPerm,
        _xAddr);


  ScTemplateSearchResult result;
  EXPECT_TRUE(m_ctx->HelperSearchTemplate(templ, result));

  EXPECT_EQ(result.Size(), 1u);
  EXPECT_EQ(m_ctx->GetElementType(result[0]["_struct"]), ScType::NodeConstStruct);
}
