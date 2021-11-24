#include <gtest/gtest.h>

#include "sc_test.hpp"

#include "sc-memory/sc_memory.hpp"
#include "sc-memory/sc_struct.hpp"
#include "sc-memory/sc_templates.hpp"
#include "template_test_utils.hpp"

using ScTemplateBuidlerTest = ScTemplateTest;
using ScTemplateStructBuidlerTest = ScTemplateTest;

TEST_F(ScTemplateBuidlerTest, addrs_in_repls)
{
  ScAddr const addr = m_ctx->CreateNode(ScType::NodeConst);
  EXPECT_TRUE(addr);

  ScTemplatePtr templ = ScTemplateBuilder().Triple(addr >> "_x", ScType::EdgeAccessVarNegPerm, ScType::NodeVar).Make();

  EXPECT_TRUE(templ);
  EXPECT_TRUE(templ->HasReplacement("_x"));
  EXPECT_TRUE(templ->HasReplacement(addr.ToString()));
}

TEST_F(ScTemplateStructBuidlerTest, double_attrs)
{
  /**
   * addr1 _-> addr3:: addr4:: _addr2;;
   */
  ScAddr const addr1 = m_ctx->CreateNode(ScType::NodeConst);
  EXPECT_TRUE(addr1.IsValid());
  ScAddr const addr2 = m_ctx->CreateNode(ScType::NodeVar);
  EXPECT_TRUE(addr2.IsValid());
  ScAddr const addr3 = m_ctx->CreateNode(ScType::NodeConstRole);
  EXPECT_TRUE(addr3.IsValid());
  ScAddr const addr4 = m_ctx->CreateNode(ScType::NodeConstRole);
  EXPECT_TRUE(addr4.IsValid());

  ScAddr const edge1 = m_ctx->CreateEdge(ScType::EdgeAccessVarPosPerm, addr1, addr2);
  EXPECT_TRUE(edge1.IsValid());
  ScAddr const edge2 = m_ctx->CreateEdge(ScType::EdgeAccessVarPosPerm, addr3, edge1);
  EXPECT_TRUE(edge2.IsValid());
  ScAddr const edge3 = m_ctx->CreateEdge(ScType::EdgeAccessVarPosPerm, addr4, edge1);
  EXPECT_TRUE(edge3.IsValid());

  auto const testOrder = [this](std::vector<ScAddr> const & addrs)
  {
    ScAddr const structAddr = m_ctx->CreateNode(ScType::NodeConstStruct);
    ScStruct st(*m_ctx, structAddr);

    for (auto const & a : addrs)
      st << a;

    ScTemplatePtr templ = ScTemplateStructBuilder(*m_ctx).Make(structAddr);
    EXPECT_TRUE(templ);
  };

  testOrder({addr1, addr2, addr3, addr4, edge1, edge2, edge3});
  testOrder({edge3, edge2, edge1, addr4, addr3, addr2, addr1});
  testOrder({addr1, addr2, addr3, addr4, edge2, edge1, edge3});
}

TEST_F(ScTemplateStructBuidlerTest, addrs_in_repls)
{
  // create template in sc-memory
  {
    SCsHelper helper(*m_ctx, std::make_shared<DummyFileInterface>());
    EXPECT_TRUE(
        helper.GenerateBySCsText("[* x _-> _y;; *] => nrel_system_identifier: [test_template_parameters_addrs];;"));
  }

  ScAddr const templAddr = m_ctx->HelperFindBySystemIdtf("test_template_parameters_addrs");
  EXPECT_TRUE(templAddr.IsValid());

  ScAddr const _yAddr = m_ctx->HelperFindBySystemIdtf("_y");
  EXPECT_TRUE(_yAddr.IsValid());

  ScTemplatePtr templ = ScTemplateStructBuilder(*m_ctx).Make(templAddr);
  EXPECT_TRUE(templ);

  EXPECT_TRUE(templ->HasReplacement("_y"));
  EXPECT_TRUE(templ->HasReplacement(_yAddr.ToString()));
}

TEST_F(ScTemplateStructBuidlerTest, edge_from_edge)
{
  /**
   * @edge1 = addr1 _-> _addr2;;
   * @adge1 _-> addr3;;
   */
  ScAddr const addr1 = m_ctx->CreateNode(ScType::NodeConst);
  EXPECT_TRUE(addr1.IsValid());
  ScAddr const addr2 = m_ctx->CreateNode(ScType::NodeVar);
  EXPECT_TRUE(addr2.IsValid());
  ScAddr const addr3 = m_ctx->CreateNode(ScType::NodeConstRole);
  EXPECT_TRUE(addr3.IsValid());

  ScAddr const edge1 = m_ctx->CreateEdge(ScType::EdgeAccessVarPosPerm, addr1, addr2);
  EXPECT_TRUE(edge1.IsValid());
  ScAddr const edge2 = m_ctx->CreateEdge(ScType::EdgeAccessVarPosPerm, edge1, addr3);
  EXPECT_TRUE(edge2.IsValid());

  auto const testOrder = [this](std::vector<ScAddr> const & addrs)
  {
    ScAddr const structAddr = m_ctx->CreateNode(ScType::NodeConstStruct);
    ScStruct st(*m_ctx, structAddr);

    for (auto const & a : addrs)
      st << a;

    ScTemplatePtr templ = ScTemplateStructBuilder(*m_ctx).Make(structAddr);
    EXPECT_TRUE(templ);
  };

  testOrder({addr1, addr2, addr3, edge1, edge2});
  testOrder({edge2, edge1, addr3, addr2, addr1});
  testOrder({addr1, addr2, addr3, edge2, edge1});
}
