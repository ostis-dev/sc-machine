#include <gtest/gtest.h>

#include "sc_test.hpp"

#include "sc-memory/sc_link.hpp"
#include "sc-memory/sc_memory.hpp"
#include "sc-memory/sc_struct.hpp"
#include "template_test_utils.hpp"

using ScTemplateSearchTest = ScTemplateTest;

TEST_F(ScTemplateSearchTest, empty)
{
  ScAddr const addr = m_ctx->CreateNode(ScType::NodeClass);
  EXPECT_TRUE(addr.IsValid());

  ScTemplatePtr templ = ScTemplateBuilder().Triple(addr, ScType::EdgeAccessVarNegPerm, ScType::NodeVar).Make();

  ScTemplateSearch search(*m_ctx, *templ);
  EXPECT_EQ(search.begin(), search.end());
}

TEST_F(ScTemplateSearchTest, empty_end)
{
  ScAddr const addr = m_ctx->CreateNode(ScType::NodeConstClass);
  EXPECT_TRUE(addr.IsValid());

  ScAddr const addr2 = m_ctx->CreateNode(ScType::NodeConstAbstract);
  EXPECT_TRUE(addr2);

  ScAddr const edge = m_ctx->CreateEdge(ScType::EdgeAccessConstPosPerm, addr, addr2);
  EXPECT_TRUE(edge);

  ScTemplatePtr templ =
      ScTemplateBuilder().Triple(addr, ScType::EdgeAccessVarPosPerm >> "_edge", ScType::NodeVar).Make();

  ScTemplateSearch search(*m_ctx, *templ);
  ScTemplateSearch::Iterator it = search.begin();
  EXPECT_NE(it, search.end());
  EXPECT_EQ(it["_edge"], edge);

  ++it;
  EXPECT_EQ(it, search.end());
  EXPECT_FALSE(it["_edge"]);
}

TEST_F(ScTemplateSearchTest, search_1)
{
  /**			_y
   *			^
   *			|
   *			| <----- _z
   *			|
   *			x <----- _s
   *
   * scs: x _-> _z:: _y;; _s _-> x;;
   */
  ScAddr const templateAddr = m_ctx->CreateNode(ScType::NodeConstStruct);
  EXPECT_TRUE(templateAddr.IsValid());

  ScStruct templStruct(*m_ctx, templateAddr);
  ScAddr xAddr;
  {
    ScAddr _yAddr, _zAddr, _sAddr;

    xAddr = m_ctx->CreateNode(ScType::NodeConstMaterial);
    EXPECT_TRUE(xAddr.IsValid());
    EXPECT_TRUE(m_ctx->HelperSetSystemIdtf("x", xAddr));

    _yAddr = m_ctx->CreateNode(ScType::Var);
    EXPECT_TRUE(_yAddr.IsValid());
    EXPECT_TRUE(m_ctx->HelperSetSystemIdtf("_y", _yAddr));

    _zAddr = m_ctx->CreateNode(ScType::NodeVarRole);
    EXPECT_TRUE(_zAddr.IsValid());
    EXPECT_TRUE(m_ctx->HelperSetSystemIdtf("_z", _zAddr));

    _sAddr = m_ctx->CreateNode(ScType::NodeVarClass);
    EXPECT_TRUE(_sAddr.IsValid());
    EXPECT_TRUE(m_ctx->HelperSetSystemIdtf("_s", _sAddr));

    ScAddr const xyAddr = m_ctx->CreateEdge(ScType::EdgeAccessVarPosPerm, xAddr, _yAddr);
    EXPECT_TRUE(xyAddr.IsValid());
    ScAddr const zxyAddr = m_ctx->CreateEdge(ScType::EdgeAccessVarPosPerm, _zAddr, xyAddr);
    EXPECT_TRUE(zxyAddr.IsValid());
    ScAddr const sxAddr = m_ctx->CreateEdge(ScType::EdgeAccessVarPosPerm, _sAddr, xAddr);
    EXPECT_TRUE(sxAddr.IsValid());

    // append created elements into struct
    templStruct << xAddr << _yAddr << _zAddr << xyAddr << zxyAddr << _sAddr << sxAddr;
  }

  ScTemplatePtr templ = ScTemplateStructBuilder(*m_ctx).Make(templateAddr);
  EXPECT_TRUE(templ);

  // create test structure that correspond to template
  {
    ScAddr const yAddr = m_ctx->CreateNode(ScType::Const);
    EXPECT_TRUE(yAddr.IsValid());

    ScAddr const zAddr = m_ctx->CreateNode(ScType::NodeConstRole);
    EXPECT_TRUE(zAddr.IsValid());

    ScAddr const sAddr = m_ctx->CreateNode(ScType::NodeConstClass);
    EXPECT_TRUE(sAddr.IsValid());

    ScAddr const xyAddr = m_ctx->CreateEdge(ScType::EdgeAccessConstPosPerm, xAddr, yAddr);
    EXPECT_TRUE(xyAddr.IsValid());
    ScAddr const zxyAddr = m_ctx->CreateEdge(ScType::EdgeAccessConstPosPerm, zAddr, xyAddr);
    EXPECT_TRUE(zxyAddr.IsValid());
    ScAddr const sxAddr = m_ctx->CreateEdge(ScType::EdgeAccessConstPosPerm, sAddr, xAddr);
    EXPECT_TRUE(sxAddr.IsValid());

    // test search by template
    {
      ScTemplateSearch search(*m_ctx, *templ);
      ScTemplateSearch::Iterator found = search.begin();
      EXPECT_NE(found, search.end());

      EXPECT_EQ(found["x"], xAddr);
      EXPECT_EQ(found["_y"], yAddr);
      EXPECT_EQ(found["_z"], zAddr);
      EXPECT_EQ(found["_s"], sAddr);

      EXPECT_EQ(++found, search.end());
    }
  }
}

TEST_F(ScTemplateSearchTest, search_2)
{
  /** SCs:
   * addr => nrel_main_idtf: [] (* <- lang;; *);;
   */
  ScAddr const addr = m_ctx->CreateNode(ScType::NodeConst);
  EXPECT_TRUE(addr.IsValid());
  ScAddr const nrelMainIdtf = m_ctx->CreateNode(ScType::NodeConstNoRole);
  EXPECT_TRUE(nrelMainIdtf.IsValid());
  ScAddr const lang = m_ctx->CreateNode(ScType::NodeConstClass);
  EXPECT_TRUE(lang.IsValid());
  ScAddr const link = m_ctx->CreateLink();
  EXPECT_TRUE(link.IsValid());

  ScAddr const edgeCommon = m_ctx->CreateEdge(ScType::EdgeDCommonConst, addr, link);
  EXPECT_TRUE(edgeCommon.IsValid());
  ScAddr const edgeAttr = m_ctx->CreateEdge(ScType::EdgeAccessConstPosPerm, nrelMainIdtf, edgeCommon);
  EXPECT_TRUE(edgeAttr.IsValid());
  ScAddr const edgeLang = m_ctx->CreateEdge(ScType::EdgeAccessConstPosPerm, lang, link);
  EXPECT_TRUE(edgeLang.IsValid());

  // now check search
  ScTemplatePtr templ = ScTemplateBuilder()
                            .TripleWithRelation(
                                addr >> "_addr",
                                ScType::EdgeDCommonVar >> "_edgeCommon",
                                ScType::Link >> "_link",
                                ScType::EdgeAccessVarPosPerm >> "_edgeAttr",
                                nrelMainIdtf >> "_nrelMainIdtf")
                            .Triple(lang >> "_lang", ScType::EdgeAccessVarPosPerm >> "_edgeLang", "_link")
                            .Make();

  // search
  {
    ScTemplateSearch search(*m_ctx, *templ);
    ScTemplateSearch::Iterator found = search.begin();
    EXPECT_NE(found, search.end());

    EXPECT_EQ(found["_addr"], addr);
    EXPECT_EQ(found["_edgeCommon"], edgeCommon);
    EXPECT_EQ(found["_link"], link);
    EXPECT_EQ(found["_edgeAttr"], edgeAttr);
    EXPECT_EQ(found["_nrelMainIdtf"], nrelMainIdtf);
    EXPECT_EQ(found["_lang"], lang);
    EXPECT_EQ(found["_edgeLang"], edgeLang);

    EXPECT_EQ(++found, search.end());
  }
}

TEST_F(ScTemplateSearchTest, unknown_type)
{
  // addr1 -> addr2;;
  ScAddr const addr1 = m_ctx->CreateNode(ScType::NodeConst);
  EXPECT_TRUE(addr1.IsValid());
  ScAddr const addr2 = m_ctx->CreateNode(ScType::NodeConstAbstract);
  EXPECT_TRUE(addr2.IsValid());
  ScAddr const edge = m_ctx->CreateEdge(ScType::EdgeAccessConstPosPerm, addr1, addr2);
  EXPECT_TRUE(edge.IsValid());

  ScTemplatePtr templ =
      ScTemplateBuilder().Triple(addr1, ScType::EdgeAccessVarPosPerm >> "edge", ScType::Unknown >> "addr2").Make();

  ScTemplateSearch search(*m_ctx, *templ);
  ScTemplateSearch::Iterator found = search.begin();
  EXPECT_NE(found, search.end());

  EXPECT_EQ(found["edge"], edge);
  EXPECT_EQ(found["addr2"], addr2);
  EXPECT_EQ(++found, search.end());
}

TEST_F(ScTemplateSearchTest, links_with_relation)
{
  /**
   * deviceAddr _<= nrel_installed_apps: _tuple;;
   * _tuple _-> _app;
   *    _app => nrel_idtf: _idtf;;
   *    _app => nrel_image: _image;;
   */

  ScAddr const deviceAddr = m_ctx->CreateNode(ScType::NodeConst);
  EXPECT_TRUE(deviceAddr.IsValid());

  ScAddr const nrelInstalledApp = m_ctx->CreateNode(ScType::NodeConstNoRole);
  EXPECT_TRUE(nrelInstalledApp.IsValid());

  ScAddr const _tuple = m_ctx->CreateNode(ScType::NodeConstTuple);
  EXPECT_TRUE(_tuple.IsValid());

  ScAddr const nrelIdtf = m_ctx->CreateNode(ScType::NodeConstNoRole);
  EXPECT_TRUE(nrelIdtf.IsValid());

  ScAddr const nrelImage = m_ctx->CreateNode(ScType::NodeConstNoRole);
  EXPECT_TRUE(nrelImage.IsValid());

  ScAddr edge = m_ctx->CreateEdge(ScType::EdgeDCommonConst, _tuple, deviceAddr);
  EXPECT_TRUE(edge.IsValid());

  edge = m_ctx->CreateEdge(ScType::EdgeAccessConstPosPerm, nrelInstalledApp, edge);
  EXPECT_TRUE(edge.IsValid());

  struct TestData
  {
    ScAddr m_app;
    ScAddr m_idtf;
    ScAddr m_image;
  };

  size_t i = 0;
  std::vector<TestData> data(50);
  for (auto & d : data)
  {
    d.m_app = m_ctx->CreateNode(ScType::NodeConstAbstract);
    EXPECT_TRUE(d.m_app.IsValid());

    edge = m_ctx->CreateEdge(ScType::EdgeAccessConstPosPerm, _tuple, d.m_app);
    EXPECT_TRUE(edge.IsValid());

    d.m_idtf = m_ctx->CreateLink();
    EXPECT_TRUE(d.m_idtf.IsValid());

    ScLink idtfLink(*m_ctx, d.m_idtf);
    EXPECT_TRUE(idtfLink.Set("idtf_" + std::to_string(i)));

    edge = m_ctx->CreateEdge(ScType::EdgeDCommonConst, d.m_app, d.m_idtf);
    EXPECT_TRUE(edge.IsValid());

    edge = m_ctx->CreateEdge(ScType::EdgeAccessConstPosPerm, nrelIdtf, edge);
    EXPECT_TRUE(edge.IsValid());

    d.m_image = m_ctx->CreateLink();
    EXPECT_TRUE(d.m_image.IsValid());

    ScLink imageLink(*m_ctx, d.m_image);
    EXPECT_TRUE(imageLink.Set("data_" + std::to_string(i)));

    edge = m_ctx->CreateEdge(ScType::EdgeDCommonConst, d.m_app, d.m_image);
    EXPECT_TRUE(edge.IsValid());

    edge = m_ctx->CreateEdge(ScType::EdgeAccessConstPosPerm, nrelImage, edge);
    EXPECT_TRUE(edge.IsValid());

    ++i;
  }

  ScTemplatePtr templ =
      ScTemplateBuilder()
          .TripleWithRelation(
              ScType::NodeVarTuple >> "_tuple",
              ScType::EdgeDCommonVar,
              deviceAddr,
              ScType::EdgeAccessVarPosPerm,
              nrelInstalledApp)
          .Triple("_tuple", ScType::EdgeAccessVarPosPerm, ScType::NodeVar >> "_app")
          .TripleWithRelation(
              "_app", ScType::EdgeDCommonVar, ScType::Link >> "_idtf", ScType::EdgeAccessVarPosPerm, nrelIdtf)
          .TripleWithRelation(
              "_app", ScType::EdgeDCommonVar, ScType::Link >> "_image", ScType::EdgeAccessVarPosPerm, nrelImage)
          .Make();

  ScTemplateSearch search(*m_ctx, *templ);
  ScTemplateSearch::Iterator found = search.begin();
  EXPECT_NE(found, search.end());

  std::vector<TestData> foundData;
  while (found != search.end())
  {
    foundData.push_back(TestData{found["_app"], found["_idtf"], found["_image"]});
    ++found;
  }

  auto compare = [](TestData const & a, TestData const & b)
  {
    return (a.m_app.Hash() < b.m_app.Hash());
  };
  std::sort(data.begin(), data.end(), compare);
  std::sort(foundData.begin(), foundData.end(), compare);

  EXPECT_EQ(foundData.size(), data.size());
  for (size_t i = 0; i < foundData.size(); ++i)
  {
    auto const & d1 = foundData[i];
    auto const & d2 = data[i];

    EXPECT_EQ(d1.m_app, d2.m_app);
    EXPECT_EQ(d1.m_idtf, d2.m_idtf);
    EXPECT_EQ(d1.m_image, d2.m_image);
  }
}

TEST_F(ScTemplateSearchTest, result_deduplication)
{
  /**
   *   a -> b (* <- sc_node_material;; *);;
   *   a -> c;;
   *
   *  We should get just one seach result, edge `a -> c` shouldn't appears twicely
   */
  ScAddr const a = m_ctx->CreateNode(ScType::Node);
  EXPECT_TRUE(a.IsValid());

  ScTemplatePtr templ = ScTemplateBuilder()
                            .Triple(a >> "a", ScType::EdgeAccessVarPosPerm, ScType::NodeVarMaterial >> "b")
                            .Triple("a", ScType::EdgeAccessVarPosPerm, ScType::NodeVar >> "c")
                            .Make();

  ScTemplateGenerate::Result genResult = ScTemplateGenerate(*m_ctx, *templ).Do();
  EXPECT_TRUE(genResult);
  EXPECT_EQ(a, (*genResult)["a"]);

  ScTemplateSearch search(*m_ctx, *templ);
  ScTemplateSearch::Iterator searchResult = search.begin();
  EXPECT_NE(searchResult, search.end());

  EXPECT_EQ(searchResult["a"], (*genResult)["a"]);
  EXPECT_EQ(searchResult["b"], (*genResult)["b"]);
  EXPECT_EQ(searchResult["c"], (*genResult)["c"]);

  EXPECT_EQ(++searchResult, search.end());
}

TEST_F(ScTemplateSearchTest, parameters)
{
  // prepare kb state
  ScAddr const node = m_ctx->CreateNode(ScType::NodeConst);
  EXPECT_TRUE(node.IsValid());

  ScAddr const target1 = m_ctx->CreateNode(ScType::NodeConstMaterial);
  EXPECT_TRUE(target1.IsValid());

  ScAddr const target2 = m_ctx->CreateNode(ScType::NodeConstMaterial);
  EXPECT_TRUE(target2.IsValid());

  ScAddr const rel1 = m_ctx->CreateNode(ScType::NodeConstNoRole);
  EXPECT_TRUE(rel1.IsValid());

  ScAddr const rel2 = m_ctx->CreateNode(ScType::NodeConstNoRole);
  EXPECT_TRUE(rel2.IsValid());

  ScTemplatePtr templ = ScTemplateBuilder()
                            .TripleWithRelation(
                                node,
                                ScType::EdgeAccessVarPosPerm,
                                ScType::NodeVarMaterial >> "_trg",
                                ScType::EdgeAccessVarPosPerm,
                                ScType::NodeVarNoRole >> "_rel")
                            .Make();

  {
    ScTemplateParams params;
    params.Add("_trg", target1).Add("_rel", rel1);

    ScTemplateGenerate::Result result = ScTemplateGenerate(*m_ctx, *templ).Do(params);
    EXPECT_TRUE(result);
  }

  {
    ScTemplateParams params;
    params.Add("_trg", target2).Add("_rel", rel2);

    ScTemplateGenerate::Result result = ScTemplateGenerate(*m_ctx, *templ).Do(params);
    EXPECT_TRUE(result);
  }

  // run test by using the same template
  // no parameters
  {
    ScTemplateSearch search(*m_ctx, *templ);
    ScTemplateSearch::Iterator it = search.begin();

    EXPECT_NE(it, search.end());
    ScAddr const foundTrg = it["_trg"];
    EXPECT_TRUE(foundTrg == target1 || foundTrg == target2);

    ScAddr const foundRel = it["_rel"];
    EXPECT_TRUE(foundRel == rel1 || foundRel == rel2);

    EXPECT_NE(++it, search.end());
    ScAddr const foundTrg2 = it["_trg"];
    EXPECT_NE(foundTrg, foundTrg2);
    EXPECT_TRUE(foundTrg2 == target1 || foundTrg2 == target2);

    ScAddr const foundRel2 = it["_rel"];
    EXPECT_NE(foundRel, foundRel2);
    EXPECT_TRUE(foundRel2 == rel1 || foundRel2 == rel2);

    EXPECT_EQ(++it, search.end());
  }

  // with parameters
  {
    ScTemplateParams params;
    params.Add("_trg", target1);

    ScTemplateSearch search(*m_ctx, *templ, params);
    ScTemplateSearch::Iterator it = search.begin();

    EXPECT_NE(it, search.end());
    EXPECT_EQ(it["_trg"], target1);
    EXPECT_EQ(it["_rel"], rel1);

    EXPECT_EQ(++it, search.end());
  }
}

TEST_F(ScTemplateSearchTest, parameters_addrs)
{
  // initialize kb state
  {
    SCsHelper helper(*m_ctx, std::make_shared<DummyFileInterface>());
    EXPECT_TRUE(helper.GenerateBySCsText("x -> y;; x -> z;;"));
  }

  ScAddr const xAddr = m_ctx->HelperFindBySystemIdtf("x");
  EXPECT_TRUE(xAddr.IsValid());

  ScAddr const yAddr = m_ctx->HelperFindBySystemIdtf("y");
  EXPECT_TRUE(yAddr.IsValid());

  ScAddr const zAddr = m_ctx->HelperFindBySystemIdtf("z");
  EXPECT_TRUE(zAddr.IsValid());

  // create template in sc-memory
  {
    SCsHelper helper(*m_ctx, std::make_shared<DummyFileInterface>());
    EXPECT_TRUE(
        helper.GenerateBySCsText("[* x _-> _y;; *] => nrel_system_identifier: [test_template_parameters_addrs];;"));
  }

  ScAddr const templAddr = m_ctx->HelperFindBySystemIdtf("test_template_parameters_addrs");
  EXPECT_TRUE(templAddr.IsValid());

  ScAddr const paramAddr = m_ctx->HelperFindBySystemIdtf("_y");
  EXPECT_TRUE(paramAddr.IsValid());

  ScTemplatePtr templ = ScTemplateStructBuilder(*m_ctx).Make(templAddr);
  EXPECT_TRUE(templ);

  {
    ScTemplateParams params;
    params.Add(paramAddr, zAddr);

    ScTemplateSearch search(*m_ctx, *templ, params);
    ScTemplateSearch::Iterator it = search.begin();

    EXPECT_NE(it, search.end());
    EXPECT_EQ(it["_y"], zAddr);

    EXPECT_EQ(++it, search.end());
  }
}

TEST_F(ScTemplateSearchTest, parameters_addrs_edge)
{
}

TEST_F(ScTemplateSearchTest, parameters_invalid_assign_to_non_type)
{
  ScAddr const addr = m_ctx->CreateNode(ScType::NodeConstAbstract);
  EXPECT_TRUE(addr);

  ScAddr const value = m_ctx->CreateNode(ScType::NodeConstAbstract);
  EXPECT_TRUE(value);

  ScTemplatePtr templ =
      ScTemplateBuilder().Triple(addr >> "_addr", ScType::EdgeAccessVarFuzPerm, ScType::NodeVar).Make();

  ScTemplateParams params;
  params.Add("_addr", value);

  ScTemplateSearch search(*m_ctx, *templ, params);
  EXPECT_THROW(search.begin(), utils::ExceptionInvalidParams);
}

TEST_F(ScTemplateSearchTest, parameters_invalid_assign_variable_value)
{
  ScAddr const addr = m_ctx->CreateNode(ScType::NodeConstAbstract);
  EXPECT_TRUE(addr);

  ScAddr const value = m_ctx->CreateNode(ScType::NodeVarAbstract);
  EXPECT_TRUE(value);

  ScTemplatePtr templ =
      ScTemplateBuilder().Triple(addr, ScType::EdgeAccessVarFuzPerm, ScType::NodeVar >> "_var").Make();

  ScTemplateParams params;
  params.Add("_var", value);

  ScTemplateSearch search(*m_ctx, *templ, params);
  EXPECT_THROW(search.begin(), utils::ExceptionInvalidParams);
}

TEST_F(ScTemplateSearchTest, parameters_invalid_assign_non_compatible_type)
{
  ScAddr const addr = m_ctx->CreateNode(ScType::NodeConstAbstract);
  EXPECT_TRUE(addr);

  ScAddr const value = m_ctx->CreateNode(ScType::NodeVarClass);
  EXPECT_TRUE(value);

  ScTemplatePtr templ =
      ScTemplateBuilder().Triple(addr, ScType::EdgeAccessVarFuzPerm, ScType::NodeVar >> "_var").Make();

  ScTemplateParams params;
  params.Add("_var", value);

  ScTemplateSearch search(*m_ctx, *templ, params);
  EXPECT_THROW(search.begin(), utils::ExceptionInvalidParams);
}

TEST_F(ScTemplateSearchTest, parameters_invalid_assign_non_existing)
{
  ScAddr const addr = m_ctx->CreateNode(ScType::NodeConstAbstract);
  EXPECT_TRUE(addr);

  ScAddr const value = m_ctx->CreateNode(ScType::NodeConstClass);
  EXPECT_TRUE(value);

  ScTemplatePtr templ =
      ScTemplateBuilder().Triple(addr, ScType::EdgeAccessVarFuzPerm, ScType::NodeVar >> "_var").Make();

  ScTemplateParams params;
  params.Add("_non_exist", value);

  ScTemplateSearch search(*m_ctx, *templ, params);
  EXPECT_THROW(search.begin(), utils::ExceptionItemNotFound);
}
