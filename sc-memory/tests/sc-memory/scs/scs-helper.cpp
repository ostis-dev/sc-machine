#include <gtest/gtest.h>

#include "sc_test.hpp"

#include "sc-memory/sc_link.hpp"
#include "sc-memory/sc_memory.hpp"
#include "sc-memory/sc_scs_helper.hpp"
#include "sc-memory/sc_templates.hpp"
#include "scs_test_utils.hpp"

using SCsHelperTest = ScMemoryTest;

TEST_F(SCsHelperTest, GenerateBySCs)
{
  std::vector<std::pair<std::string, std::string>> tests = {
      {"x -> y;;", "x _-> _y;;"},
      {"x1 => nrel_x1: [test_content*];;", "x1 _=> nrel_x1:: _[];;"},
      {"x2 ~> y2 (* <- z2;; *);;", "x2 _~> _y2 (* _<- _z2;; *);;"},
      {"x3 <- y3 (* <- sc_node_class;; *);;", "sc_node_class -> _y3;; _y3 _-> x3;;"},
  };

  for (auto const & t : tests)
  {
    SCsHelper helper(*m_ctx, std::make_shared<DummyFileInterface>());

    EXPECT_TRUE(helper.GenerateBySCsText(t.first));

    ScTemplatePtr templ = ScTemplateSCsBuilder(*m_ctx).Make(t.second);
    EXPECT_TRUE(templ);

    ScTemplateSearch search(*m_ctx, *templ);
    ScTemplateSearch::Iterator result = search.begin();
    EXPECT_NE(result, search.end());
    EXPECT_EQ(++result, search.end());
  }
}

TEST_F(SCsHelperTest, GenerateBySCs_FileURL)
{
  std::string const data = "x -> \"file://test.scs\";;";

  SCsHelper helper(*m_ctx, std::make_shared<TestFileInterface>());
  EXPECT_TRUE(helper.GenerateBySCsText(data));

  ScAddr const xAddr = m_ctx->HelperResolveSystemIdtf("x");
  EXPECT_TRUE(xAddr.IsValid());

  ScTemplatePtr templ =
      ScTemplateBuilder().Triple(xAddr, ScType::EdgeAccessVarPosPerm, ScType::LinkVar >> "_link").Make();
  EXPECT_TRUE(templ);

  ScTemplateSearch search(*m_ctx, *templ);
  ScTemplateSearch::Iterator result = search.begin();
  EXPECT_NE(result, search.end());

  ScAddr const linkAddr = result["_link"];
  EXPECT_TRUE(linkAddr.IsValid());

  EXPECT_EQ(++result, search.end());

  ScType const linkType = m_ctx->GetElementType(linkAddr);
  EXPECT_EQ(linkType, ScType::LinkConst);

  ScLink const link(*m_ctx, linkAddr);
  EXPECT_EQ(link.DetermineType(), ScLink::Type::Custom);
  EXPECT_EQ(link.GetAsString(), "content: file://test.scs");
}

TEST_F(SCsHelperTest, GenerateBySCs_Aliases)
{
  std::string const content = "SCsHelper_GenerateBySCs_Aliases";
  std::string const data = "@alias = [" + content + "];;";

  SCsHelper helper(*m_ctx, std::make_shared<TestFileInterface>());
  EXPECT_TRUE(helper.GenerateBySCsText(data));

  ScStreamPtr const stream = ScStreamMakeRead(content);

  ScAddrVector const links = m_ctx->FindLinksByContent(stream);
  EXPECT_EQ(links.size(), 1u);

  ScLink const link(*m_ctx, links[0]);
  std::string const content2 = link.GetAsString();
  EXPECT_EQ(content, content2);
}

TEST_F(SCsHelperTest, GenerateBySCs_NestedAliases)
{
  std::string const data =
      "a -> b;;"
      "@link_a = a;;"
      "@link_b = b;;"
      "@edge_alias = (@link_a -> @link_b);;";

  SCsHelper helper(*m_ctx, std::make_shared<TestFileInterface>());
  EXPECT_TRUE(helper.GenerateBySCsText(data));

  ScAddr const aAddr = m_ctx->HelperResolveSystemIdtf("a");
  EXPECT_TRUE(aAddr.IsValid());

  ScAddr const bAddr = m_ctx->HelperResolveSystemIdtf("b");
  EXPECT_TRUE(bAddr.IsValid());

  ScTemplatePtr templ = ScTemplateBuilder().Triple(aAddr, ScType::EdgeAccessVarPosPerm, bAddr).Make();

  ScTemplateSearch search(*m_ctx, *templ);
  ScTemplateSearch::Iterator result = search.begin();
  EXPECT_NE(result, search.end());
  EXPECT_NE(++result, search.end());
  EXPECT_EQ(++result, search.end());
}

TEST_F(SCsHelperTest, GenerateBySCs_EdgeAlias)
{
  std::string const data =
      "@edge_alias = (c -> b);;"
      "a -> @edge_alias;;";
  SCsHelper helper(*m_ctx, std::make_shared<TestFileInterface>());
  EXPECT_TRUE(helper.GenerateBySCsText(data));

  ScAddr const aAddr = m_ctx->HelperResolveSystemIdtf("a");
  EXPECT_TRUE(aAddr.IsValid());

  ScAddr const bAddr = m_ctx->HelperResolveSystemIdtf("b");
  EXPECT_TRUE(bAddr.IsValid());

  ScAddr const cAddr = m_ctx->HelperResolveSystemIdtf("c");
  EXPECT_TRUE(cAddr.IsValid());

  ScTemplatePtr templ =
      ScTemplateBuilder()
          .TripleWithRelation(cAddr, ScType::EdgeAccessVarPosPerm, bAddr, ScType::EdgeAccessVarPosPerm, aAddr)
          .Make();

  ScTemplateSearch search(*m_ctx, *templ);
  ScTemplateSearch::Iterator result = search.begin();
  EXPECT_NE(result, search.end());
  EXPECT_EQ(++result, search.end());
}

TEST_F(SCsHelperTest, GenerateBySCs_ContourAlias)
{
  std::string const data =
      "@alias = [* x -> y;; *];;"
      "z -> @alias;;";
  SCsHelper helper(*m_ctx, std::make_shared<TestFileInterface>());
  EXPECT_TRUE(helper.GenerateBySCsText(data));

  ScAddr const xAddr = m_ctx->HelperResolveSystemIdtf("x");
  EXPECT_TRUE(xAddr.IsValid());

  ScAddr const yAddr = m_ctx->HelperResolveSystemIdtf("y");
  EXPECT_TRUE(xAddr.IsValid());

  ScAddr const zAddr = m_ctx->HelperResolveSystemIdtf("z");
  EXPECT_TRUE(zAddr.IsValid());

  ScTemplatePtr templ = ScTemplateBuilder()
                            .Triple(zAddr, ScType::EdgeAccessVarPosPerm, ScType::NodeVarStruct >> "_contour")
                            .Triple("_contour", ScType::EdgeAccessVarPosPerm, xAddr)
                            .Triple("_contour", ScType::EdgeAccessVarPosPerm, yAddr)
                            .Triple(xAddr, ScType::EdgeAccessVarPosPerm >> "_edge", yAddr)
                            .Triple("_contour", ScType::EdgeAccessVarPosPerm, "_edge")
                            .Make();

  ScTemplateSearch search(*m_ctx, *templ);
  ScTemplateSearch::Iterator result = search.begin();
  EXPECT_NE(result, search.end());
  EXPECT_EQ(++result, search.end());
}

TEST_F(SCsHelperTest, GenerateBySCs_Contents)
{
  std::string const dataString = "v_string -> [string];;";
  std::string const dataFloat = "v_float -> [^\"float:7\"];;";
  std::string const dataDouble = "v_double -> [^\"double:8\"];;";
  std::string const dataInt8 = "v_int8 -> [^\"int8:9\"];;";
  std::string const dataInt16 = "v_int16 -> [^\"int16:10\"];;";
  std::string const dataInt32 = "v_int32 -> [^\"int32:11\"];;";
  std::string const dataInt64 = "v_int64 -> [^\"int64:12\"];;";
  std::string const dataUint8 = "v_uint8 -> [^\"uint8:13\"];;";
  std::string const dataUint16 = "v_uint16 -> [^\"uint16:14\"];;";
  std::string const dataUint32 = "v_uint32 -> [^\"uint32:15\"];;";
  std::string const dataUint64 = "v_uint64 -> [^\"uint64:16\"];;";

  auto const testLink = [this](std::string const & data, std::string const & keynode)
  {
    SCsHelper helper(*m_ctx, std::make_shared<TestFileInterface>());
    EXPECT_TRUE(helper.GenerateBySCsText(data));

    ScAddr const kAddr = m_ctx->HelperResolveSystemIdtf(keynode);
    ScTemplatePtr templ =
        ScTemplateBuilder().Triple(kAddr, ScType::EdgeAccessVarPosPerm, ScType::LinkVar >> "_link").Make();

    ScTemplateSearch search(*m_ctx, *templ);
    ScTemplateSearch::Iterator result = search.begin();
    EXPECT_NE(result, search.end());

    ScAddr const linkAddr = result["_link"];
    EXPECT_TRUE(linkAddr.IsValid());

    EXPECT_EQ(++result, search.end());
    return linkAddr;
  };

  {
    ScLink const linkFloat(*m_ctx, testLink(dataString, "v_string"));
    EXPECT_EQ(linkFloat.DetermineType(), ScLink::Type::String);
    EXPECT_EQ(linkFloat.Get<std::string>(), "string");
  }

  {
    ScLink const linkFloat(*m_ctx, testLink(dataFloat, "v_float"));
    EXPECT_EQ(linkFloat.DetermineType(), ScLink::Type::Float);
    EXPECT_EQ(linkFloat.Get<float>(), 7.f);
  }

  {
    ScLink const linkDouble(*m_ctx, testLink(dataDouble, "v_double"));
    EXPECT_EQ(linkDouble.DetermineType(), ScLink::Type::Double);
    EXPECT_EQ(linkDouble.Get<double>(), 8.0);
  }

  {
    ScLink const linkInt8(*m_ctx, testLink(dataInt8, "v_int8"));
    EXPECT_EQ(linkInt8.DetermineType(), ScLink::Type::Int8);
    EXPECT_EQ(linkInt8.Get<int8_t>(), 9);
  }

  {
    ScLink const linkInt16(*m_ctx, testLink(dataInt16, "v_int16"));
    EXPECT_EQ(linkInt16.DetermineType(), ScLink::Type::Int16);
    EXPECT_EQ(linkInt16.Get<int16_t>(), 10);
  }

  {
    ScLink const linkInt32(*m_ctx, testLink(dataInt32, "v_int32"));
    EXPECT_EQ(linkInt32.DetermineType(), ScLink::Type::Int32);
    EXPECT_EQ(linkInt32.Get<int32_t>(), 11);
  }

  {
    ScLink const linkInt64(*m_ctx, testLink(dataInt64, "v_int64"));
    EXPECT_EQ(linkInt64.DetermineType(), ScLink::Type::Int64);
    EXPECT_EQ(linkInt64.Get<int64_t>(), 12);
  }

  {
    ScLink const linkUint8(*m_ctx, testLink(dataUint8, "v_uint8"));
    EXPECT_EQ(linkUint8.DetermineType(), ScLink::Type::UInt8);
    EXPECT_EQ(linkUint8.Get<uint8_t>(), 13u);
  }

  {
    ScLink const linkUint16(*m_ctx, testLink(dataUint16, "v_uint16"));
    EXPECT_EQ(linkUint16.DetermineType(), ScLink::Type::UInt16);
    EXPECT_EQ(linkUint16.Get<uint16_t>(), 14u);
  }

  {
    ScLink const linkUint32(*m_ctx, testLink(dataUint32, "v_uint32"));
    EXPECT_EQ(linkUint32.DetermineType(), ScLink::Type::UInt32);
    EXPECT_EQ(linkUint32.Get<uint32_t>(), 15u);
  }

  {
    ScLink const linkUint64(*m_ctx, testLink(dataUint64, "v_uint64"));
    EXPECT_EQ(linkUint64.DetermineType(), ScLink::Type::UInt64);
    EXPECT_EQ(linkUint64.Get<uint64_t>(), 16u);
  }
}

TEST_F(SCsHelperTest, GenerateBySCs_SingleNode)
{
  SCsHelper helper(*m_ctx, std::make_shared<DummyFileInterface>());
  std::string const scsData = "node <- sc_node_class;;";

  EXPECT_TRUE(helper.GenerateBySCsText(scsData));

  ScAddr const node = m_ctx->HelperFindBySystemIdtf("node");
  EXPECT_TRUE(node.IsValid());
  EXPECT_EQ(m_ctx->GetElementType(node), ScType::NodeConstClass);
}

TEST_F(SCsHelperTest, GenerateBySCs_Visibility_System)
{
  SCsHelper helper(*m_ctx, std::make_shared<DummyFileInterface>());

  EXPECT_TRUE(helper.GenerateBySCsText("x -> y;;"));
  EXPECT_TRUE(helper.GenerateBySCsText("x ~> z;;"));

  ScAddr const xAddr = m_ctx->HelperResolveSystemIdtf("x");
  EXPECT_TRUE(xAddr.IsValid());

  ScAddr const yAddr = m_ctx->HelperResolveSystemIdtf("y");
  EXPECT_TRUE(yAddr.IsValid());

  ScAddr const zAddr = m_ctx->HelperResolveSystemIdtf("z");
  EXPECT_TRUE(zAddr.IsValid());

  {
    ScTemplatePtr templ = ScTemplateBuilder().Triple(xAddr, ScType::EdgeAccessVarPosPerm, yAddr).Make();

    ScTemplateSearch search(*m_ctx, *templ);
    EXPECT_NE(search.begin(), search.end());
  }

  {
    ScTemplatePtr templ = ScTemplateBuilder().Triple(xAddr, ScType::EdgeAccessVarPosTemp, zAddr).Make();

    ScTemplateSearch search(*m_ctx, *templ);
    EXPECT_NE(search.begin(), search.end());
  }
}

TEST_F(SCsHelperTest, GenerateBySCs_Visibility_Global)
{
  SCsHelper helper(*m_ctx, std::make_shared<DummyFileInterface>());

  {
    ScAddr const xAddr = m_ctx->HelperResolveSystemIdtf("x_global");
    EXPECT_FALSE(xAddr.IsValid());
  }

  EXPECT_TRUE(helper.GenerateBySCsText("x_global -> .y;;"));
  EXPECT_TRUE(helper.GenerateBySCsText("x_global -> .y;;"));

  ScAddr const xAddr = m_ctx->HelperResolveSystemIdtf("x_global");
  EXPECT_TRUE(xAddr.IsValid());

  ScTemplatePtr templ =
      ScTemplateBuilder().Triple(xAddr, ScType::EdgeAccessVarPosPerm >> "_edge", ScType::Unknown >> "_trg").Make();

  ScTemplateSearch search(*m_ctx, *templ);
  std::vector<std::pair<ScAddr, ScAddr>> values;

  for (auto const & it : search)
    values.emplace_back(it["_trg"], it["_edge"]);

  EXPECT_EQ(values.size(), 2u);

  EXPECT_EQ(values[0].first, values[1].first);
  EXPECT_NE(values[0].second, values[1].second);
}

TEST_F(SCsHelperTest, GenerateBySCs_Visibility_Local)
{
  SCsHelper helper(*m_ctx, std::make_shared<DummyFileInterface>());

  {
    ScAddr const xAddr = m_ctx->HelperResolveSystemIdtf("x_local");
    EXPECT_FALSE(xAddr.IsValid());
  }

  EXPECT_TRUE(helper.GenerateBySCsText("x_local -> ..y;;"));
  EXPECT_TRUE(helper.GenerateBySCsText("x_local -> ..z;;"));

  ScAddr const xAddr = m_ctx->HelperResolveSystemIdtf("x_local");
  EXPECT_TRUE(xAddr.IsValid());

  ScTemplatePtr templ =
      ScTemplateBuilder().Triple(xAddr, ScType::EdgeAccessVarPosPerm, ScType::Unknown >> "_trg").Make();

  ScTemplateSearch search(*m_ctx, *templ);
  std::vector<ScAddr> values;
  for (auto const & it : search)
    values.push_back(it["_trg"]);

  EXPECT_EQ(values.size(), 2u);
  EXPECT_NE(values[0], values[1]);
}
