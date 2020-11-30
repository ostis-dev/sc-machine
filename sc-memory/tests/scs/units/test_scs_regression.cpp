#include <gtest/gtest.h>

#include "test_scs_utils.hpp"

TEST(scs_regression, issue_353)
{
  std::string const data =
      "a -> [*"
        "_x <- sc_node_class;;"
      "*];;";

  scs::Parser parser;
  EXPECT_TRUE(parser.Parse(data));

  // it should not create edge from struct type to node and place it into contour
  auto const & triples = parser.GetParsedTriples();
  EXPECT_EQ(triples.size(), 5u);
}
