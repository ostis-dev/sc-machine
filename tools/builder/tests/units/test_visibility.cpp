#include <gtest/gtest.h>

#include "sc-memory/sc_link.hpp"

#include "builder_test.hpp"

namespace
{

std::string GetIdtf(ScMemoryContext & ctx, ScAddr const& addr)
{
  ScAddr const nrelIdtf = ctx.HelperResolveSystemIdtf("nrel_idtf", ScType::NodeConstNoRole);
  EXPECT_TRUE(nrelIdtf.IsValid());

  ScTemplatePtr templ = ScTemplateBuilder()
    .TripleWithRelation(
      addr,
      ScType::EdgeDCommonVar,
      ScType::Link >> "_link",
      ScType::EdgeAccessVarPosPerm,
      nrelIdtf)
    .Make();

  ScTemplateSearch search(ctx, *templ);
  ScTemplateSearch::Iterator found = search.begin();
  EXPECT_NE(found, search.end());

  ScAddr const linkAddr = found["_link"];
  EXPECT_TRUE(linkAddr.IsValid());
  EXPECT_EQ(++found, search.end());

  ScLink link(ctx, linkAddr);
  EXPECT_TRUE(link.IsType<std::string>());

  return link.Get<std::string>();
};

} // namespace

TEST_F(ScBuilderTest, visibility_sys_idtf)
{
  ScAddr const visFirst = m_ctx->HelperResolveSystemIdtf("visibility_first");
  EXPECT_TRUE(visFirst.IsValid());

  ScAddr const visSecond = m_ctx->HelperResolveSystemIdtf("visibility_second");
  EXPECT_TRUE(visSecond.IsValid());

  ScAddr const element = m_ctx->HelperResolveSystemIdtf("visibility_sys_idtf");
  EXPECT_TRUE(element.IsValid());

  ScTemplatePtr templ = ScTemplateBuilder()
    .Triple(
      visFirst,
      ScType::EdgeAccessVarPosPerm,
      element)
    .Triple(
      visSecond,
      ScType::EdgeAccessVarPosPerm,
      element)
    .Make();

  ScTemplateSearch search(*m_ctx, *templ);
  ScTemplateSearch::Iterator found = search.begin();
  EXPECT_NE(found, search.end());
}

TEST_F(ScBuilderTest, visibility_global)
{
  ScAddr const visFirst = m_ctx->HelperResolveSystemIdtf("visibility_first_global");
  EXPECT_TRUE(visFirst.IsValid());

  ScAddr const visSecond = m_ctx->HelperResolveSystemIdtf("visibility_second_global");
  EXPECT_TRUE(visSecond.IsValid());

  ScTemplatePtr templ = ScTemplateBuilder()
    .Triple(
      visFirst,
      ScType::EdgeAccessVarPosTemp,
      ScType::Node >> ".visibility_global")
    .Triple(
      visSecond,
      ScType::EdgeAccessVarPosTemp,
      ".visibility_global")
    .Make();

  ScTemplateSearch search(*m_ctx, *templ);
  ScTemplateSearch::Iterator found = search.begin();
  EXPECT_NE(found, search.end());

  ScAddr const element = found[".visibility_global"];
  EXPECT_TRUE(element.IsValid());

  EXPECT_EQ(++found, search.end());

  EXPECT_EQ(GetIdtf(*m_ctx, element), ".visibility_global");
}

TEST_F(ScBuilderTest, visibility_local)
{
  ScAddr const visFirst = m_ctx->HelperResolveSystemIdtf("visibility_first_local");
  EXPECT_TRUE(visFirst.IsValid());

  ScAddr const visSecond = m_ctx->HelperResolveSystemIdtf("visibility_second_local");
  EXPECT_TRUE(visSecond.IsValid());

  ScAddr const visLocal = m_ctx->HelperResolveSystemIdtf("visibility_local");
  EXPECT_TRUE(visLocal.IsValid());

  ScTemplatePtr templ = ScTemplateBuilder()
    .Triple(
      visLocal,
      ScType::EdgeAccessVarPosPerm,
      ScType::NodeVar >> "_local")
    .Make();

  ScTemplateSearch search(*m_ctx, *templ);
  ScTemplateSearch::Iterator found = search.begin();
  EXPECT_NE(found, search.end());

  std::unordered_map<std::string, ScAddr> elements;
  while (found != search.end())
  {
    ScAddr const a = found["_local"];
    EXPECT_TRUE(a.IsValid());

    std::string const idtf = GetIdtf(*m_ctx, a);
    EXPECT_FALSE(idtf.empty());

    elements.insert({idtf, a});
    ++found;
  }

  EXPECT_EQ(elements.size(), 2u);

  ScAddr const localFirst = elements["..visibility_local_first"];
  EXPECT_TRUE(localFirst.IsValid());

  ScAddr const localSecond = elements["..visibility_local_second"];
  EXPECT_TRUE(localSecond.IsValid());

  EXPECT_NE(localFirst, localSecond);
}
