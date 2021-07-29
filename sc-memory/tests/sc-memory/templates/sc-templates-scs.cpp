#include <gtest/gtest.h>

#include "sc_test.hpp"

#include "sc-memory/sc_memory.hpp"
#include "template_test_utils.hpp"

using ScTemplateSCsTest = ScTemplateTest;

TEST_F(ScTemplateSCsTest, build_ok)
{
  ScAddr const addr = m_ctx->CreateNode(ScType::NodeConst);
  EXPECT_TRUE(addr.IsValid());
  EXPECT_TRUE(m_ctx->HelperSetSystemIdtf("d", addr));

  char const * data = "_a _-> d;; _a <- sc_node_class;;";
  EXPECT_TRUE(ScTemplateSCsBuilder(*m_ctx).Make(data));
}

TEST_F(ScTemplateSCsTest, build_fail)
{
  char const * data = "_a _-> b";
  EXPECT_FALSE(ScTemplateSCsBuilder(*m_ctx).Make(data));
}

TEST_F(ScTemplateSCsTest, search)
{
  ScTemplatePtr genTempl =
      ScTemplateBuilder()
          .Triple(ScType::NodeVar >> "_a", ScType::EdgeAccessVarPosPerm >> "_edge", ScType::NodeVarTuple >> "b")
          .Make();

  ScTemplateGenerate::Result genResult = ScTemplateGenerate(*m_ctx, *genTempl).Do();
  EXPECT_TRUE(genResult);

  ScAddr const bAddr = (*genResult)["b"];
  EXPECT_TRUE(bAddr.IsValid());
  EXPECT_TRUE(m_ctx->HelperSetSystemIdtf("b", bAddr));

  // do search
  char const * data = "_a _-> b (* <- sc_node_tuple;; *);;";
  ScTemplatePtr templ = ScTemplateSCsBuilder(*m_ctx).Make(data);
  EXPECT_TRUE(templ);

  ScTemplateSearch search(*m_ctx, *templ);
  ScTemplateSearch::Iterator found = search.begin();
  EXPECT_NE(found, search.end());

  EXPECT_EQ(found["_a"], (*genResult)["_a"]);
  EXPECT_EQ(found["b"], (*genResult)["b"]);
  EXPECT_EQ(++found, search.end());
}

TEST_F(ScTemplateSCsTest, generate)
{
  ScAddr const cAddr = m_ctx->CreateNode(ScType::Node);
  EXPECT_TRUE(cAddr.IsValid());
  EXPECT_TRUE(m_ctx->HelperSetSystemIdtf("c1", cAddr));

  char const * data = "c1 _=> _b1;; _b1 <- sc_node_abstract;;";
  ScTemplatePtr templ = ScTemplateSCsBuilder(*m_ctx).Make(data);
  EXPECT_TRUE(templ);

  ScTemplateGenerate::Result genResult = ScTemplateGenerate(*m_ctx, *templ).Do();
  EXPECT_TRUE(genResult);

  // check
  ScTemplatePtr searchTempl =
      ScTemplateBuilder()
          .Triple(cAddr >> "c1", ScType::EdgeDCommonVar >> "_edge", ScType::NodeVarAbstract >> "_b1")
          .Make();

  ScTemplateSearch search(*m_ctx, *searchTempl);
  ScTemplateSearch::Iterator searchResult = search.begin();
  EXPECT_NE(searchResult, search.end());

  EXPECT_EQ((*genResult)["c1"], searchResult["c1"]);
  EXPECT_EQ((*genResult)["_b1"], searchResult["_b1"]);
  EXPECT_EQ(++searchResult, search.end());
}

TEST_F(ScTemplateSCsTest, generate_and_search)
{
  ScAddr const cAddr = m_ctx->CreateNode(ScType::Node);
  EXPECT_TRUE(cAddr.IsValid());
  EXPECT_TRUE(m_ctx->HelperSetSystemIdtf("g1", cAddr));

  char const * data = "g1 _-> _l1 (* <- sc_node_material;; *);; g1 _-> _f1;;";
  ScTemplatePtr templ = ScTemplateSCsBuilder(*m_ctx).Make(data);
  EXPECT_TRUE(templ);

  ScTemplateGenerate::Result genResult = ScTemplateGenerate(*m_ctx, *templ).Do();
  EXPECT_TRUE(genResult);
  EXPECT_EQ(m_ctx->GetElementType((*genResult)["_l1"]), ScType::NodeConstMaterial);
  EXPECT_EQ(m_ctx->GetElementType((*genResult)["_f1"]), ScType::NodeConst);

  // check
  ScTemplateSearch search(*m_ctx, *templ);
  ScTemplateSearch::Iterator searchResult = search.begin();
  EXPECT_NE(searchResult, search.end());

  EXPECT_EQ((*genResult)["g1"], searchResult["g1"]);
  EXPECT_EQ((*genResult)["_l1"], searchResult["_l1"]);
  EXPECT_EQ((*genResult)["_f1"], searchResult["_f1"]);
  EXPECT_EQ(++searchResult, search.end());
}

TEST_F(ScTemplateSCsTest, idtf_does_not_exist)
{
  ScTemplateSCsBuilder builder(*m_ctx);
  ScTemplatePtr templ = builder.Make("non_existing_item _-> _z;;");
  EXPECT_FALSE(templ);
  EXPECT_GT(builder.GetErrorMessage().size(), 0u);
}
