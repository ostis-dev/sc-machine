#include <gtest/gtest.h>

#include "sc-memory/sc_link.hpp"
#include "sc-memory/sc_templates.hpp"

#include "builder_test.hpp"

TEST_F(ScBuilderTest, clean_global_idtfs)
{
  ScAddr const nrelSCsIdtf = m_ctx->HelperResolveSystemIdtf("nrel_scs_global_idtf");
  EXPECT_TRUE(nrelSCsIdtf.IsValid());

  ScTemplatePtr templ = ScTemplateBuilder()
    .TripleWithRelation(
      ScType::Unknown,
      ScType::EdgeDCommonVar,
      ScType::Link >> "_link",
      ScType::EdgeAccessVarPosPerm,
      nrelSCsIdtf)
    .Make();

  ScTemplateSearch search(*m_ctx, *templ);
  EXPECT_EQ(search.begin(), search.end());
}
