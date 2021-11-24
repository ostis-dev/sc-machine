#include <gtest/gtest.h>

#include "sc_test.hpp"

#include "sc-memory/sc_memory.hpp"
#include "sc-memory/sc_struct.hpp"
#include "sc-memory/sc_templates.hpp"
#include "sc-memory/sc_type.hpp"
#include "template_test_utils.hpp"

using ScTemplateCommonTest = ScTemplateTest;

TEST_F(ScTemplateCommonTest, smoke)
{
  ScAddr const addr1 = m_ctx->CreateNode(ScType::NodeConst);
  EXPECT_TRUE(addr1.IsValid());

  ScAddr const addr2 = m_ctx->CreateNode(ScType::NodeConst);
  EXPECT_TRUE(addr2.IsValid());

  ScAddr const addr3 = m_ctx->CreateNode(ScType::NodeConst);
  EXPECT_TRUE(addr3.IsValid());

  ScAddr const edge1 = m_ctx->CreateEdge(ScType::EdgeAccessConstPosPerm, addr1, addr2);
  EXPECT_TRUE(edge1.IsValid());

  ScAddr const edge2 = m_ctx->CreateEdge(ScType::EdgeAccessConstPosPerm, addr3, edge1);
  EXPECT_TRUE(edge2.IsValid());

  ScTemplatePtr templ =
      ScTemplateBuilder()
          .Triple(addr1 >> "addr1", ScType::EdgeAccessVarPosPerm >> "edge1", ScType::NodeVar >> "addr2")
          .Triple(ScType::NodeVar >> "_addr1T2", ScType::EdgeAccessVarPosPerm >> "_addr2T2", "edge1")
          .Triple("addr2", ScType::EdgeDCommonVar >> "_addr2T3", "edge1")
          .Make();

  auto const genResult = ScTemplateGenerate(*m_ctx, *templ).Do();
  EXPECT_TRUE(genResult);

  ScIterator5Ptr const it5 = m_ctx->Iterator5(
      addr1, ScType::EdgeAccessConstPosPerm, ScType::Node, ScType::EdgeAccessConstPosPerm, ScType::Node);

  EXPECT_TRUE(it5->Next());
  EXPECT_EQ(it5->Get(0), (*genResult)["addr1"]);
  EXPECT_EQ(it5->Get(1), (*genResult)["edge1"]);
  EXPECT_EQ(it5->Get(2), (*genResult)["addr2"]);
  EXPECT_EQ(it5->Get(3), (*genResult)["_addr2T2"]);
  EXPECT_EQ(it5->Get(4), (*genResult)["_addr1T2"]);

  ScIterator3Ptr const it3 =
      m_ctx->Iterator3((*genResult)["addr2"], ScType::EdgeDCommon, ScType::EdgeAccessConstPosPerm);

  EXPECT_TRUE(it3->Next());
  EXPECT_EQ(it3->Get(0), (*genResult)["addr2"]);
  EXPECT_EQ(it3->Get(1), (*genResult)["_addr2T3"]);
  EXPECT_EQ(it3->Get(2), (*genResult)["edge1"]);

  ScTemplateSearch search(*m_ctx, *templ);
  ScTemplateSearch::Iterator found = search.begin();
  EXPECT_NE(found, search.end());

  EXPECT_EQ(it5->Get(0), found["addr1"]);
  EXPECT_EQ(it5->Get(1), found["edge1"]);
  EXPECT_EQ(it5->Get(2), found["addr2"]);
  EXPECT_EQ(it5->Get(3), (*genResult)["_addr2T2"]);
  EXPECT_EQ(it5->Get(4), (*genResult)["_addr1T2"]);

  EXPECT_EQ(it3->Get(0), found["addr2"]);
  EXPECT_EQ(it3->Get(1), (*genResult)["_addr2T3"]);
  EXPECT_EQ(it3->Get(2), found["edge1"]);

  EXPECT_EQ(++found, search.end());
}

TEST_F(ScTemplateCommonTest, search)
{
  size_t const testCount = 10;
  ScAddrVector nodes, edges;

  ScAddr const addrSrc = m_ctx->CreateNode(ScType::NodeConst);
  EXPECT_TRUE(addrSrc.IsValid());
  for (size_t i = 0; i < testCount; ++i)
  {
    ScAddr const addrTrg = m_ctx->CreateNode(ScType::NodeConst);
    EXPECT_TRUE(addrTrg.IsValid());

    ScAddr const addrEdge = m_ctx->CreateEdge(ScType::EdgeAccessConstPosPerm, addrSrc, addrTrg);
    EXPECT_TRUE(addrEdge.IsValid());

    nodes.push_back(addrTrg);
    edges.push_back(addrEdge);
  }

  ScTemplatePtr templ =
      ScTemplateBuilder()
          .Triple(addrSrc >> "addrSrc", ScType::EdgeAccessVarPosPerm >> "edge", ScType::NodeVar >> "addrTrg")
          .Make();

  ScTemplateSearch search(*m_ctx, *templ);
  ScTemplateSearch::Iterator found = search.begin();
  EXPECT_TRUE(found != search.end());

  while (found != search.end())
  {
    EXPECT_EQ(found["addrSrc"], addrSrc);

    EXPECT_TRUE(HasAddr(edges, found["edge"]));
    EXPECT_TRUE(HasAddr(nodes, found["addrTrg"]));

    ++found;
  }
}

TEST_F(ScTemplateCommonTest, searchTripleWithRelation)
{
  ScAddr const addr1 = m_ctx->CreateNode(ScType::NodeConst);
  EXPECT_TRUE(addr1.IsValid());

  ScAddr const addr2 = m_ctx->CreateNode(ScType::NodeConst);
  EXPECT_TRUE(addr2.IsValid());

  ScAddr const addr3 = m_ctx->CreateNode(ScType::NodeConst);
  EXPECT_TRUE(addr3.IsValid());

  ScAddr const edge1 = m_ctx->CreateEdge(ScType::EdgeAccessConstPosPerm, addr1, addr2);
  EXPECT_TRUE(edge1.IsValid());

  ScAddr const edge2 = m_ctx->CreateEdge(ScType::EdgeAccessConstPosPerm, addr3, edge1);
  EXPECT_TRUE(edge2.IsValid());

  {
    ScTemplatePtr templ =
        ScTemplateBuilder()
            .TripleWithRelation(
                addr1, ScType::EdgeAccessVarPosPerm, ScType::NodeVar, ScType::EdgeAccessVarPosPerm, addr3)
            .Make();

    ScTemplateSearch search(*m_ctx, *templ);
    ScTemplateSearch::Iterator found = search.begin();
    EXPECT_TRUE(found != search.end());
    EXPECT_EQ(++found, search.end());
  }

  {
    ScTemplatePtr templ = ScTemplateBuilder()
                              .TripleWithRelation(
                                  addr1 >> "1",
                                  ScType::EdgeAccessVarPosPerm >> "2",
                                  ScType::NodeVar >> "3",
                                  ScType::EdgeAccessVarPosPerm >> "4",
                                  addr3 >> "5")
                              .Make();

    ScTemplateSearch search(*m_ctx, *templ);
    ScTemplateSearch::Iterator found = search.begin();
    EXPECT_TRUE(found != search.end());
    EXPECT_EQ(++found, search.end());
  }
}

TEST_F(ScTemplateCommonTest, params_correct)
{
  ScAddr const addrConst = m_ctx->CreateNode(ScType::NodeConst);
  ScAddr const addrTest3 = m_ctx->CreateNode(ScType::NodeConstTuple);
  ScAddr const addrTest6 = m_ctx->CreateNode(ScType::NodeConstClass);

  ScTemplatePtr templ =
      ScTemplateBuilder()
          .Triple(addrConst >> "1", ScType::EdgeAccessVarPosPerm >> "_2", ScType::NodeVarTuple >> "_3")
          .Triple("_3", ScType::EdgeAccessVarPosPerm >> "_5", ScType::NodeVarClass >> "_6")
          .Make();

  ScTemplateParams params;
  params.Add("_3", addrTest3).Add("_6", addrTest6);

  ScTemplateGenerate::Result const result = ScTemplateGenerate(*m_ctx, *templ).Do(params);
  EXPECT_TRUE(result);

  ScTemplatePtr searchTempl =
      ScTemplateBuilder()
          .Triple(addrConst >> "1", ScType::EdgeAccessVarPosPerm >> "_2", ScType::NodeVarTuple >> "_3")
          .Triple("_3", ScType::EdgeAccessVarPosPerm >> "_5", ScType::NodeVarClass >> "_6")
          .Make();

  ScTemplateSearch search(*m_ctx, *searchTempl);
  ScTemplateSearch::Iterator found = search.begin();
  EXPECT_NE(found, search.end());
  EXPECT_EQ(found["_3"], addrTest3);
  EXPECT_EQ(found["_6"], addrTest6);
  EXPECT_EQ(++found, search.end());
}

TEST_F(ScTemplateCommonTest, params_invalid)
{
  ScAddr const addrConst = m_ctx->CreateNode(ScType::NodeConst);
  ScAddr const addrTest3 = m_ctx->CreateNode(ScType::NodeConstTuple);
  ScAddr const addrEdge2 = m_ctx->CreateEdge(ScType::EdgeAccessConstPosPerm, addrConst, addrTest3);

  ScTemplatePtr templ =
      ScTemplateBuilder()
          .Triple(
              addrConst >> "1",
              ScType::EdgeAccessVarPosPerm >> "_2",  // can't be replaced by param in template generation
              ScType::NodeVar >> "_3")               // can't be replaced by param in template generation
          .Make();

  ScTemplateGenerate::Result result = ScTemplateGenerate(*m_ctx, *templ).Do();
  EXPECT_TRUE(result);

  // test edge
  {
    ScTemplateParams params;
    params.Add("_2", addrEdge2);

    result = ScTemplateGenerate(*m_ctx, *templ).Do(params);
    EXPECT_FALSE(result);
  }

  // test invalid params
  {
    // target is const
    {
      EXPECT_EQ(
          false,
          TestTemplParams(*m_ctx)(addrConst >> "1", ScType::EdgeAccessVarPosPerm >> "_2", ScType::NodeConst >> "_3"));
    }

    // source is const
    {
      EXPECT_EQ(
          false,
          TestTemplParams(*m_ctx)(ScType::NodeConst >> "_1", ScType::EdgeAccessVarPosPerm >> "_2", addrConst >> "3"));
    }

    // edge is const
    {
      EXPECT_EQ(
          false,
          TestTemplParams(*m_ctx)(ScType::NodeVar >> "_1", ScType::EdgeAccessConstPosPerm >> "_2", addrConst >> "3"));
    }
  }
}

TEST_F(ScTemplateCommonTest, a_a_a)
{
  /**
   * _struct
   * _-> rrel_location:: _apiai_location;
   * _=> nrel_translation:: _apiai_speech
   *       (* _<- _lang;; *);;
   */

  ScAddr const _structAddr = m_ctx->CreateNode(ScType::NodeVarStruct);
  EXPECT_TRUE(_structAddr.IsValid());
  EXPECT_TRUE(m_ctx->HelperSetSystemIdtf("_struct", _structAddr));

  ScAddr const _apiai_locationAddr = m_ctx->CreateNode(ScType::NodeVar);
  EXPECT_TRUE(_apiai_locationAddr.IsValid());
  EXPECT_TRUE(m_ctx->HelperSetSystemIdtf("_apiai_location", _apiai_locationAddr));

  ScAddr const _apiai_speechAddr = m_ctx->CreateNode(ScType::NodeVar);
  EXPECT_TRUE(_apiai_speechAddr.IsValid());
  EXPECT_TRUE(m_ctx->HelperSetSystemIdtf("_apiai_speech", _apiai_speechAddr));

  ScAddr const _langAddr = m_ctx->CreateNode(ScType::NodeVar);
  EXPECT_TRUE(_langAddr.IsValid());
  EXPECT_TRUE(m_ctx->HelperSetSystemIdtf("_lang", _langAddr));

  ScAddr const rrel_locationAddr = m_ctx->CreateNode(ScType::NodeConst);
  EXPECT_TRUE(rrel_locationAddr.IsValid());
  EXPECT_TRUE(m_ctx->HelperSetSystemIdtf("rrel_location", rrel_locationAddr));

  ScAddr const nrel_translationAddr = m_ctx->CreateNode(ScType::NodeConst);
  EXPECT_TRUE(nrel_translationAddr.IsValid());
  EXPECT_TRUE(m_ctx->HelperSetSystemIdtf("nrel_translation", nrel_translationAddr));

  ScAddr const _struct_locationEdgeAddr =
      m_ctx->CreateEdge(ScType::EdgeAccessVarPosPerm, _structAddr, _apiai_locationAddr);
  EXPECT_TRUE(_struct_locationEdgeAddr.IsValid());

  ScAddr const _rrel_locationEdgeAddr =
      m_ctx->CreateEdge(ScType::EdgeAccessVarPosPerm, rrel_locationAddr, _struct_locationEdgeAddr);
  EXPECT_TRUE(_rrel_locationEdgeAddr.IsValid());

  ScAddr const _struct_speechEdgeAddr = m_ctx->CreateEdge(ScType::EdgeDCommonVar, _structAddr, _apiai_speechAddr);
  EXPECT_TRUE(_struct_speechEdgeAddr.IsValid());

  ScAddr const _nrel_translationEdgeAddr =
      m_ctx->CreateEdge(ScType::EdgeAccessVarPosPerm, nrel_translationAddr, _struct_speechEdgeAddr);
  EXPECT_TRUE(_nrel_translationEdgeAddr.IsValid());

  ScAddr const _langEdgeAddr = m_ctx->CreateEdge(ScType::EdgeAccessVarPosPerm, _langAddr, _apiai_speechAddr);
  EXPECT_TRUE(_langEdgeAddr.IsValid());

  // create template
  ScAddr const templStructAddr = m_ctx->CreateNode(ScType::NodeConstStruct);
  EXPECT_TRUE(templStructAddr.IsValid());
  ScStruct templStruct(*m_ctx, templStructAddr);

  templStruct << _structAddr << _apiai_locationAddr << _apiai_speechAddr << _langAddr << rrel_locationAddr
              << nrel_translationAddr << _struct_locationEdgeAddr << _rrel_locationEdgeAddr << _struct_speechEdgeAddr
              << _nrel_translationEdgeAddr << _langEdgeAddr;

  ScTemplatePtr templ = ScTemplateStructBuilder(*m_ctx).Make(templStructAddr);
  EXPECT_TRUE(templ);

  // check creation by this template
  {
    ScTemplateGenerate::Result result = ScTemplateGenerate(*m_ctx, *templ).Do();
    EXPECT_TRUE(result);
  }

  // check search by this template
  {
    ScTemplateSearch search(*m_ctx, *templ);
    ScTemplateSearch::Iterator found = search.begin();
    EXPECT_NE(found, search.end());
  }
}

TEST_F(ScTemplateCommonTest, a_a_a_a_f)
{
  SCsHelper scs(*m_ctx, std::make_shared<DummyFileInterface>());
  EXPECT_TRUE(scs.GenerateBySCsText("x => nrel: [];;"));

  ScAddr const nrelAddr = m_ctx->HelperResolveSystemIdtf("nrel");
  EXPECT_TRUE(nrelAddr.IsValid());

  ScAddr const xAddr = m_ctx->HelperResolveSystemIdtf("x");
  EXPECT_TRUE(xAddr.IsValid());

  ScTemplatePtr templ =
      ScTemplateBuilder()
          .TripleWithRelation(
              ScType::Unknown >> "_x", ScType::EdgeDCommonVar, ScType::Link, ScType::EdgeAccessVarPosPerm, nrelAddr)
          .Make();

  ScTemplateSearch search(*m_ctx, *templ);
  ScTemplateSearch::Iterator found = search.begin();
  EXPECT_NE(found, search.end());

  EXPECT_EQ(found["_x"], xAddr);
  EXPECT_EQ(++found, search.end());
}

TEST_F(ScTemplateCommonTest, big_template_smoke)
{
  ScAddr const set1 = m_ctx->CreateNode(ScType::NodeConstClass);
  ScAddr const rel = m_ctx->CreateNode(ScType::NodeConstNoRole);

  static const size_t el_num = 1 << 12;
  std::set<ScAddr, ScAddLessFunc> elements;
  for (size_t i = 0; i < el_num; ++i)
  {
    ScAddr const a = m_ctx->CreateNode(ScType::NodeConst);
    EXPECT_TRUE(a.IsValid());
    elements.insert(a);
  }

  // create template for pending events check
  size_t index = 0;
  ScTemplateBuilder builder;
  for (auto const & a : elements)
  {
    builder.TripleWithRelation(
        set1, ScType::EdgeDCommonVar, a >> ("_el" + std::to_string(index++)), ScType::EdgeAccessVarPosPerm, rel);
  }

  ScTemplatePtr templ = builder.Make();

  ScTemplateGenerate::Result genResult = ScTemplateGenerate(*m_ctx, *templ).Do();
  EXPECT_TRUE(genResult);

  // ensure whole data created correctly
  ScTemplateSearch search(*m_ctx, *templ);
  ScTemplateSearch::Iterator found = search.begin();
  EXPECT_NE(found, search.end());

  while (found != search.end())
  {
    for (size_t i = 0; i < elements.size(); ++i)
    {
      EXPECT_TRUE(elements.find(found["_el" + std::to_string(i)]) != elements.end());
    }
    ++found;
  }
}

TEST_F(ScTemplateCommonTest, named_struct)
{
  std::string const idtf = "test";
  ScTemplateNamedStruct::NamesMap names = {{idtf, 1}};

  ScAddrVector elements = {ScAddr(1), ScAddr(2), ScAddr(3)};

  ScAddrVector elementsCopy = elements;
  ScTemplateNamedStruct::NamesMap namesCopy = names;

  ScTemplateNamedStruct namedStruct(std::move(namesCopy), std::move(elementsCopy));

  EXPECT_FALSE(namedStruct.IsEmpty());
  EXPECT_EQ(elements[1], namedStruct[idtf]);
  EXPECT_FALSE(namedStruct["non_exist"].IsValid());

  namedStruct.Clear();
  EXPECT_TRUE(namedStruct.IsEmpty());
}

TEST_F(ScTemplateCommonTest, named_struct_builder)
{
  ScTemplateNamedStruct namedStruct;
  {
    ScAddr const addr(1);
    ScTemplateNamedStruct::Builder builder(namedStruct);
    EXPECT_TRUE(namedStruct.IsEmpty());
    builder.Add("_test", addr);
    builder.Add(addr);
    EXPECT_FALSE(namedStruct.IsEmpty());
    EXPECT_EQ(namedStruct.GetElements().size(), 1u);
  }

  // test builder
  ScTemplateNamedStruct::Builder builder(namedStruct);
  EXPECT_TRUE(namedStruct.IsEmpty());  // builder clears struct

  ScAddr const addr1(1);
  ScAddr const addr2(2);
  builder.Add("name1", addr1);
  builder.Add(addr2);

  EXPECT_EQ(namedStruct.GetElements().size(), 2u);

  EXPECT_EQ(namedStruct["name1"], addr1);
  EXPECT_EQ(namedStruct[addr1.ToString()], ScAddr::Empty);
  EXPECT_EQ(namedStruct[addr2.ToString()], ScAddr::Empty);

  EXPECT_THROW(builder.Add("name1", addr2), utils::ExceptionInvalidState);
  EXPECT_NO_THROW(builder.Add("name2", addr1));
}
